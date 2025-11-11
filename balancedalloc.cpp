#include <bits/stdc++.h>
using namespace std;
using rng_t = mt19937_64;

static rng_t global_rng((uint64_t)chrono::high_resolution_clock::now().time_since_epoch().count());

// random integer in [0, m-1]
inline int rnd_bin(rng_t &rng, int m) {
    uniform_int_distribution<int> dist(0, m-1);
    return dist(rng);
}

double compute_gap(const vector<int>& loads, int n, int m) {
    int mx = *max_element(loads.begin(), loads.end());
    return double(mx) - double(n) / double(m);
}

// checkpoints: 1..m and k*m (1..m)
vector<int> make_checkpoints(int m) {
    vector<int> v;
    v.reserve(2*m);
    for(int i=1; i<=m; i++) v.push_back(i);
    for(int k=1; k<=m; k++) v.push_back(k*m);
    sort(v.begin(), v.end());
    v.erase(unique(v.begin(), v.end()), v.end());
    return v;
}

// d-choice using snapshot or direct view
int choose_d_choice(const vector<int>& view, int m, rng_t &rng, int d) {
    int best = rnd_bin(rng,m);
    int bestload = view[best];
    for(int i=1; i<d; i++) {
        int x = rnd_bin(rng,m);
        int l = view[x];
        if (l < bestload || (l == bestload && uniform_int_distribution<int>(0,1)(rng)==0)) {
            best = x;
            bestload = l;
        }
    }
    return best;
}

// compute quartiles for partial information trials
vector<int> compute_thresholds(const vector<int>& loads) {
    vector<int> sorted = loads;
    sort(sorted.begin(), sorted.end());
    int m = sorted.size();
    int median = sorted[m/2];
    int q25 = sorted[m/4];
    int q75 = sorted[(3*m)/4];
    return {q25, median, q75};
}

// choice using 1 question as partial information
int choose_k1(const vector<int>& loads, rng_t &rng) {
    int m = loads.size();
    auto thr = compute_thresholds(loads);
    int median = thr[1];

    int i = rnd_bin(rng, m);
    int j = rnd_bin(rng, m);
    while (j == i) j = rnd_bin(rng, m);

    bool above_i = (loads[i] > median);
    bool above_j = (loads[j] > median);

    if (above_i != above_j) return above_i ? j : i;
    return uniform_int_distribution<int>(0,1)(rng) ? i : j;
}

// choice using 2 questions as partial information
int choose_k2(const vector<int>& loads, rng_t &rng) {
    int m = loads.size();
    auto thr = compute_thresholds(loads);
    int q25 = thr[0], median = thr[1], q75 = thr[2];

    int i = rnd_bin(rng, m);
    int j = rnd_bin(rng, m);
    while (j == i) j = rnd_bin(rng, m);

    bool above_i = (loads[i] > median);
    bool above_j = (loads[j] > median);

    if (above_i != above_j) return above_i ? j : i;

    // both above median
    if (above_i && above_j) {
        bool top75_i = (loads[i] > q75);
        bool top75_j = (loads[j] > q75);
        if (top75_i != top75_j) return top75_i ? j : i;
        return uniform_int_distribution<int>(0,1)(rng) ? i : j;
    }

    // both above median
    bool top25_i = (loads[i] > q25);
    bool top25_j = (loads[j] > q25);
    if (top25_i != top25_j) return top25_i ? j : i;
    return uniform_int_distribution<int>(0,1)(rng) ? i : j;
}

// trial of the experiment where we throw iteratively up to m^2 balls in m bins
vector<double> run_trial(int m, const vector<int> &checkpoints, int d_choice, int batch_size, bool use_1plusbeta, double beta, int k_limited, rng_t &rng) {
    vector<int> loads(m,0);
    vector<double> gaps;
    gaps.reserve(checkpoints.size());
    int cp_idx = 0; // index to iterate over the checkpoints vector
    int n = 0;

    while(n < m*m) {
        int bsize = min(batch_size, m*m-n);

        // snapshot for the whole batch
        vector<int> snapshot = loads;

        for(int t=0;t<bsize;t++) {
            ++n;

            int bin = -1;
            if (k_limited == 1) bin = choose_k1(snapshot, rng);
            else if (k_limited == 2) bin = choose_k2(snapshot, rng);
            else if (use_1plusbeta) {
                uniform_real_distribution<double> ud(0.0,1.0);
                if (ud(rng) < beta) bin = choose_d_choice(snapshot, m, rng, 2);
                else bin = rnd_bin(rng, m);
            }
            else bin = (batch_size>1 ? choose_d_choice(snapshot, m, rng, d_choice)
                                     : choose_d_choice(loads, m, rng, d_choice));
            loads[bin]++;

            // case where checkpoint arrives in the middle of a batch
            if (cp_idx < (int)checkpoints.size() && checkpoints[cp_idx] == n) {
                gaps.push_back(compute_gap(loads, n, m));
                cp_idx++;
            }
            if (cp_idx >= (int)checkpoints.size()) break;
        }
        if (cp_idx >= (int)checkpoints.size()) break;
    }
    return gaps;
}

struct Stats {
    vector<int> ns;
    vector<double> mean, stddev;
};

// computes statistics for each checkpoint combining all repetitions
Stats aggregate(const vector<vector<double>>& data, const vector<int>& ns) {
    int T = data.size(), K = ns.size();
    Stats s;
    s.ns = ns;
    s.mean.assign(K, 0.0);
    s.stddev.assign(K, 0.0);

    for(int k=0; k<K; k++) {
        for(int t=0; t<T; t++) s.mean[k] += data[t][k];
        s.mean[k] /= T;
        for(int t=0; t<T; t++) s.stddev[k] += pow(data[t][k] - s.mean[k], 2);
        s.stddev[k] = sqrt(s.stddev[k] / max(1, T-1));
    }
    return s;
}

void write_csv(const string& fname, const Stats& s) {
    ofstream f(fname);
    f << "n,mean_gap,stddev_gap\n";
    f << fixed << setprecision(4);
    for(size_t i=0; i<s.ns.size(); i++) f << s.ns[i] << "," << s.mean[i] << "," << s.stddev[i] << "\n";
    f.close();
    cout << ">> wrote " << fname << "\n";
}

void help(char *p) {
    cout << "Usage: " << p << " [opts]\n"
         << "  --m INT          #bins (default 500)\n"
         << "  --T INT          #trials (default 20)\n"
         << "  --d INT          d-choice (default 2)\n"
         << "  --b INT          batch size (default 1 = no batching)\n"
         << "  --beta FLOAT     enable (1+beta)-choice instead of d-choice\n"
         << "  --k INT          #questions about partial information (default 0)";
}

int main(int argc,char**argv){
    int m=500, T=20, d=2, b=1, k=0;
    double beta=-1.0;

    for(int i=1; i<argc; i++){
        string a=argv[i];
        if (a=="--m" && i+1<argc) m=stoi(argv[++i]);
        else if (a=="--T" && i+1<argc) T=stoi(argv[++i]);
        else if (a=="--d" && i+1<argc) d=stoi(argv[++i]); // d is ignored if beta or k are specified
        else if (a=="--b" && i+1<argc) b=stoi(argv[++i]);
        else if (a=="--beta" && i+1<argc) beta=stod(argv[++i]); // beta is ignored if k is specified
        else if (a=="--k" && i+1<argc) k=stoi(argv[++i]);
        else { help(argv[0]); return 0; }
    }

    bool use_beta = (beta >= 0.0 && beta <= 1.0);

    if (k > 0) cout << "m=" << m << " T=" << T << " b=" << b << " scheme=k-limited" << " k=" << k << "\n";
    else if (use_beta) cout << "m=" << m << " T=" << T << " b=" << b << " scheme=1+beta" << " beta=" << beta << "\n";
    else cout << "m=" << m << " T=" << T << " b=" << b << " scheme=d-choice" << " d=" << d << "\n";

    vector<int> checkpoints = make_checkpoints(m);
    vector<vector<double>> data;
    data.reserve(T);

    for(int t=0; t<T; t++) {
        rng_t rng(global_rng());
        data.push_back(run_trial(m, checkpoints, d, b, use_beta, beta, k, rng));
    }

    Stats s = aggregate(data, checkpoints);

    string fname;
    if (k > 0) fname = "results_k" + to_string(k);
    else if (use_beta) fname = "results_1plusbeta" + to_string(beta);
    else fname = "results_d" + to_string(d);

    if (b > 1) fname += "_b" + to_string(b);
    fname += ".csv";
    write_csv(fname, s);
    return 0;
}
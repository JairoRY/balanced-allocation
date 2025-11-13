# Balanced Allocation Assignment

This project simulates various ball-and-bin allocation schemes to analyze load balancing and measure the **gap** ($G_n$)—the difference between the maximum bin load and the average load ($n/m$).

## 1. Requirements

### 1.1. Simulation (`balancedalloc.cpp`)
* **C++ Compiler:** Requires a C++ compiler supporting C++11 or later (e.g., GCC or Clang).

### 1.2. Plotting (`plots.py`)
* **Python:** Python 3 is required.
* **Libraries:** The following Python libraries must be installed:
    * `pandas`
    * `matplotlib`
    * `re` (Standard library)
    * `sys` (Standard library)

## 2. Compilation

The C++ code must be compiled before the simulation can be run.

### Command

Use the following command to compile `balancedalloc.cpp` with optimization enabled:

```bash
g++ -O2 balancedalloc.cpp -o balancedalloc
```

## 3. Execution

The execution process is two-fold: first run the C++ simulation, then run the Python plotting script on the generated CSV data.

### 3.1. Simulation (./balancedalloc)

The program simulates throwing up to $m^2$ balls into $m$ bins, collecting statistics over $T$ trials.

#### Usage:

```bash
./balancedalloc [opts]
```

### 3.2. Plotting (python3 plots.py)

The plotting script reads the CSV output and generates visualizations for the mean gap ($\overline{G_n}$) and standard deviation ($\sigma_{G_n}$).

#### Usage:

```bash
python3 plots.py <csv_file>
```

Example:

```bash
python3 plots.py results_d2.csv
# This will display four plots based on the data.
```

## 4. Output

### 4.1. Simulation Output (CSV Files)
The C++ simulation generates a CSV file based on the scheme and parameters used.

#### Filename Format:
- d-choice: results_dX.csv (e.g., results_d2.csv)
- $(1+\beta)$-choice: results_1plusbetaY.csv (e.g., results_1plusbeta0.500000.csv)
- k-limited: results_kZ.csv (e.g., results_k1.csv)

Batching (--b) adds _bM to the filename (e.g., results_k1_b500.csv).

### 4.2. Plotting Output (Visualizations)

The Python script generates and displays four distinct plots for the specified scheme:
- Mean Gap ($\overline{G_n}$) for the initial phase ($n \le m$).
- Mean Gap ($\overline{G_n}$) for the main phase ($n \ge m$).
- Standard Deviation ($\sigma_{G_n}$) for the initial phase ($n \le m$).
- Standard Deviation ($\sigma_{G_n}$) for the main phase ($n \ge m$).

Each plot is titled with the detected scheme name (e.g., "Two-choice").

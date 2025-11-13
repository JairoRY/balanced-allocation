#!/usr/bin/env python3
import sys
import re
import pandas as pd
import matplotlib.pyplot as plt

"""Detects the scheme name based on filename patterns."""
def detect_scheme_from_filename(fname: str) -> str:
    d_match = re.search(r"_d(\d+)", fname)
    if d_match:
        d = int(d_match.group(1))
        if d == 1:
            return "One-choice"
        elif d == 2:
            return "Two-choice"
        elif d == 3:
            return "Three-choice"
        else:
            return f"{d}-choice"

    beta_match = re.search(r"_1plusbeta([0-9]*\.?[0-9]+)", fname)
    if beta_match:
        beta = float(beta_match.group(1))
        return f"(1+β)-choice (β={beta:.2f})"
    
    k_match = re.search(r"_k(\k+)", fname)
    if k_match:
        k = int(k_match.group(1))
        return f"Partial information (k={k})"

    return "Unknown scheme"

"""Helper to make a plot with proper axis limits and style."""
def plot_range(df, x, y, color, title, ylabel, x_start):
    plt.figure(figsize=(8,5))
    plt.plot(df[x], df[y], color=color)
    plt.title(title, fontsize=14)
    plt.xlabel("n", fontsize=12)
    plt.ylabel(ylabel, fontsize=12)
    plt.grid(True)
    plt.xlim(left=x_start, right=df["n"].max())
    plt.ylim(bottom=0)
    plt.tight_layout()
    plt.show()

def main():
    if len(sys.argv) < 2:
        print("Usage: python plot_gaps.py <csv_file>")
        sys.exit(1)

    csv_file = sys.argv[1]
    scheme = detect_scheme_from_filename(csv_file)
    print(f"Detected scheme: {scheme}")

    df = pd.read_csv(csv_file)
    df = df.sort_values("n")

    # Infer m as sqrt(max(n))
    max_n = df["n"].max()
    m = int(max_n ** 0.5)

    # Split ranges
    df_1_to_m = df[df["n"] <= m]
    df_m_to_m2 = df[df["n"] >= m]

    # === Mean gap plots ===
    plot_range(df_1_to_m, "n", "mean_gap", "blue",
               f"{scheme}", r"$\overline{G_n}$",
               x_start=0)

    plot_range(df_m_to_m2, "n", "mean_gap", "blue",
               f"{scheme}", r"$\overline{G_n}$",
               x_start=m)

    # === Std deviation plots ===
    plot_range(df_1_to_m, "n", "stddev_gap", "darkred",
               f"{scheme}", r"$\sigma_{G_n}$",
               x_start=0)

    plot_range(df_m_to_m2, "n", "stddev_gap", "darkred",
               f"{scheme}", r"$\sigma_{G_n}$",
               x_start=m)

if __name__ == "__main__":
    main()

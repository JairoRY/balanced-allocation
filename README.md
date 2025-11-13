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
​```

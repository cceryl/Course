import matplotlib.pyplot as plt
import re
import numpy as np
import time
import random
import subprocess
import sys
import os

data_gen_file = "data_gen.py"
size_file = "size.txt"
time_A_file = "time_A.txt"
time_B_file = "time_B.txt"

plot_log_mode = True


def test_once(size):
    # Generate data
    # Command format: python .\data_gen.py --L_row <size> --L_col <size> --R_row <size> --R_col <size>
    cmd = "python " + data_gen_file + " --L_row " + \
        str(size) + " --L_col " + str(size) + " --R_row " + \
        str(size) + " --R_col " + str(size)
    os.system(cmd)
    # Compile the C program
    # Command format: C:\\mingw64\\bin\\gcc.exe <current_dir>\\*.c -mavx2 -o <current_dir>\\lab3.exe
    cmd = "C:\\mingw64\\bin\\gcc.exe " + os.getcwd() + "\\*.c -mavx2 -o " + \
        os.getcwd() + "\\lab3.exe"
    os.system(cmd)
    # Get the stdout of the program
    # First redirect the stdout to a file, then read the file
    # Command format: <current_dir>\\lab3.exe
    cmd = os.getcwd() + "\\lab3.exe" + " > " + os.getcwd() + "\\output.txt"
    os.system(cmd)
    # Read the output file and parse the time
    # The first line is time A, the second line is time B, the third line is "p" or "f"
    with open(os.getcwd() + "\\output.txt", "r") as f:
        time_A = float(f.readline())
        time_B = float(f.readline())
        mode = f.readline()
    # Return the result
    return time_A, time_B, mode


def test_all(size_begin, size_end, step):
    # Initialize the data
    size = size_begin
    time_A_list = []
    time_B_list = []
    # Test all sizes
    while size <= size_end:
        time_A, time_B, mode = test_once(size)
        time_A_list.append(time_A)
        time_B_list.append(time_B)
        # If mode is "f", report and exit
        if mode == "f":
            print("Error: The program is not correct.")
            return
        size += step
    # Write the result to files, use append mode
    with open(size_file, "a") as f:
        for i in range(len(time_A_list)):
            f.write(str(size_begin + i * step) + "\n")
    with open(time_A_file, "a") as f:
        for i in range(len(time_A_list)):
            f.write(str(time_A_list[i]) + "\n")
    with open(time_B_file, "a") as f:
        for i in range(len(time_B_list)):
            f.write(str(time_B_list[i]) + "\n")


def plot():
    # Read the data
    with open(size_file, "r") as f:
        size_list = [int(x) for x in f.readlines()]
    with open(time_A_file, "r") as f:
        time_A_list = [float(x) for x in f.readlines()]
    with open(time_B_file, "r") as f:
        time_B_list = [float(x) for x in f.readlines()]
    accer_list = []
    for i in range(len(time_A_list)):
        if (time_B_list[i] == 0):
            accer_list.append(0)
        else:
            accer_list.append(time_A_list[i] / time_B_list[i])
    # Plot the data, using log size as x-axis
    if (plot_log_mode):
        plt.plot(np.log(size_list), time_A_list, label="Naive GEMM")
        plt.plot(np.log(size_list), time_B_list, label="Optimized GEMM")
        plt.xlabel("log(size)")
        plt.ylabel("time")
        plt.legend()
        plt.show()
    else:
        # plt.plot(size_list, time_A_list, label="Naive GEMM")
        # plt.plot(size_list, time_B_list, label="Optimized GEMM")
        plt.plot(size_list, accer_list, label="Acceleration")
        plt.xlabel("size")
        plt.ylabel("Acceleration")
        plt.legend()
        plt.show()


def main():
    # Test all sizes
    # test_all(0 + 4, 512, 4)
    # test_all(512 + 16, 1024, 16)
    # test_all(1024 + 64, 2048, 64)
    # test_all(2048 + 256, 4096, 256)
    # Plot the data
    plot()


if __name__ == "__main__":
    main()

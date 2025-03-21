# Heuristic algorithm for solving 3D BPP
## Introduction
This repository contains the implementation of a heuristic algorithm for solving the 3D Bin Packing Problem (3D BPP). The algorithm is based on this paper: [Optimizing Three-Dimensional Bin Packing Through Simulation](https://www.researchgate.net/publication/228974015_Optimizing_Three-Dimensional_Bin_Packing_Through_Simulation).

The idea of the algorithm is to simulate the packing process of the items in the containers. The algorithm starts by sorting the items in decreasing order of volume. Then, it iterates over the items and tries to pack them in the containers. If no item fits in the current container, the next container is used. The algorithm stops when all items are packed or all containers are full.

Part of the code is based on [3dbinpacking](https://github.com/enzoruiz/3dbinpacking).

## Requirements
`plotly==5.24.1`: render the 3D visualization of the containers and items.

## Usage
An example of how to use the algorithm is in the file `main.py`.
# E-commerce packaging problem
## Introduction
E-commerce packaging problem is a combinatorial optimization problem that arises in the context of e-commerce. The problem is to determine the optimal way to pack a set of items into a minimum number of containers. The objective is to minimize the total volumn of the containers used while ensuring that all items are packed.

This repository uses a heuristic algorithm to solve the e-commerce packaging problem. The algorithm is based on this paper: [Optimizing Three-Dimensional Bin Packing Through Simulation](https://www.researchgate.net/publication/228974015_Optimizing_Three-Dimensional_Bin_Packing_Through_Simulation).

The dataset used is `task3.csv`. The possible container sizes are (35, 23, 13), (37, 26, 13), (38, 26, 13), (40, 28, 16), (42, 30, 18), (42, 30, 40), (52, 40, 17), (54, 45, 36). The result filling rate is 79.99%.

## Requirements
`plotly==5.24.1`: render the 3D visualization of the containers and items.
`pandas==2.2.3`: read the dataset.

## Usage
`solver.py`: the main script to solve the e-commerce packaging problem. Note that all scalars are multiplied by 10 to get a better performance.

`validator.py`: validate the solution and calculate performance statistics. You may also visualize some of the containers.
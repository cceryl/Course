# PPO to solve 3D Bin Packing Problem
## Introduction
This repository contains the implementation of a Proximal Policy Optimization (PPO) model to solve the 3D Bin Packing Problem. The model is based on this paper: [Solving 3D packing problem using Transformer network and reinforcement learning](https://www.sciencedirect.com/science/article/pii/S0957417422021716).

## Requirements
`numpy==1.26.4`: For numerical operations and data manipulation.
`gym==0.26.2`: For creating the environment and interacting with the model.
`stable_baseline3==2.4.0`: For PPO model.
`plotly==5.24.1`: For visualization.

## Usage
Use `train.py` to train the model. The model will be saved in the `models` directory. Specify the `restart` parameter to train a new model or continue training an existing model.

Use `evaluate.py` to test the model. The model will be loaded from the `models` directory. Specify the `tree_search` parameter to use tree search or not.
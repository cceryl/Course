from container import Container
from environment import BinPackingTestEnv
from item import generate_items

import os

from stable_baselines3 import PPO
from stable_baselines3.common.vec_env import DummyVecEnv
from stable_baselines3.common.callbacks import CheckpointCallback


container_size = (10, 10)
items = generate_items(8, 8, 8, 10)
tree_search = True
tree_split_factor = 2
tree_prune_threshold = 5

env = BinPackingTestEnv(Container('Container', container_size[0], container_size[1]), items)

best_model_steps = 0
for file in os.listdir('./models/'):
    if file.startswith('ppo_binpacking') and file.endswith('.zip'):
        steps = int(file.split('_')[2])
        if steps > best_model_steps:
            best_model_steps = steps

best_model_path = f'./models/ppo_binpacking_{best_model_steps}_steps.zip'
model = PPO.load(best_model_path)

if tree_search:
    done = False
    env.reset()
    test_envs = [env]

    while not done:
        new_envs: list[BinPackingTestEnv] = []
        for env in test_envs:
            for _ in range(tree_split_factor):
                copy = env.copy()
                obs = copy.state
                action, _ = model.predict(obs)
                _, _, done, _ = copy.step(action)
                new_envs.append(copy)

        new_envs.sort(key=lambda x: x.container.get_filling_ratio(), reverse=True)
        test_envs = new_envs[:tree_prune_threshold]

    for env in test_envs:
        print(f"filling ratio: {env.container.get_filling_ratio()}")
        env.render()

else:
    obs = env.reset()
    done = False

    print(f"items: {len(env.items)}")

    while not done:
        action, _ = model.predict(obs)
        obs, rewards, done, info = env.step(action)

        if info['success']:
            env.render()
        else:
            print(f"failure: {info['reason']}")

    print(f"filling ratio: {env.container.get_filling_ratio()}")

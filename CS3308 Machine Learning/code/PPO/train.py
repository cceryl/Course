from container import Container
from environment import BinPackingEnv

import os

from stable_baselines3 import PPO
from stable_baselines3.common.vec_env import DummyVecEnv
from stable_baselines3.common.callbacks import CheckpointCallback


container_size = (10, 10)
max_items = 10
n_envs = 1
restart = False


def create_env():
    return lambda: BinPackingEnv(Container('Container', container_size[0], container_size[1]), max_items)


envs = [create_env() for _ in range(n_envs)]
env = DummyVecEnv(envs)

os.makedirs('./models/', exist_ok=True)
save_model_callback = CheckpointCallback(save_freq=10000, save_path='./models/', name_prefix='ppo_binpacking')

if restart:
    for file in os.listdir('./models/'):
        os.remove(f'./models/{file}')

    model = PPO("MultiInputPolicy", env, verbose=1)
    model.learn(total_timesteps=1000000, callback=save_model_callback)

else:
    best_model_steps = 0
    for file in os.listdir('./models/'):
        if file.startswith('ppo_binpacking') and file.endswith('.zip'):
            steps = int(file.split('_')[2])
            if steps > best_model_steps:
                best_model_steps = steps

    best_model_path = f'./models/ppo_binpacking_{best_model_steps}_steps.zip'
    print(f"loading model: {best_model_path}")
    
    model = PPO.load(best_model_path)
    model.set_env(env)

    for file in os.listdir('./models/'):
        os.remove(f'./models/{file}')

    model.save('./models/ppo_binpacking_0_steps.zip')

    model.learn(total_timesteps=1000000, callback=save_model_callback)

from container import Container
from encoder import Encoder
from decoder import Decoder
from item import Item
from item import generate_items

import random

import gym
from gym import spaces
import numpy as np


class BinPackingEnv(gym.Env):
    def __init__(self, container: Container, max_items: int):
        super(BinPackingEnv, self).__init__()

        self.state = None
        self.action_space = spaces.MultiDiscrete([max_items, 6, container.length, container.width])
        self.observation_space = spaces.Dict({
            'container': spaces.Box(
                low=np.zeros((container.length, container.width, 7), dtype=np.float32),
                high=np.full((container.length, container.width, 7), [
                    container.height, container.length, container.width, container.length, container.length, container.width, container.width
                ], dtype=np.float32),
                dtype=np.float32
            ),
            'items_obs': spaces.Box(low=0, high=max(container.length, container.width, container.height), shape=(max_items, 3), dtype=np.float32),
            'item_mask': spaces.Box(low=0, high=1, shape=(max_items,), dtype=bool)
        })
        self.done = False

        self.container = container
        self.encoder = Encoder()
        self.decoder = Decoder()
        self.max_items = max_items

    def reset(self):
        self.container.reset()

        max_height = max(self.container.length, self.container.width)
        height = random.randint(1, max_height)
        n_items = random.randint(1, self.max_items)
        self.items = generate_items(self.container.length, self.container.width, height, n_items)

        self.state = self.encoder.encode_state(self.container, self.items, self.max_items)
        self.done = False

        return self.state

    def copy(self) -> 'BinPackingEnv':
        env = BinPackingEnv(self.container.copy(), self.max_items)
        env.state = self.state
        env.items = [item.copy() for item in self.items]
        env.done = self.done
        return env

    def step(self, action: list[int]):
        decoder = Decoder()
        index, rotation, position = decoder.decode_action(action)

        if index >= len(self.items):
            return self.state, -1, self.done, {'success': False, 'reason': 'Index out of range'}

        item = self.items[index]
        item.rotation_type = rotation

        success = self.container.add_item(item, position)
        if not success:
            return self.state, -1, self.done, {'success': False, 'reason': 'Item does not fit'}

        self.items.remove(item)

        if len(self.items) == 0:
            self.done = True

        filling_ratio = self.container.get_filling_ratio()
        height = self.container.height
        reward = 1 + filling_ratio - 0.1 * height

        self.state = self.encoder.encode_state(self.container, self.items, self.max_items)

        return self.state, reward, self.done, {'success': True}

    def render(self):
        self.container.render()


class BinPackingTestEnv(gym.Env):
    def __init__(self, container: Container, items: list[Item]):
        super(BinPackingTestEnv, self).__init__()

        self.state = None
        self.action_space = spaces.MultiDiscrete([len(items), 6, container.length, container.width])
        self.observation_space = spaces.Dict({
            'container': spaces.Box(
                low=np.zeros((container.length, container.width, 7), dtype=np.float32),
                high=np.full((container.length, container.width, 7), [
                    container.height, container.length, container.width, container.length, container.length, container.width, container.width
                ], dtype=np.float32),
                dtype=np.float32
            ),
            'items_obs': spaces.Box(low=0, high=max(container.length, container.width, container.height), shape=(len(items), 3), dtype=np.float32),
            'item_mask': spaces.Box(low=0, high=1, shape=(len(items),), dtype=bool)
        })
        self.done = False

        self.container = container
        self.encoder = Encoder()
        self.decoder = Decoder()
        self.items = [item.copy() for item in items]
        self.unpacked_items = [item.copy() for item in items]

    def copy(self) -> 'BinPackingTestEnv':
        env = BinPackingTestEnv(self.container.copy(), self.items)
        env.state = self.state
        env.unpacked_items = [item.copy() for item in self.unpacked_items]
        env.done = self.done
        return env

    def reset(self):
        self.container.reset()

        self.state = self.encoder.encode_state(self.container, self.unpacked_items, len(self.items))
        self.done = False

        return self.state

    def step(self, action: list[int]):
        index, rotation, position = self.decoder.decode_action(action)

        if index >= len(self.unpacked_items):
            return self.state, -1, self.done, {'success': False, 'reason': 'Index out of range'}

        item = self.unpacked_items[index]
        item.rotation_type = rotation

        success = self.container.add_item(item, position)
        if not success:
            return self.state, -1, self.done, {'success': False, 'reason': 'Item does not fit'}

        self.unpacked_items.remove(item)

        if len(self.unpacked_items) == 0:
            self.done = True

        filling_ratio = self.container.get_filling_ratio()
        height = self.container.height
        reward = 1 + filling_ratio - 0.1 * height

        self.state = self.encoder.encode_state(self.container, self.unpacked_items, len(self.items))

        return self.state, reward, self.done, {'success': True}

    def render(self):
        self.container.render()

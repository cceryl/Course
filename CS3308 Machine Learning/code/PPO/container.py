from item import Item
from render import render_container

import numpy as np


class Container:
    def __init__(self, name: str, length: int, width: int):
        self.name: str = name
        self.length: int = length
        self.width: int = width
        self.height: int = 0
        self.height_map = np.zeros((length, width), dtype=int)
        self.items: list[Item] = []

    def copy(self) -> 'Container':
        container = Container(self.name, self.length, self.width)
        container.items = [item.copy() for item in self.items]
        container.height_map = np.copy(self.height_map)
        container.height = self.height
        return container

    def reset(self):
        self.height = 0
        self.height_map = np.zeros((self.length, self.width), dtype=int)
        self.items = []

    def get_volume(self) -> int:
        return self.length * self.width * self.height

    def get_filling_ratio(self) -> float:
        return sum([item.get_volume() for item in self.items]) / self.get_volume() if self.height > 0 else 1

    def check_item_fit(self, item: Item, position: list[int]) -> bool:
        """
        Check if the item fits in the container at the given position.
        """

        length, width, _ = item.get_dimension()
        x, y, _ = position

        if x < 0 or x + length > self.length or y < 0 or y + width > self.width:
            return False

        z = np.max(self.height_map[x:x + length, y:y + width])
        item.position = [x, y, z]

        return True

    def add_item(self, item: Item, position: list[int]) -> bool:
        if not self.check_item_fit(item, position):
            return False

        self.items.append(item)

        length, width, height = item.get_dimension()
        x, y, z = item.position

        self.height_map[x:x + length, y:y + width] = z + height
        self.height = max(self.height, z + height)

        return True

    def render(self):
        render_container(self.items, [self.length, self.width, self.height]).show()

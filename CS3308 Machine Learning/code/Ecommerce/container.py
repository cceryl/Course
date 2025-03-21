from item import Item
from constants import RotationType
from render import render_container


class Container:
    def __init__(self, name: str, length: int, width: int, height: int):
        self.name: str = name
        self.length: int = length
        self.width: int = width
        self.height: int = height
        self.items: list[Item] = []

    def copy(self) -> 'Container':
        container = Container(self.name, self.length, self.width, self.height)
        container.items = [item.copy() for item in self.items]
        return container

    def reset(self):
        self.items = []

    def get_volume(self) -> int:
        return self.length * self.width * self.height

    def get_filling_ratio(self) -> float:
        return sum([item.get_volume() for item in self.items]) / self.get_volume() if self.items else 0

    def can_fit(self, item: Item) -> bool:
        """
        Check if the item can fit in the container in any orientation, without checking for overlapping.
        """

        length, width, height = item.get_dimension()

        possible_orientations = [
            (length, width, height),
            (length, height, width),
            (width, length, height),
            (width, height, length),
            (height, length, width),
            (height, width, length)
        ]

        return any([length <= self.length and width <= self.width and height <= self.height for length, width, height in possible_orientations])

    def check_item_fit(self, item: Item, position: list[int]) -> bool:
        """
        Check if the item fits in the container at the given position.
        """

        length, width, height = item.get_dimension()
        x, y, z = position

        if x < 0 or x + length > self.length or y < 0 or y + width > self.width or z < 0 or z + height > self.height:
            return False

        item.position = position

        return all([not item.overlap(other_item) for other_item in self.items])

    def add_item(self, item: Item, position: list[int]) -> bool:
        for rotation in range(len(RotationType.All)):
            item.rotation_type = rotation
            if self.check_item_fit(item, position):
                self.items.append(item.copy())
                return True

        return False

    def add_item_no_check(self, item: Item):
        self.items.append(item.copy())

    def validate(self):
        for item in self.items:
            length, width, height = item.get_dimension()
            x, y, z = item.position

            if x < 0 or x + length > self.length or y < 0 or y + width > self.width or z < 0 or z + height > self.height:
                return False

        for i in range(len(self.items)):
            for j in range(i+1, len(self.items)):
                if self.items[i].overlap(self.items[j]):
                    return False

        return True

    def render(self):
        render_container(self.items, [self.length, self.width, self.height]).show()

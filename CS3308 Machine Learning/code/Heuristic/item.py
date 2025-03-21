from constants import RotationType

import random


class Item:
    def __init__(self, name: str, length: int, width: int, height: int):
        self.name: str = name
        self.length: int = length
        self.width: int = width
        self.height: int = height
        self.rotation_type: RotationType = RotationType.Rotate_LWH
        self.position: list[int] = [0, 0, 0]

    def get_volume(self) -> int:
        return self.length * self.width * self.height

    def get_dimension(self) -> tuple[int, int, int]:
        """
        Get the real dimension of the item according to the rotation type.
        """

        return {
            RotationType.Rotate_LWH: (self.length, self.width, self.height),
            RotationType.Rotate_LHW: (self.length, self.height, self.width),
            RotationType.Rotate_WHL: (self.width, self.height, self.length),
            RotationType.Rotate_WLH: (self.width, self.length, self.height),
            RotationType.Rotate_HLW: (self.height, self.length, self.width),
            RotationType.Rotate_HWL: (self.height, self.width, self.length)
        }[self.rotation_type]

    def overlap(self, other_item: 'Item') -> bool:
        """
        Check if the item overlaps with another item.
        """

        length, width, height = self.get_dimension()
        x, y, z = self.position
        other_length, other_width, other_height = other_item.get_dimension()
        other_x, other_y, other_z = other_item.position

        if x + length <= other_x or other_x + other_length <= x:
            return False

        if y + width <= other_y or other_y + other_width <= y:
            return False

        if z + height <= other_z or other_z + other_height <= z:
            return False

        return True


def generate_items(length: int, width: int, height: int, n: int) -> list[Item]:
    """
    Split a large item into smaller items.
    """

    item_sizes = [[length, width, height]]
    max_steps = 1000

    while (len(item_sizes) < n and max_steps > 0):
        max_steps -= 1

        item = item_sizes.pop(random.choices(
            range(len(item_sizes)),
            weights=[item[0] * item[1] * item[2] for item in item_sizes],
            k=1
        )[0])

        axis = random.choices(
            [0, 1, 2],
            weights=[item[0], item[1], item[2]],
            k=1
        )[0]

        position = random.normalvariate(0.5, 0.1)
        position = int(position * item[axis])

        if position == 0 or position == item[axis]:
            item_sizes.append(item)
            continue

        item1 = item.copy()
        item2 = item.copy()

        item1[axis] = position
        item2[axis] = item[axis] - position

        item1[0], item1[1], item1[2] = random.sample([item1[0], item1[1], item1[2]], 3)
        item2[0], item2[1], item2[2] = random.sample([item2[0], item2[1], item2[2]], 3)

        item_sizes.append(item1)
        item_sizes.append(item2)

    if max_steps == 0:
        raise Exception('Failed to generate items: reached max steps')

    return [Item(f"Item {i}", item[0], item[1], item[2]) for i, item in enumerate(item_sizes)]

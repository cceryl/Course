from item import Item
from container import Container

import numpy as np


class Encoder:
    def __init__(self):
        pass

    def encode_item(self, item: Item) -> np.ndarray:
        return np.array([item.length, item.width, item.height]).astype(np.float32)

    def encode_items(self, items: list[Item], n_items: int) -> tuple[np.ndarray, np.ndarray]:
        """
        Encode the items by (length, width, height) for each item.
        If the number of items is less than n_items, the remaining items are padded with zeros.
        Extra items will be ignored, meaning that the agent can only see the first n_items items.
        Return the encoded items and a mask to indicate the presence of an item.
        """

        encoded = np.zeros((n_items, 3), dtype=np.float32)
        for i, item in enumerate(items[:n_items]):
            encoded[i] = self.encode_item(item)

        mask = np.zeros(n_items, dtype=bool)
        mask[:len(items)] = True

        return encoded, mask

    def encode_container(self, container: Container) -> np.ndarray:
        """
        Encode the container by (height, higher_len, higher_wid, pos_len, neg_len, pos_wid, neg_wid) for each cell on a 2D plane.
        'height' is the height of the highest item that covers the cell.
        'higher_len' is the distance to the nearest higher cell in the length direction.
        'higher_wid' is the distance to the nearest higher cell in the width direction.
        'pos_len' is the distance to the edge of the plane in the positive length direction.
        'neg_len' is the distance to the edge of the plane in the negative length direction.
        'pos_wid' is the distance to the edge of the plane in the positive width direction.
        'neg_wid' is the distance to the edge of the plane in the negative width direction.
        """

        encoded = np.zeros((container.length, container.width, 7), dtype=int)
        for x in range(container.length):
            for y in range(container.width):
                h = container.height_map[x, y]

                higher_len = 0
                for i in range(x, container.length):
                    if container.height_map[i, y] > h:
                        break
                    higher_len += 1

                higher_wid = 0
                for j in range(y, container.width):
                    if container.height_map[x, j] > h:
                        break
                    higher_wid += 1

                pos_len = 0
                for i in range(x, container.length):
                    if container.height_map[i, y] != h:
                        break
                    pos_len += 1

                neg_len = 0
                for i in range(x, -1, -1):
                    if container.height_map[i, y] != h:
                        break
                    neg_len += 1

                pos_wid = 0
                for j in range(y, container.width):
                    if container.height_map[x, j] != h:
                        break
                    pos_wid += 1

                neg_wid = 0
                for j in range(y, -1, -1):
                    if container.height_map[x, j] != h:
                        break
                    neg_wid += 1

                encoded[x, y] = np.array([h, higher_len, higher_wid, pos_len, neg_len, pos_wid, neg_wid])

        return encoded.astype(np.float32)

    def encode_state(self, container: Container, items: list[Item], n_items: int) -> dict[str, np.ndarray]:
        """
        Encode the state of the environment.
        The encoded container is a height map with plane information. Size: (container.length * container.width * 7).
        The encoded items are a list of item dimensions. Size: (n_items * 3).
        The item mask is a list of zeros and ones to indicate the presence of an item. Size: (n_items).
        If the number of items is less than n_items, the remaining items are padded with zeros.
        Extra items will be ignored, meaning that the agent can only see the first n_items items.
        """

        container_encoded = self.encode_container(container)
        items_encoded, items_mask = self.encode_items(items, n_items)

        return {
            'container': container_encoded,
            'items_obs': items_encoded,
            'item_mask': items_mask
        }

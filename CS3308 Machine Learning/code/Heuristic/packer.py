from container import Container
from item import Item


class Packer:
    def __init__(self):
        self.items: list[Item] = []
        self.containers: list[Container] = []

    def add_container(self, container: Container):
        self.containers.append(container)

    def add_item(self, item: Item):
        self.items.append(item)

    def try_add_item(self, container: Container, item: Item) -> bool:
        if not container.items:
            return container.add_item(item, [0, 0, 0])

        def vector_sum(a, b):
            return [a[i] + b[i] for i in range(0, 3)]

        for axis in range(0, 3):
            for container_item in container.items:
                length, width, height = container_item.get_dimension()
                offset = [
                    length if axis == 0 else 0,
                    width if axis == 1 else 0,
                    height if axis == 2 else 0
                ]

                if container.add_item(item, vector_sum(container_item.position, offset)):
                    return True

        return False

    def pack(self, bigger_first: bool = False, distribute_items: bool = False) -> list[Item]:
        """
        Pack the items into the containers.
        Return the list of items that could not be packed.
        """

        self.containers.sort(key=lambda container: container.get_volume(), reverse=bigger_first)
        self.items.sort(key=lambda item: item.get_volume(), reverse=bigger_first)

        unpacked_items = self.items.copy()

        for container in self.containers:
            for item in unpacked_items:
                self.try_add_item(container, item)

            if distribute_items:
                for item in container.items:
                    unpacked_items.remove(item)

        return unpacked_items

    def render(self):
        for container in self.containers:
            if not container.items:
                continue

            container.render()

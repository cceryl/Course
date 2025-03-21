from container import Container
from item import Item


class Packer:
    def __init__(self, container: Container):
        self.items: list[Item] = []
        self.container: Container = container.copy()

    def reset(self):
        self.items = []
        self.container.reset()

    def add_item(self, item: Item):
        self.items.append(item)
    
    def add_items(self, items: list[Item]):
        self.items.extend(items)

    def try_add_item_to_container(self, container: Container, item: Item) -> bool:
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

    def pack(self) -> list[Container]:
        """
        Pack the items into the containers.
        Return a list of containers that are used.
        """

        self.items = [item for item in self.items if self.container.can_fit(item)]
        self.items.sort(key=lambda item: item.get_volume(), reverse=True)

        used_containers = []

        while self.items:
            container = Container('Container', self.container.length, self.container.width, self.container.height)

            for item in self.items:
                self.try_add_item_to_container(container, item)

            for item in container.items:
                self.items.remove(item)

            used_containers.append(container)

        return used_containers

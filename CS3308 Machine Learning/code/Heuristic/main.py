from item import generate_items
from container import Container
from packer import Packer

packer = Packer()

# Create containers
packer.add_container(Container('box', 6, 7, 5))
packer.add_container(Container('box', 9, 12, 3))
packer.add_container(Container('box', 4, 4, 5))
packer.add_container(Container('box', 8, 9, 10))
packer.add_container(Container('box', 10, 10, 10))

# Generate items by splitting a 10x10x10 cube into 20 smaller cubes
items = generate_items(10, 10, 10, 20)
for item in items:
    packer.add_item(item)

# Pack items into containers
# If bigger_first is True, bigger items will be packed first
# If distribute_items is True, items will be distributed among containers.
#     That is, if no item can fit in the current container, the next container will be set as the current container.
# If distribute_items is False, each container will try to pack all items.
unpacked = packer.pack(bigger_first=True, distribute_items=True)
packer.render()

print(f"Unpacked items: {len(unpacked)}")
for item in unpacked:
    print(f"    Item ({item.get_dimension()} can't be packed")

total_volume = sum([0 if not container.items else container.get_volume() for container in packer.containers])
filled_volume = sum([sum([item.get_volume() for item in container.items]) for container in packer.containers])

print(f"Total filling ratio: {filled_volume / total_volume}")
for container in packer.containers:
    print(f"    {container.name}: {container.get_filling_ratio()}")

from packer import Packer
from item import Item
from container import Container

import math

import pandas as pd


# Load & preprocess data
data_path = 'handout/task3.csv'
data = pd.read_csv(data_path)

items: list[Item] = []
impossible_items: list[Item] = []
container_sizes = [(350, 230, 130), (370, 260, 130), (380, 260, 130), (400, 280, 160),
                   (420, 300, 180), (420, 300, 400), (520, 400, 170), (540, 450, 360)]
containers = [Container('Container', *size) for size in container_sizes]

for index, row in data.iterrows():
    sku_code = row['sku_code']

    length = math.ceil(row['长(CM)'] * 10)
    width = math.ceil(row['宽(CM)'] * 10)
    height = math.ceil(row['高(CM)'] * 10)

    item = Item(sku_code, length, width, height)

    if not any([container.can_fit(item) for container in containers]):
        print(f"Item {sku_code} is too big to fit in any container if placed in orthogonal orientation, skipping")
        impossible_items.append(sku_code)
        continue

    quantity = row['qty']
    for _ in range(quantity):
        items.append(item.copy())

items.sort(key=lambda x: x.get_volume())
print(f"Loaded {len(items)} items")


# Initialize packers & hyperparameters
packers = [Packer(container) for container in containers]
used_containers: list[Container] = []
unpacked_items = [item.copy() for item in items]

pack_batch_size_large = 16
pack_batch_size_middle = 32
pack_batch_size_small = 96
filling_ratio_threshold = 0.65


# A batch consists of items of different sizes, to increase the chance of finding a more filled container
def get_batch():
    if len(unpacked_items) >= pack_batch_size_large + pack_batch_size_middle + pack_batch_size_small:
        small_items = unpacked_items[:pack_batch_size_small]
        large_items = unpacked_items[-pack_batch_size_large:]
        rest = unpacked_items[pack_batch_size_small:-pack_batch_size_large]
        start_index = len(rest) // 2 - pack_batch_size_middle // 2
        middle_items = rest[start_index:start_index + pack_batch_size_middle]
        return small_items + middle_items + large_items
    else:
        return unpacked_items


# Pack items with a filling ratio threshold
while unpacked_items:
    batch = get_batch()
    for packer in packers:
        packer.reset()
        packer.add_items(batch)

    results = [packer.pack() for packer in packers]

    best_result, best_ratio = None, 0

    for result in results:
        # Discard containers that are too empty
        valid_containers = [container for container in result
                            if container.get_filling_ratio() > filling_ratio_threshold]
        if not valid_containers:
            continue

        # Choose the containers with the highest average filling ratio
        average_ratio = sum([container.get_filling_ratio() for container in valid_containers]) / len(valid_containers)
        if average_ratio > best_ratio:
            best_result = valid_containers
            best_ratio = average_ratio

    if not best_result:
        break

    used_containers.extend(best_result)
    for container in best_result:
        for item in container.items:
            unpacked_items.remove(item)

    print(f"Ratio: {best_ratio}")
    print(f"Used {len(used_containers)} containers, {len(unpacked_items)} items left")


# Pack remaining items without restrictions
while unpacked_items:
    batch = get_batch()
    for packer in packers:
        packer.reset()
        packer.add_items(batch)

    results = [packer.pack() for packer in packers]

    best_result, best_ratio = None, 0

    for result in results:
        if not result:
            continue

        average_ratio = sum([container.get_filling_ratio() for container in result]) / len(result)
        if average_ratio > best_ratio:
            best_result = result
            best_ratio = average_ratio

    if not best_result:
        raise Exception("No valid containers found")

    used_containers.extend(best_result)
    for container in best_result:
        for item in container.items:
            unpacked_items.remove(item)

    print(f"Ratio: {best_ratio}")
    print(f"Used {len(used_containers)} containers, {len(unpacked_items)} items left")


# Print results
total_volumn_used = sum([container.get_volume() for container in used_containers])
total_item_volumn = sum([item.get_volume() for item in items])

print(f"Total volume used: {total_volumn_used} ({len(used_containers)} containers)")
print(f"Total item volume: {total_item_volumn}")
print(f"Fill rate: {total_item_volumn / total_volumn_used}")
print(f"Total impossible items: {len(impossible_items)}")


# Serialize results to file
def serialize_item(item: Item) -> str:
    return f"{item.name},{item.length},{item.width},{item.height},{item.position[0]},{item.position[1]},{item.position[2]},{int(item.rotation_type)}"


output_path = 'task3_output.txt'

with open(output_path, 'w') as f:
    for container in used_containers:
        f.write(f"Container,{container.length},{container.width},{container.height}\n")
        for item in container.items:
            f.write(serialize_item(item) + '\n')

    for item in impossible_items:
        f.write(f"Impossible,{item}\n")

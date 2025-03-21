from item import Item
from container import Container
from constants import RotationType

import math

import pandas as pd


# Load & preprocess data
data_path = 'task3.csv'
data = pd.read_csv(data_path)

items: list[Item] = []
impossible_items: list[Item] = []
container_sizes = [(350, 230, 130), (370, 260, 130), (380, 260, 130), (400, 280, 160),
                   (420, 300, 180), (420, 300, 400), (520, 400, 170), (540, 450, 360)]
containers = [Container('Container', *size) for size in container_sizes]
real_total_volume = 0

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

    real_total_volume += row['长(CM)'] * row['宽(CM)'] * row['高(CM)'] * row['qty']

items.sort(key=lambda x: x.get_volume())
print(f"Loaded {len(items)} items")


# Load & preprocess answer
answer_path = 'task3_output_7999.txt'
ans_containers: list[Container] = []
ans_impossible_items: list[Item] = []


def parse_container(line):
    # format: Container,42,30,40
    line = line.strip()
    parts = line.split(',')
    return Container('Container', int(parts[1]), int(parts[2]), int(parts[3]))


def parse_item(line):
    # format: DQ1615-700,42,31,15,0,0,18,0
    line = line.strip()
    parts = line.split(',')
    item = Item(parts[0], int(parts[1]), int(parts[2]), int(parts[3]))
    item.position = [int(parts[4]), int(parts[5]), int(parts[6])]
    item.rotation_type = RotationType.All[int(parts[7])]
    return item


def parse_impossible_item(line):
    # format: Impossible,DX9655-100
    line = line.strip()
    parts = line.split(',')
    return parts[1]


with open(answer_path, 'r') as f:
    lines = f.readlines()
    for line in lines:
        if line.startswith('Container'):
            ans_containers.append(parse_container(line))
        elif line.startswith('Impossible'):
            ans_impossible_items.append(parse_impossible_item(line))
        else:
            item = parse_item(line)
            ans_containers[-1].add_item_no_check(item)

print(f"Loaded {len(ans_containers)} containers and {len(ans_impossible_items)} impossible items from answer")


# Check if impossible items match
if impossible_items != ans_impossible_items:
    print("Impossible items do not match")
    exit(1)
print("Impossible items match")


# Check if containers are valid
for container in ans_containers:
    if not container.validate():
        print("Invalid container")
        exit(1)
print("Containers are valid")


# Check if all items are packed
ans_items: list[Item] = []
for container in ans_containers:
    ans_items.extend(container.items)
ans_items.sort(key=lambda x: x.get_volume())
for item in items:
    try:
        ans_items.remove(item)
    except ValueError:
        print(f"An unknown item {item.name} is packed")
        exit(1)
if len(ans_items) > 0:
    print(f"{len(ans_items)} items are not packed")
    exit(1)
print("All items are packed")


# Visualize some containers
containers_to_visualize_idx = [0]
for idx in containers_to_visualize_idx:
    container = ans_containers[idx]
    container.render()


# Calculate filling rates
total_volumn_used = sum([container.get_volume() for container in ans_containers])
total_item_volumn = sum([item.get_volume() for item in items])
filling_rate = total_item_volumn / total_volumn_used

filling_rates = [container.get_filling_ratio() for container in ans_containers]
best_filling_rate = max(filling_rates)
worst_filling_rate = min(filling_rates)


print(f"Total volume used: {total_volumn_used / 1000} ({len(ans_containers)} containers)")
print(f"Total item volume: {total_item_volumn / 1000}")
print(f"Filling rate: {filling_rate}")
print(f"Real total item volume: {real_total_volume}")
print(f"Real filling rate: {real_total_volume / (total_volumn_used / 1000)}")
print(f"Best filling rate: {best_filling_rate}")
print(f"Worst filling rate: {worst_filling_rate}")
print(f"Total impossible items: {len(impossible_items)}")
print("All checks passed")

from item import Item

import plotly.graph_objects as go


color_set = ['#00008B', '#8B0000', '#006400', '#B8860B', '#A9A9A9',
             '#B22222', '#8B4513', '#2F4F4F', '#228B22', '#D2691E',
             '#8B0000', '#006400', '#00008B', '#B8860B', '#A9A9A9',
             '#B22222', '#8B4513', '#2F4F4F', '#228B22', '#D2691E',
             '#A52A2A', '#8B008B', '#9932CC', '#8B0000', '#556B2F',
             '#FF4500', '#2E8B57', '#6A5ACD', '#708090', '#FF6347']


def render_item(item: Item, color: str) -> list[object]:
    """
    Return the plotly data for the item.
    """

    data = []
    x, y, z = item.position
    l, w, h = item.get_dimension()

    x_vals = [x, x + l, x + l, x, x, x + l, x + l, x]
    y_vals = [y, y, y + w, y + w, y, y, y + w, y + w]
    z_vals = [z, z, z, z, z + h, z + h, z + h, z + h]

    data.append(go.Mesh3d(
        x=x_vals, y=y_vals, z=z_vals,
        color=color,
        opacity=1,
        i=[7, 0, 0, 0, 4, 4, 6, 6, 4, 0, 3, 2],
        j=[3, 4, 1, 2, 5, 6, 5, 2, 0, 1, 6, 3],
        k=[0, 7, 2, 3, 6, 7, 1, 1, 5, 5, 7, 6],
        flatshading=True
    ))

    for i in range(8):
        for j in range(i + 1, 8):
            point_i = [x_vals[i], y_vals[i], z_vals[i]]
            point_j = [x_vals[j], y_vals[j], z_vals[j]]

            if sum([point_i[k] != point_j[k] for k in range(3)]) != 1:
                continue

            data.append(go.Scatter3d(
                x=[point_i[0], point_j[0]],
                y=[point_i[1], point_j[1]],
                z=[point_i[2], point_j[2]],
                mode='lines',
                line=dict(color='black', width=3)
            ))

    return data


def render_container(items: list[Item], container_size: list[int]) -> go.Figure:
    data = []
    item_color_index = 0
    for item in items:
        data += render_item(item, color_set[item_color_index])
        item_color_index = (item_color_index + 1) % len(color_set)

    fig = go.Figure(data=data)

    fig.update_layout(
        scene=dict(
            xaxis=dict(range=[0, container_size[0]]),
            yaxis=dict(range=[0, container_size[1]]),
            zaxis=dict(range=[0, container_size[2]]),
        ),
    )

    return fig

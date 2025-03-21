# Star

## Introduction

The program provides a set of components to customize the appearance of the stars, allowing users to experiment with different parameters.

![Interface](images/Interface.png)

## Features

1. **Vertex Slider**: Adjusts the number of vertices of the star, influencing its overall shape and complexity.

   ![Vertex Slider](images/vertex_slider_1.png)
   ![Vertex Slider](images/vertex_slider_2.png)

2. **Angle Slider**: Modifies the angle of the inner points of the star (enabled only when regular mode is disabled).

   ![Angle Slider](images/angle_slider_1.png)
   ![Angle Slider](images/angle_slider_2.png)

3. **Density Slider**: Adjusts the density of the star, affecting the arrangement of inner points (enabled only in regular mode). The concept of density is shown in the image below as "{ vertex / density }". See ["Regular Star Polygon" and "Schläfli symbol"](https://en.wikipedia.org/wiki/Star_polygon) for more information.

   ![Schläfli symbol](images/schlafli_symbol.png)

    Star polygon with the same vertices and different density (1, 2 and 3).

   ![Density Slider](images/density_slider_1.png)
   ![Density Slider](images/density_slider_2.png)
   ![Density Slider](images/density_slider_3.png)

4. **Rotate Slider**: Changes the rotation angle of the star around its center in a counterclockwise direction.

   ![Rotate Slider](images/rotate_slider_1.png)
   ![Rotate Slider](images/rotate_slider_2.png)

5. **Regular Star Polygon Button**: Toggles the regular mode, restricting the drawing to regular star shapes. In regular mode, possible numbers of vertices and density is limited, and angles can not be modified.

6. **Remove Angle Limit Button**: Toggles the limit on the angle of the inner points, allowing them to extend beyond the outer points or penetrate the center. This function is kept as a feature because it may create interesting "star polygons".

   ![Not a Bug](images/feature.png)

7. **Line Color Chooser and Fill Color Chooser**: Allows users to customize the colors of the star's outline and fill.

   ![Color Choosers](images/color_choosers.png)

8. **Zooming**: Users can use the mouse wheel to zoom in and out of the star shape in the graphical window.

   ![Zooming](images/zooming_1.png)
   ![Zooming](images/zooming_2.png)


# Binary Tree

## Introduction

The program provides a set of components to customize the parameters of a full binary tree, including levels, strides, heights and colors.

   ![Interface](images/tree.png)

## Features

1. **Level Slider**: Adjusts the number of levels in the binary tree.

   ![Level Slider](images/level_slider_1.png)
   ![Level Slider](images/level_slider_2.png)

2. **Stride Slider**: Adjusts the distance between adjacent nodes on the same level.

   ![Stride Slider](images/stride_slider_1.png)
   ![Stride Slider](images/stride_slider_2.png)

3. **Height Slider**: Adjusts the height difference between adjacent levels.

   ![Height Slider](images/height_slider_1.png)
   ![Height Slider](images/height_slider_2.png)

4. **Line Color Chooser**: Chooses the color of the lines connecting nodes and the node outline. 
   
   ![Line Color](images/line_color.png)

5. **Node Color Chooser**: Chooses the color to fill the nodes.
   
   ![Node Color](images/node_color.png)

6. **Zooming**: Zoom in and out of the drawing with mousewheel.
   
   ![Zoom](images/zoom_1.png)
   ![Zoom](images/zoom_2.png)

7. **Dragging**: Move the entire graph within the drawing window by dragging with left mouse button.
   
   ![Drag](images/drag_1.png)
   ![Drag](images/drag_2.png)
# constructible
Constructible number count

This is a project about constructible numbers. Starting with a set of points, count the number of constrible points added after using only a ruler and compass. See https://en.wikipedia.org/wiki/Constructible_number

**constructible_points.nb**

Mathematica notebook to apply ruler and compass construction to a set of points. All possible line-line, line-circle, and circle-circle intersections are generated from initial points.

**constructible_plot.nb**

Starting with {0,0} and {1,0} yields six constructible points. These are given at the top of this Mathematica notebook. Then all possible lines and circles are plotted, i.e., the intersections will yield the next iteration of constructible points.

The 203 points from this look like

![203 points symmetric across x and y axes](https://github.com/burnsba/constructible/raw/master/constructible_203.png)  

The lines and circles look like

![plot of constructible lines and circles](https://github.com/burnsba/constructible/raw/master/constructible_lines_circles.png)

-----

Sequences

{{0,0}, {1,0}} => 2, 6, 203  

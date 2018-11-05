# constructible
Constructible number count

This is a project about counting constructible numbers. 

-----

# Background

Starting with some initial points, the following rules are applied:

1. An infinite line can be drawn between two points.  
2. A circle can be drawn with origin at one point, and radius the Euclidean distance to some second point.  
3. Any line-line, line-circle, or circle-circle intersection will yield a new point (a circle can have either one or two intersections). 

This is ruler and compass construction as first studied by Greek mathematicians. See https://en.wikipedia.org/wiki/Constructible_number

# Description

This project is focused on counting the number of constructible points from iterating the line and circle generating rules. An iteration is:  

1. Start with a set of points.
2. Iterate over every possible pair, and
3. construct a line, left circle, and right circle from the pair (take the distance between the two points, and that's the radius; the circle origin can be at either point).
4. Iterate over every possible pair of objects in (3) and,
5. find the intersections.

For example, starting with {{0,0},{1,0}} yields 2,6,203,?

# Sub-projects

This repository has sub-folders based on programming language. There is an implementation
of the constructible count project in Mathematica, and a sample notebook to plot the points from the
2-series. The Mathematica project was re-written in c.

## C

See the C folder for specific details, but here are some brief notes on the primary files of interest.

**constructible.c**

Primary file of interest, calculates constructible points using the multiprecision GMP library.

**global.h**

Configuration options for the project are in this file.

**upper_bound.c**

Calculate an upper bound for the sequence of constructible points.

**test_gmp.c**

Compile and run this test file to ensure GMPlib is installed on the machine.

## C\#

**ConstructiblePlot**

This is a project used to plot the data points. This accepts the output file from the c constructible project. This can draw the single points, or generate the lines and circles.

## Mathematica

**constructible_points.nb**

Mathematica notebook to apply ruler and compass construction to a set of points. All possible line-line, line-circle, and circle-circle intersections are generated from initial points.

# Data

I have included a Data folder with sample output from the c constructible program. The first line gives the starting points and the iteration number and count. The data files are as follows:  

    data-2p,i2.out                => starts from two points, two iterations
    data-3p,i1-equilateral.out    => starts frpm three points (equilateral triangle), one iteration
    data-3p,i1-right_triangle.out => starts from three points (right triable), one iteration
    data-3p,i2-equilateral.out    => starts from three points (equilateral triangle), two iterations
    data-4p,i1-square.out         => starts from four points (square), one iteration

# Image

The C# project was used to generate these images. The file names follow the same convention as the data points above. Some of these images are cropped.  

The first iteration of two points yields six points. The construction of lines and circles from these looks like

![lines and circles](https://github.com/burnsba/constructible/raw/master/image/small-2p,i1-lines.jpg)  

The intersection of these lines is the following points

![203 points](https://github.com/burnsba/constructible/raw/master/image/small-2p,i2-points.jpg)  

It gets quite crowded constructing the next iteration of lines and circles

![lines and circles](https://github.com/burnsba/constructible/raw/master/image/small-2p,i2-lines.jpg)  

Starting with three points in an equilateral triangle yields 28 points. The construction of lines and circles from these looks like

![lines and cirlces](https://github.com/burnsba/constructible/raw/master/image/small-3p,i1-equilateral-lines.jpg)  

The intersections of these lines is the following points

![lines and cirlces](https://github.com/burnsba/constructible/raw/master/image/small-3p,i2-equilateral-points.jpg)  

One last image, the resulting intersction points from the first iteration of a square

![69 points](https://github.com/burnsba/constructible/raw/master/image/small-4p,i1-square-points.jpg)  

-----

# Sequences

    two points:
    
        {0, 0}, {0, 1}
        2,6,203

    three points:

        right triangle
        {0, 0}, {0, 1}, {1, 0}
        3,28,465127
        (run time: ~15 seconds)

        equilateral triangle
        {-1, 0}, {1, 0}, {0, Sqrt[3]}
        3,12,2569
        (run time: ~1 second)

    four points:

        square
        {0, 0}, {0, 1}, {1, 0}, {1, 1}
        4,69,13020491
        (run time: ~530 seconds (~9 minutes) + time to sort ~1,500 MB)

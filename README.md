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
3. construct a line, left circle, and right circle from the pair.
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

## Mathematica

**constructible_points.nb**

Mathematica notebook to apply ruler and compass construction to a set of points. All possible line-line, line-circle, and circle-circle intersections are generated from initial points.

**constructible_plot.nb**

Starting with {0,0} and {1,0} yields six constructible points. These are given at the top of this Mathematica notebook. Then all possible lines and circles are plotted, i.e., the intersections will yield the next iteration of constructible points.

The 203 points from this look like

![203 points symmetric across x and y axes](https://github.com/burnsba/constructible/raw/master/constructible_203.png)  

The lines and circles look like

![plot of constructible lines and circles](https://github.com/burnsba/constructible/raw/master/constructible_lines_circles.png)

-----

# Sequences

    two points:
    
        {0,0},{0,1}
        2,6,203

    three points:

        right triangle
        {0,0},{0,1},{1,0}
        3,28,465793
        (run time: ~15 seconds)

        equilateral triangle
        {-1/2,0},{1/2,0},{0,Sqrt[3]/2}
        3,12,2569
        (run time: ~1 second)

    four points:

        square
        {0,0},{0,1},{1,0},{1,1}
        4,69,13020491
        (run time: ~530 seconds (~9 minutes) + time to sort ~1,500 MB)

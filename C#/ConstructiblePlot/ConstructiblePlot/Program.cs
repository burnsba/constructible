/*
* Program to plot the constructible points.
*
* Copyright (C) 2018 Ben Burns.
*
* MIT License, see /LICENSE for details.
*/
using System;
using System.Collections.Generic;
using System.Configuration;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using NDesk.Options;

namespace ConstructiblePlot
{
    /// <summary>
    /// Helper class, keep the input data in as high as reasonable
    /// precision as possible.
    /// </summary>
    public class Point2d
    {
        public decimal X { get; set; }
        public decimal Y { get; set; }
    }

    /// <summary>
    /// What the program will do.
    /// </summary>
    public enum ProgramMode
    {
        Unknown,
        Points,
        Lines
    }

    class Program
    {
        /// <summary>
        /// Circle inside color.
        /// </summary>
        private static SolidBrush circleInnerBrush = new SolidBrush(Color.FromArgb(142, 185, 255));
        
        /// <summary>
        /// Brush used to draw lines.
        /// </summary>
        private static Pen blackPen = new Pen(Color.Black);

        /// <summary>
        /// Circle border.
        /// </summary>
        private static Pen circleBorderPen = new Pen(Color.Black);

        /// <summary>
        /// Calculates the Euclidean distance between two points.
        /// </summary>
        /// <param name="p1x">First point x.</param>
        /// <param name="p1y">First point y.</param>
        /// <param name="p2x">Second point x.</param>
        /// <param name="p2y">Second point y.</param>
        /// <returns>Distance between the two points.</returns>
        private static double PointDistance(decimal p1x, decimal p1y, decimal p2x, decimal p2y)
        {
            var dx = p2x - p1x;
            var dy = p2y - p1y;
            var sum = dx * dx + dy * dy;

            return Math.Sqrt((double)sum);
        }

        /// <summary>
        /// Draws a point (filled in, with border) on the graphic.
        /// </summary>
        /// <param name="g">Thing to draw on.</param>
        /// <param name="x">Output x value in pixels.</param>
        /// <param name="y">Output y value in pixels.</param>
        /// <param name="radius">Radius of point in pixels.</param>
        private static void DrawPoint(Graphics g, float x, float y, float radius)
        {
            float d = radius * 2;
            g.FillEllipse(circleInnerBrush, x - radius, y - radius, d, d);
            g.DrawEllipse(circleBorderPen, x - radius, y - radius, d, d);
        }

        /// <summary>
        /// Draws a circle (no fill, only border) on the graphic.
        /// </summary>
        /// <param name="g">Thing to draw on.</param>
        /// <param name="x">Output x value in pixels.</param>
        /// <param name="y">Output y value in pixels.</param>
        /// <param name="radius">Radius of point in pixels.</param>
        private static void DrawCircle(Graphics g, float x, float y, float radius)
        {
            float d = radius * 2;
            g.DrawEllipse(circleBorderPen, x - radius, y - radius, d, d);
        }

        /// <summary>
        /// Accepts a line defined by two points and extends it past the borders
        /// of the output image. This is then grawn on the graphic.
        /// </summary>
        /// <param name="context">Context specifying output parameters.</param>
        /// <param name="g">Thing to draw on.</param>
        /// <param name="p1">Coordinates of first point, in pixels of output.</param>
        /// <param name="p2">Coordinates of second point, in pixels of output.</param>
        private static void DrawLine(Context context, Graphics g, PointF p1, PointF p2)
        {
            var dx = p2.X - p1.X;
            var dy = p2.Y - p1.Y;
            
            var newp1 = new PointF(p1.X - 1.0f * (float)context.OutputWidth * dx, p1.Y - 1.0f * (float)context.OutputHeight * dy);
            var newp2 = new PointF(p1.X + (float)context.OutputWidth * dx, p1.Y + (float)context.OutputHeight * dy);

            g.DrawLine(blackPen, newp1, newp2);
        }

        static void Main(string[] args)
        {
            // Load command line arguments or quit
            var context = Context.ParseCommandLineOptions(args);

            // Read data file
            var dataContext = DataContext.LoadDataContext(context);

            // Done with setup, draw output.

            using (var bmp = new System.Drawing.Bitmap(context.OutputWidth, context.OutputHeight))
            {
                // First, prepare to fill background with white.
                var bg = new Rectangle(0, 0, context.OutputWidth, context.OutputHeight);
                var solidWhiteBrush = new SolidBrush(Color.White);

                using (Graphics g = Graphics.FromImage(bmp))
                {
                    // Ok, white out the background here.
                    g.FillRectangle(solidWhiteBrush, bg);

                    // Now it's just lines, circles, and points,
                    int pointsCount = dataContext.Points.Count;

                    for (int p1_index = 0; p1_index < pointsCount; p1_index++)
                    {
                        // Translate the data point to coordinates on the output
                        var datap1 = dataContext.Points[p1_index];
                        decimal view_p1x = (datap1.X - dataContext.ViewOffsetX) * dataContext.ScaleX;
                        decimal view_p1y = (datap1.Y - dataContext.ViewOffsetY * dataContext.AspectRatio) * dataContext.ScaleY;
                        PointF p1f = new PointF((float)view_p1x, (float)view_p1y);
                        
                        if (context.Mode == ProgramMode.Points)
                        {
                            // Points in point mode.
                            DrawPoint(g, p1f.X, p1f.Y, context.PointSize);
                        }
                        else if (context.Mode == ProgramMode.Lines)
                        {
                            if (context.DrawPointsInLineMode)
                            {
                                // Optional point in line mode.
                                DrawPoint(g, p1f.X, p1f.Y, context.PointSize);
                            }

                            for (int p2_index = p1_index + 1; p2_index < pointsCount; p2_index++)
                            {
                                // Translate the data point to coordinates on the output
                                var datap2 = dataContext.Points[p2_index];
                                decimal view_p2x = (datap2.X - dataContext.ViewOffsetX) * dataContext.ScaleX;
                                decimal view_p2y = (datap2.Y - dataContext.ViewOffsetY * dataContext.AspectRatio) * dataContext.ScaleY;
                                PointF p2f = new PointF((float)view_p2x, (float)view_p2y);

                                // Line
                                DrawLine(context, g, p1f, p2f);

                                // Circles
                                float radius = (float)PointDistance(view_p1x, view_p1y, view_p2x, view_p2y);
                                DrawCircle(g, p1f.X, p1f.Y, radius);
                                DrawCircle(g, p2f.X, p2f.Y, radius);
                            }
                        }
                    }
                }

                // Done. Save image.
                using (var fs = new FileStream(context.OutputFile, FileMode.Create))
                {
                    bmp.Save(fs, context.Encoder, context.EncParameters);
                }
            }
        }
    }
}

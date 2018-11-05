/*
* Variables used to transform data.
*
* Copyright (C) 2018 Ben Burns.
*
* MIT License, see /LICENSE for details.
*/
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ConstructiblePlot
{
    /// <summary>
    /// Parses data file, sets associated factors for writing output.
    /// </summary>
    public class DataContext
    {
        /// <summary>
        /// Raw data points as read from file. The points in the file are probably
        /// higher precision than decimal, but it's ok to lose some precision
        /// when drawing this to an image.
        /// </summary>
        public List<Point2d> Points { get; set; }

        /// <summary>
        /// Smallest known raw data x value.
        /// </summary>
        public decimal MinX { get; set; }

        /// <summary>
        /// Smallest known raw data y value.
        /// </summary>
        public decimal MinY { get; set; }

        /// <summary>
        /// Largest known raw data x value.
        /// </summary>
        public decimal MaxX { get; set; }

        /// <summary>
        /// Largest known raw data y value.
        /// </summary>
        public decimal MaxY { get; set; }

        /// <summary>
        /// Difference between the largest and smallest known raw data x values.
        /// </summary>
        public decimal RangeX { get; set; }

        /// <summary>
        /// Difference between the largest and smallest known raw data y values.
        /// </summary>
        public decimal RangeY { get; set; }

        /// <summary>
        /// Ratio between RangeX and RangeY.
        /// </summary>
        public decimal AspectRatio { get; set; }

        /// <summary>
        /// This will be the width of the data to be sent to the output. This will
        /// be larger than RangeX to provide a buffer around the data.
        /// </summary>
        public decimal ViewRangeX { get; set; }

        /// <summary>
        /// This will be the height of the data to be sent to the output. This will
        /// be larger than RangeY to provide a buffer around the data.
        /// </summary>
        public decimal ViewRangeY { get; set; }

        /// <summary>
        /// Shift the data by this much before sending to output.
        /// </summary>
        public decimal ViewOffsetX { get; set; }

        /// <summary>
        /// Shift the data by this much before sending to output.
        /// </summary>
        public decimal ViewOffsetY { get; set; }

        /// <summary>
        /// Scale factor to send the data point (no units) to the image (in pixels).
        /// </summary>
        public decimal ScaleX { get; set; }

        /// <summary>
        /// Scale factor to send the data point (no units) to the image (in pixels). This
        /// is based on the output image width and adjusted by the AspectRatio.
        /// </summary>
        public decimal ScaleY { get; set; }

        public DataContext()
        {
            MinX = decimal.MaxValue;
            MinY = decimal.MaxValue;
            MaxX = decimal.MinValue;
            MaxY = decimal.MinValue;
            Points = new List<Point2d>();
        }

        public static DataContext LoadDataContext(Context context)
        {
            var dc = new DataContext();

            using (var fileStream = File.OpenRead(context.InputFile))
            {
                using (var streamReader = new StreamReader(fileStream, Encoding.UTF8, true, 4096))
                {
                    // Read the file line by line. Ignore any line that starts with 
                    // a semicolon.
                    // Additionally, only accept points that have a valid x and y.
                    // Also, set the known min and known max values.
                    String line;
                    while ((line = streamReader.ReadLine()) != null)
                    {
                        if (line[0] == ';')
                        {
                            continue;
                        }

                        int goodRow = 0;
                        decimal x = 0, y = 0;

                        var splits = line.Split(',');
                        if (splits.Count() == 2)
                        {
                            decimal d;
                            if (decimal.TryParse(splits[0], out d))
                            {
                                if (d < dc.MinX)
                                {
                                    dc.MinX = d;
                                }

                                if (d > dc.MaxX)
                                {
                                    dc.MaxX = d;
                                }

                                x = d;
                                goodRow++;
                            }

                            if (decimal.TryParse(splits[1], out d))
                            {
                                if (d < dc.MinY)
                                {
                                    dc.MinY = d;
                                }

                                if (d > dc.MaxY)
                                {
                                    dc.MaxY = d;
                                }

                                y = d;
                                goodRow++;
                            }

                            if (goodRow == 2)
                            {
                                dc.Points.Add(new Point2d() { X = x, Y = y });
                            }
                            else
                            {
                                Console.WriteLine("Invalid line: " + line);
                            }
                        }
                        else
                        {
                            Console.WriteLine("Invalid line: " + line);
                        }
                    }
                }
            }

            // Now that the min and maxes are known the rest of the
            // values can be set.

            dc.RangeX = dc.MaxX - dc.MinX;
            dc.RangeY = dc.MaxY - dc.MinY;

            dc.AspectRatio = dc.RangeX / dc.RangeY;

            dc.ViewRangeX = 4.0m * dc.RangeX;
            dc.ViewRangeY = 4.0m * dc.RangeY;

            dc.ViewOffsetX = dc.MinX - 1.5m * dc.RangeX;
            dc.ViewOffsetY = dc.MinY - 1.5m * dc.RangeY;

            dc.ScaleX = (decimal)(context.OutputWidth) / dc.ViewRangeX;
            dc.ScaleY = ((decimal)(context.OutputWidth) / dc.ViewRangeY) / dc.AspectRatio;

            return dc;
        }
    }
}

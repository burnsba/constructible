/*
* Application context for program.
*
* Copyright (C) 2018 Ben Burns.
*
* MIT License, see /LICENSE for details.
*/
using System;
using System.Collections.Generic;
using System.Drawing.Imaging;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using NDesk.Options;

namespace ConstructiblePlot
{
    /// <summary>
    /// Application wide settings.
    /// </summary>
    public class Context
    {
        // Default values if not passed via command line.
        private const int DefaultOutputWidth = 2000;
        private const int DefaultOutputHeight = 2000;
        private const int DefaultPointSize = 10;

        /// <summary>
        /// Program mode, either plotting points or drawing lines.
        /// </summary>
        public ProgramMode Mode { get; set; }

        /// <summary>
        /// Input data file.
        /// </summary>
        public string InputFile { get; set; }

        /// <summary>
        /// Output file name. Ending with ".jpg" is optional.
        /// </summary>
        public string OutputFile { get; set; }

        /// <summary>
        /// Output image width, in pixels.
        /// </summary>
        public int OutputWidth { get; set; }

        /// <summary>
        /// Output image height, in pixels.
        /// </summary>
        public int OutputHeight { get; set; }

        /// <summary>
        /// Whether or not to draw intersections in line mode. Default is no.
        /// </summary>
        public bool DrawPointsInLineMode { get; set; }

        /// <summary>
        /// Intersection point size, in pixels.
        /// </summary>
        public int PointSize { get; set; }

        /// <summary>
        /// Output image format.
        /// </summary>
        public ImageCodecInfo Encoder { get; set; }

        /// <summary>
        /// Output image format parameters.
        /// </summary>
        public EncoderParameters EncParameters { get; set; }

        public Context()
        {
            Mode = ProgramMode.Unknown;
            InputFile = string.Empty;
            OutputFile = string.Empty;
            OutputWidth = DefaultOutputWidth;
            OutputHeight = DefaultOutputHeight;
            PointSize = DefaultPointSize;
            DrawPointsInLineMode = false;

            Encoder = GetEncoder(ImageFormat.Jpeg);
            System.Drawing.Imaging.Encoder enc = System.Drawing.Imaging.Encoder.Quality;
            var encParameters = new EncoderParameters(1);
            encParameters.Param[0] = new EncoderParameter(enc, 100L);

            EncParameters = encParameters;
        }

        private static ImageCodecInfo GetEncoder(ImageFormat format)
        {
            ImageCodecInfo[] codecs = ImageCodecInfo.GetImageDecoders();
            foreach (ImageCodecInfo codec in codecs)
            {
                if (codec.FormatID == format.Guid)
                {
                    return codec;
                }
            }
            return null;
        }

        /// <summary>
        /// Parses command line options and returns context for the application.
        /// If something goes wrong here the program exits.
        /// </summary>
        /// <param name="args">Command line args to parse.</param>
        /// <returns>Context for the application.</returns>
        public static Context ParseCommandLineOptions(string[] args)
        {
            // Support modes
            string validModes = "points, lines";

            var c = new Context();

            string outputMode = string.Empty;
            int requiredCount = 0;
            bool showHelp = false;

            // NDesk options
            var commandLineOptions = new OptionSet()
            {
                // required parameter, required value
                {
                "m|mode=",
                    "output {MODE}, one of: " + validModes,
                    (string s) => {
                        outputMode = s;
                        requiredCount++;
                    }
                },
                {
                // optional parameter flag
                "d|draw-points",
                    "If set, will include points in line mode",
                    v => c.DrawPointsInLineMode = true
                },
                {
                // optional parameter, required value
                "point-size=",
                    "{SIZE} of points, in pixels",
                    (int i) => c.PointSize = i
                },
                {
                // required parameter, required value
                "i|input-file=",
                    "data input {FILE}",
                    (string s) => {
                        c.InputFile = s;
                        requiredCount++;
                    }
                },
                {
                // required parameter, required value
                "o|output-file=",
                    "image output {FILE}",
                    (string s) => {
                        c.OutputFile = s;
                        requiredCount++;
                    }
                },
                {
                // optional paramter, required value
                "h|height=",
                    "{HEIGHT} in pixels of output file. Default is " + DefaultOutputHeight.ToString(),
                    (int i) => c.OutputHeight = i
                },
                {
                // optional paramter, required value
                "w|width=",
                    "{WIDTH} in pixels of output file. Height is scaled according to the aspect ratio of the range of the data. Requires width > height. Default is " + DefaultOutputWidth.ToString(),
                    (int i) => c.OutputWidth = i
                },
                {
                "?|help",  "show this message and exit",
                    v => showHelp = v != null
                },
            };

            List<string> extra;
            try
            {
                extra = commandLineOptions.Parse(args);
            }
            catch (OptionException e)
            {
                Console.WriteLine(e.Message);
                commandLineOptions.WriteOptionDescriptions(Console.Out);

                Environment.Exit(1);
            }

            if (showHelp)
            {
                commandLineOptions.WriteOptionDescriptions(Console.Out);
                Environment.Exit(1);
            }

            if (requiredCount != 3)
            {
                Console.WriteLine("The following arguments are required: ");
                Console.WriteLine("mode, input-file, output-file");

                commandLineOptions.WriteOptionDescriptions(Console.Out);
                Environment.Exit(1); ;
            }

            if (string.Compare(outputMode, "points", true) == 0)
            {
                c.Mode = ProgramMode.Points;
            }
            else if (string.Compare(outputMode, "lines", true) == 0)
            {
                c.Mode = ProgramMode.Lines;
            }
            else
            {
                Console.WriteLine("Invalid mode. Available options are:");
                Console.WriteLine(validModes);
                Environment.Exit(1);
            }

            if (c.OutputWidth <= 0)
            {
                Console.WriteLine("Requires output width > 0.");
                Environment.Exit(1);
            }

            if (c.OutputHeight <= 0)
            {
                Console.WriteLine("Requires output height > 0.");
                Environment.Exit(1);
            }

            if (c.PointSize <= 0)
            {
                Console.WriteLine("Requires point size > 0.");
                Environment.Exit(1);
            }

            if (!(c.OutputFile.ToLower().EndsWith(".jpg")))
            {
                c.OutputFile = c.OutputFile + ".jpg";
            }

            return c;
        }
    }
}

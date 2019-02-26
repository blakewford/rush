using System;
using System.Drawing;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Collections;
using System.Collections.Generic;
using System.Web.Script.Serialization;

public class Monitor
{
    private const int IMAGE_SIZE = 640;
    private const int CHART_SIZE = 512;
    private const int MINIMUM_RANGE = 32;
    private const int BORDER_SIZE = IMAGE_SIZE/20;
    private static Color AXIS_COLOR = Color.LightGray;
    private static Mutex mMutex = new Mutex();
    private static ArrayList mAccumulatedFpsValues = new ArrayList();
    private static ArrayList mAccumulatedVertValues = new ArrayList();

    private struct DebugInfo
    {
        public int fps;
        public int vertices;
    }

    private static void InitializeBitmap(ref Bitmap bitmap, Color background, Rectangle rect)
    {
        System.Drawing.Imaging.BitmapData data = bitmap.LockBits(rect, System.Drawing.Imaging.ImageLockMode.WriteOnly, bitmap.PixelFormat);

        int size  = Math.Abs(data.Stride) * rect.Width;

        IntPtr p = data.Scan0;
        byte[] raw = new byte[size];

//        Console.WriteLine((data.Stride/data.Width) == 4);

        for(int i = 0; i < size; i+=4)
        {
            raw[i+0] = background.B;
            raw[i+1] = background.G;
            raw[i+2] = background.R;
            raw[i+3] = background.A;
        }

        System.Runtime.InteropServices.Marshal.Copy(raw, 0, p, size);
        bitmap.UnlockBits(data);
    }

    public static void MonitorLoop(IPAddress address)
    {
        try
        {
            Socket socket = new Socket(address.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
            socket.Bind(new IPEndPoint(address, 8080));
            socket.Listen(1);

            Socket client = socket.Accept();

            byte[] data = new Byte[64];
            var deserializer = new JavaScriptSerializer();
            while(true)
            {
                Console.WriteLine("Ready...");

                UInt16 size = 0;
                client.Receive(data, 2, SocketFlags.None);
                size = BitConverter.ToUInt16(data, 0);

                if(size > 0)
                {
                    client.Receive(data, size, SocketFlags.None);

                    string json = Encoding.UTF8.GetString(data).Substring(0, size);

                    var debugInfo = deserializer.Deserialize<DebugInfo>(json);
                    Console.WriteLine("{0}: {1} {2}", DateTime.Now.ToString(), debugInfo.fps, debugInfo.vertices);

                    if(debugInfo.fps < CHART_SIZE)
                    {
                        mMutex.WaitOne();
                        mAccumulatedFpsValues.Add(debugInfo.fps);
                        mAccumulatedVertValues.Add(debugInfo.vertices);
                        mMutex.ReleaseMutex();
                    }
                }
            }
//            client.Close();
        }
        catch(Exception)
        {
//            Console.WriteLine(e.ToString());
        }
    }

    private static void BuildChart(ref Bitmap bitmap, string title, Brush brush)
    {
        InitializeBitmap(ref bitmap, AXIS_COLOR, new Rectangle(0, 0, IMAGE_SIZE, IMAGE_SIZE));

        Graphics g = Graphics.FromImage(bitmap);
        RectangleF titleRect = new RectangleF(BORDER_SIZE, IMAGE_SIZE-(BORDER_SIZE*2), BORDER_SIZE*4, IMAGE_SIZE-(BORDER_SIZE*2));
        RectangleF unitRect  = new RectangleF(IMAGE_SIZE/2, IMAGE_SIZE-(BORDER_SIZE*2), (IMAGE_SIZE/2) + BORDER_SIZE, IMAGE_SIZE-(BORDER_SIZE*2));
        RectangleF zeroRect  = new RectangleF(BORDER_SIZE*3, CHART_SIZE-BORDER_SIZE, BORDER_SIZE*4, CHART_SIZE-BORDER_SIZE);
        g.DrawString(title, new Font("Helvetica",16), brush, titleRect);
        g.DrawString("Time (s)", new Font("Helvetica",16), Brushes.Black, unitRect);
        g.DrawString("0", new Font("Helvetica",16), brush, zeroRect);
        g.Flush();
    }

    private static double PolishData(ArrayList accumulated, ref ArrayList data)
    {
        int average = 0;
        foreach(int value in accumulated)
        {
            average += value;
        }
        average /= accumulated.Count;
        accumulated.Clear();
        mMutex.ReleaseMutex();
        int count = CHART_SIZE/32; //History in seconds
        while(count-- > 0)
            data.Add(average);
        while(data.Count > CHART_SIZE)
            data.RemoveAt(0);

        double max = int.MinValue;
        foreach(int value in data)
        {
            if(value > max)
                max = value;
        }

        double scale = 1.0f;
        int range = Math.Max(MINIMUM_RANGE, Convert.ToInt32(max));
        if(max > CHART_SIZE)
        {
            scale = 1/Math.Ceiling(max/CHART_SIZE);
        }
        else
        {
            for(; (CHART_SIZE%range) != 0; range++)
                ;

            scale = CHART_SIZE/range;
        }

        return scale;
//        return 1; //scale
    }

    private static void ChartThread(ref Bitmap bitmap, string title, Brush brush, ArrayList accumulated, ArrayList data)
    {
        double previousScale = 1.0f;
        Rectangle rect = new Rectangle((IMAGE_SIZE-CHART_SIZE), 0, CHART_SIZE, CHART_SIZE);
        while(true)
        {
            if(accumulated.Count == 0) continue;

            InitializeBitmap(ref bitmap, Color.White, rect);
            mMutex.WaitOne();
            double scale = PolishData(accumulated, ref data);
            if(scale != previousScale)
            {
                BuildChart(ref bitmap, title, brush);
                Graphics g = Graphics.FromImage(bitmap);
                RectangleF maxRect = new RectangleF(Convert.ToSingle(BORDER_SIZE*1.5), 0, Convert.ToSingle(BORDER_SIZE*3.5), 0);
                g.DrawString((CHART_SIZE/scale).ToString(), new Font("Helvetica",16), brush, maxRect);
                g.Flush();
                previousScale = scale;
            }
            int current = 0;
            int start = (IMAGE_SIZE-CHART_SIZE) + (CHART_SIZE - data.Count);
            foreach(int value in data)
            {
                int scaledValue = Convert.ToInt32(value*scale);
                int xValue = start + current;
                int yValue = (CHART_SIZE - scaledValue - 2);
                int weight = 5;
                while(weight-- > 0)
                {
                    if(yValue > 0 && yValue < CHART_SIZE)
                        bitmap.SetPixel(xValue, yValue++, new Pen(brush).Color);
                }
                current++;
            }
            bitmap.Save(title + ".png");
            Thread.Sleep(1000);
        }
    }

    public static int Main(String[] args)
    {
        ArrayList fpsData = new ArrayList();
        ArrayList vertData = new ArrayList();
        Bitmap fpsBitmap = new Bitmap(IMAGE_SIZE, IMAGE_SIZE);
        Bitmap vertBitmap = new Bitmap(IMAGE_SIZE, IMAGE_SIZE);
        BuildChart(ref fpsBitmap, "FPS", Brushes.BlueViolet);
        BuildChart(ref vertBitmap, "Verticies", Brushes.BlueViolet);

        new Thread(() => ChartThread(ref fpsBitmap, "FPS", Brushes.BlueViolet, mAccumulatedFpsValues, fpsData)).Start();
        new Thread(() => ChartThread(ref vertBitmap, "Verticies", Brushes.CornflowerBlue, mAccumulatedVertValues, vertData)).Start();

        foreach(IPAddress address in Dns.GetHostEntry(Dns.GetHostName()).AddressList)
        {
            if(address.AddressFamily == AddressFamily.InterNetwork)
            {
                MonitorLoop(address);
            }
        }

        return 0;
    }
}

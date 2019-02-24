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
        InitializeBitmap(ref bitmap, Color.LightGray, new Rectangle(0, 0, IMAGE_SIZE, IMAGE_SIZE));

        int borderSize = IMAGE_SIZE/20;

        Graphics g = Graphics.FromImage(bitmap);
        RectangleF titleRect = new RectangleF(borderSize, IMAGE_SIZE-(borderSize*2), borderSize*2, IMAGE_SIZE-(borderSize*2));
        RectangleF unitRect  = new RectangleF(IMAGE_SIZE/2, IMAGE_SIZE-(borderSize*2), (IMAGE_SIZE/2) + borderSize, IMAGE_SIZE-(borderSize*2));
        g.DrawString(title, new Font("Helvetica",16), brush, titleRect);
        g.DrawString("Time (s)", new Font("Helvetica",16), Brushes.Black, unitRect);
        g.Flush();
    }

    private static double PolishData(ref ArrayList data)
    {
        int average = 0;
        foreach(int value in mAccumulatedFpsValues)
        {
            average += value;
        }
        average /= mAccumulatedFpsValues.Count;
        mAccumulatedFpsValues.Clear();
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

    public static int Main(String[] args)
    {
        ArrayList fpsData = new ArrayList();
        ArrayList vertData = new ArrayList();
        Bitmap bitmap = new Bitmap(IMAGE_SIZE, IMAGE_SIZE);
        BuildChart(ref bitmap, "FPS", Brushes.BlueViolet);

        Rectangle rect = new Rectangle((IMAGE_SIZE-CHART_SIZE), 0, CHART_SIZE, CHART_SIZE);
        new Thread(() => {
            while(true)
            {
                if(mAccumulatedFpsValues.Count == 0) continue;

                InitializeBitmap(ref bitmap, Color.White, rect);

                mMutex.WaitOne();
                double scale = PolishData(ref fpsData);
                int current = 0;
                int start = (IMAGE_SIZE-CHART_SIZE) + (CHART_SIZE - fpsData.Count);
                foreach(int value in fpsData)
                {
                    int scaledValue = Convert.ToInt32(value*scale);

                    int xValue = start + current;
                    int yValue = (CHART_SIZE - scaledValue - 2);
                    int weight = 5;
                    while(weight-- > 0)
                    {
                        if(yValue > 0 && yValue < CHART_SIZE)
                            bitmap.SetPixel(xValue, yValue++, Color.BlueViolet);
                    }
                    current++;
                }
                bitmap.Save("Chart.png");
                Thread.Sleep(1000);
            }
        }).Start();

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

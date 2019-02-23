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
    private const int IMAGE_SIZE = 512;
    private static Mutex mMutex = new Mutex();
    private static ArrayList mAccumulatedValues = new ArrayList();

    private struct DebugInfo
    {
        public int fps;
        public int vertices;
    }

    private static void InitializeBitmap(ref Bitmap bitmap)
    {
        Color background = Color.White;
        Rectangle rect = new Rectangle(0, 0, bitmap.Width, bitmap.Height);
        System.Drawing.Imaging.BitmapData data = bitmap.LockBits(rect, System.Drawing.Imaging.ImageLockMode.WriteOnly, bitmap.PixelFormat);

        int size  = Math.Abs(data.Stride) * bitmap.Height;

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

                    if(debugInfo.fps < IMAGE_SIZE)
                    {
                        mMutex.WaitOne();
                        mAccumulatedValues.Add(debugInfo.fps);
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

    public static int Main(String[] args)
    {
        ArrayList data = new ArrayList();
        Bitmap bitmap = new Bitmap(IMAGE_SIZE, IMAGE_SIZE);
        new Thread(() => {
            while(true)
            {
                if(mAccumulatedValues.Count == 0) continue;

                InitializeBitmap(ref bitmap);

                mMutex.WaitOne();
                int average = 0;
                foreach(int value in mAccumulatedValues)
                {
                    average += value;
                }
                average /= mAccumulatedValues.Count;
                mAccumulatedValues.Clear();
                mMutex.ReleaseMutex();

                int count = IMAGE_SIZE/32; //History in seconds
                while(count-- > 0)
                    data.Add(average);
                while(data.Count > IMAGE_SIZE)
                    data.RemoveAt(0);

                int current = 0;
                int start = IMAGE_SIZE - data.Count;
                foreach(int value in data)
                {
                    bitmap.SetPixel(start + current, IMAGE_SIZE - value, Color.Black);
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

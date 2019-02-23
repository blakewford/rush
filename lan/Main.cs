using System;
using System.Drawing;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

public class Monitor
{
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
            while(true)
            {
                Console.WriteLine("Ready...");

                UInt16 size = 0;
                client.Receive(data, 2, SocketFlags.None);
                size = BitConverter.ToUInt16(data, 0);

                if(size > 0)
                {
                    client.Receive(data, BitConverter.ToUInt16(data, 0), SocketFlags.None);
                    Console.WriteLine("{0}: {1}", DateTime.Now.ToString(), Encoding.ASCII.GetString(data));
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
        Bitmap bitmap = new Bitmap(512, 512);
        new Thread(() => {
            while(true)
            {
                InitializeBitmap(ref bitmap);
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

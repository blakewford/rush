using System;
using System.Net;
using System.Net.Sockets;
using System.Text;

public class Monitor
{
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

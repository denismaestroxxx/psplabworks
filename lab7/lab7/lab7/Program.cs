using lab7;
using System.Net;

class Server
{
    public static void Main()
    {
        var server = new CurrencyServer(IPAddress.Loopback, 5015);
        server.Start();
    }
}
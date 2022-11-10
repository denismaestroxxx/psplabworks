namespace Server
{
    internal class Program
    {
        static void Main(string[] args)
        {
            var httpServer = new HttpServer();
            httpServer.Start();
        }
    }
}
namespace Server
{
    internal class Program
    {
        static void Main(string[] args)
        {
            var server = new SortServer("127.0.0.1", 5001);
            server.Run();
        }
    }
}
using Newtonsoft.Json;
using System.Diagnostics;

namespace Client
{
    internal class Program
    {
        static void Main(string[] args)
        {
            var client = new SortClient("127.0.0.1", 5001); Random rnd = new Random();
            int size = 500000;
            int[] array = new int[size];
            for (var i = 0; i < size; i++)
            {
                array[i] = rnd.Next(1, 100000);
            }
            var sw = new Stopwatch();
            sw.Start();
            var sortedArray = client.Process(array);
            var elapsedMs = sw.ElapsedMilliseconds;
            Console.WriteLine($"Sorted array:\n{JsonConvert.SerializeObject(sortedArray)}\nElapsed ms: {elapsedMs}");
            Console.ReadKey();
        }
    }
}
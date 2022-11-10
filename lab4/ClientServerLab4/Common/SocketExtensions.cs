using System.Net.Sockets;

namespace Common
{
    public static class SocketExtensions
    {
        public static void SendOneDimensionalArray(this Socket socket, int[] array)
        {
            var buffer = new byte[array.Length * sizeof(int)];
            Buffer.BlockCopy(array, 0, buffer, 0, buffer.Length);
            socket.Send(buffer);
        }

        public static int[] ReciveOneDimensionalArray(this Socket socket)
        {
            var message = new List<byte>();
            var buffer = new byte[Constants.BufferSize];
            var offSet = 0;
            do
            {
                var byteCount = socket.Receive(buffer, buffer.Length, 0);
                message.AddRange(buffer[..byteCount]);
                offSet += byteCount;
            }
            while (socket.Available > 0);

            var count = offSet / sizeof(int);
            var array = new int[count];
            Buffer.BlockCopy(message.ToArray(), 0, array, 0, offSet);

            return array;
        }
    }
}
using System.Net.Sockets;
using System.Net;
using Common;

namespace Client
{
    public class SortClient
    {
        private readonly AddressFamily _addressFamily;
        private readonly ProtocolType _protocolType;
        private readonly IPEndPoint _ipPoint;
        private Socket _socket;

        public SortClient(string serverAddress,
            int port,
            AddressFamily addressFamily = AddressFamily.InterNetwork,
            ProtocolType protocolType = ProtocolType.Tcp)
        {
            _addressFamily = addressFamily;
            _protocolType = protocolType;
            _ipPoint = new IPEndPoint(IPAddress.Parse(serverAddress), port);
            _socket = new Socket(addressFamily, SocketType.Stream, protocolType);
        }

        public int[] Process(int[] array)
        {
            try
            {
                _socket.Connect(_ipPoint);

                _socket.SendOneDimensionalArray(array);

                var sortedArray = _socket.ReciveOneDimensionalArray();

                ResetSocket();

                return sortedArray;
            }
            catch (Exception e)
            {
                Console.WriteLine(e.Message);
            }

            return Array.Empty<int>();
        }

        private void ResetSocket()
        {
            _socket.Shutdown(SocketShutdown.Both);
            _socket.Close();
            _socket = new Socket(_addressFamily, SocketType.Stream, _protocolType);
        }
    }
}

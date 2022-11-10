using System.Net.Sockets;
using System.Net;
using Common;
using Server.Sorting;

namespace Server
{
    public class SortServer
    {
        private readonly AddressFamily _addressFamily;
        private readonly ProtocolType _protocolType;

        private readonly IPEndPoint _ipPoint;
        private readonly Socket _listenSocket;
        private bool _isRunning;

        public SortServer(string address,
            int port,
            AddressFamily addressFamily = AddressFamily.InterNetwork,
            ProtocolType protocolType = ProtocolType.Tcp)
        {
            _addressFamily = addressFamily;
            _protocolType = protocolType;

            _ipPoint = new IPEndPoint(IPAddress.Parse(address), port);
            _listenSocket = new Socket(addressFamily, SocketType.Stream, protocolType);

            _isRunning = false;
            _listenSocket.Bind(_ipPoint);
        }

        public void Run()
        {
            _listenSocket.Listen(10);
            _isRunning = true;
            new Thread(StartClientsProcessing).Start();

            Console.WriteLine($"Server '{_ipPoint.Address}:{_ipPoint.Port}' started. Waiting for clients...\n");
        }

        public void Stop()
        {
            _listenSocket.Shutdown(SocketShutdown.Receive);
            _isRunning = false;
        }

        private void StartClientsProcessing()
        {
            while (_isRunning)
            {
                Socket clientSocket = _listenSocket.Accept();
                ThreadPool.QueueUserWorkItem(
                state =>
                {
                    ProcessClient(state as Socket);
                }, clientSocket);
            }

            _listenSocket.Shutdown(SocketShutdown.Send);
        }

        private void ProcessClient(Socket clientSocket)
        {
            Console.WriteLine("Started processing client");
            var array = clientSocket.ReciveOneDimensionalArray();
            var sorter = new MergeSortSolver();
            sorter.Sort(array);
            clientSocket.SendOneDimensionalArray(array);
            clientSocket.Shutdown(SocketShutdown.Both);
            clientSocket.Close();
            Console.WriteLine("Processed client with result");
        }
    }
}

using MicroServer.Net.Sntp;
using System;
using System.Net;
using System.Net.Sockets;
using uITron3;

namespace MicroServer.Net.Sockets
{
	public class Socket : IDisposable
	{
		private readonly AddressFamily interNetwork;
		private readonly SocketType stream;
		private readonly ProtocolType protocol;
		lwip lwip;

		public Socket(AddressFamily interNetwork, SocketType stream, ProtocolType protocol)
		{
			this.interNetwork = interNetwork;
			this.stream = stream;
			this.protocol = protocol;
		}

		public IPEndPoint LocalEndPoint { get; internal set; }
		public int ReceiveTimeout { get; internal set; }
		public int SendTimeout { get; internal set; }
		public int Available { get; internal set; }

		public void Close()
		{
			throw new NotImplementedException();
		}

		internal int SendTo(byte[] message, EndPoint endPoint)
		{
			throw new NotImplementedException();
		}

		internal void ReceiveFrom(SntpMessage request, ref EndPoint senderRemote)
		{
			throw new NotImplementedException();
		}

		internal int Send(byte[] sendBuffer, int sentBytes, SocketFlags none)
		{
			throw new NotImplementedException();
		}

		internal int SendTo(byte[] message, int size, SocketFlags socketFlags, EndPoint endPoint)
		{
			throw new NotImplementedException();
		}

		internal int SendTo(byte[] message, SocketFlags socketFlags, EndPoint endPoint)
		{
			throw new NotImplementedException();
		}

		internal void SetSocketOption(object socket, object reuseAddress, int v)
		{
			throw new NotImplementedException();
		}

		internal void Bind(IPEndPoint iPEndPoint)
		{
			throw new NotImplementedException();
		}

		internal void Listen(int listenBacklog)
		{
			throw new NotImplementedException();
		}

		internal int SendTo(byte[] message, int size, int offset, SocketFlags socketFlags, EndPoint endPoint)
		{
			throw new NotImplementedException();
		}

		internal int Send(byte[] message)
		{
			throw new NotImplementedException();
		}

		internal int Send(byte[] message, SocketFlags socketFlags)
		{
			throw new NotImplementedException();
		}

		internal Socket Accept()
		{
			throw new NotImplementedException();
		}

		internal int Send(byte[] message, int offSet, int size, SocketFlags socketFlags)
		{
			throw new NotImplementedException();
		}

		internal void Connect(IPEndPoint remoteEndPoint)
		{
			throw new NotImplementedException();
		}

		internal void Receive(byte[] messageBuffer)
		{
			throw new NotImplementedException();
		}

		public void Dispose()
		{
			throw new NotImplementedException();
		}

		internal bool Poll(int v, SelectMode selectRead)
		{
			throw new NotImplementedException();
		}

		internal int ReceiveFrom(byte[] buffer, SocketFlags none, ref EndPoint remoteEndPoint)
		{
			throw new NotImplementedException();
		}
	}
}

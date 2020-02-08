using MicroServer.Logging;
using System;
using System.Net;
using System.Net.Sockets;
using uITron3;

namespace MicroServer.Net.Sockets
{
	/// <summary>
	/// A class that listen for UDP packets from remote clients.
	/// </summary>
	public class UdpListener : SocketListener
	{
		public UdpListener(Itron itron)
			: base(itron)
		{
		}

		#region Methods

		/// <summary>
		///  Starts the service listener if it is in a stopped state.
		/// </summary>
		/// <param name="servicePort">The port used to listen on.</param>
		/// <param name="allowBroadcast">Allows the listener to accept broadcast packets.</param>
		public bool Start(Itron itron, int servicePort, bool allowBroadcast)
		{
			_itron = itron;
			if (servicePort > IPEndPoint.MaxPort || servicePort < IPEndPoint.MinPort)
				throw new ArgumentOutOfRangeException("port", "Port must be less then " + IPEndPoint.MaxPort + " and more then " + IPEndPoint.MinPort);

			if (IsActive)
				throw new InvalidOperationException("Udp listener is already active and must be stopped before starting");

			if (InterfaceAddress == null) {
				Logger.WriteDebug(this, "Unable to set interface address");
				throw new InvalidOperationException("Unable to set interface address");
			}

			try {
				_socket = new Socket(_itron, AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
				_socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, 1);

				if (allowBroadcast)
					_socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.Broadcast, 1);

				_socket.Bind(new IPEndPoint(InterfaceAddress, servicePort));
				_socket.ReceiveTimeout = ReceiveTimeout;
				_socket.SendTimeout = SendTimeout;

				IsActive = true;
				T_CTSK ctsk = new T_CTSK();
				ctsk.task = StartUdpListening;
				_itron.cre_tsk(ID.ID_AUTO, ref ctsk, out _thread);
				_itron.sta_tsk(_thread, 0);

				Logger.WriteInfo(this, "Started listening for requests on " + _socket.LocalEndPoint.ToString());

			}
			catch (Exception ex) {
				Logger.WriteError(this, "Starting listener Failed: " + ex.Message.ToString(), ex);
				return false;
			}
			return true;
		}

		/// <summary>
		///  Listener thread
		/// </summary>
		private void StartUdpListening(object exinf)
		{
			while (IsActive) {
				try {
					OnSocket(_socket);
				}
				catch (Exception ex) {
					OnClientDisconnected(_socket, ex);
				}
			}
			//_socket.Close();
		}

		#endregion Methods
	}
}

using MicroServer.Logging;
using System;
using System.Net;
using System.Net.Sockets;
using uITron3;

namespace MicroServer.Net.Sockets
{
	/// <summary>
	/// A class that listen for TCP packets from remote clients.
	/// </summary>
	public class TcpListener : SocketListener
	{
		public TcpListener(Itron itron)
			: base(itron)
		{
		}

		#region Methods

		/// <summary>
		///  Starts the service listener if it is in a stopped state.
		/// </summary>
		/// <param name="servicePort">The port used to listen on.</param>
		public bool Start(Itron itron, int servicePort)
		{
			_itron = itron;
			if (servicePort > IPEndPoint.MaxPort || servicePort < IPEndPoint.MinPort)
				throw new ArgumentOutOfRangeException("port", "Port must be less then " + IPEndPoint.MaxPort + " and more then " + IPEndPoint.MinPort);

			if (IsActive)
				throw new InvalidOperationException("Tcp listener is already active and must be stopped before starting");

			if (InterfaceAddress == null) {
				Logger.WriteDebug(this, "Unabled to set interface address");
				throw new InvalidOperationException("Unabled to set interface address");
			}

			try {
				_socket = new Socket(itron, AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
				_socket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, 1);
				_socket.Bind(new IPEndPoint(InterfaceAddress, servicePort));
				_socket.ReceiveTimeout = ReceiveTimeout;
				_socket.SendTimeout = SendTimeout;
				_socket.Listen(ListenBacklog);

				IsActive = true;
				T_CTSK ctsk = new T_CTSK();
				ctsk.task = StartTcpListening;
				ER ret = _itron.cre_tsk(ID.ID_AUTO, ref ctsk, out _thread);
				ret = _itron.sta_tsk(_thread, 0);

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
		private void StartTcpListening(object exinf)
		{
			while (IsActive) {
				using (var _tcpSocket = _socket.Accept()) {
					try {
						OnSocket(_tcpSocket);
					}
					catch (Exception ex) {
						OnClientDisconnected(_tcpSocket, ex);
					}
				}
			}
			_socket.Close();
		}

		#endregion Methods
	}
}

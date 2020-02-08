using MicroServer.Net.Sntp;
using System;
using System.Net;
using System.Net.Sockets;
using uITron3;
using Socket = MicroServer.Net.Sockets.Socket;

namespace TrimbleMonitorNtpPlus
{
	internal class NtpServer
	{
		private readonly ID _packetProcessing;
		private readonly Socket _socket;
		private Itron _itron;

		public NtpServer(Itron itron)
		{
			_itron = itron;
			_socket = new Socket(itron, AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
			_socket.Bind(new IPEndPoint(IPAddress.Any, 123));
			T_CTSK ctsk = new T_CTSK();
			ctsk.task = WorkerThread;
			ER er = _itron.cre_tsk(ID.ID_AUTO, ref ctsk, out _packetProcessing);
		}

		/// <summary>
		///     Worker thread for processing received TSIP packets
		/// </summary>
		private void WorkerThread(object exinf)
		{
			while (true) {
				var sender = new IPEndPoint(IPAddress.Any, 0);
				EndPoint senderRemote = sender;

				var request = new SntpMessage();

				try {
					_socket.ReceiveFrom(request.ToArray(), ref senderRemote);
				}
				catch (Exception) {
					// Swallow the exception
				}

				try {
					if (request.Mode == Mode.Client) {
						var response = new SntpMessage {
							LeapIndicator = LeapIndicator.NoWarning,
							VersionNumber = request.VersionNumber,
							Mode = Mode.Server,
							Stratum = Stratum.Primary,
							PollInterval = 8,
							Precision = 0.012345,
							ReferenceIdentifier = ReferenceIdentifier.GPS,
							ReferenceDateTime = DateTime.UtcNow.AddMilliseconds(-1000),
							ReceiveDateTime = DateTime.UtcNow,
							TransmitDateTime = DateTime.UtcNow
						};

						//response.RootDelay = 0;
						//response.RootDispersion = 0;

						// Copy the Transmit Date to the Originate Date
						response.OriginateDateTime = request.TransmitDateTime;

						// Sent return data
						var sent = _socket.SendTo(response.ToArray(), senderRemote);
					}
				}
				catch {
					// Swallow the exception
				}
			}
		}

		public void Start()
		{
			_itron.sta_tsk(_packetProcessing, 0);
		}

		public void Stop()
		{
			_itron.sus_tsk(_packetProcessing);
		}
	}
}
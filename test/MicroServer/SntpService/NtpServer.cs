using MicroServer.Net.Sntp;
using System;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using Socket = MicroServer.Net.Sockets.Socket;

namespace TrimbleMonitorNtpPlus
{
	internal class NtpServer
	{
		private readonly Thread _packetProcessing;
		private readonly Socket _socket;

		public NtpServer()
		{
			_socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
			_socket.Bind(new IPEndPoint(IPAddress.Any, 123));
			_packetProcessing = new Thread(WorkerThread);
		}

		/// <summary>
		///     Worker thread for processing received TSIP packets
		/// </summary>
		private void WorkerThread()
		{
			while (true) {
				var sender = new IPEndPoint(IPAddress.Any, 0);
				EndPoint senderRemote = sender;

				var request = new SntpMessage();

				try {
					_socket.ReceiveFrom(request, ref senderRemote);
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
			_packetProcessing.Start();
		}

		public void Stop()
		{
			_packetProcessing.Suspend();
		}
	}
}
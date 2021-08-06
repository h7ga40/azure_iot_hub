using MicroServer.Logging;
using MicroServer.Net.Sockets;
using System;

namespace MicroServer.Net.Sntp
{
	public class SntpMessageEventArgs : EventArgs
	{
		/// <summary>
		///     Channel for the connected client.
		/// </summary>
		public SocketChannel Channel { get; private set; }

		/// <summary>
		///     Buffer for the connected client.
		/// </summary>
		public SocketBuffer ChannelBuffer { get; private set; }

		/// <summary>
		///     Requested message for the connected client.
		/// </summary>
		public SntpMessage RequestMessage { get; private set; }

		/// <summary>
		///     Response message for the connected client.
		/// </summary>
		public SntpMessage ResponseMessage { get; set; }

		/// <summary>
		///     Initializes a new instance of the <see cref="SntpMessageEventArgs" /> class.
		/// </summary>
		/// <param name="channel">Socket channel request is recived on.</param>
		/// <param name="data">Raw data received from socket.</param>
		public SntpMessageEventArgs(SocketChannel channel, SocketBuffer data)
		{
			if (channel == null) throw new ArgumentNullException("channel");
			Channel = channel;

			if (data == null) throw new ArgumentNullException("data");
			ChannelBuffer = data;

			try {
				// Parse the sntp message
				RequestMessage = new SntpMessage(data.Buffer);

				// log that the packet was successfully parsed
				Logger.WriteDebug(this, "PACKET with channel id " + Channel.ChannelId.ToString() +
						" successfully parsed from client endpoint " + Channel.RemoteEndpoint.ToString());
				Logger.WriteDebug(RequestMessage.ToString());

			}
			catch (Exception ex) {
				Logger.WriteError(this, "Error parsing message:" + ex.Message.ToString(), ex);
				// disconnect ?
				return;
			}
		}
	}
}
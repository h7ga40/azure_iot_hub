using MicroServer.Extensions;
using MicroServer.Logging;
using MicroServer.Net.Sockets;
using System;
using System.Net;
using System.Net.Sockets;
using Socket = MicroServer.Net.Sockets.Socket;
using ThreadPool = MicroServer.Threading.ThreadPool;
using WaitCallback = MicroServer.Threading.WaitCallback;

namespace MicroServer.Net.Dns
{
	/// <summary>
	/// A class that serves DNS packet requests from remote clients.
	/// </summary>
	public class DnsService : SocketService
	{
		#region Private Properties

		private UdpListener _listener;
		private ZoneCollection _zoneFile = new ZoneCollection();

		private IPAddress _primaryServer = Constants.DNS_DEFAULT_PRIMARY_SERVER;
		private IPAddress _alternateServer = Constants.DNS_DEFAULT_ALTERNATE_SERVER;

		private IPAddress _interfaceAddress = IPAddress.Any;
		private readonly int _receiveTimeout = Constants.DNS_RECEIVE_TIMEOUT;
		private readonly int _sendTimeout = Constants.DNS_SEND_TIMEOUT;
		private string _dnsSuffix;
		private string _serverName;
		private bool _isProxy = true;

		#endregion Private Properties

		#region Public Properties

		/// <summary>
		/// Interface IP address.
		/// </summary>
		public new IPAddress InterfaceAddress {
			get => _interfaceAddress;
			set => _interfaceAddress = value;
		}

		/// <summary>
		/// The DNS name without the hostname part.
		/// </summary>
		public string DnsSuffix {
			get => _dnsSuffix;
			set => _dnsSuffix = value;
		}

		/// <summary>
		/// The DNS hostname part.
		/// </summary>
		public string ServerName {
			get => _serverName;
			set => _serverName = value;
		}

		/// <summary>
		/// The DNS hostname part.
		/// </summary>
		public ZoneCollection ZoneFile {
			get => _zoneFile;
			set => _zoneFile = value;
		}

		/// <summary>
		/// Capture domain name resolution requests and route them to other DNS servers
		/// </summary>
		public bool IsProxy {
			get => _isProxy;
			set => _isProxy = value;
		}

		/// <summary>
		/// Primary SNTP Server address.
		/// </summary>
		public string PrimaryServer {
			get => _primaryServer.ToString();
			set => _primaryServer = IPAddress.Parse(value);
		}

		/// <summary>
		///  Alternate SNTP server is used if communication with primary server failed.
		/// </summary>
		public string AlternateServer {
			get => _alternateServer.ToString();
			set => _alternateServer = IPAddress.Parse(value);
		}

		#endregion Public Properties

		#region Methods

		/// <summary>
		///  Starts the service listener if in a stopped state.
		/// </summary>
		public override bool Start()
		{
			_listener = new UdpListener {
				InterfaceAddress = _interfaceAddress,
				BufferSize = Constants.DNS_MAX_MESSAGE_SIZE,
				ReceiveTimeout = Constants.DNS_RECEIVE_TIMEOUT,
				SendTimeout = Constants.DNS_SEND_TIMEOUT
			};
			_listener.ClientConnected += OnClientConnect;
			_listener.ClientDisconnected += OnClientDisconnect;

			return _listener.Start(Constants.DNS_SERVICE_PORT, false);
		}

		/// <summary>
		///  Stops the service listener if in started state.
		/// </summary>
		public override bool Stop()
		{
			try {
				return _listener.Stop();
			}

			catch (Exception ex) {
				Logger.WriteError(this, "Stopping Service failed: " + ex.Message.ToString(), ex);
			}

			return false;
		}

		/// <summary>
		///  Remote client connects and makes a request.
		/// </summary>
		private void OnClientConnect(object sender, ClientConnectedEventArgs args)
		{

			var channelBuffer = args.ChannelBuffer;

			if (channelBuffer != null
				&& args.Channel.IsConnected
				&& channelBuffer.BytesTransferred >= Constants.DNS_MIN_MESSAGE_SIZE
				&& channelBuffer.BytesTransferred <= Constants.DNS_MAX_MESSAGE_SIZE) {
				Logger.WriteDebug(this, "PACKET request with channel id " +
					args.Channel.ChannelId.ToString() +
					" was received from " +
					args.Channel.RemoteEndpoint.ToString() +
					" and queued for processing...");

				var messageArgs = new DnsMessageEventArgs(args.Channel, args.ChannelBuffer);
				OnDnsMessageReceived(this, messageArgs);

				ThreadPool.QueueUserWorkItem(new WaitCallback(ProcessRequest), messageArgs);
			}
		}

		/// <summary>
		///  Remote client is disconnected.
		/// </summary>
		private void OnClientDisconnect(object sender, ClientDisconnectedEventArgs args)
		{
			Logger.WriteDebug(this, "Client was disconnected");
		}

		/// <summary>
		///  Process request and send reply back to rempte client.
		/// </summary>
		private void ProcessRequest(object state)
		{
			var args = (DnsMessageEventArgs)state;

			// create response message  
			args.ResponseMessage = new DnsMessage {
				Header = args.RequestMessage.Header,
				QuestionRecordCount = args.RequestMessage.QuestionRecordCount,
				Questions = args.RequestMessage.Questions
			};

			// classify the client mesage type
			if (args.RequestMessage.IsQuery == true && args.RequestMessage.QuestionRecordCount != 0) {
				// QUERY
				switch (args.RequestMessage.OperationCode) {
				case OperationCode.Query:
					DnsQuery(args);
					break;

				case OperationCode.InverseQuery:
				case OperationCode.Status:
				case OperationCode.Refresh:
				case OperationCode.Registration:
				case OperationCode.Release:
				case OperationCode.WACK:
					break;

				default:
					DnsError(args);
					break;
				}
			}
			else {
				return;
			}
		}

		/// <summary>
		/// A query is performed and a response is created. 
		/// </summary>
		private void DnsQuery(DnsMessageEventArgs args)
		{
			foreach (var question in args.RequestMessage.Questions) {
				var answers = _zoneFile.Get(question);

				if (answers != null) {
					// resolve locally
					args.ResponseMessage.Answers = new Answer[] { answers };
					args.ResponseMessage.AnswerRecordCount += 1;

					SendReply(args);
				}
				else {
					// resolve remotely

					if (IsProxy) {
						SendRequest(args);
						SendReply(args);
					}
				}
			}
		}

		/// <summary>
		/// A error has occured.  A server failure message is sent back to remote client. 
		/// </summary>
		private void DnsError(DnsMessageEventArgs args)
		{
			args.ResponseMessage.Additionals = new Additional[0];
			args.ResponseMessage.AdditionalRecordCount = 0;
			args.ResponseMessage.Authorities = new Authority[0];
			args.ResponseMessage.AuthorityRecordCount = 0;
			args.ResponseMessage.ResponseCode = ReturnCode.ServerFailure;

			SendReply(args);
		}

		/// <summary>
		/// A message is processsed and reply is sent back to remote client. 
		/// </summary>
		private void SendReply(DnsMessageEventArgs args)
		{
			args.ResponseMessage.IsQuery = false;

			try {
				args.Channel.SendTo(args.ResponseMessage.ToArray(), args.Channel.RemoteEndpoint);

				Logger.WriteInfo(this, "PACKET with message id " +
								args.ResponseMessage.TransactionId.ToHexString("0x") + " successfully sent to client endpoint " +
								args.Channel.RemoteEndpoint.ToString());

				Logger.WriteDebug(args.ResponseMessage.ToString());

				OnDnsMessageSent(this, args);

			}
			catch (SocketException ex) {
				Logger.WriteError(this, ex.Message + "Socket Error Code: " + ex.ErrorCode.ToString(), ex);
			}
			catch (Exception ex) {
				Logger.WriteError(this, "Error Message:" + ex.Message.ToString(), ex);
			}
		}

		/// <summary>
		/// A message was recived then forwared onto a remote name server.  
		/// </summary>
		private void SendRequest(DnsMessageEventArgs args)
		{
			try {
				using (var _sendSocket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp)) {
					var messageBuffer = new byte[Constants.DNS_MAX_MESSAGE_SIZE];

					IPEndPoint remoteEndPoint = null;

					try {
						remoteEndPoint = new IPEndPoint(_primaryServer, Constants.DNS_CLIENT_PORT);
						_sendSocket.Connect(remoteEndPoint);
					}

					catch {
						Logger.WriteInfo(this, "Failed to contact primary server " + _primaryServer + " failed.  Trying again using alternate server " + _alternateServer
							+ " endpoint " + remoteEndPoint.ToString());

						remoteEndPoint = new IPEndPoint(_alternateServer, Constants.DNS_CLIENT_PORT);
						_sendSocket.Connect(remoteEndPoint);
					}

					if (remoteEndPoint != null) {
						_sendSocket.ReceiveTimeout = Constants.DNS_RECEIVE_TIMEOUT;
						_sendSocket.SendTimeout = Constants.DNS_SEND_TIMEOUT;

						// send message
						_sendSocket.Connect(remoteEndPoint);
						_sendSocket.Send(args.RequestMessage.ToArray());
						Logger.WriteDebug(this, "PACKET successfully sent to client endpoint " +
										remoteEndPoint.ToString());
						Logger.WriteDebug(args.RequestMessage.ToString());

						// receive message
						_sendSocket.Receive(messageBuffer);
						_sendSocket.Close();

						args.ResponseMessage = new DnsMessage(messageBuffer);
						Logger.WriteDebug(this, "PACKET successfully received from client endpoint " +
										remoteEndPoint.ToString());
						Logger.WriteDebug(args.ResponseMessage.ToString());
					}
				}
			}
			catch (SocketException ex) {
				Logger.WriteError(this, ex.Message + "Socket Error Code: " + ex.ErrorCode.ToString(), ex);
			}
			catch (Exception ex) {
				Logger.WriteError(this, "Error Message:" + ex.Message.ToString(), ex);
			}
		}

		#endregion Methods

		#region Events

		/// <summary>
		///     A sntp message was recived and processed.
		/// </summary>
		public event DnsMessageEventHandler OnDnsMessageReceived = delegate { };

		/// <summary>
		///     A sntp message was recived and processed and sent to an sntp address.
		/// </summary>
		public event DnsMessageEventHandler OnDnsMessageSent = delegate { };

		#endregion Events
	}
}

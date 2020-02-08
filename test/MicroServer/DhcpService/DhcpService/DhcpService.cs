using MicroServer.Extensions;
using MicroServer.Logging;
using MicroServer.Net.Sockets;
using MicroServer.Utilities;
using System;
using System.Collections;
using System.Net;
using System.Net.Sockets;
using System.Text;
using uITron3;
using ThreadPool = MicroServer.Threading.ThreadPool;
using WaitCallback = MicroServer.Threading.WaitCallback;

namespace MicroServer.Net.Dhcp
{
	public class DhcpService : SocketService
	{
		#region Private Properties

		private IPAddress _interfaceAddress = IPAddress.Any;
		private InternetAddress _startAddress = new InternetAddress(192, 168, 50, 100);
		private InternetAddress _endAddress = new InternetAddress(192, 168, 50, 254);
		private InternetAddress _subnet = new InternetAddress(255, 255, 255, 0);
		private InternetAddress _gateway = new InternetAddress(192, 168, 50, 1);
		private NameServerCollection _dnsServers = new NameServerCollection() { new InternetAddress(192, 168, 50, 1) };

		private string _dnsSuffix;
		private string _serverName;
		private string _bootFileName;

		private readonly object _leaseSync = new object();
		private Hashtable _options = new Hashtable();
		private BindingManager _bindmgr = new BindingManager();

		private UdpListener _listener;

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
		/// The root path for storage of the binding file.
		/// </summary>
		public string StorageRoot {
			get => _bindmgr.StorageRoot;
			set {
				if (value != null)
					_bindmgr.StorageRoot = value;
			}
		}

		/// <summary>
		/// This defines the maximum amount of time that the client may use the IP address.
		/// </summary>
		public int LeaseDuration {
			get => _bindmgr.LeaseDuration;
			set => _bindmgr.LeaseDuration = value;
		}

		/// <summary>
		/// This defines the maximum amount that client has to accept a lease offer.
		/// </summary>
		public int OfferTimeout {
			get => _bindmgr.OfferTimeOut;
			set => _bindmgr.OfferTimeOut = value;
		}

		/// <summary>
		/// This defines the renewal (T1) time value.
		/// </summary>
		public int LeaseRenewal {
			get => _bindmgr.LeaseRenewal;
			set => _bindmgr.LeaseRenewal = value;
		}

		/// <summary>
		/// This defines the rebinding (T2) time value.
		/// </summary>
		public int LeaseRebinding {
			get => _bindmgr.LeaseRebinding;
			set => _bindmgr.LeaseRebinding = value;
		}

		/// <summary>
		/// This defines the pool of ip address the client may use.
		/// </summary>
		public void PoolRange(string startAddress, string endAddress)
		{
			if (!StringUtility.IsNullOrEmpty(startAddress) && !StringUtility.IsNullOrEmpty(endAddress)) {
				_startAddress = new InternetAddress(startAddress);
				_endAddress = new InternetAddress(endAddress);
				_bindmgr.PoolRange(_startAddress, _endAddress);
			}
		}

		/// <summary>
		/// This defines a static ip address reservation the client may use.
		/// </summary>
		public void PoolReservation(string address, string macAddress)
		{
			_bindmgr.Reservation(InternetAddress.Parse(address), PhysicalAddress.Parse(macAddress));
		}

		/// <summary>
		/// The subnet mask corresponds to the default subnet mask associated with the given client IP address.
		/// </summary>
		public string SubnetMask {
			get => _subnet.ToString();
			set => _subnet = new InternetAddress(value);
		}

		/// <summary>
		/// The gateway address (default route) that corresponds with the given client IP address.
		/// </summary>
		public string GatewayAddress {
			get => _gateway.ToString();
			set => _gateway = new InternetAddress(value);
		}

		/// <summary>
		/// The DNS name without the hostname part that corresponds with the given client IP address.
		/// </summary>
		public string DnsSuffix {
			get => _dnsSuffix;
			set => _dnsSuffix = value;
		}

		/// <summary>
		/// The DNS hostname part that corresponds with the given IP address.
		/// </summary>
		public string ServerName {
			get => _serverName;
			set => _serverName = value;
		}

		/// <summary>
		/// The boot file name the clients may use.
		/// </summary>
		public string BootFileName {
			get => _bootFileName;
			set => _bootFileName = value;
		}

		/// <summary>
		/// The the IP address(es) of the DNS servers that the client uses for name resolution.
		/// </summary>        
		public NameServerCollection NameServers {
			get => _dnsServers;
			set => _dnsServers = value;
		}

		/// <summary>
		/// Add or define a new custom option type.
		/// </summary>   
		public void AddOption(DhcpOption option, byte[] data)
		{
			_options.Add(option, data);
		}

		/// <summary>
		/// Remove custom option type.
		/// </summary>   
		public bool RemoveOption(DhcpOption option)
		{
			if (_options.Contains(option)) {
				_options.Remove(option);
				return true;
			}
			return false;
		}

		#endregion Public Properties

		#region Methods

		/// <summary>
		///  Starts the service listener if in a stopped state.
		/// </summary>
		public override bool Start(Itron itron, ThreadPool threadPool)
		{
			_itron = itron;
			_bindmgr.Start(itron);
			_listener = new UdpListener(itron) {
				InterfaceAddress = _interfaceAddress,
				BufferSize = Constants.DHCP_MAX_MESSAGE_SIZE,
				ReceiveTimeout = Constants.DHCP_RECEIVE_TIMEOUT,
				SendTimeout = Constants.DHCP_SEND_TIMEOUT
			};
			_listener.ClientConnected += OnClientConnect;
			_listener.ClientDisconnected += OnClientDisconnect;

			return _listener.Start(itron, Constants.DHCP_SERVICE_PORT, true);
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
				&& channelBuffer.BytesTransferred >= Constants.DHCP_MIN_MESSAGE_SIZE
				&& channelBuffer.BytesTransferred <= Constants.DHCP_MAX_MESSAGE_SIZE) {
				Logger.WriteDebug(this, "PACKET request with channel id " +
					args.Channel.ChannelId.ToString() +
					" was received from " +
					args.Channel.RemoteEndpoint.ToString() +
					" and queued for processing...");

				var messageArgs = new DhcpMessageEventArgs(args.Channel, args.ChannelBuffer);
				OnDhcpMessageReceived(this, messageArgs);

				threadPool.QueueUserWorkItem(new WaitCallback(ProcessRequest), messageArgs);
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
		///  Process boot REQUEST and send reply back to remote client.
		/// </summary>
		private void ProcessRequest(object state)
		{
			var args = (DhcpMessageEventArgs)state;

			args.ResponseMessage = new DhcpMessage();

			if (args.RequestMessage.Operation == OperationCode.BootRequest) {
				var messageTypeData = args.RequestOptions.MessageType;

				if (messageTypeData != null && messageTypeData.Length == 1) {
					var messageType = (MessageType)messageTypeData[0];

					var requestId = args.RequestMessage.SessionId.ToHexString("0x");
					var requestMac = args.RequestMessage.ClientHardwareAddress.ToString();

					ID tskid = ID.NULL;
					ER ret = _itron.get_tid(ref tskid);
					// classify the client mesage type
					switch (messageType) {
					case MessageType.Discover:
						Logger.WriteInfo(this, "DISCOVER for message id " + requestId + " from client " + requestMac + " processed on thread #" + tskid);
						DhcpDiscover(args);
						break;

					case MessageType.Request:
						Logger.WriteInfo(this, "REQUEST for message id " + requestId + " from client " + requestMac + " processed on thread #" + tskid);
						DhcpRequest(args);
						break;

					case MessageType.Decline:
						Logger.WriteInfo(this, "DECLINE for message id " + requestId + " from client " + requestMac + " processed on thread #" + tskid);
						DhcpDecline(args);
						break;

					case MessageType.Release:
						Logger.WriteInfo(this, "RELEASE for message id " + requestId + " from client " + requestMac + " processed on thread #" + tskid);
						DhcpRelease(args);
						break;

					case MessageType.Inform:
						Logger.WriteInfo(this, "INFORM for message id " + requestId + " from client " + requestMac + " processed on thread #" + tskid);
						DhcpInform(args);
						break;

					case MessageType.Nak:
					case MessageType.Ack:
					case MessageType.Offer:
						break;

					default:
						Logger.WriteInfo(this, "UNKNOWN message (" + messageType.ToString() + ") received and ignored");
						break;
					}
				}
				else {
					Logger.WriteDebug(this, "UNKNOWN message received and ignored");
				}
			}
		}

		/// <summary>
		///  Process DISCOVERY message type.
		/// </summary>
		private void DhcpDiscover(DhcpMessageEventArgs args)
		{
			#region Pre-Processing
			//  Start pre-process validation

			if (!args.RequestMessage.ClientAddress.Equals(InternetAddress.Any)) {
				Logger.WriteDebug(this, "Ignoring DISCOVER message: ciAddr field is non-zero: "
					+ args.RequestMessage.ClientAddress.ToString());
				return; // if processing not should continue
			}

			if (args.RequestOptions.ServerIdentifier != null) {
				Logger.WriteDebug(this, "Ignoring DISCOVER message: ServerId option is not null");
				return; // if processing should not continue
			}

			// End pre-Process validation
			#endregion Pre-Processing

			if (args.ResponseBinding.Address.Equals(InternetAddress.Any) && args.RequestOptions.AddressRequest != null) {
				args.ResponseBinding.Address = new InternetAddress(args.RequestOptions.AddressRequest);
			}

			var offer = _bindmgr.GetOffer(args.ResponseBinding);
			if (offer == null) {
				Logger.WriteDebug(this, "DISCOVER message: "
						+ "No binding available for client");
			}
			else {
				SendOffer(args, offer);
			}
		}

		/// <summary>
		///  Process REQUEST message type.
		/// </summary>
		private void DhcpRequest(DhcpMessageEventArgs args)
		{
			RequestState requestState;

			#region Pre-Processing
			//  Start pre-process validation
			//---------------------------------------------------------------------
			//|              |INIT-REBOOT  |SELECTING    |RENEWING     |REBINDING |
			//---------------------------------------------------------------------
			//|broad/unicast |broadcast    |broadcast    |unicast      |broadcast |
			//|server-ip     |MUST NOT     |MUST         |MUST NOT     |MUST NOT  |
			//|requested-ip  |MUST         |MUST         |MUST NOT     |MUST NOT  |
			//|ciaddr        |zero         |zero         |IP address   |IP address|
			//---------------------------------------------------------------------
			// first determine what KIND of request we are dealing with
			if (args.RequestMessage.ClientAddress.Equals(InternetAddress.Any)) {
				// the ciAddr MUST be 0.0.0.0 for Init-Reboot and Selecting
				if (args.RequestOptions.AddressRequest == null) {
					// init-reboot MUST NOT have server-id option
					requestState = RequestState.InitReboot;
				}
				else {
					// selecting MUST have server-id option
					requestState = RequestState.Selecting;
				}
			}
			else {
				// the ciAddr MUST NOT be 0.0.0.0 for Renew and Rebind
				if (!args.RequestMessage.IsBroadcast) {
					// renew is unicast
					// NOTE: this will not happen if the v4 broadcast interface used at startup,
					//		 but handling of DHCPv4 renew/rebind is the same anyway
					requestState = RequestState.Renewing;
				}
				else {
					// rebind is broadcast
					requestState = RequestState.Rebinding;
				}
			}

			if ((requestState == RequestState.InitReboot) || (requestState == RequestState.Selecting)) {
				if (args.RequestOptions.AddressRequest == null) {
					Logger.WriteDebug(this, "Ignoring REQUEST " + RequestStateString.GetName(requestState) + " message: Requested IP option is null");
					return; // if processing should not continue
				}

				if (requestState == RequestState.Selecting) {
					// if the client provided a ServerID option, then it MUST
					// match our configured ServerID, otherwise ignore the request
					var serverIdentifier = new InternetAddress(args.RequestOptions.ServerIdentifier);
					//InternetAddress localServerIdentifier = new InternetAddress(((IPEndPoint)_dhcpSocket.LocalEndPoint).Address.GetAddressBytes());
					var localServerIdentifier = new InternetAddress(InterfaceAddress);
					if (!serverIdentifier.Equals(localServerIdentifier)) {
						Logger.WriteDebug(this, "Ignoring REQUEST " + RequestStateString.GetName(requestState) + " message: "
							+ "Requested ServerId: " + serverIdentifier.ToString()
							+ " Local ServerId: " + localServerIdentifier.ToString());
						return; // if processing should not continue
					}
				}
			}
			else {  // type == Renewing or Rebinding
				if (args.RequestOptions.AddressRequest != null) {
					Logger.WriteDebug(this, "Ignoring REQUEST " + RequestStateString.GetName(requestState) + " message: "
						+ "Requested IP option is not null");
					return; // if processing should not continue
				}
			}

			//  End pre-process validation
			#endregion Pre-Processing

			Logger.WriteDebug(this, "Processing REQUEST " + RequestStateString.GetName(requestState) + " message");

			//TODO:  should also check for ip on link

			if (args.RequestOptions.AddressRequest != null) {
				args.ResponseBinding.Address = new InternetAddress(args.RequestOptions.AddressRequest);
			}

			var assignment = _bindmgr.GetAssigned(args.ResponseBinding);
			if (assignment == null) {
				args.ResponseMessage.AddOption(DhcpOption.DhcpMessage, Encoding.UTF8.GetBytes("No binding available for client"));
				Logger.WriteDebug(this, "REQUEST " + RequestStateString.GetName(requestState) + " message: No binding available for client sending NAK");
				SendNak(args);
			}
			else {
				SendAck(args, assignment);
				OnLeaseAcknowledged(this, new DhcpLeaseEventArgs(assignment.Address, assignment));
			}
		}

		/// <summary>
		///  Process RELEASE message type.
		/// </summary>
		private void DhcpRelease(DhcpMessageEventArgs args)
		{
			#region Pre-Processing
			//  Start pre-process validation

			if (args.RequestMessage.ClientAddress.Equals(InternetAddress.Any)) {
				Logger.WriteDebug(this, "Ignoring RELEASE message: "
					+ "ciAddr is zero");
				return; // if processing should not continue
			}

			// End pre-Process validation
			#endregion Pre-Processing

			var lease = _bindmgr.SetReleased(args.ResponseBinding);

			// no reply for v4 release
			if (lease == null) {
				Logger.WriteDebug(this, "RELEASE message: "
					+ "No binding available for client to release");
			}
			else {
				OnLeaseReleased(this, new DhcpLeaseEventArgs(lease.Address, lease));
			}
		}

		/// <summary>
		///  Process INFORM message type.
		/// </summary>
		private void DhcpInform(DhcpMessageEventArgs args)
		{
			#region Pre-Processing
			//  Start pre-process validation

			// if the client provided a ServerID option, then it MUST
			// match our configured ServerID, otherwise ignore the request
			var serverIdentifier = new InternetAddress(args.RequestOptions.ServerIdentifier);
			//InternetAddress localServerIdentifier = new InternetAddress(((IPEndPoint)_dhcpSocket.LocalEndPoint).Address.GetAddressBytes());
			var localServerIdentifier = new InternetAddress(InterfaceAddress);
			if (!serverIdentifier.Equals(localServerIdentifier)) {
				Logger.WriteDebug(this, "Ignoring INFORM message: "
					+ "Requested ServerId: " + serverIdentifier.ToString()
					+ " Local ServerId: " + localServerIdentifier.ToString());
				return; // if processing should not continue
			}

			// End pre-Process validation
			#endregion Pre-Processing

			SendAck(args);
		}

		/// <summary>
		///  Process DECLINE message type.
		/// </summary>
		private void DhcpDecline(DhcpMessageEventArgs args)
		{

			#region Pre-Processing
			//  Start pre-process validation

			if (args.RequestOptions.AddressRequest == null) {
				Logger.WriteDebug(this, "Ignoring DECLINE message: "
					+ "Requested IP option is null");
				return; // if processin should not continue     	
			}

			// End pre-Process validation
			#endregion Pre-Processing

			if (args.RequestOptions.AddressRequest != null) {
				args.ResponseBinding.Address = new InternetAddress(args.RequestOptions.AddressRequest);
			}

			var lease = _bindmgr.SetDeclined(args.ResponseBinding);

			// no reply for v4 decline
			if (lease == null) {
				Logger.WriteDebug(this, "DECLINE message: "
					+ "No binding available for client to decline");
			}
			else {
				OnLeaseDeclined(this, new DhcpLeaseEventArgs(lease.Address, lease));
			}
		}

		/// <summary>
		///  Send OFFER message back to remote client.
		/// </summary>
		private void SendOffer(DhcpMessageEventArgs args, BindingLease offer)
		{
			// set header
			args.ResponseMessage.AssignedAddress = offer.Address;
			args.ResponseMessage.NextServerAddress = args.ResponseMessage.NextServerAddress;
			args.ResponseMessage.AddOption(DhcpOption.DhcpMessageType, (byte)MessageType.Offer);

			SetLeaseOptions(args);
			SetClientOptions(args);

			var paramList = args.RequestOptions.ParameterList;
			if (paramList != null) {
				args.ResponseMessage.OptionOrdering = paramList;
			}

			ID tskid = ID.NULL;
			ER ret = _itron.get_tid(ref tskid);
			Logger.WriteInfo(this, "OFFER for message id " + args.RequestMessage.SessionId.ToHexString("0x") + " sent on thread(#" + tskid + ")");
			SendReply(args);
		}

		/// <summary>
		///  Send ACK message back to remote client.
		/// </summary>
		private void SendAck(DhcpMessageEventArgs args, BindingLease lease)
		{
			// set header
			args.ResponseMessage.AssignedAddress = lease.Address;

			// set lease options
			SetLeaseOptions(args);

			SendAck(args);
		}

		/// <summary>
		///  Send ACK message back to remote client.
		/// </summary>
		private void SendAck(DhcpMessageEventArgs args)
		{
			// set header
			args.ResponseMessage.ClientAddress = args.RequestMessage.ClientAddress;
			args.ResponseMessage.NextServerAddress = args.RequestMessage.NextServerAddress;

			// set client options
			args.ResponseMessage.AddOption(DhcpOption.DhcpMessageType, (byte)MessageType.Ack);
			SetClientOptions(args);

			var paramList = args.RequestOptions.ParameterList;
			if (paramList != null) {
				args.ResponseMessage.OptionOrdering = paramList;
			}

			ID tskid = ID.NULL;
			ER ret = _itron.get_tid(ref tskid);
			Logger.WriteInfo(this, "ACK (Acknowledge) for message id " + args.RequestMessage.SessionId.ToHexString("0x") + " sent on thread(#" + tskid + ")");
			SendReply(args);
		}

		/// <summary>
		///  Send NAK message back to remote client.
		/// </summary>
		private void SendNak(DhcpMessageEventArgs args)
		{
			// Set options
			args.ResponseMessage.AddOption(DhcpOption.DhcpMessageType, (byte)MessageType.Nak);

			ID tskid = ID.NULL;
			ER ret = _itron.get_tid(ref tskid);
			Logger.WriteInfo(this, "NAK (Negative Acknowledge) for message id " + args.RequestMessage.SessionId.ToHexString("0x") + " sent on thread(#" +
				tskid + ")" + " with option message: " + ByteUtility.GetSafeString(args.ResponseMessage.GetOptionData(DhcpOption.DhcpMessage)));
			SendReply(args);
		}

		/// <summary>
		///  Send response message back to remote client.
		/// </summary>
		private void SendReply(DhcpMessageEventArgs args)
		{
			// set global headers
			args.ResponseMessage.Operation = OperationCode.BootReply;
			args.ResponseMessage.Hardware = args.RequestMessage.Hardware;
			args.ResponseMessage.HardwareAddressLength = args.RequestMessage.HardwareAddressLength;
			args.ResponseMessage.SessionId = args.RequestMessage.SessionId;
			args.ResponseMessage.Flags = args.RequestMessage.Flags;
			args.ResponseMessage.RelayAgentAddress = args.RequestMessage.RelayAgentAddress;
			args.ResponseMessage.ClientHardwareAddress = args.RequestMessage.ClientHardwareAddress;

			if (!StringUtility.IsNullOrEmpty(_serverName))
				args.ResponseMessage.ServerName = Encoding.UTF8.GetBytes(_serverName);

			if (!StringUtility.IsNullOrEmpty(_bootFileName))
				args.ResponseMessage.BootFileName = Encoding.UTF8.GetBytes(_bootFileName);

			// set global options
			//args.ResponseMessage.AddOption(DhcpOption.ServerIdentifier, ((IPEndPoint)_dhcpSocket.LocalEndPoint).Address.GetAddressBytes());
			args.ResponseMessage.AddOption(DhcpOption.ServerIdentifier, (InterfaceAddress.GetAddressBytes()));

			// determin the proper ip address and port for sendto
			var relayAgentAddress = args.RequestMessage.RelayAgentAddress;
			var clientAddress = args.RequestMessage.ClientAddress;

			if (!relayAgentAddress.Equals(InternetAddress.Any)) {
				args.Channel.RemoteEndpoint = new IPEndPoint(relayAgentAddress.ToIPAddress(), Constants.DHCP_SERVICE_PORT);
			}
			else {
				if (!clientAddress.Equals(InternetAddress.Any)) {
					args.Channel.RemoteEndpoint = new IPEndPoint(clientAddress.ToIPAddress(), Constants.DHCP_CLIENT_PORT);
				}
				else {
					args.Channel.RemoteEndpoint = new IPEndPoint(InternetAddress.Broadcast.ToIPAddress(), Constants.DHCP_CLIENT_PORT);
				}
			}

			try {
				args.Channel.SendTo(args.ResponseMessage.ToArray(), args.Channel.RemoteEndpoint);
				Logger.WriteDebug(this, "PACKET with message id " +
								args.ResponseMessage.SessionId.ToHexString("0x") + " successfully sent to client endpoint " +
								args.Channel.RemoteEndpoint.ToString());

				Logger.WriteDebug(args.ResponseMessage.ToString());

				OnDhcpMessageSent(this, args);
			}
			catch (SocketException ex) {
				Logger.WriteError(this, ex.Message + "Socket Error Code: " + ex.ErrorCode.ToString(), ex);
			}
			catch (Exception ex) {
				Logger.WriteError(this, "Error Message:" + ex.Message.ToString(), ex);
			}
		}

		/// <summary>
		///  Set global lease options.
		/// </summary>
		private void SetLeaseOptions(DhcpMessageEventArgs args)
		{
			args.ResponseMessage.AddOption(DhcpOption.AddressTime, ByteUtility.ReverseByteOrder(BitConverter.GetBytes(LeaseDuration)));
			args.ResponseMessage.AddOption(DhcpOption.Renewal, ByteUtility.ReverseByteOrder(BitConverter.GetBytes(LeaseRenewal)));
			args.ResponseMessage.AddOption(DhcpOption.Rebinding, ByteUtility.ReverseByteOrder(BitConverter.GetBytes(LeaseRebinding)));
		}

		/// <summary>
		///  Set client response lease options.
		/// </summary>
		private void SetClientOptions(DhcpMessageEventArgs args)
		{
			// set client response options
			args.ResponseMessage.AddOption(DhcpOption.SubnetMask, _subnet.ToArray());
			args.ResponseMessage.AddOption(DhcpOption.Router, _gateway.ToArray());

			if (!StringUtility.IsNullOrEmpty(_dnsSuffix)) {
				args.ResponseMessage.AddOption(DhcpOption.DomainNameSuffix, Encoding.UTF8.GetBytes(_dnsSuffix));
			}

			if (_dnsServers != null && _dnsServers.Count > 0) {
				var dnsServerAddresses = new byte[_dnsServers.Count * 4];
				for (var i = 0; i < _dnsServers.Count; i++) {
					var ipAddressValue = (InternetAddress)_dnsServers[i];
					ipAddressValue.ToArray().CopyTo(dnsServerAddresses, i * 4);
				}
				args.ResponseMessage.AddOption(DhcpOption.DomainNameServer, dnsServerAddresses);
			}

			// set custom response options
			if (_options != null && _options.Count > 0) {
				foreach (DictionaryEntry option in _options) {
					args.ResponseMessage.AddOption((DhcpOption)option.Key, (byte[])option.Value);
				}
			}
		}

		#endregion Methods

		#region Events

		/// <summary>
		///     A dhcp message was recived and processed.
		/// </summary>
		public event DhcpMessageEventHandler OnDhcpMessageReceived = delegate { };

		/// <summary>
		///     A dhcp message was recived, processed and sent to a remote client.
		/// </summary>
		public event DhcpMessageEventHandler OnDhcpMessageSent = delegate { };

		/// <summary>
		///     A lease binding was acknowledged by a remote clinet.
		/// </summary>
		public event DhcpAddressLeaseEventHandler OnLeaseAcknowledged = delegate { };

		/// <summary>
		///     A lease binding was released by a remote client.
		/// </summary>
		public event DhcpAddressLeaseEventHandler OnLeaseReleased = delegate { };

		/// <summary>
		///     A lease binding was declined by a remote client.
		/// </summary>
		public event DhcpAddressLeaseEventHandler OnLeaseDeclined = delegate { };

		#endregion Events
	}
}

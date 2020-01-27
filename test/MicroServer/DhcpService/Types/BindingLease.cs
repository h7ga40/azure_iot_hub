using MicroServer.Extensions;
using MicroServer.Utilities;
using System;

namespace MicroServer.Net.Dhcp
{
	public class BindingLease
	{
		private static readonly Random RANDOM = new Random();

		private string _clientId;
		private PhysicalAddress _owner;
		private InternetAddress _address;
		private byte[] _hostName;
		private DateTime _expiration;
		private uint _sessionId;
		private LeaseState _state;

		public string ClientId {
			get => _clientId;
			set => _clientId = value;
		}

		public PhysicalAddress Owner {
			get => _owner;
			set => _owner = value;
		}

		public InternetAddress Address {
			get => _address;
			set => _address = value;
		}

		public byte[] HostName {
			get => _hostName;
			set => _hostName = value;
		}

		public DateTime Expiration {
			get => _expiration;
			set => _expiration = value;
		}

		public uint SessionId {
			get => _sessionId;
			set => _sessionId = value;
		}

		public LeaseState State {
			get => _state;
			set => _state = value;
		}

		public BindingLease(PhysicalAddress owner, InternetAddress address)
			: this(RANDOM.Next(Int32.MaxValue).ToString(), owner, address, new byte[0], DateTime.Now.AddDays(7), 0, LeaseState.Unassigned)
		{
		}

		public BindingLease(string clientId, PhysicalAddress owner, InternetAddress address, byte[] hostName, DateTime expiration, uint sessionId, LeaseState state)
		{
			_clientId = clientId;
			_owner = owner;
			_address = address;
			_hostName = hostName;
			_expiration = expiration;
			_sessionId = sessionId;
			_state = state;
		}

		public override string ToString()
		{
			var response = "CID: " + ClientId.ToString() +
						" | SESID: " + SessionId.ToHexString("0x") +
						" | IP: " + Address.ToString() +
						" | OWNER: " + Owner.ToString() +
						" | NAME: " + ByteUtility.GetSafeString(_hostName) +
						" | EXPN: " + Expiration.ToLocalTime().ToString() +
						" | STATE: " + LeaseStateString.GetName(_state);
			return response;
		}
	}
}

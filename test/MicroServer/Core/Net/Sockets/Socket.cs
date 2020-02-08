using MicroServer.Net.Sntp;
using System;
using System.Net;
using System.Net.Sockets;
using uITron3;

namespace MicroServer.Net.Sockets
{
	public class Socket : IDisposable
	{
		private interface IDiscripter
		{
			void Bind(EndPoint localEP);
			void Connect(EndPoint remoteEP);
			void Close();
			void Listen(int backlog);
			Socket Accept();
			int Send(byte[] buffer, int offset, int size, SocketFlags socketFlags, out SocketError errorCode);
			int SendTo(byte[] buffer, int offset, int size, SocketFlags socketFlags, EndPoint remoteEP);
			int Receive(byte[] buffer, int offset, int size, SocketFlags socketFlags, out SocketError errorCode);
			int ReceiveFrom(byte[] buffer, int offset, int size, SocketFlags socketFlags, ref EndPoint remoteEP);
			bool Poll(int microSeconds, SelectMode mode);
			void SetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName, byte[] optionValue);
			byte[] GetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName);
		}

		private readonly AddressFamily addressFamily;
		private readonly SocketType socketType;
		private readonly ProtocolType protocolType;
		private IDiscripter fd;
		private Itron itron;

		internal static readonly TMO SOCKET_TIMEOUT = new TMO(20000);

		public Socket(Itron itron, AddressFamily addressFamily, SocketType socketType, ProtocolType protocolType)
		{
			if (itron == null) {
				throw new ArgumentException("itron");
			}
			this.itron = itron;

			switch (addressFamily) {
			case AddressFamily.InterNetwork:
				switch (socketType) {
				case SocketType.Stream:
					fd = new TCPv4(itron);
					break;
				case SocketType.Dgram:
					fd = new UDPv4(itron);
					break;
				default:
					throw new SocketException();
				}
				break;
			case AddressFamily.InterNetworkV6:
				switch (socketType) {
				case SocketType.Stream:
					fd = new TCPv6(itron);
					break;
				case SocketType.Dgram:
					fd = new UDPv6(itron);
					break;
				default:
					throw new SocketException();
				}
				break;
			default:
				throw new SocketException();
			}

			this.addressFamily = addressFamily;
			this.socketType = socketType;
			this.protocolType = protocolType;
		}

		private Socket(IDiscripter fd)
		{
			this.fd = fd;
		}

		public void Dispose()
		{
			Dispose(true);
		}

		protected virtual void Dispose(bool dispose)
		{
		}

		public IPEndPoint LocalEndPoint { get; private set; }
		public int ReceiveTimeout { get; set; }
		public int SendTimeout { get; set; }
		public int Available { get; }

		public void Bind(EndPoint localEP)
		{
			LocalEndPoint = (IPEndPoint)localEP;
			fd.Bind(localEP);
		}

		public void Connect(EndPoint remoteEP)
		{
			fd.Connect(remoteEP);
		}

		public void Close()
		{
			fd.Close();
		}

		public void Listen(int backlog)
		{
			fd.Listen(backlog);
		}

		public Socket Accept()
		{
			return fd.Accept();
		}

		public int Send(byte[] buffer, int offset, int size, SocketFlags socketFlags, out SocketError errorCode)
		{
			return fd.Send(buffer, offset, size, socketFlags, out errorCode);
		}

		public int Send(byte[] buffer, int size, SocketFlags socketFlags, out SocketError errorCode)
		{
			return fd.Send(buffer, 0, size, socketFlags, out errorCode);
		}

		public int Send(byte[] buffer, SocketFlags socketFlags, out SocketError errorCode)
		{
			return fd.Send(buffer, 0, buffer != null ? buffer.Length : 0, socketFlags, out errorCode);
		}

		public int Send(byte[] buffer, int offset, int size, SocketFlags socketFlags)
		{
			SocketError errorCode;
			return fd.Send(buffer, offset, size, socketFlags, out errorCode);
		}

		public int Send(byte[] buffer, int size, SocketFlags socketFlags)
		{
			SocketError errorCode;
			return fd.Send(buffer, 0, size, socketFlags, out errorCode);
		}

		public int Send(byte[] buffer, SocketFlags socketFlags)
		{
			SocketError errorCode;
			return fd.Send(buffer, 0, buffer != null ? buffer.Length : 0, socketFlags, out errorCode);
		}

		public int Send(byte[] buffer)
		{
			SocketError errorCode;
			return fd.Send(buffer, 0, buffer != null ? buffer.Length : 0, 0, out errorCode);
		}

		public int SendTo(byte[] buffer, int offset, int size, SocketFlags socketFlags, EndPoint remoteEP)
		{
			return fd.SendTo(buffer, offset, size, socketFlags, remoteEP);
		}

		public int SendTo(byte[] buffer, int size, SocketFlags socketFlags, EndPoint remoteEP)
		{
			return fd.SendTo(buffer, 0, size, socketFlags, remoteEP);
		}

		public int SendTo(byte[] buffer, SocketFlags socketFlags, EndPoint remoteEP)
		{
			return fd.SendTo(buffer, 0, buffer != null ? buffer.Length : 0, socketFlags, remoteEP);
		}

		public int SendTo(byte[] buffer, EndPoint remoteEP)
		{
			return fd.SendTo(buffer, 0, buffer != null ? buffer.Length : 0, SocketFlags.None, remoteEP);
		}

		public int Receive(byte[] buffer, int offset, int size, SocketFlags socketFlags, out SocketError errorCode)
		{
			return fd.Receive(buffer, offset, size, socketFlags, out errorCode);
		}

		public int Receive(byte[] buffer, int size, SocketFlags socketFlags, out SocketError errorCode)
		{
			return fd.Receive(buffer, 0, size, socketFlags, out errorCode);
		}

		public int Receive(byte[] buffer, SocketFlags socketFlags, out SocketError errorCode)
		{
			return fd.Receive(buffer, 0, buffer != null ? buffer.Length : 0, socketFlags, out errorCode);
		}

		public int Receive(byte[] buffer, out SocketError errorCode)
		{
			return fd.Receive(buffer, 0, buffer != null ? buffer.Length : 0, SocketFlags.None, out errorCode);
		}

		public int Receive(byte[] buffer)
		{
			SocketError errorCode;
			return fd.Receive(buffer, 0, buffer != null ? buffer.Length : 0, SocketFlags.None, out errorCode);
		}

		public int ReceiveFrom(byte[] buffer, int offset, int size, SocketFlags socketFlags, ref EndPoint remoteEP)
		{
			return fd.ReceiveFrom(buffer, offset, size, socketFlags, ref remoteEP);
		}

		public int ReceiveFrom(byte[] buffer, int size, SocketFlags socketFlags, ref EndPoint remoteEP)
		{
			return fd.ReceiveFrom(buffer, 0, size, socketFlags, ref remoteEP);
		}

		public int ReceiveFrom(byte[] buffer, SocketFlags socketFlags, ref EndPoint remoteEP)
		{
			return fd.ReceiveFrom(buffer, 0, buffer != null ? buffer.Length : 0, socketFlags, ref remoteEP);
		}

		public int ReceiveFrom(byte[] buffer, ref EndPoint remoteEP)
		{
			return fd.ReceiveFrom(buffer, 0, buffer != null ? buffer.Length : 0, SocketFlags.None, ref remoteEP);
		}

		public bool Poll(int microSeconds, SelectMode mode)
		{
			return fd.Poll(microSeconds, mode);
		}

		public void SetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName, byte[] optionValue)
		{
			fd.SetSocketOption(optionLevel, optionName, optionValue);
		}

		public void SetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName, int optionValue)
		{
			fd.SetSocketOption(optionLevel, optionName, new byte[] { (byte)optionValue });
		}

		public byte[] GetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName)
		{
			return fd.GetSocketOption(optionLevel, optionName);
		}

		private class TCPv4 : IDiscripter
		{
			private Itron itron;
			private ID cepid;
			private ID repid;
			private TCPv4 listner;
			private IPEndPoint localEP;
			private IPEndPoint remoteEP;

			public TCPv4(Itron itron)
			{
				this.itron = itron;
			}

			public TCPv4(TCPv4 listner, ID repid, IPEndPoint remoteEP)
			{
				this.itron = listner.itron;
				this.listner = listner;
				this.cepid = listner.cepid;
				this.repid = repid;
				this.remoteEP = remoteEP;
			}

			public void Bind(EndPoint localEP)
			{
				T_TCP_CCEP ccep = new T_TCP_CCEP() {
					cepatr = ATR.TA_NULL,
					callback = socket_tcp_callback
				};

				ER ret = itron.tcp_cre_cep(ID.ID_AUTO, ref ccep, out cepid);
				if (ret != ER.E_OK) {
					throw new SocketException();
				}

				this.localEP = (IPEndPoint)localEP;
			}

			public void Connect(EndPoint remoteEP)
			{
				ER ret;

				if (remoteEP.AddressFamily != AddressFamily.InterNetwork) {
					throw new SocketException();
				}

				this.remoteEP = (IPEndPoint)remoteEP;

				if (cepid == 0) {
					T_TCP_CCEP ccep = new T_TCP_CCEP() {
						cepatr = ATR.TA_NULL,
						callback = socket_tcp_callback
					};

					ret = itron.tcp_cre_cep(ID.ID_AUTO, ref ccep, out cepid);
					if (ret != ER.E_OK) {
						throw new SocketException();
					}
				}


				T_IPV4EP lep = new T_IPV4EP(new byte[T_IPV4EP.length], 0);
				lep.ipaddr = BitConverter.ToUInt32(localEP.Address.GetAddressBytes(), 0);
				lep.portno = (ushort)localEP.Port;
				T_IPV4EP rep = new T_IPV4EP(new byte[T_IPV4EP.length], 0);
				rep.ipaddr = BitConverter.ToUInt32(this.remoteEP.Address.GetAddressBytes(), 0);
				rep.portno = (ushort)this.remoteEP.Port;
				ret = itron.tcp_con_cep(cepid, lep, rep, Socket.SOCKET_TIMEOUT);
				if (ret != ER.E_OK) {
					throw new SocketException();
				}
			}

			public void Close()
			{
				ER ret, ret2;

				ret = itron.tcp_can_cep(cepid, FN.TFN_TCP_ALL);
				ret = itron.tcp_sht_cep(cepid);
				ret = itron.tcp_cls_cep(cepid, (repid != 0) ? new TMO(0) : Socket.SOCKET_TIMEOUT);
				ret2 = itron.tcp_del_cep(cepid);
			}

			public void Listen(int backlog)
			{
				T_IPV4EP myaddr = new T_IPV4EP(new byte[T_IPV4EP.length], 0);
				myaddr.ipaddr = BitConverter.ToUInt32(this.localEP.Address.GetAddressBytes(), 0);
				myaddr.portno = (ushort)this.localEP.Port;
				T_TCP_CREP crep = new T_TCP_CREP() {
					repatr = ATR.TA_NULL,
					myaddr = myaddr,
				};
				ER ret;

				ret = itron.tcp_cre_rep(ID.ID_AUTO, ref crep, out repid);
				if (ret != ER.E_OK) {
					throw new SocketException();
				}
			}

			public Socket Accept()
			{
				T_TCP_CCEP ccep = new T_TCP_CCEP() {
					cepatr = ATR.TA_NULL,
					callback = socket_tcp_callback
				};

				ER ret = itron.tcp_cre_cep(ID.ID_AUTO, ref ccep, out cepid);
				if (ret != ER.E_OK) {
					return null;
				}

				T_IPV4EP rep = new T_IPV4EP(new byte[T_IPV4EP.length], 0);
				ret = itron.tcp_acp_cep(cepid, repid, rep, TMO.TMO_FEVR);
				if (ret < 0) {
					throw new SocketException();
				}

				IPEndPoint remoteEP = new IPEndPoint(new IPAddress(rep.data), rep.portno);

				return new Socket(new TCPv4(this, repid, remoteEP));
			}

			public int Send(byte[] buffer, int offset, int size, SocketFlags socketFlags, out SocketError errorCode)
			{
				ER ret;

				if ((socketFlags & SocketFlags.OutOfBand) != 0) {
					ret = itron.tcp_snd_oob(cepid, new pointer(buffer, offset), size, SOCKET_TIMEOUT);
					if (ret != ER.E_OK) {
						errorCode = SocketError.SocketError;
						return 0;
					}

					errorCode = SocketError.Success;
					return (int)ret;
				}

				ret = itron.tcp_snd_dat(cepid, new pointer(buffer, offset), size, SOCKET_TIMEOUT);
				if (ret != ER.E_OK) {
					errorCode = SocketError.SocketError;
					return 0;
				}

				errorCode = SocketError.Success;
				return (int)ret;
			}

			public int SendTo(byte[] buffer, int offset, int size, SocketFlags socketFlags, EndPoint remoteEP)
			{
				throw new NotImplementedException();
			}

			public int Receive(byte[] buffer, int offset, int size, SocketFlags socketFlags, out SocketError errorCode)
			{
				ER ret;

				if ((socketFlags & SocketFlags.OutOfBand) != 0) {
					ret = itron.tcp_rcv_oob(cepid, new pointer(buffer, offset), size);
					if (ret != ER.E_OK) {
						errorCode = SocketError.SocketError;
						return 0;
					}

					errorCode = SocketError.Success;
					return (int)ret;
				}

				ret = itron.tcp_rcv_dat(cepid, new pointer(buffer, offset), size, SOCKET_TIMEOUT);
				if (ret != ER.E_OK) {
					errorCode = SocketError.SocketError;
					return 0;
				}

				errorCode = SocketError.Success;
				return (int)ret;
			}

			public int ReceiveFrom(byte[] buffer, int offset, int size, SocketFlags socketFlags, ref EndPoint remoteEP)
			{
				throw new NotImplementedException();
			}

			public bool Poll(int microSeconds, SelectMode mode)
			{
				throw new NotImplementedException();
			}

			private void socket_tcp_callback(ID cepid, FN fncd, pointer p_parblk)
			{
				switch (fncd) {
				case FN.TFN_TCP_RCV_BUF:
					break;
				}
			}

			public void SetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName, byte[] optionValue)
			{
				ER ret;

				switch (optionLevel) {
				case SocketOptionLevel.Socket:
					switch (optionName) {
					case SocketOptionName.ReuseAddress:
						return;
					case SocketOptionName.Broadcast:
						return;
					}
					break;
				case SocketOptionLevel.Tcp:
					ret = itron.tcp_set_opt(cepid, (int)optionName, new pointer(optionValue), optionValue.Length);
					if (ret != ER.E_OK) {
						throw new SocketException();
					}
					break;
				}

				throw new NotImplementedException();
			}

			public byte[] GetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName)
			{
				ER ret;
				byte[] optionValue;

				switch (optionLevel) {
				case SocketOptionLevel.Socket:
					switch (optionName) {
					case SocketOptionName.ReuseAddress:
						return new byte[] { 1 };
					case SocketOptionName.Broadcast:
						return new byte[] { 1 };
					}
					break;
				case SocketOptionLevel.Tcp:
					optionValue = new byte[1];
					ret = itron.tcp_get_opt(cepid, (int)optionName, new pointer(optionValue), optionValue.Length);
					if (ret != ER.E_OK) {
						throw new SocketException();
					}
					return optionValue;
				}

				throw new NotImplementedException();
			}
		}

		private class UDPv4 : IDiscripter
		{
			private Itron itron;
			private ID cepid;
			private IPEndPoint localEP;

			public UDPv4(Itron itron)
			{
				this.itron = itron;
			}

			public void Bind(EndPoint localEP)
			{
				IPEndPoint local = (IPEndPoint)localEP;
				T_IPV4EP myaddr = new T_IPV4EP(new byte[T_IPV4EP.length], 0);
				myaddr.ipaddr = BitConverter.ToUInt32(local.Address.GetAddressBytes(), 0);
				myaddr.portno = (ushort)local.Port;

				T_UDP_CCEP ccep = new T_UDP_CCEP() {
					cepatr = ATR.TA_NULL,
					myaddr = myaddr,
					callback = socket_udp_callback
				};

				ER ret = itron.udp_cre_cep(ID.ID_AUTO, ref ccep, out cepid);
				if (ret != ER.E_OK) {
					throw new SocketException();
				}

				this.localEP = local;
			}

			public void Connect(EndPoint remoteEP)
			{
				throw new NotImplementedException();
			}

			public void Close()
			{
				ER ret;

				ret = itron.udp_del_cep(cepid);
			}

			public void Listen(int backlog)
			{
				throw new NotImplementedException();
			}

			public Socket Accept()
			{
				throw new NotImplementedException();
			}

			public int Send(byte[] buffer, int offset, int size, SocketFlags socketFlags, out SocketError errorCode)
			{
				throw new NotImplementedException();
			}

			public int SendTo(byte[] buffer, int offset, int size, SocketFlags socketFlags, EndPoint remoteEP)
			{
				ER ret;

				IPEndPoint remoteIPEP = (IPEndPoint)remoteEP;

				T_IPV4EP rep = new T_IPV4EP(new byte[T_IPV4EP.length], 0);
				rep.ipaddr = BitConverter.ToUInt32(remoteIPEP.Address.GetAddressBytes(), 0);
				rep.portno = (ushort)remoteIPEP.Port;

				ret = itron.udp_snd_dat(cepid, rep, new pointer(buffer, offset), size, SOCKET_TIMEOUT);
				if (ret != ER.E_OK) {
					return 0;
				}

				return (int)ret;
			}

			public int Receive(byte[] buffer, int offset, int size, SocketFlags socketFlags, out SocketError errorCode)
			{
				throw new NotImplementedException();
			}

			public int ReceiveFrom(byte[] buffer, int offset, int size, SocketFlags socketFlags, ref EndPoint remoteEP)
			{
				ER ret;

				T_IPV4EP rep = new T_IPV4EP(new byte[T_IPV4EP.length], 0);

				ret = itron.udp_rcv_dat(cepid, rep, new pointer(buffer, offset), size, SOCKET_TIMEOUT);
				if (ret != ER.E_OK) {
					return 0;
				}

				remoteEP = new IPEndPoint(new IPAddress(rep.data), rep.portno);

				return (int)ret;
			}

			public bool Poll(int microSeconds, SelectMode mode)
			{
				switch (mode) {
				case SelectMode.SelectRead:
					break;
				case SelectMode.SelectWrite:
					break;
				case SelectMode.SelectError:
					break;
				}

				return false;
			}

			private void socket_udp_callback(ID cepid, FN fncd, pointer p_parblk)
			{
				switch (fncd) {
				case FN.TFN_UDP_RCV_DAT:
					break;
				}
			}

			public void SetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName, byte[] optionValue)
			{
				ER ret;

				switch (optionLevel) {
				case SocketOptionLevel.Socket:
					switch (optionName) {
					case SocketOptionName.ReuseAddress:
						return;
					case SocketOptionName.Broadcast:
						return;
					}
					break;
				case SocketOptionLevel.Udp:
					ret = itron.udp_set_opt(cepid, (int)optionName, new pointer(optionValue), optionValue.Length);
					if (ret != ER.E_OK) {
						throw new SocketException();
					}
					break;
				}

				throw new NotImplementedException();
			}

			public byte[] GetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName)
			{
				ER ret;
				byte[] optionValue;

				switch (optionLevel) {
				case SocketOptionLevel.Socket:
					switch (optionName) {
					case SocketOptionName.ReuseAddress:
						return new byte[] { 1 };
					case SocketOptionName.Broadcast:
						return new byte[] { 1 };
					}
					break;
				case SocketOptionLevel.Udp:
					optionValue = new byte[1];
					ret = itron.udp_get_opt(cepid, (int)optionName, new pointer(optionValue), optionValue.Length);
					if (ret != ER.E_OK) {
						throw new SocketException();
					}
					return optionValue;
				}

				throw new NotImplementedException();
			}
		}

		private class TCPv6 : IDiscripter
		{
			private Itron itron;
			private ID cepid;
			private ID repid;
			private TCPv6 listner;
			private IPEndPoint localEP;
			private IPEndPoint remoteEP;

			public TCPv6(Itron itron)
			{
				this.itron = itron;
			}

			public TCPv6(TCPv6 listner, ID repid, IPEndPoint remoteEP)
			{
				this.itron = listner.itron;
				this.listner = listner;
				this.cepid = listner.cepid;
				this.repid = repid;
				this.remoteEP = remoteEP;
			}

			public void Bind(EndPoint localEP)
			{
				T_TCP6_CCEP ccep = new T_TCP6_CCEP() {
					cepatr = ATR.TA_NULL,
					callback = socket_tcp6_callback
				};

				ER ret = itron.tcp6_cre_cep(ID.ID_AUTO, ref ccep, out cepid);
				if (ret != ER.E_OK) {
					throw new SocketException();
				}

				this.localEP = (IPEndPoint)localEP;
			}

			public void Connect(EndPoint remoteEP)
			{
				ER ret;

				if (remoteEP.AddressFamily != AddressFamily.InterNetworkV6) {
					throw new SocketException();
				}

				this.remoteEP = (IPEndPoint)remoteEP;

				if (cepid == 0) {
					T_TCP6_CCEP ccep = new T_TCP6_CCEP() {
						cepatr = ATR.TA_NULL,
						callback = socket_tcp6_callback
					};

					ret = itron.tcp6_cre_cep(ID.ID_AUTO, ref ccep, out cepid);
					if (ret != ER.E_OK) {
						throw new SocketException();
					}
				}


				T_IPV6EP lep = new T_IPV6EP(this.localEP.Address.GetAddressBytes(), this.localEP.Port);
				T_IPV6EP rep = new T_IPV6EP(this.remoteEP.Address.GetAddressBytes(), this.remoteEP.Port);
				ret = itron.tcp6_con_cep(cepid, lep, rep, Socket.SOCKET_TIMEOUT);
				if (ret != ER.E_OK) {
					throw new SocketException();
				}
			}

			public void Close()
			{
				ER ret, ret2;

				ret = itron.tcp6_can_cep(cepid, FN.TFN_TCP_ALL);
				ret = itron.tcp6_sht_cep(cepid);
				ret = itron.tcp6_cls_cep(cepid, (repid != 0) ? new TMO(0) : Socket.SOCKET_TIMEOUT);
				ret2 = itron.tcp6_del_cep(cepid);
			}

			public void Listen(int backlog)
			{
				T_TCP6_CREP crep = new T_TCP6_CREP() {
					repatr = ATR.TA_NULL,
					myaddr = new T_IPV6EP(this.localEP.Address.GetAddressBytes(), this.localEP.Port),
				};
				ER ret;

				ret = itron.tcp6_cre_rep(ID.ID_AUTO, ref crep, out repid);
				if (ret != ER.E_OK) {
					throw new SocketException();
				}
			}

			public Socket Accept()
			{
				T_TCP6_CCEP ccep = new T_TCP6_CCEP() {
					cepatr = ATR.TA_NULL,
					callback = socket_tcp6_callback
				};

				ER ret = itron.tcp6_cre_cep(ID.ID_AUTO, ref ccep, out cepid);
				if (ret != ER.E_OK) {
					return null;
				}

				T_IPV6EP rep = new T_IPV6EP(new byte[T_IPV6EP.length], 0);
				ret = itron.tcp6_acp_cep(cepid, repid, rep, TMO.TMO_FEVR);
				if (ret < 0) {
					throw new SocketException();
				}

				IPEndPoint remoteEP = new IPEndPoint(new IPAddress(rep.data), rep.portno);

				return new Socket(new TCPv6(this, repid, remoteEP));
			}

			public int Send(byte[] buffer, int offset, int size, SocketFlags socketFlags, out SocketError errorCode)
			{
				ER ret;

				if ((socketFlags & SocketFlags.OutOfBand) != 0) {
					ret = itron.tcp6_snd_oob(cepid, new pointer(buffer, offset), size, SOCKET_TIMEOUT);
					if (ret != ER.E_OK) {
						errorCode = SocketError.SocketError;
						return 0;
					}

					errorCode = SocketError.Success;
					return (int)ret;
				}

				ret = itron.tcp6_snd_dat(cepid, new pointer(buffer, offset), size, SOCKET_TIMEOUT);
				if (ret != ER.E_OK) {
					errorCode = SocketError.SocketError;
					return 0;
				}

				errorCode = SocketError.Success;
				return (int)ret;
			}

			public int SendTo(byte[] buffer, int offset, int size, SocketFlags socketFlags, EndPoint remoteEP)
			{
				throw new NotImplementedException();
			}

			public int Receive(byte[] buffer, int offset, int size, SocketFlags socketFlags, out SocketError errorCode)
			{
				ER ret;

				if ((socketFlags & SocketFlags.OutOfBand) != 0) {
					ret = itron.tcp6_rcv_oob(cepid, new pointer(buffer, offset), size);
					if (ret != ER.E_OK) {
						errorCode = SocketError.SocketError;
						return 0;
					}

					errorCode = SocketError.Success;
					return (int)ret;
				}

				ret = itron.tcp6_rcv_dat(cepid, new pointer(buffer, offset), size, SOCKET_TIMEOUT);
				if (ret != ER.E_OK) {
					errorCode = SocketError.SocketError;
					return 0;
				}

				errorCode = SocketError.Success;
				return (int)ret;
			}

			public int ReceiveFrom(byte[] buffer, int offset, int size, SocketFlags socketFlags, ref EndPoint remoteEP)
			{
				throw new NotImplementedException();
			}

			public bool Poll(int microSeconds, SelectMode mode)
			{
				throw new NotImplementedException();
			}

			private void socket_tcp6_callback(ID cepid, FN fncd, pointer p_parblk)
			{
				switch (fncd) {
				case FN.TFN_TCP_RCV_BUF:
					break;
				}
			}

			public void SetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName, byte[] optionValue)
			{
				ER ret;

				switch (optionLevel) {
				case SocketOptionLevel.Socket:
					switch (optionName) {
					case SocketOptionName.ReuseAddress:
						return;
					case SocketOptionName.Broadcast:
						return;
					}
					break;
				case SocketOptionLevel.Tcp:
					ret = itron.tcp6_set_opt(cepid, (int)optionName, new pointer(optionValue), optionValue.Length);
					if (ret != ER.E_OK) {
						throw new SocketException();
					}
					break;
				}

				throw new NotImplementedException();
			}

			public byte[] GetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName)
			{
				ER ret;
				byte[] optionValue;

				switch (optionLevel) {
				case SocketOptionLevel.Socket:
					switch (optionName) {
					case SocketOptionName.ReuseAddress:
						return new byte[] { 1 };
					case SocketOptionName.Broadcast:
						return new byte[] { 1 };
					}
					break;
				case SocketOptionLevel.Tcp:
					optionValue = new byte[1];
					ret = itron.tcp6_get_opt(cepid, (int)optionName, new pointer(optionValue), optionValue.Length);
					if (ret != ER.E_OK) {
						throw new SocketException();
					}
					return optionValue;
				}

				throw new NotImplementedException();
			}
		}

		private class UDPv6 : IDiscripter
		{
			private Itron itron;
			private ID cepid;
			private IPEndPoint localEP;

			public UDPv6(Itron itron)
			{
				this.itron = itron;
			}

			public void Bind(EndPoint localEP)
			{
				T_UDP6_CCEP ccep = new T_UDP6_CCEP() {
					cepatr = ATR.TA_NULL,
					callback = socket_udp6_callback
				};

				ER ret = itron.udp6_cre_cep(ID.ID_AUTO, ref ccep, out cepid);
				if (ret != ER.E_OK) {
					throw new SocketException();
				}

				this.localEP = (IPEndPoint)localEP;
			}

			public void Connect(EndPoint remoteEP)
			{
				throw new NotImplementedException();
			}

			public void Close()
			{
				ER ret;

				ret = itron.udp6_del_cep(cepid);
			}

			public void Listen(int backlog)
			{
				throw new NotImplementedException();
			}

			public Socket Accept()
			{
				throw new NotImplementedException();
			}

			public int Send(byte[] buffer, int offset, int size, SocketFlags socketFlags, out SocketError errorCode)
			{
				throw new NotImplementedException();
			}

			public int SendTo(byte[] buffer, int offset, int size, SocketFlags socketFlags, EndPoint remoteEP)
			{
				ER ret;

				IPEndPoint remoteIPEP = (IPEndPoint)remoteEP;

				T_IPV6EP rep = new T_IPV6EP(remoteIPEP.Address.GetAddressBytes(), remoteIPEP.Port);

				ret = itron.udp6_snd_dat(cepid, rep, new pointer(buffer, offset), (ushort)size, SOCKET_TIMEOUT);
				if (ret != ER.E_OK) {
					return 0;
				}

				return (int)ret;
			}

			public int Receive(byte[] buffer, int offset, int size, SocketFlags socketFlags, out SocketError errorCode)
			{
				throw new NotImplementedException();
			}

			public int ReceiveFrom(byte[] buffer, int offset, int size, SocketFlags socketFlags, ref EndPoint remoteEP)
			{
				ER ret;

				T_IPV6EP rep = new T_IPV6EP(new byte[T_IPV6EP.length], 0);

				ret = itron.udp6_rcv_dat(cepid, rep, new pointer(buffer, offset), size, SOCKET_TIMEOUT);
				if (ret != ER.E_OK) {
					return 0;
				}

				remoteEP = new IPEndPoint(new IPAddress(rep.data), rep.portno);

				return (int)ret;
			}

			public bool Poll(int microSeconds, SelectMode mode)
			{
				throw new NotImplementedException();
			}

			private void socket_udp6_callback(ID cepid, FN fncd, pointer p_parblk)
			{
				switch (fncd) {
				case FN.TFN_UDP_RCV_DAT:
					break;
				}
			}

			public void SetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName, byte[] optionValue)
			{
				ER ret;

				switch (optionLevel) {
				case SocketOptionLevel.Socket:
					switch (optionName) {
					case SocketOptionName.ReuseAddress:
						return;
					case SocketOptionName.Broadcast:
						return;
					}
					break;
				case SocketOptionLevel.Udp:
					ret = itron.udp6_set_opt(cepid, (int)optionName, new pointer(optionValue), optionValue.Length);
					if (ret != ER.E_OK) {
						throw new SocketException();
					}
					break;
				}

				throw new NotImplementedException();
			}

			public byte[] GetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName)
			{
				ER ret;
				byte[] optionValue;

				switch (optionLevel) {
				case SocketOptionLevel.Socket:
					switch (optionName) {
					case SocketOptionName.ReuseAddress:
						return new byte[] { 1 };
					case SocketOptionName.Broadcast:
						return new byte[] { 1 };
					}
					break;
				case SocketOptionLevel.Udp:
					optionValue = new byte[1];
					ret = itron.udp6_get_opt(cepid, (int)optionName, new pointer(optionValue), optionValue.Length);
					if (ret != ER.E_OK) {
						throw new SocketException();
					}
					return optionValue;
				}

				throw new NotImplementedException();
			}
		}
	}
}

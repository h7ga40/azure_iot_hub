using System;
using System.Collections.Generic;
using System.Text;
using SimBase;

namespace SimBase
{
	public interface IHub
	{
		void Add(IEtherConnection connection);
		void Remove(IEtherConnection connection);
		void Send(IEtherConnection sender, EtherPacket packet);
		void Terminate();
	}

	public interface IEtherConnection
	{
		IHub Hub { get; set; }
		void Receive(EtherPacket packet);
	}

	public class EtherIF : IDisposable, IEtherConnection, IUnitInterface
	{
		private IUnitSim m_UnitSim;
		private string m_Name;
		private int m_IFKind;
		private ulong m_MacAddr;
		private T_IPV6ADDR m_SelfIPv6;
		private uint m_SelfIP;
		private uint m_SubnetMask;
		private uint m_GateWay;
		private uint m_Broadcast;
		private uint m_TargetIP;
		private ushort m_TargetPortNo;
		private IHub m_Hub;
		static readonly T_IPV6ADDR m_LinkLocal = new T_IPV6ADDR(
			new byte[] { 0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 }, 0);
		static Dictionary<IUnitSim, Dictionary<uint, ulong>> m_ArpTables = new Dictionary<IUnitSim, Dictionary<uint, ulong>>();
		static LinkedList<EtherPacket> m_ResolvingPackets = new LinkedList<EtherPacket>();
		static EtherIF m_DHCPServer;
		static ulong[] m_DHCPIPAddrs = new ulong[10];

		public EtherIF(IUnitSim unitSim, string name, int ifKind, ulong macAddr)
		{
			m_UnitSim = unitSim;
			m_Name = name;
			m_IFKind = ifKind;
			m_MacAddr = macAddr;
			m_SelfIPv6 = GetIPv6Addr(macAddr);

			if (!m_ArpTables.ContainsKey(m_UnitSim)) {
				m_ArpTables[m_UnitSim] = new Dictionary<uint, ulong>();
			}

			if (m_DHCPServer == null) {
				m_DHCPServer = this;
			}
		}

		const byte EUI64_UBIT = 0x02;

		private static T_IPV6ADDR GetIPv6Addr(ulong macAddr)
		{
			byte[] addr = BitConverter.GetBytes(macAddr);
			return new T_IPV6ADDR(new byte[] { 0xfe, 0x80, 0x00,0x00,0x00,0x00,0x00,0x00,
				(byte)(addr[5] ^ EUI64_UBIT), addr[4], addr[3], 0xff, 0xfe, addr[2], addr[1], addr[0] }, 0);
		}

		private static ulong GetMacAddr(T_IPV6ADDR ipv6Addr)
		{
			byte[] addr = new byte[T_IPV6ADDR.length];
			byte[] macAddr = new byte[sizeof(ulong)];

			pointer.memcpy(new pointer(addr, 0), ipv6Addr, T_IPV6ADDR.length);

			macAddr[5] = (byte)(addr[8] ^ EUI64_UBIT);
			macAddr[4] = addr[9];
			macAddr[3] = addr[10];
			macAddr[2] = addr[13];
			macAddr[1] = addr[14];
			macAddr[0] = addr[15];

			return BitConverter.ToUInt64(macAddr, 0);
		}

		public void Init(uint addr, uint subNet)
		{
			m_SelfIP = addr;
			m_SubnetMask = subNet;
			m_GateWay = (addr & subNet) | 1;
			m_Broadcast = (addr & subNet) | (0xFFFFFFFF & ~subNet);
		}

		public T_IPV6ADDR SelfIPv6 { get { return m_SelfIPv6; } }

		public uint SelfIP { get { return m_SelfIP; } }

		public uint SubnetMask { get { return m_SubnetMask; } }

		public uint Broadcast { get { return m_Broadcast; } }

		public uint TargetIP
		{
			get { return m_TargetIP; }
			set { m_TargetIP = value; }
		}

		public ushort TargetPortNo
		{
			get { return m_TargetPortNo; }
			set { m_TargetPortNo = value; }
		}

		public IHub Hub
		{
			get { return m_Hub; }
			set { m_Hub = value; }
		}

		#region IEtherConnection メンバ

		public void Receive(EtherPacket packet)
		{
			// MACアドレスのチェック
			if (((packet.DestinationMac & 0x010000000000UL) == 0)
				&& (packet.DestinationMac != m_MacAddr)
				&& (packet.DestinationMac != 0xFFFFFFFFFFFFUL)) {
				return;
			}

			switch (packet.TypeLen) {
			// ARPパケットの場合
			case EtherPacket.ETHTYPE_ARP:
				ReceiveArp(packet);
				break;
			// IPパケットの場合
			case EtherPacket.ETHTYPE_IP:
				ReceiveIPv4(packet);
				break;
			// IPv6パケットの場合
			case EtherPacket.ETHTYPE_IPV6:
				ReceiveIPv6(packet);
				break;
			}
		}

		#endregion

		private void CheckAndUpdateArpTable(uint ipAddr, ulong macAddr)
		{
			// MACアドレスのチェック
			if ((macAddr == 0xFFFFFFFFFFFFUL)
				|| ((macAddr & 0xFFFFFF800000) == 0x01005E000000UL)) {
				// マルチキャストは登録しない
				return;
			}

			// IPアドレスのチェック
			if ((m_SelfIP & m_SubnetMask) != (ipAddr & m_SubnetMask)) {
				// サブネットが違う場合は登録しない
				return;
			}

			if ((ipAddr == 0xFFFFFFFF) || (ipAddr == m_Broadcast)
				|| ((ipAddr >= 0xE0000000) && (ipAddr <= 0xEFFFFFFF))) {
				// マルチキャストは登録しない
				return;
			}

			UpdateArpTable(ipAddr, macAddr);
		}

		private void UpdateArpTable(uint ipAddr, ulong macAddr)
		{
			m_ArpTables[m_UnitSim][ipAddr] = macAddr;

			for (LinkedListNode<EtherPacket> node = m_ResolvingPackets.First;
				node != null; node = node.Next) {
				EtherPacket packet = node.Value;
				packet.DestinationMac = macAddr;
				IPv4Packet ipv4 = new IPv4Packet(packet, EtherPacket.length);
				if (ipv4.Destination != ipAddr)
					continue;

				m_Hub.Send(this, packet);

				LinkedListNode<EtherPacket> prev = node.Previous;
				m_ResolvingPackets.Remove(node);

				if ((prev == null) && ((prev = m_ResolvingPackets.First) == null))
					break;

				node = prev;
			}
		}

		private void ReceiveArp(EtherPacket packet)
		{
			ARPPacket arp = new ARPPacket(packet, EtherPacket.length);

			// 自分宛のIPアドレスの場合
			if (arp.DstPcAddr != m_SelfIP) {
				return;
			}

			byte[] buf = null;
			EtherPacket response = null;

			switch (arp.Operation) {
			case ArpOperation.Request:
				UpdateArpTable(arp.SrcPcAddr, arp.SrcHwAddr);

				buf = new byte[EtherPacket.length + ARPPacket.Fields.DstPcAddr.offset + ARPPacket.Fields.DstPcAddr.size];
				response = new EtherPacket(buf, 0);
				response.Length = buf.Length;
				response.SourceMac = m_MacAddr;
				response.DestinationMac = arp.SrcHwAddr;
				response.TypeLen = EtherPacket.ETHTYPE_ARP;

				ARPPacket res = new ARPPacket(response, EtherPacket.length);
				res.SetArpPacket(ArpOperation.Reply, m_MacAddr, m_SelfIP, arp.SrcHwAddr, arp.SrcPcAddr);
				break;
			case ArpOperation.Reply:
				UpdateArpTable(arp.SrcPcAddr, arp.SrcHwAddr);
				break;
			}

			if (buf != null)
				m_Hub.Send(this, response);
		}

		private void SendArpRequest(uint dstAddr)
		{
			byte[] buf = new byte[EtherPacket.length + ARPPacket.Fields.DstPcAddr.offset + ARPPacket.Fields.DstPcAddr.size];
			EtherPacket response = new EtherPacket(buf, 0);
			response.Length = buf.Length;
			response.SourceMac = m_MacAddr;
			response.DestinationMac = 0xFFFFFFFFFFFFUL;
			response.TypeLen = EtherPacket.ETHTYPE_ARP;

			ARPPacket res = new ARPPacket(response, EtherPacket.length);
			res.SetArpPacket(ArpOperation.Request, m_MacAddr, m_SelfIP, 0, dstAddr);

			m_Hub.Send(this, response);
		}

		const byte ICMP_ER = 0;
		const byte ICMP_ECHO = 8;

		ushort PP_HTONS(ushort x)
		{
			return (ushort)((((x) & 0xff) << 8) | (((x) & 0xff00) >> 8));
		}

		private void ReceiveIcmp(EtherPacket packet)
		{
			EtherPacket response = null;
			IPv4Packet ipv4 = new IPv4Packet(packet, EtherPacket.length), ripv4;
			ICMPPacket icmp = new ICMPPacket(ipv4, IPv4Packet.length), ricmp;

			switch (icmp.ICMPType) {
			// エコー要求
			case ICMP_ECHO:
				// IPアドレスのチェック
				if (ipv4.Destination != m_SelfIP)
					return;

				CheckAndUpdateArpTable(ipv4.Source, packet.SourceMac);

				byte[] buf = new byte[packet.Length];
				response = new EtherPacket(buf, 0);
				ripv4 = new IPv4Packet(response, EtherPacket.length);
				ricmp = new ICMPPacket(ripv4, IPv4Packet.length);
				response.Length = buf.Length;
				pointer.memcpy(response, packet, response.Length);
				response.SourceMac = m_MacAddr;
				response.DestinationMac = packet.SourceMac;
				ripv4.Source = m_SelfIP;
				ripv4.Destination = ipv4.Source;
				ricmp.ICMPType = ICMP_ER;
				/* adjust the checksum */
				if (ricmp.ICMPChecksum >= PP_HTONS(0xffff - (ICMP_ECHO << 8)))
					ricmp.ICMPChecksum += (ushort)(PP_HTONS(ICMP_ECHO << 8) + 1);
				else
					ricmp.ICMPChecksum += PP_HTONS(ICMP_ECHO << 8);
				ipv4.SetIPCheckSum();
				break;
			}

			if (response != null)
				m_Hub.Send(this, response);
		}

		/// <summary>要求</summary>
		const byte BOOTREQUEST = 1;
		/// <summary>応答</summary>
		const byte BOOTREPLY = 2;

		/// <summary>Discoverパケットの指定</summary>
		const byte DHCP_DISCOVER = 0x01;
		/// <summary>Offerパケットの指定</summary>
		const byte DHCP_OFFER = 0x02;
		/// <summary>Requestパケットの指定</summary>
		const byte DHCP_REQUEST = 0x03;

		const byte DHCP_DECLINE = 0x04;
		/// <summary>ACKパケットの指定</summary>
		const byte DHCP_PACK = 0x05;
		/// <summary>NACKパケットの指定</summary>
		const byte DHCP_PNAK = 0x06;
		/// <summary>Releaseパケットの指定</summary>
		const byte DHCP_RELEASE = 0x07;

		const byte DHCP_INFRM = 0x08;

		private void ReceiveDhcp(EtherPacket packet)
		{
			EtherPacket response = null;
			IPv4Packet ipv4 = new IPv4Packet(packet, EtherPacket.length), ripv4;
			UDPPacket udp = new UDPPacket(ipv4, IPv4Packet.length), rudp;
			DHCPPacket dhcp = new DHCPPacket(udp, UDPPacket.length), offer, pack;

			if (dhcp.OP != BOOTREQUEST)
				return;

			int msgType = -1;
			int pos = EtherPacket.length + IPv4Packet.length + UDPPacket.length + DHCPPacket.length + 4;
			int len;
			while (pos < packet.Length) {
				byte code = packet[pos++];
				if (code == 255)
					break;
				len = packet[pos++];
				switch (code) {
				case 0x35:
					if (len == 1)
						msgType = packet[pos];
					break;
				}
				pos += len;
			}

			// DHCP DISCOVER
			if (msgType == 0x01) {
				// DHCP OFFER
				msgType = 0x02;
			}
			// DHCP REQUEST
			else if (msgType == 0x03) {
				// DHCP PACK
				msgType = 0x05;
			}
			else
				return;

			len = 4/* Magic Code */ + 3/* DHCP OFFER */ + 1/* Stoper */ + 6/*Subnet Mask*/ + 6/*Router Option*/ + 6/*Server Identifier*/;
			byte[] buf = new byte[EtherPacket.length + IPv4Packet.length + UDPPacket.length + DHCPPacket.length + len];
			response = new EtherPacket(buf, 0);
			response.Length = buf.Length;
			response.SourceMac = m_MacAddr;
			response.DestinationMac = packet.SourceMac;
			response.TypeLen = EtherPacket.ETHTYPE_IP;

			ripv4 = new IPv4Packet(response, EtherPacket.length);
			ripv4.Version = 4;
			ripv4.InternetHeaderLength = 5;
			ripv4.TypeOfServices = 0;
			ripv4.DatagramLength = (ushort)(buf.Length - EtherPacket.length);
			ripv4.Identification = (ushort)m_Random.Next(UInt16.MaxValue);
			ripv4.Flags = 2;    // UDPは分割しないでー
			ripv4.FragmentOffset = 0;
			ripv4.TimeToLive = 16;  // 寿命
			ripv4.Protocol = IPv4Packet.IP_PROTO_UDP;
			ripv4.Source = m_SelfIP;
			ripv4.Destination = 0xFFFFFFFF;

			rudp = new UDPPacket(ripv4, IPv4Packet.length);
			rudp.SourcePort = 0x43;
			rudp.DestinationPort = udp.SourcePort;
			rudp.UDPLength = (ushort)(buf.Length - EtherPacket.length - IPv4Packet.length);

			offer = new DHCPPacket(rudp, UDPPacket.length);
			byte[] macaddr = BitConverter.GetBytes(m_MacAddr);
			Array.Reverse(macaddr);
			offer.OP = BOOTREPLY;
			offer.XId = dhcp.XId;
			offer.HwAddrType = 1;
			offer.HwAddrLength = 6;
			offer.YIAddr = GetIPAddr(dhcp.CHAddr);
			offer.SIAddr = m_SelfIP;
			offer.GIAddr = m_GateWay;
			pointer.memcpy(offer.CHAddr, dhcp.CHAddr, 6);

			pos = buf.Length - len;
			// Magic Code
			buf[pos++] = 0x63;
			buf[pos++] = 0x82;
			buf[pos++] = 0x53;
			buf[pos++] = 0x63;
			// DHCP OFFER/PACK
			buf[pos++] = 0x35;
			buf[pos++] = 0x01;
			buf[pos++] = (byte)msgType;
			// Subnet Mask
			buf[pos++] = 0x01;
			buf[pos++] = 0x04;
			buf[pos++] = (byte)((m_SubnetMask >> 24) & 0xFF);
			buf[pos++] = (byte)((m_SubnetMask >> 16) & 0xFF);
			buf[pos++] = (byte)((m_SubnetMask >> 8) & 0xFF);
			buf[pos++] = (byte)(m_SubnetMask & 0xFF);
			// Router Option
			buf[pos++] = 0x03;
			buf[pos++] = 0x04;
			buf[pos++] = (byte)((m_GateWay >> 24) & 0xFF);
			buf[pos++] = (byte)((m_GateWay >> 16) & 0xFF);
			buf[pos++] = (byte)((m_GateWay >> 8) & 0xFF);
			buf[pos++] = (byte)(m_GateWay & 0xFF);
			// Server Identifier
			buf[pos++] = 0x36;
			buf[pos++] = 0x04;
			buf[pos++] = (byte)((m_SelfIP >> 24) & 0xFF);
			buf[pos++] = (byte)((m_SelfIP >> 16) & 0xFF);
			buf[pos++] = (byte)((m_SelfIP >> 8) & 0xFF);
			buf[pos++] = (byte)(m_SelfIP & 0xFF);
			/* Stoper */
			buf[pos++] = 0xFF;

			ripv4.SetIPCheckSum();
			rudp.SetUDPChecksum(rudp.UDPLength,
				new ip_addr(ripv4, IPv4Packet.Fields.Source.offset),
				new ip_addr(ripv4, IPv4Packet.Fields.Destination.offset));

			if (response != null)
				m_Hub.Send(this, response);
		}

		private uint GetIPAddr(array<byte> array)
		{
			ulong smac = ((ulong)array[0] << 40) + ((ulong)array[1] << 32) + ((ulong)array[2] << 24)
				+ ((ulong)array[3] << 16) + ((ulong)array[4] << 8) + (ulong)array[5];

			for (int i = 0; i < m_DHCPIPAddrs.Length; i++) {
				ulong mac = m_DHCPIPAddrs[i];
				if (mac == smac)
					return m_SelfIP + (uint)i + 1u;
			}
			for (int i = 0; i < m_DHCPIPAddrs.Length; i++) {
				ulong mac = m_DHCPIPAddrs[i];
				if (mac == 0) {
					m_DHCPIPAddrs[i] = smac;
					return m_SelfIP + (uint)i + 1u;
				}
			}
			return 0;
		}

		private void ReceiveIPv4(EtherPacket packet)
		{
			IPv4Packet ipv4 = new IPv4Packet(packet, EtherPacket.length);
			bool multicast = false;

			// MACアドレスのチェック（IPv4マルチキャスト）
			if ((packet.DestinationMac & 0xFFFFFF800000) == 0x01005E000000UL) {

				if ((ipv4.Destination >= 0xE0000000) && (ipv4.Destination <= 0xEFFFFFFF)) {
					// MACアドレスの下位23ビットとIPアドレスの下位23ビットが同じ場合は、対象マルチキャストアドレス
					if ((packet.DestinationMac & 0x0000007FFFFF) != (ipv4.Destination & 0x0000007FFFFF))
						return;

					multicast = true;
				}
			}
			// ブロードキャストも受け取る
			else if (packet.DestinationMac == 0xFFFFFFFFFFFF) {
				multicast = true;
			}

			if (ipv4.Protocol == IPv4Packet.IP_PROTO_ICMP) {
				ReceiveIcmp(packet);
				return;
			}

			// IPアドレスのチェック
			if ((ipv4.Destination != m_SelfIP) && (ipv4.Destination != m_Broadcast)
				 && !multicast)
				return;

			CheckAndUpdateArpTable(ipv4.Source, packet.SourceMac);

			if (ipv4.Protocol == IPv4Packet.IP_PROTO_UDP) {
				UDPPacket udp = new UDPPacket(ipv4, IPv4Packet.length);

				// DHCPサーバー処理
				if ((udp.DestinationPort == 0x43) && (m_DHCPServer == this)) {
					ReceiveDhcp(packet);
					return;
				}
			}

			if ((ipv4.Protocol != IPv4Packet.IP_PROTO_UDP)
				&& (ipv4.Protocol != IPv4Packet.IP_PROTO_UDPLITE)
				&& (ipv4.Protocol != IPv4Packet.IP_PROTO_TCP))
				return;

			int len = packet.Length - EtherPacket.length - IPv4Packet.length;
			if (len < 0)
				return;

			byte[] data = Itron.GetIp4Packet(new pointer(ipv4, IPv4Packet.length), len,
				new ip_addr(ipv4.Source), new ip_addr(ipv4.Destination), ipv4.Protocol);

			m_UnitSim.Input(m_IFKind, data);
		}

		private void ReceiveIPv6(EtherPacket packet)
		{
			IPv6Packet ipv6 = new IPv6Packet(packet, EtherPacket.length);
			bool multicast = false;

			// MACアドレスのチェック（IPv6マルチキャスト）
			if ((packet.DestinationMac & 0xFFFF00000000UL) == 0x333300000000UL) {
				multicast = true;
			}

			if (ipv6.NextHeader == IPv6Packet.IP_PROTO_ICMPV6) {
				ReceiveICMPv6(packet);
				return;
			}

			// IPアドレスのチェック
			if ((pointer.memcmp(ipv6.DestinationAddress, m_SelfIPv6, T_IPV6ADDR.length) != 0)
				&& (pointer.memcmp(ipv6.DestinationAddress, m_LinkLocal, T_IPV6ADDR.length) != 0)
				&& !multicast)
				return;

			if ((ipv6.NextHeader != IPv4Packet.IP_PROTO_UDP)
				&& (ipv6.NextHeader != IPv4Packet.IP_PROTO_UDPLITE)
				&& (ipv6.NextHeader != IPv4Packet.IP_PROTO_TCP))
				return;

			int len = packet.Length - EtherPacket.length - IPv6Packet.length;
			if (len < 0)
				return;

			byte[] data = Itron.GetIp4Packet(new pointer(ipv6, IPv6Packet.length), len,
				new ip_addr(ipv6.SourceAddress), new ip_addr(ipv6.DestinationAddress), ipv6.NextHeader);

			m_UnitSim.Input(m_IFKind, data);
		}

		private void ReceiveICMPv6(EtherPacket packet)
		{
			IPv6Packet ipv6 = new IPv6Packet(packet, EtherPacket.length);
			ICMPv6Packet icmp = new ICMPv6Packet(ipv6, IPv6Packet.length);

			switch (icmp.ICMPType) {
			case 135:
				NeighborSolicitationPacket ns = new NeighborSolicitationPacket(icmp, ICMPv6Packet.length);
				if (pointer.memcmp(ns.TargetAddress, m_SelfIPv6, T_IPV6ADDR.length) != 0)
					break;

				ushort len = ICMPv6Packet.length + NeighborAdvertisementPacket.length + ICMPv6OptionPacket.length + EthernetAddressPacket.length;
				byte[] data = new byte[EtherPacket.length + IPv6Packet.length + len];
				EtherPacket response = new EtherPacket(data, 0);
				IPv6Packet ripv6 = new IPv6Packet(response, EtherPacket.length);
				ICMPv6Packet ricmp = new ICMPv6Packet(ripv6, IPv6Packet.length);
				NeighborAdvertisementPacket rna = new NeighborAdvertisementPacket(ricmp, ICMPv6Packet.length);
				ICMPv6OptionPacket ropt = new ICMPv6OptionPacket(rna, NeighborAdvertisementPacket.length);
				EthernetAddressPacket rlla = new EthernetAddressPacket(ropt, ICMPv6OptionPacket.length);

				response.Length = data.Length;
				response.DestinationMac = packet.SourceMac;
				response.SourceMac = m_MacAddr;
				response.TypeLen = EtherPacket.ETHTYPE_IPV6;
				ripv6.Version = 6;
				ripv6.TrafficClass = 0;
				ripv6.FlowLabel = 0;
				ripv6.PaylaodLength = len;
				ripv6.NextHeader = IPv6Packet.IP_PROTO_ICMPV6;
				ripv6.HopLimit = 255;
				pointer.memcpy(ripv6.SourceAddress, m_SelfIPv6, T_IPV6ADDR.length);
				pointer.memcpy(ripv6.DestinationAddress, ipv6.SourceAddress, T_IPV6ADDR.length);
				ricmp.ICMPType = 136;
				ricmp.ICMPCode = 0;
				rna.RouterFlag = 0;
				rna.SolicitedFlag = 1;
				rna.OverrideFlag = 1;
				rna.Reserved = 0;
				pointer.memcpy(rna.TargetAddress, m_SelfIPv6, T_IPV6ADDR.length);
				ropt.Type = 2;
				ropt.Length = 1;
				byte[] macAddr = BitConverter.GetBytes(m_MacAddr);
				Array.Reverse(macAddr);
				pointer.memcpy(rlla.LinkLayerAddress, new pointer(macAddr, 2), 6);

				ricmp.SetICMPChecksum(ripv6.PaylaodLength,
					new ip6_addr(ripv6, IPv6Packet.Fields.SourceAddress.offset),
					new ip6_addr(ripv6, IPv6Packet.Fields.DestinationAddress.offset));

				m_Hub.Send(this, response);
				break;
			}
		}

		#region IDisposable メンバ

		public void Dispose()
		{
		}

		#endregion

		public void Terminate()
		{
		}

		#region IUnitInterface メンバ

		public string InterfaceName
		{
			get { return m_Name; }
		}

		public IUnitSim UnitSim
		{
			get { return m_UnitSim; }
		}

		public int IFKind
		{
			get { return m_IFKind; }
		}

		public void Push(byte[] data)
		{
			if (data[0] == 4) {
				pointer ipv4;
				int len;
				ip_addr src, dst;
				byte proto;

				ipv4 = Itron.CastIp4Packet(data, out len, out src, out dst, out proto);

				Push4(ipv4, len, src, dst, proto);
			}
			else if (data[0] == 6) {
				pointer ipv6;
				int len;
				ip6_addr src, dst;
				byte proto;

				ipv6 = Itron.CastIp6Packet(data, out len, out src, out dst, out proto);

				Push6(ipv6, len, src, dst, proto);
			}
		}

		#endregion

		Random m_Random = new Random();

		private void Push4(pointer data, int len, ip_addr src, ip_addr dst, byte proto)
		{
			ulong dmac;
			bool resolve = false;

			// マルチキャストの場合
			if ((dst.addr >= 0xE0000000) && (dst.addr <= 0xEFFFFFFF)) {
				dmac = 0x01005E000000UL | (dst.addr & 0x7FFFFF);
			}
			// ブロードキャストの場合
			else if ((dst.addr == 0xFFFFFFFF) || ((dst.addr & m_SubnetMask) == ~m_SubnetMask)) {
				dmac = 0xFFFFFFFFFFFFUL;
			}
			// IPアドレスでARPテーブルから検索
			else if (!m_ArpTables[m_UnitSim].TryGetValue(dst.addr, out dmac)) {
				dmac = 0;
				resolve = true;
			}

			byte[] buf = new byte[EtherPacket.length + IPv4Packet.length + len];

			EtherPacket packet = new EtherPacket(buf, 0);
			packet.Length = buf.Length;
			packet.DestinationMac = dmac;
			packet.SourceMac = m_MacAddr;
			packet.TypeLen = EtherPacket.ETHTYPE_IP;

			IPv4Packet ipv4 = new IPv4Packet(packet, EtherPacket.length);
			ipv4.Version = 4;
			ipv4.InternetHeaderLength = 5;
			ipv4.TypeOfServices = 0;
			ipv4.DatagramLength = (ushort)(len + IPv4Packet.length);
			ipv4.Identification = (ushort)m_Random.Next(UInt16.MaxValue);
			ipv4.Flags = 2; // UDPは分割しないでー
			ipv4.FragmentOffset = 0;
			ipv4.TimeToLive = 16;   // 寿命
			ipv4.Protocol = proto;
			ipv4.Source = src.addr;
			ipv4.Destination = dst.addr;

			pointer.memcpy(new pointer(ipv4, IPv4Packet.length), data, len);

			ipv4.SetIPCheckSum();

			if (!resolve) {
				m_Hub.Send(this, packet);
			}
			else {
				m_ResolvingPackets.AddLast(packet);
				SendArpRequest(dst.addr);
			}
		}

		private void Push6(pointer data, int len, ip6_addr src, ip6_addr dst, byte proto)
		{
			ulong dmac;

			// ブロードキャストの場合
			if (pointer.memcmp(dst.addr, m_LinkLocal, T_IPV6ADDR.length) == 0) {
				dmac = 0x333300000001UL;
			}
			// IPアドレスからMACアドレスを作成
			else {
				dmac = GetMacAddr(new T_IPV6ADDR(dst.addr));
			}

			byte[] buf = new byte[EtherPacket.length + IPv6Packet.length + len];

			EtherPacket packet = new EtherPacket(buf, 0);
			packet.Length = buf.Length;
			packet.DestinationMac = dmac;
			packet.SourceMac = m_MacAddr;
			packet.TypeLen = EtherPacket.ETHTYPE_IPV6;

			IPv6Packet ipv6 = new IPv6Packet(packet, EtherPacket.length);
			ipv6.Version = 6;
			ipv6.TrafficClass = 0;
			ipv6.FlowLabel = 0;
			ipv6.PaylaodLength = (ushort)len;
			ipv6.NextHeader = proto;
			ipv6.HopLimit = 1;
			pointer.memcpy(ipv6.SourceAddress, m_SelfIPv6, T_IPV6ADDR.length);
			pointer.memcpy(ipv6.DestinationAddress, dst.addr, T_IPV6ADDR.length);

			pointer.memcpy(new pointer(ipv6, IPv6Packet.length), data, len);

			m_Hub.Send(this, packet);
		}

		class NeighborCache
		{
			ip6_addr m_TargetAddr = new ip6_addr(new byte[16]);
			ulong m_MacAddr;
			bool m_IsRouter;
			public enum TState
			{
				/// <summary>不完全</summary>
				INCOMPLETE,
				/// <summary>到達可能</summary>
				REACHABLE,
				/// <summary>期限切れ</summary>
				STALE,
				/// <summary>遅延</summary>
				DELAY,
				/// <summary>探索中</summary>
				PROBE
			}
			TState m_State;
			long m_Timer;
			Queue<EtherPacket> m_Queue = new Queue<EtherPacket>();
			EtherIF m_Owner;

			public NeighborCache(EtherIF owner, EtherPacket packet)
			{
				m_Owner = owner;
				IPv6Packet ipv6 = new IPv6Packet(packet, EtherPacket.length);
				pointer.memcpy(m_TargetAddr, ipv6.DestinationAddress, ip6_addr.length);

				m_Queue.Enqueue(packet);
				SendNeighborSolicitation();
			}

			public ip6_addr TargetAddr { get { return m_TargetAddr; } }

			public ulong MacAddr { get { return m_MacAddr; } }

			public bool IsRouter { get { return m_IsRouter; } }

			public TState State { get { return m_State; } }

			private void SendNeighborSolicitation()
			{
				ushort len = ICMPv6Packet.length + NeighborSolicitationPacket.length + ICMPv6OptionPacket.length + EthernetAddressPacket.length;
				byte[] data = new byte[EtherPacket.length + IPv6Packet.length + len];
				EtherPacket response = new EtherPacket(data, 0);
				IPv6Packet ripv6 = new IPv6Packet(response, EtherPacket.length);
				ICMPv6Packet ricmp = new ICMPv6Packet(ripv6, IPv6Packet.length);
				NeighborSolicitationPacket rna = new NeighborSolicitationPacket(ricmp, ICMPv6Packet.length);
				ICMPv6OptionPacket ropt = new ICMPv6OptionPacket(rna, NeighborSolicitationPacket.length);
				EthernetAddressPacket rlla = new EthernetAddressPacket(ropt, ICMPv6OptionPacket.length);

				response.Length = data.Length;
				response.DestinationMac = BitConverter.ToUInt64(new byte[] {
					0x00, 0x00, 0x33, 0x33, m_TargetAddr.addr[12], m_TargetAddr.addr[13], m_TargetAddr.addr[14], m_TargetAddr.addr[15] }, 0);
				response.SourceMac = m_Owner.m_MacAddr;
				response.TypeLen = EtherPacket.ETHTYPE_IPV6;
				ripv6.Version = 6;
				ripv6.TrafficClass = 0;
				ripv6.FlowLabel = 0;
				ripv6.PaylaodLength = len;
				ripv6.NextHeader = IPv6Packet.IP_PROTO_ICMPV6;
				ripv6.HopLimit = 255;
				pointer.memcpy(ripv6.SourceAddress, m_Owner.m_SelfIPv6, T_IPV6ADDR.length);
				pointer.memcpy(ripv6.DestinationAddress, EtherIF.m_LinkLocal, T_IPV6ADDR.length);
				ricmp.ICMPType = 135;
				ricmp.ICMPCode = 0;
				rna.Reserved = 0;
				pointer.memcpy(rna.TargetAddress, m_TargetAddr, T_IPV6ADDR.length);
				ropt.Type = 1;
				ropt.Length = 1;
				byte[] macAddr = BitConverter.GetBytes(m_MacAddr);
				Array.Reverse(macAddr);
				pointer.memcpy(rlla.LinkLayerAddress, new pointer(macAddr, 2), 6);

				ricmp.SetICMPChecksum(ripv6.PaylaodLength,
					new ip6_addr(ripv6, IPv6Packet.Fields.SourceAddress.offset),
					new ip6_addr(ripv6, IPv6Packet.Fields.DestinationAddress.offset));

				m_Owner.Hub.Send(m_Owner, response);

				m_State = TState.INCOMPLETE;
				m_Timer = SimBase.SimRunner.Frequency;  // 1[s]
			}

			public void Send(EtherPacket packet)
			{
				switch (m_State) {
				case TState.INCOMPLETE:
					m_Queue.Enqueue(packet);
					break;
				case TState.REACHABLE:
					packet.DestinationMac = m_MacAddr;
					m_Owner.Hub.Send(m_Owner, packet);
					break;
				case TState.STALE:
					packet.DestinationMac = m_MacAddr;
					m_Owner.Hub.Send(m_Owner, packet);
					m_State = TState.DELAY;
					m_Timer = SimBase.SimRunner.Frequency;  // 1[s]
					break;
				case TState.DELAY:
					m_Queue.Enqueue(packet);
					break;
				case TState.PROBE:
					m_Queue.Enqueue(packet);
					SendNeighborSolicitation();
					break;
				}
			}

			public long GetTimer()
			{
				return m_Timer;
			}

			public void Progress(long interval)
			{
				m_Timer -= interval;
				if (m_Timer <= 0) {
					m_Timer = 0;
				}
			}

			public void CallTimeOut()
			{
				if (m_Timer != 0)
					return;

				switch (m_State) {
				case TState.INCOMPLETE:
					break;
				case TState.REACHABLE:
					break;
				case TState.STALE:
					break;
				case TState.DELAY:
					break;
				case TState.PROBE:
					break;
				}
			}

			public void Receive(EtherPacket packet)
			{
				if (packet.DestinationMac != m_Owner.m_MacAddr)
					return;

				if (packet.SourceMac != m_MacAddr)
					return;

				if (packet.TypeLen != EtherPacket.ETHTYPE_IPV6)
					return;

				IPv6Packet ipv6 = new IPv6Packet(packet, EtherPacket.length);
				if (ipv6.Version != 6)
					return;

				if (ipv6.NextHeader != IPv6Packet.IP_PROTO_ICMPV6)
					return;

				if (pointer.memcmp(ipv6.SourceAddress, m_TargetAddr, T_IPV6ADDR.length) != 0)
					return;

				if (pointer.memcmp(ipv6.DestinationAddress, m_Owner.m_SelfIPv6, T_IPV6ADDR.length) != 0)
					return;

				ICMPv6Packet icmp = new ICMPv6Packet(ipv6, IPv6Packet.length);
				if (icmp.ICMPType != 136)
					return;

				NeighborAdvertisementPacket na = new NeighborAdvertisementPacket(icmp, ICMPv6Packet.length);

				if (pointer.memcmp(na.TargetAddress, m_Owner.m_SelfIPv6, T_IPV6ADDR.length) != 0)
					return;

				int len, end = ipv6.PaylaodLength;
				int pos = ICMPv6Packet.length + NeighborAdvertisementPacket.length;
				for (ICMPv6OptionPacket opt = new ICMPv6OptionPacket(na, pos); pos < end;
					len = 8 * opt.Length, pos += len, opt = new ICMPv6OptionPacket(na, pos)) {
					if ((opt.Type != 2) || (opt.Length != 1))
						continue;

					EthernetAddressPacket lla = new EthernetAddressPacket(opt, ICMPv6OptionPacket.length);
					byte[] macAddr = new byte[sizeof(uint)];
					pointer.memcpy(new pointer(macAddr, 2), lla.LinkLayerAddress, 6);
					Array.Reverse(macAddr);

					if ((m_State == TState.INCOMPLETE) || (m_State == TState.PROBE)
						|| ((na.SolicitedFlag == 0) && (na.OverrideFlag == 1))) {
						m_MacAddr = BitConverter.ToUInt64(macAddr, 0);
						m_IsRouter = na.RouterFlag != 0;

						while (m_Queue.Count > 0) {
							EtherPacket item = m_Queue.Dequeue();
							item.DestinationMac = m_MacAddr;
							m_Owner.Hub.Send(m_Owner, item);
						}

						m_State = TState.REACHABLE;
						m_Timer = 10 * SimBase.SimRunner.Frequency; // 10[s]
					}
					break;
				}
			}
		}
	}
}

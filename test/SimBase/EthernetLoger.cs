using System;
using System.Collections.Generic;

namespace SimBase
{
	public class EthernetLoger : IEtherConnection
	{
		IHub m_Hub;
		List<EthernetLogItem> m_EthernetLog = new List<EthernetLogItem>();
		bool m_Changed;
		ulong _macaddr;

		public EthernetLoger()
		{
			Random random = new Random();
			_macaddr = (ulong)(random.NextDouble() * 0x1000000000000UL) & 0xFEFFFFFFFFFFUL | 0x020000000000UL;
		}

		public IHub Hub {
			get { return m_Hub; }
			set { m_Hub = value; }
		}

		public List<EthernetLogItem> EthernetLog {
			get { return m_EthernetLog; }
		}

		public bool Changed {
			get { return m_Changed; }
			set { m_Changed = value; }
		}

		public int ID {
			get {
				return (int)_macaddr;
			}
		}

		public void Receive(EtherPacket packet)
		{
			byte[] data = new byte[packet.Length];
			packet.Read(data, 0, data.Length);
			m_EthernetLog.Add(new EthernetLogItem(DateTime.Now, new EtherPacket(data)));
			m_Changed = true;
		}
	}

	public class EthernetLogItem
	{
		DateTime m_Time;
		EtherPacket m_Data;

		public EthernetLogItem(DateTime time, EtherPacket data)
		{
			m_Time = time;
			m_Data = data;
		}

		public string Time {
			get { return m_Time.ToString("yyyy/MM/dd HH:mm:ss.ffff"); }
		}

		public string Destination {
			get { return BitConverter.ToString(m_Data.data, m_Data.offset, 6).Replace('-', ':'); }
		}

		public string Source {
			get { return BitConverter.ToString(m_Data.data, m_Data.offset + 6, 6).Replace('-', ':'); }
		}

		public string Type {
			get {
				ushort type = lwip.lwip_ntohs(BitConverter.ToUInt16(m_Data.data, m_Data.offset + 12));
				switch (type) {
				case EtherPacket.ETHTYPE_ARP:
					return "ARP";
				case EtherPacket.ETHTYPE_IP: {
						IPv4Packet ipv4 = new IPv4Packet(m_Data, EtherPacket.length);
						switch (ipv4.Protocol) {
						case IPv4Packet.IP_PROTO_ICMP:
							return "ICMP";
						case IPv4Packet.IP_PROTO_IGMP:
							return "IGMP";
						case IPv4Packet.IP_PROTO_UDP: {
								UDPPacket udp = new UDPPacket(ipv4, IPv4Packet.length);
								if (udp.DestinationPort == 0x43) {
									return "DHCP";
								}
								return "UDP";
							}
						case IPv4Packet.IP_PROTO_UDPLITE:
							return "UDPLITE";
						case IPv4Packet.IP_PROTO_TCP:
							return "TCP";
						default:
							return "IPv4";
						}
					}
				case EtherPacket.ETHTYPE_IPV6: {
						IPv6Packet ipv6 = new IPv6Packet(m_Data, EtherPacket.length);
						switch (ipv6.NextHeader) {
						case IPv6Packet.IP_PROTO_ICMPV6:
							return "ICMPv6";
						case IPv4Packet.IP_PROTO_UDP:
							return "UDP";
						case IPv4Packet.IP_PROTO_UDPLITE:
							return "UDPLITE";
						case IPv4Packet.IP_PROTO_TCP:
							return "TCP";
						default:
							return "IPv6";
						}
					}
				default:
					return type.ToString("X4");
				}
			}
		}

		public string Data {
			get { return BitConverter.ToString(m_Data.data, m_Data.offset + 16).Replace("-", " "); }
		}

		public EtherPacket Packet { get { return m_Data; } }
	}
}

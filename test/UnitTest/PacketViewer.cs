using SimEnv;
using System;
using System.Collections.Generic;
using System.Text;
using uITron3;

namespace SystemSim
{
	public class PacketViewer
	{
		private EtherPacket m_Packet;
		private List<PacketViewItem> m_PacketFields = new List<PacketViewItem>();

		public PacketViewer()
		{
		}

		public EtherPacket Packet => m_Packet;

		public List<PacketViewItem> PacketFields => m_PacketFields;

		public void SetPacket(EtherPacket packet)
		{
			m_Packet = packet;
			m_PacketFields.Clear();

			switch (m_Packet.TypeLen) {
			case EtherPacket.ETHTYPE_ARP:
				AnalyzeArp(new ARPPacket(packet, EtherPacket.length), packet.Length - EtherPacket.length);
				break;
			case EtherPacket.ETHTYPE_IP:
				AnalyzeIp(new IPv4Packet(packet, EtherPacket.length), packet.Length - EtherPacket.length);
				break;
			case EtherPacket.ETHTYPE_IPV6:
				AnalyzeIpv6(new IPv6Packet(packet, EtherPacket.length), packet.Length - EtherPacket.length);
				break;
			default:
				m_PacketFields.Add(new PacketViewItem("データ", "", packet, 0, packet.Length));
				break;
			}
		}

		public static string GetHwAddr(ulong p)
		{
			var t = BitConverter.GetBytes(p);
			return String.Format("{0:X2}:{1:X2}:{2:X2}:{3:X2}:{4:X2}:{5:X2}", t[5], t[4], t[3], t[2], t[1], t[0]);
		}

		public static string GetPcAddr(uint p)
		{
			var t = BitConverter.GetBytes(p);
			return String.Format("{0}.{1}.{2}.{3}", t[3], t[2], t[1], t[0]);
		}

		public static string GetHardwareType(ushort p)
		{
			switch (p) {
			case 1: return "Ethernet (10Mb)";
			case 2: return "Experimental Ethernet (3Mb)";
			case 3: return "Amateur Radio AX.25";
			case 4: return "Proteon ProNET Token Ring";
			case 5: return "Chaos";
			case 6: return "IEEE 802 Networks";
			case 7: return "ARCNET";
			case 8: return "Hyperchannel";
			case 9: return "Lanstar";
			case 10: return "Autonet Short Address";
			case 11: return "LocalTalk";
			case 12: return "LocalNet (IBM PCNet or SYTEK LocalNET)";
			case 13: return "Ultra link";
			case 14: return "SMDS";
			case 15: return "Frame Relay";
			case 16: return "Asynchronous Transmission Mode (ATM)";
			case 17: return "HDLC";
			case 18: return "Fibre Channel";
			case 19: return "Asynchronous Transmission Mode (ATM)";
			case 20: return "Serial Line";
			case 21: return "Asynchronous Transmission Mode (ATM)";
			default: return "???";
			}
		}

		public static string GetProtocolType(ushort p)
		{
			if (((p >= 1) && (p <= 3)) || ((p >= 10) && (p <= 14)))
				return "Unassigned";

			switch (p) {
			case 0: return "Reserved";
			case 4: return "Internet Protocol";
			case 5: return "ST Datagram Mode";
			case 6: return "Simple Internet Protocol";
			case 7: return "TP/IX: The Next Internet";
			case 8: return "The P Internet Protocol";
			case 9: return "TUBA";
			case 15: return "Reserved";
			default: return "???";
			}
		}

		public static string GetOperation(ArpOperation p)
		{
			switch ((ushort)p) {
			case 1: return "REQUEST";
			case 2: return "REPLY";
			case 3: return "request Reverse";
			case 4: return "reply Reverse";
			case 5: return "DRARP-Request";
			case 6: return "DRARP-Reply";
			case 7: return "DRARP-Error";
			case 8: return "InARP-Request";
			case 9: return "InARP-Reply";
			case 10: return "ARP-NAK";
			default: return "???";
			}
		}

		public void AnalyzeArp(ARPPacket packet, int length)
		{
			m_PacketFields.Add(new PacketViewItem("ハードウェアタイプ", GetHardwareType(packet.HardwareType), packet, ARPPacket.Fields.HardwareType.offset, ARPPacket.Fields.HardwareType.size));
			m_PacketFields.Add(new PacketViewItem("プロトコルタイプ", GetProtocolType(packet.ProtocolType), packet, ARPPacket.Fields.ProtocolType.offset, ARPPacket.Fields.ProtocolType.size));
			m_PacketFields.Add(new PacketViewItem("ハードウェアアドレス長", packet.HwAddrLength.ToString(), packet, ARPPacket.Fields.HwAddrLength.offset, ARPPacket.Fields.HwAddrLength.size));
			m_PacketFields.Add(new PacketViewItem("プロトコルアドレス長", packet.PcAddrLength.ToString(), packet, ARPPacket.Fields.PcAddrLength.offset, ARPPacket.Fields.PcAddrLength.size));
			m_PacketFields.Add(new PacketViewItem("操作種別", GetOperation(packet.Operation), packet, ARPPacket.Fields.Operation.offset, ARPPacket.Fields.Operation.size));
			m_PacketFields.Add(new PacketViewItem("送信元MACアドレス", GetHwAddr(packet.SrcHwAddr), packet, ARPPacket.Fields.SrcHwAddr.offset, ARPPacket.Fields.SrcHwAddr.size));
			m_PacketFields.Add(new PacketViewItem("送信元IPアドレス", GetPcAddr(packet.SrcPcAddr), packet, ARPPacket.Fields.SrcPcAddr.offset, ARPPacket.Fields.SrcPcAddr.size));
			m_PacketFields.Add(new PacketViewItem("宛先MACアドレス", GetHwAddr(packet.DstHwAddr), packet, ARPPacket.Fields.DstHwAddr.offset, ARPPacket.Fields.DstHwAddr.size));
			m_PacketFields.Add(new PacketViewItem("宛先IPアドレス", GetPcAddr(packet.DstPcAddr), packet, ARPPacket.Fields.DstPcAddr.offset, ARPPacket.Fields.DstPcAddr.size));
		}

		public void AnalyzeIp(IPv4Packet packet, int length)
		{
			m_PacketFields.Add(new PacketViewItem("IPヴァージョン", packet.Version.ToString(), packet, IPv4Packet.Fields.Version.offset, IPv4Packet.Fields.Version.size));
			m_PacketFields.Add(new PacketViewItem("IPヘッダー長", (4 * packet.InternetHeaderLength).ToString(), packet, IPv4Packet.Fields.InternetHeaderLength.offset, IPv4Packet.Fields.InternetHeaderLength.size));
			m_PacketFields.Add(new PacketViewItem("サービスタイプ", GetTypeOfServices(packet.TypeOfServices), packet, IPv4Packet.Fields.TypeOfServices.offset, IPv4Packet.Fields.TypeOfServices.size));
			m_PacketFields.Add(new PacketViewItem("データグラム長", packet.DatagramLength.ToString(), packet, IPv4Packet.Fields.DatagramLength.offset, IPv4Packet.Fields.DatagramLength.size));
			m_PacketFields.Add(new PacketViewItem("データ識別番号", packet.Identification.ToString(), packet, IPv4Packet.Fields.Identification.offset, IPv4Packet.Fields.Identification.size));
			m_PacketFields.Add(new PacketViewItem("フラグ", GetFlags(packet.Flags), packet, IPv4Packet.Fields.Flags.offset, IPv4Packet.Fields.Flags.size));
			m_PacketFields.Add(new PacketViewItem("フラグメントオフセット", packet.FragmentOffset.ToString(), packet, IPv4Packet.Fields.FragmentOffset.offset, IPv4Packet.Fields.FragmentOffset.size));
			m_PacketFields.Add(new PacketViewItem("TTL", packet.TimeToLive.ToString(), packet, IPv4Packet.Fields.TimeToLive.offset, IPv4Packet.Fields.TimeToLive.size));
			m_PacketFields.Add(new PacketViewItem("プロトコル", GetProtocol(packet.Protocol), packet, IPv4Packet.Fields.Protocol.offset, IPv4Packet.Fields.Protocol.size));
			m_PacketFields.Add(new PacketViewItem("ヘッダーチェックサム", packet.HeaderChecksum.ToString("X"), packet, IPv4Packet.Fields.HeaderChecksum.offset, IPv4Packet.Fields.HeaderChecksum.size));
			m_PacketFields.Add(new PacketViewItem("送信元IPアドレス", GetPcAddr(packet.Source), packet, IPv4Packet.Fields.Source.offset, IPv4Packet.Fields.Source.size));
			m_PacketFields.Add(new PacketViewItem("宛先IPアドレス", GetPcAddr(packet.Destination), packet, IPv4Packet.Fields.Destination.offset, IPv4Packet.Fields.Destination.size));

			switch (packet.Protocol) {
			case IPv4Packet.IP_PROTO_ICMP:
				AnalyzeIcmp(new ICMPPacket(packet, IPv4Packet.length), length - IPv4Packet.length);
				break;
			case IPv4Packet.IP_PROTO_UDP:
				AnalyzeUdp(new UDPPacket(packet, IPv4Packet.length), length - IPv4Packet.length);
				break;
			case IPv4Packet.IP_PROTO_TCP:
				AnalyzeTcp(new TCPPacket(packet, IPv4Packet.length), length - IPv4Packet.length);
				break;
			default:
				m_PacketFields.Add(new PacketViewItem("データ", "サイズ：" + (length - IPv4Packet.length).ToString(), packet, IPv4Packet.length, length - IPv4Packet.length));
				break;
			}
		}

		private string GetTypeOfServices(byte p)
		{
			return p.ToString("X2");
		}

		private string GetFlags(byte p)
		{
			var temp = new List<string>();

			if ((p & 1) != 0)
				temp.Add("More Fragment");
			if ((p & 2) != 0)
				temp.Add("Don't Fragment");

			return String.Join("|", temp);
		}

		private string GetProtocol(byte p)
		{
			switch (p) {
			case 0: return "Internet protocol";
			case 1: return "Internet control message protocol";
			case 3: return "Gateway-gateway protocol";
			case 6: return "Transmission control protocol";
			case 8: return "Exterior gateway protocol";
			case 12: return "PARC universal packet protocol";
			case 17: return "User datagram protocol";
			case 20: return "Host monitoring protocol";
			case 22: return "Xerox NS IDP";
			case 27: return "\"reliable datagram\" protocol";
			case 66: return "MIT remote virtual disk";
			default: return "???";
			}
		}

		private void AnalyzeIpv6(IPv6Packet packet, int length)
		{
			m_PacketFields.Add(new PacketViewItem("IPヴァージョン", packet.Version.ToString(), packet, IPv6Packet.Fields.Version.offset, IPv6Packet.Fields.Version.size));
			m_PacketFields.Add(new PacketViewItem("トラフィッククラス", packet.TrafficClass.ToString(), packet, IPv6Packet.Fields.TrafficClass.offset, IPv6Packet.Fields.TrafficClass.size));
			m_PacketFields.Add(new PacketViewItem("フローラベル", packet.FlowLabel.ToString(), packet, IPv6Packet.Fields.FlowLabel.offset, IPv6Packet.Fields.FlowLabel.size));
			m_PacketFields.Add(new PacketViewItem("ペイロード長", packet.PaylaodLength.ToString(), packet, IPv6Packet.Fields.PaylaodLength.offset, IPv6Packet.Fields.PaylaodLength.size));
			m_PacketFields.Add(new PacketViewItem("次ヘッダ", GetProtocol(packet.NextHeader), packet, IPv6Packet.Fields.NextHeader.offset, IPv6Packet.Fields.NextHeader.size));
			m_PacketFields.Add(new PacketViewItem("ホップ制限", packet.HopLimit.ToString(), packet, IPv6Packet.Fields.HopLimit.offset, IPv6Packet.Fields.HopLimit.size));
			m_PacketFields.Add(new PacketViewItem("送信元IPアドレス", GetAddress(new ip6_addr(packet.SourceAddress)), packet, IPv6Packet.Fields.SourceAddress.offset, IPv6Packet.Fields.SourceAddress.size));
			m_PacketFields.Add(new PacketViewItem("宛先IPアドレス", GetAddress(new ip6_addr(packet.DestinationAddress)), packet, IPv6Packet.Fields.DestinationAddress.offset, IPv6Packet.Fields.DestinationAddress.size));

			switch (packet.NextHeader) {
			case IPv4Packet.IP_PROTO_UDP:
				AnalyzeUdp(new UDPPacket(packet, IPv6Packet.length), length - IPv6Packet.length);
				break;
			case IPv6Packet.IP_PROTO_ICMPV6:
				AnalyzeIcmpV6(new ICMPv6Packet(packet, IPv6Packet.length), length - IPv6Packet.length);
				break;
			default:
				m_PacketFields.Add(new PacketViewItem("データ", "サイズ：" + (length - IPv4Packet.length).ToString(), packet, IPv4Packet.length, length - IPv4Packet.length));
				break;
			}
		}

		/// <summary>
		/// IPv6 アドレスを文字列に変換する。
		/// </summary>
		/// <param name="p_ip6addr"></param>
		/// <returns></returns>
		private string GetAddress(ip6_addr p_ip6addr)
		{
			var buf = new char["0123:4567:89ab:cdef:0123:4567:89ab:cdef".Length];
			bool omit = false, zero = false;
			int start;
			int ix;

			start = 0;
			if (p_ip6addr == null) {
				buf[start++] = ':';
				buf[start++] = ':';
			}
			else {
				for (ix = 0; ix < ip6_addr.length / 2; ix++) {
					if (omit) {
						start += convert_hexdigit(buf, start, p_ip6addr.saddr[ix], 16, 0, ' ');
						if (ix < 7)
							buf[start++] = ':';
					}
					else if (ix > 0 && ix < 7 && p_ip6addr.saddr[ix] == 0)
						zero = true;
					else {
						if (zero) {
							omit = true;
							buf[start++] = ':';
						}
						start += convert_hexdigit(buf, start, p_ip6addr.saddr[ix], 16, 0, ' ');
						if (ix < 7)
							buf[start++] = ':';
					}
				}
			}
			buf[start] = '\0';
			return new string(buf);
		}

		private int convert_hexdigit(char[] buf, int pos, uint val, uint radix, int width, char padchar)
		{
			var radhex = "0123456789abcdef";

			var digits = new char[11];
			int start;
			int ix, pad;

			ix = 0;
			start = pos;
			do {
				digits[ix++] = radhex[(int)((val % radix) & 0x0f)];
				val /= radix;
			} while (val != 0);
			for (pad = ix; pad < width; pad++)
				buf[start++] = padchar;
			while (ix-- > 0)
				buf[start++] = digits[ix];
			buf[start] = '\0';
			return start - pos;
		}

		private void AnalyzeIcmp(ICMPPacket packet, int length)
		{
			m_PacketFields.Add(new PacketViewItem("ICMPタイプ", GetICMPType(packet.ICMPType), packet, ICMPPacket.Fields.ICMPType.offset, ICMPPacket.Fields.ICMPType.size));
			m_PacketFields.Add(new PacketViewItem("ICMPコード", GetICMPCode(packet.ICMPCode), packet, ICMPPacket.Fields.ICMPCode.offset, ICMPPacket.Fields.ICMPCode.size));
			m_PacketFields.Add(new PacketViewItem("ICMPチェックサム", packet.ICMPChecksum.ToString("X"), packet, ICMPPacket.Fields.ICMPChecksum.offset, ICMPPacket.Fields.ICMPChecksum.size));
		}

		private string GetICMPType(byte type)
		{
			switch (type) {
			case 0: return "エコー応答（echo reply）";
			case 3: return "あて先不達（destination unreachable）";
			case 4: return "ソース・クエンチ（source quench、送信元抑制）";
			case 5: return "リダイレクト要求（redirect、経路変更要求）";
			case 8: return "エコー要求（echo request）";
			case 11: return "時間超過（time exceeded）";
			case 12: return "パラメータ異常（parameter problem）";
			case 13: return "タイムスタンプ要求（timestamp request）";
			case 14: return "タイムスタンプ応答（timestamp reply）";
			case 15: return "情報要求（information request）";
			case 16: return "情報応答（information reply）";
			case 17: return "アドレス・マスク要求（address mask request）";
			case 18: return "アドレス・マスク応答（address mask reply）";
			default: return "???";
			}
		}

		private string GetICMPCode(byte code)
		{
			return code.ToString();
		}

		private void AnalyzeIcmpV6(ICMPv6Packet packet, int length)
		{
			m_PacketFields.Add(new PacketViewItem("ICMPタイプ", GetICMPv6Type(packet.ICMPType), packet, ICMPPacket.Fields.ICMPType.offset, ICMPPacket.Fields.ICMPType.size));
			m_PacketFields.Add(new PacketViewItem("ICMPコード", GetICMPv6Code(packet.ICMPType, packet.ICMPCode), packet, ICMPPacket.Fields.ICMPCode.offset, ICMPPacket.Fields.ICMPCode.size));
			m_PacketFields.Add(new PacketViewItem("ICMPチェックサム", packet.ICMPChecksum.ToString("X"), packet, ICMPPacket.Fields.ICMPChecksum.offset, ICMPPacket.Fields.ICMPChecksum.size));

			length -= ICMPv6Packet.length;
			int pos = ICMPv6Packet.length, len;
			switch (packet.ICMPType) {
			case 133:
				AnalyzeRouterSolicitation(new RouterSolicitationPacket(packet, pos), length);
				len = RouterSolicitationPacket.length;
				break;
			case 134:
				AnalyzeRouterAdvertisement(new RouterAdvertisementPacket(packet, pos), length);
				len = RouterAdvertisementPacket.length;
				break;
			case 135:
				AnalyzeNeighborSolicitation(new NeighborSolicitationPacket(packet, pos), length);
				len = NeighborSolicitationPacket.length;
				break;
			case 136:
				AnalyzeNeighborAdvertisement(new NeighborAdvertisementPacket(packet, pos), length);
				len = NeighborAdvertisementPacket.length;
				break;
			default:
				return;
			}

			for (; ; ) {
				pos += len;
				length -= len;
				if (length <= 0)
					break;

				len = AnalyzeICMPv6Option(new ICMPv6OptionPacket(packet, pos), length);
			}
		}

		private string GetICMPv6Type(byte type)
		{
			switch (type) {
			case 1: return "宛先到達不可（Destination Unreachable）";
			case 2: return "過大パケット（Packet is Too Big）";
			case 3: return "時間経過（Time Exceed）";
			case 4: return "パラメータ異常（Parameter Problem）";
			case 128: return "エコー要求（Echo Request）";
			case 129: return "エコー応答（Echo Reply）";
			case 130: return "マルチキャストリスナー検索（Multicast Listner Query）";
			case 131: return "マルチキャストリスナー応答（Multicast Listner Report）";
			case 132: return "マルチキャストリスナー終了（Multicast Listner Done）";
			case 133: return "ルーター要請 (router solicitation) ";
			case 134: return "ルーター広告 (router advertisement)";
			case 135: return "近隣要請 (Neighbor solicitation) ";
			case 136: return "近隣広告 (Neighbor advertisement) ";
			case 137: return "リダイレクト (redirect) ";
			case 138: return "ルータリナンバ (Router Renumber) ";
			case 139: return "ノード情報問い合わせ(Node Information Query) ";
			case 140: return "ノード情報応答(Node Information Reply) ";
			case 141: return "逆近隣探索要請 (Inverse Neighbor Discovery Solicitation) ";
			case 142: return "逆近隣探索広告 (Inverse Neighbor Discovery Advertisement) ";
			case 143: return "MLDv2 マルチキャストリスナレポート (Multicast Listener Report) ";
			case 144: return "ホームエージェントアドレス発見要求 (Home Agent Address Discovery Request) ";
			case 145: return "ホームエージェントアドレス発見応答 (Home Agent Address Discovery Reply) ";
			case 146: return "モバイルプリフィックス要請(Mobile Prefix Solicitation) ";
			case 147: return "モバイルプリフィックス広告(Mobile Prefix Advertisement) ";
			case 148: return "証明書パス要請(Certification Path Solicitation) ";
			case 149: return "証明書パス広告(Certification Path Advertisement) ";
			default: return "???";
			}
		}

		private string GetICMPv6Code(byte type, byte code)
		{
			switch (type) {
			// RFC 4443
			case 1:
				switch (code) {
				case 0: return "宛先までの経路が無い";
				case 1: return "宛先との通信が管理上禁止されている";
				case 2: return "(未割り当て)";
				case 3: return "宛先未到達";
				case 4: return "ポート未到達";
				default: return "???";
				}
			case 2:
				switch (code) {
				case 0: return "";
				default: return "???";
				}
			case 3:
				switch (code) {
				case 0: return "転送時のホップ制限超過";
				case 1: return "フラグメント組立時間超過";
				default: return "???";
				}
			case 4:
				switch (code) {
				case 0: return "異常なヘッダフィールドに遭遇した";
				case 1: return "未知の次ヘッダタイプに遭遇した";
				case 2: return "未知の IPv6 オプションに遭遇した";
				default: return "???";
				}
			case 128:
			case 129:
				switch (code) {
				case 0: return "";
				default: return "???";
				}
			// RFC 2710
			case 130:
			case 131:
			case 132:
				switch (code) {
				case 0: return "";
				default: return "???";
				}
			// RFC 4861
			case 133:
			case 134:
			case 135:
			case 136:
			case 137:
				switch (code) {
				case 0: return "";
				default: return "???";
				}
			// RFC 2894
			case 138:
				switch (code) {
				case 0: return "ルータリナンバリングコマンド";
				case 1: return "ルータリナンバリング結果";
				case 255: return "シーケンス番号リセット";
				default: return "???";
				}
			// RFC 4620
			case 139:
				switch (code) {
				case 0: return "ＩＰｖ６アドレス";
				case 1: return "名前、運用確認";
				case 2: return "ＩＰｖ４アドレス";
				default: return "???";
				}
			case 140:
				switch (code) {
				case 0: return "成功応答";
				case 1: return "回答の拒否";
				case 2: return "未知の問合せ種別";
				default: return "???";
				}
			// RFC 3122
			case 141:
			case 142:
				switch (code) {
				case 0: return "";
				default: return "???";
				}
			// RFC 3810
			case 143:
				switch (code) {
				case 0: return "";
				default: return "???";
				}
			// RFC 3775
			case 144:
			case 145:
			case 146:
			case 147:
				switch (code) {
				case 0: return "";
				default: return "???";
				}
			// RFC 3971
			case 148:
			case 149:
				switch (code) {
				case 0: return "";
				default: return "???";
				}
			default: return "???";
			}
		}

		private void AnalyzeRouterSolicitation(RouterSolicitationPacket packet, int length)
		{
			m_PacketFields.Add(new PacketViewItem("予約領域", packet.Reserved.ToString(), packet, RouterSolicitationPacket.Fields.Reserved.offset, RouterSolicitationPacket.Fields.Reserved.size));
		}

		private void AnalyzeRouterAdvertisement(RouterAdvertisementPacket packet, int length)
		{
			m_PacketFields.Add(new PacketViewItem("現在の最大ホップ数", packet.CurrentHopLimit.ToString(), packet, RouterAdvertisementPacket.Fields.CurrentHopLimit.offset, RouterAdvertisementPacket.Fields.CurrentHopLimit.size));
			m_PacketFields.Add(new PacketViewItem("管理対象アドレス設定フラグ", packet.ManagedAddressConfiguration.ToString(), packet, RouterAdvertisementPacket.Fields.ManagedAddressConfiguration.offset, RouterAdvertisementPacket.Fields.ManagedAddressConfiguration.size));
			m_PacketFields.Add(new PacketViewItem("管理対象アドレス設定フラグ", packet.OtherConfiguration.ToString(), packet, RouterAdvertisementPacket.Fields.OtherConfiguration.offset, RouterAdvertisementPacket.Fields.OtherConfiguration.size));
			m_PacketFields.Add(new PacketViewItem("予約領域", packet.Reserved.ToString(), packet, RouterAdvertisementPacket.Fields.Reserved.offset, RouterAdvertisementPacket.Fields.Reserved.size));
			m_PacketFields.Add(new PacketViewItem("ルーター有効期間", packet.RouterLifetime.ToString(), packet, RouterAdvertisementPacket.Fields.RouterLifetime.offset, RouterAdvertisementPacket.Fields.RouterLifetime.size));
			m_PacketFields.Add(new PacketViewItem("到達可能時間", packet.ReachableTime.ToString(), packet, RouterAdvertisementPacket.Fields.ReachableTime.offset, RouterAdvertisementPacket.Fields.ReachableTime.size));
			m_PacketFields.Add(new PacketViewItem("再送タイマー", packet.RetransTimer.ToString(), packet, RouterAdvertisementPacket.Fields.RetransTimer.offset, RouterAdvertisementPacket.Fields.RetransTimer.size));
		}

		private void AnalyzeNeighborSolicitation(NeighborSolicitationPacket packet, int length)
		{
			m_PacketFields.Add(new PacketViewItem("予約領域", packet.Reserved.ToString(), packet, NeighborSolicitationPacket.Fields.Reserved.offset, NeighborSolicitationPacket.Fields.Reserved.size));
			m_PacketFields.Add(new PacketViewItem("ターゲットアドレス", GetAddress(new ip6_addr(packet.TargetAddress, 0)), packet, NeighborSolicitationPacket.Fields.TargetAddress.offset, NeighborSolicitationPacket.Fields.TargetAddress.size));
		}

		private void AnalyzeNeighborAdvertisement(NeighborAdvertisementPacket packet, int length)
		{
			m_PacketFields.Add(new PacketViewItem("ルーターフラグ", packet.RouterFlag.ToString(), packet, NeighborAdvertisementPacket.Fields.RouterFlag.offset, NeighborAdvertisementPacket.Fields.RouterFlag.size));
			m_PacketFields.Add(new PacketViewItem("要請フラグ", packet.SolicitedFlag.ToString(), packet, NeighborAdvertisementPacket.Fields.SolicitedFlag.offset, NeighborAdvertisementPacket.Fields.SolicitedFlag.size));
			m_PacketFields.Add(new PacketViewItem("上書きフラグ", packet.OverrideFlag.ToString(), packet, NeighborAdvertisementPacket.Fields.OverrideFlag.offset, NeighborAdvertisementPacket.Fields.OverrideFlag.size));
			m_PacketFields.Add(new PacketViewItem("予約領域", packet.Reserved.ToString(), packet, NeighborAdvertisementPacket.Fields.Reserved.offset, NeighborAdvertisementPacket.Fields.Reserved.size));
			m_PacketFields.Add(new PacketViewItem("ターゲットアドレス", GetAddress(new ip6_addr(packet.TargetAddress, 0)), packet, NeighborAdvertisementPacket.Fields.TargetAddress.offset, NeighborAdvertisementPacket.Fields.TargetAddress.size));
		}

		private int AnalyzeICMPv6Option(ICMPv6OptionPacket packet, int length)
		{
			m_PacketFields.Add(new PacketViewItem("タイプ", GetICMPv6OptionType(packet.Type), packet, ICMPv6OptionPacket.Fields.Type.offset, ICMPv6OptionPacket.Fields.Type.size));
			m_PacketFields.Add(new PacketViewItem("データ長", (8 * packet.Length).ToString(), packet, ICMPv6OptionPacket.Fields.Length.offset, ICMPv6OptionPacket.Fields.Length.size));

			switch (packet.Type) {
			// 送信元リンク層アドレス
			case 1:
				AnalyzeEthernetAddress(new EthernetAddressPacket(packet, ICMPv6OptionPacket.length), length - ICMPv6OptionPacket.length);
				break;
			// 宛先リンク層アドレス
			case 2:
				AnalyzeEthernetAddress(new EthernetAddressPacket(packet, ICMPv6OptionPacket.length), length - ICMPv6OptionPacket.length);
				break;
			// プレフィックス情報
			case 3:
				AnalyzePrefixInformation(new PrefixInformationPacket(packet, ICMPv6OptionPacket.length), length - ICMPv6OptionPacket.length);
				break;
			// リダイレクトヘッダ
			case 4:
				AnalyzeRedirectedHeader(new RedirectedHeaderPacket(packet, ICMPv6OptionPacket.length), length - ICMPv6OptionPacket.length);
				break;
			// MTU
			case 5:
				AnalyzeMTU(new MTUPacket(packet, ICMPv6OptionPacket.length), length - ICMPv6OptionPacket.length);
				break;
			// IP Address/Prefix
			case 17:
				AnalyzeIPAddressPrefix(new IPAddressPrefixPacket(packet, ICMPv6OptionPacket.length), length - ICMPv6OptionPacket.length);
				break;
			// Link-layer Address
			case 19:
				AnalyzeLinkLayerAddress(new LinkLayerAddressPacket(packet, ICMPv6OptionPacket.length), length - ICMPv6OptionPacket.length);
				break;
			// Neighbor Advertisement Acknowledgment
			case 20:
				AnalyzeNeighborAdvertisementAcknowled(new NeighborAdvertisementAcknowledgmentPacket(packet, ICMPv6OptionPacket.length), length - ICMPv6OptionPacket.length);
				break;
			}

			return 8 * packet.Length;
		}

		private string GetICMPv6OptionType(byte type)
		{
			switch (type) {
			case 1: return "送信元リンク層アドレス";   // [RFC4861]
			case 2: return "宛先リンク層アドレス";    // [RFC4861]
			case 3: return "プレフィックス情報"; // [RFC4861]
			case 4: return "リダイレクトヘッダ"; // [RFC4861]
			case 5: return "MTU";   // [RFC4861]
			case 6: return "NBMA Shortcut Limit Option";    // [RFC2491] 
			case 7: return "Advertisement Interval Option"; // [RFC6275] 
			case 8: return "Home Agent Information Option"; // [RFC6275] 
			case 9: return "Source Address List";   // [RFC3122] 
			case 10: return "Target Address List";  // [RFC3122] 
			case 11: return "CGA option";   // [RFC3971] 
			case 12: return "RSA Signature option"; // [RFC3971] 
			case 13: return "Timestamp option"; // [RFC3971] 
			case 14: return "Nonce option"; // [RFC3971] 
			case 15: return "Trust Anchor option";  // [RFC3971] 
			case 16: return "Certificate option";   // [RFC3971] 
			case 17: return "IP Address/Prefix Option"; // [RFC5568] 
			case 18: return "New Router Prefix Information Option"; // [RFC4068] 
			case 19: return "Link-layer Address Option";    // [RFC5568] 
			case 20: return "Neighbor Advertisement Acknowledgment Option"; // [RFC5568] 
			case 23: return " MAP Option";  //  [RFC4140] 
			case 24: return " Route Information Option";    //  [RFC4191] 
			case 25: return "リカーシブDNSサーバー"; // [RFC5006][RFC6106]
			case 26: return " RA Flags Extension Option";   // [RFC5175] 
			case 27: return "Handover Key Request Option";  // [RFC5269] 
			case 28: return "Handover Key Reply Option";    // [RFC5269] 
			case 29: return "Handover Assist Information Option";   // [RFC5271] 
			case 30: return "Mobile Node Identifier Option";    // [RFC5271] 
			case 31: return "DNS検索リスト"; // [RFC6106]
			case 32: return "Proxy Signature (PS)"; // [RFC6496] 
			case 33: return "Address Registration Option";  // [RFC6775] 
			case 34: return "6LoWPAN Context Option";   // [RFC6775] 
			case 35: return "Authoritative Border Router Option";   // [RFC6775] 
			case 138: return "CARD Request option"; // [RFC4065] 
			case 139: return "CARD Reply option";   // [RFC4065] 
			case 253: return "RFC3692-style Experiment 1";  // [RFC4727] 
			case 254: return "RFC3692-style Experiment 2";  // [RFC4727] 
			default: return String.Format("不明（{0:X}）", type);
			}
		}

		private void AnalyzeEthernetAddress(EthernetAddressPacket packet, int length)
		{
			m_PacketFields.Add(new PacketViewItem("リンク層アドレス", GetEtherAddr(packet.LinkLayerAddress), packet, EthernetAddressPacket.Fields.LinkLayerAddress.offset, EthernetAddressPacket.Fields.LinkLayerAddress.size));
		}

		private string GetEtherAddr(array<byte> addr)
		{
			return String.Format("{0:X2}:{1:X2}:{2:X2}:{3:X2}:{4:X2}:{5:X2}",
				addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
		}

		private void AnalyzePrefixInformation(PrefixInformationPacket packet, int length)
		{
			m_PacketFields.Add(new PacketViewItem("プレフィックス長", packet.PrefixLength.ToString(), packet, PrefixInformationPacket.Fields.PrefixLength.offset, PrefixInformationPacket.Fields.PrefixLength.size));
			m_PacketFields.Add(new PacketViewItem("同一リンクフラグ", packet.OnLinkFlag.ToString(), packet, PrefixInformationPacket.Fields.OnLinkFlag.offset, PrefixInformationPacket.Fields.OnLinkFlag.size));
			m_PacketFields.Add(new PacketViewItem("自動アドレス設定フラグ", packet.AutonomousAddressConfigurationFlag.ToString(), packet, PrefixInformationPacket.Fields.AutonomousAddressConfigurationFlag.offset, PrefixInformationPacket.Fields.AutonomousAddressConfigurationFlag.size));
			m_PacketFields.Add(new PacketViewItem("予約領域", packet.Reserved1.ToString(), packet, PrefixInformationPacket.Fields.Reserved1.offset, PrefixInformationPacket.Fields.Reserved1.size));
			m_PacketFields.Add(new PacketViewItem("有効期間", packet.ValidLifetime.ToString(), packet, PrefixInformationPacket.Fields.ValidLifetime.offset, PrefixInformationPacket.Fields.ValidLifetime.size));
			m_PacketFields.Add(new PacketViewItem("参照可能期間", packet.PreferredLifetime.ToString(), packet, PrefixInformationPacket.Fields.PreferredLifetime.offset, PrefixInformationPacket.Fields.PreferredLifetime.size));
			m_PacketFields.Add(new PacketViewItem("予約領域", packet.Reserved2.ToString(), packet, PrefixInformationPacket.Fields.Reserved2.offset, PrefixInformationPacket.Fields.Reserved2.size));
			m_PacketFields.Add(new PacketViewItem("プレフィックス", packet.Prefix.ToString(), packet, PrefixInformationPacket.Fields.Prefix.offset, PrefixInformationPacket.Fields.Prefix.size));
		}

		private void AnalyzeRedirectedHeader(RedirectedHeaderPacket packet, int length)
		{
			m_PacketFields.Add(new PacketViewItem("予約領域", packet.Reserved.ToString(), packet, RedirectedHeaderPacket.Fields.Reserved.offset, RedirectedHeaderPacket.Fields.Reserved.size));
			m_PacketFields.Add(new PacketViewItem("IPヘッダー＋データ", packet.IPHeaderData.ToString(), packet, RedirectedHeaderPacket.Fields.IPHeaderData.offset, RedirectedHeaderPacket.Fields.IPHeaderData.size));
		}

		private void AnalyzeMTU(MTUPacket packet, int length)
		{
			m_PacketFields.Add(new PacketViewItem("予約領域", packet.Reserved.ToString(), packet, MTUPacket.Fields.Reserved.offset, MTUPacket.Fields.Reserved.size));
			m_PacketFields.Add(new PacketViewItem("MTU", packet.MTU.ToString(), packet, MTUPacket.Fields.MTU.offset, MTUPacket.Fields.MTU.size));
		}

		private void AnalyzeIPAddressPrefix(IPAddressPrefixPacket packet, int length)
		{
			m_PacketFields.Add(new PacketViewItem("オプションコード", GetIPAddressPrefixPacketOptionCode(packet.OptionCode), packet, IPAddressPrefixPacket.Fields.OptionCode.offset, IPAddressPrefixPacket.Fields.OptionCode.size));
			m_PacketFields.Add(new PacketViewItem("プレフィックス長", packet.PrefixLength.ToString(), packet, IPAddressPrefixPacket.Fields.PrefixLength.offset, IPAddressPrefixPacket.Fields.PrefixLength.size));
			m_PacketFields.Add(new PacketViewItem("予約領域", packet.Reserved.ToString(), packet, IPAddressPrefixPacket.Fields.Reserved.offset, IPAddressPrefixPacket.Fields.Reserved.size));
			m_PacketFields.Add(new PacketViewItem("IPv6アドレス", packet.IPv6Address.ToString(), packet, IPAddressPrefixPacket.Fields.IPv6Address.offset, IPAddressPrefixPacket.Fields.IPv6Address.size));
		}

		private string GetIPAddressPrefixPacketOptionCode(byte optionCode)
		{
			switch (optionCode) {
			case 1: return "Old Care-of Address";
			case 2: return "New Care-of Address";
			case 3: return "NAR's IP address";
			case 4: return "NAR's Prefix, sent in PrRtAdv.";
			default: return String.Format("不明({0})", optionCode);
			}
		}

		private void AnalyzeLinkLayerAddress(LinkLayerAddressPacket packet, int length)
		{
			m_PacketFields.Add(new PacketViewItem("オプションコード", GetLinkLayerAddressOptionCode(packet.OptionCode), packet, LinkLayerAddressPacket.Fields.OptionCode.offset, LinkLayerAddressPacket.Fields.OptionCode.size));
			m_PacketFields.Add(new PacketViewItem("リンク層アドレス", GetEtherAddr(packet.LinkLayerAddress), packet, LinkLayerAddressPacket.Fields.LinkLayerAddress.offset, LinkLayerAddressPacket.Fields.LinkLayerAddress.size));
		}

		private string GetLinkLayerAddressOptionCode(byte optionCode)
		{
			switch (optionCode) {
			case 0: return "Wildcard requesting resolution for all nearby access points";
			case 1: return "Link-Layer Address of the New Access Point";
			case 2: return "Link-Layer Address of the MN";
			case 3: return "Link-Layer Address of the NAR (i.e., Proxied Originator)";
			case 4: return "Link-Layer Address of the source of RtSolPr or PrRtAdv message";
			case 5: return "The access point identified by the LLA belongs to the current interface of the router";
			case 6: return "No prefix information available for the access point identified by the LLA";
			case 7: return "No fast handover support available for the access point identified by the LLA";
			default: return String.Format("不明({0})", optionCode);
			}
		}

		private void AnalyzeNeighborAdvertisementAcknowled(NeighborAdvertisementAcknowledgmentPacket packet, int length)
		{
			m_PacketFields.Add(new PacketViewItem("オプションコード", GetNeighborAdvertisementAcknowledgment(packet.OptionCode), packet, NeighborAdvertisementAcknowledgmentPacket.Fields.OptionCode.offset, NeighborAdvertisementAcknowledgmentPacket.Fields.OptionCode.size));
			m_PacketFields.Add(new PacketViewItem("Status", packet.Status.ToString(), packet, NeighborAdvertisementAcknowledgmentPacket.Fields.Status.offset, NeighborAdvertisementAcknowledgmentPacket.Fields.Status.size));
			m_PacketFields.Add(new PacketViewItem("予約領域", packet.Reserved.ToString(), packet, NeighborAdvertisementAcknowledgmentPacket.Fields.Reserved.offset, NeighborAdvertisementAcknowledgmentPacket.Fields.Reserved.size));
		}

		private string GetNeighborAdvertisementAcknowledgment(byte optionCode)
		{
			switch (optionCode) {
			case 1: return "NCoA is invalid, perform address configuration";
			case 2: return "NCoA is invalid, use the supplied NCoA.";
			case 3: return "NCoA is invalid, use NAR's IP address as NCoA in FBU";
			case 4: return "PCoA supplied, do not send FBU";
			case 128: return "Link-Layer Address unrecognized";
			default: return String.Format("不明({0})", optionCode);
			}
		}

		private void AnalyzeUdp(UDPPacket packet, int length)
		{
			m_PacketFields.Add(new PacketViewItem("送信元ポート番号", packet.SourcePort.ToString(), packet, UDPPacket.Fields.SourcePort.offset, UDPPacket.Fields.SourcePort.size));
			m_PacketFields.Add(new PacketViewItem("宛先ポート番号", packet.DestinationPort.ToString(), packet, UDPPacket.Fields.DestinationPort.offset, UDPPacket.Fields.DestinationPort.size));
			m_PacketFields.Add(new PacketViewItem("UDPデータ長", packet.UDPLength.ToString(), packet, UDPPacket.Fields.UDPLength.offset, UDPPacket.Fields.UDPLength.size));
			m_PacketFields.Add(new PacketViewItem("UDPチェックサム", packet.UDPChecksum.ToString("X"), packet, UDPPacket.Fields.UDPChecksum.offset, UDPPacket.Fields.UDPChecksum.size));

			if ((packet.DestinationPort == 0x43) || (packet.DestinationPort == 0x44)) {
				AnalyzeDhcp(new pointer(packet, UDPPacket.length), length - UDPPacket.length);
			}
			if (packet.DestinationPort == 3610) {
				AnalyzeEchonet(new pointer(packet, UDPPacket.length), length - UDPPacket.length);
			}
			else {
				m_PacketFields.Add(new PacketViewItem("データ", "サイズ：" + (length - UDPPacket.length).ToString(), packet, UDPPacket.length, length - UDPPacket.length));
			}
		}

		private void AnalyzeDhcp(pointer packet, int length)
		{
			var dhcp = new DHCPPacket(packet, 0);

			m_PacketFields.Add(new PacketViewItem("メッセージタイプ", dhcp.OP.ToString(), packet, DHCPPacket.Fields.OP.offset, DHCPPacket.Fields.OP.size));
			m_PacketFields.Add(new PacketViewItem("ハードウェアアドレスタイプ", dhcp.HwAddrType.ToString(), packet, DHCPPacket.Fields.HwAddrType.offset, DHCPPacket.Fields.HwAddrType.size));
			m_PacketFields.Add(new PacketViewItem("ハードウェアアドレス長", dhcp.HwAddrLength.ToString(), packet, DHCPPacket.Fields.HwAddrLength.offset, DHCPPacket.Fields.HwAddrLength.size));
			m_PacketFields.Add(new PacketViewItem("ホップカウント", dhcp.HOpt.ToString(), packet, DHCPPacket.Fields.HOpt.offset, DHCPPacket.Fields.HOpt.size));
			m_PacketFields.Add(new PacketViewItem("トランザクションID", dhcp.XId.ToString(), packet, DHCPPacket.Fields.XId.offset, DHCPPacket.Fields.XId.size));
			m_PacketFields.Add(new PacketViewItem("秒数", dhcp.Secs.ToString(), packet, DHCPPacket.Fields.Secs.offset, DHCPPacket.Fields.Secs.size));
			m_PacketFields.Add(new PacketViewItem("フラグ", dhcp.Flags.ToString(), packet, DHCPPacket.Fields.Flags.offset, DHCPPacket.Fields.Flags.size));
			m_PacketFields.Add(new PacketViewItem("クライアントIPアドレス", dhcp.CIAddr.ToString(), packet, DHCPPacket.Fields.CIAddr.offset, DHCPPacket.Fields.CIAddr.size));
			m_PacketFields.Add(new PacketViewItem("ユーザーIPアドレス", dhcp.YIAddr.ToString(), packet, DHCPPacket.Fields.YIAddr.offset, DHCPPacket.Fields.YIAddr.size));
			m_PacketFields.Add(new PacketViewItem("サーバーIPアドレス", dhcp.SIAddr.ToString(), packet, DHCPPacket.Fields.SIAddr.offset, DHCPPacket.Fields.SIAddr.size));
			m_PacketFields.Add(new PacketViewItem("ゲートウェイIPアドレス", dhcp.GIAddr.ToString(), packet, DHCPPacket.Fields.GIAddr.offset, DHCPPacket.Fields.GIAddr.size));
			m_PacketFields.Add(new PacketViewItem("クライアントハードウェアアドレス", dhcp.CHAddr.ToString(), packet, DHCPPacket.Fields.CHAddr.offset, DHCPPacket.Fields.CHAddr.size));
			m_PacketFields.Add(new PacketViewItem("サーバーホスト名", dhcp.SName.ToString(), packet, DHCPPacket.Fields.SName.offset, DHCPPacket.Fields.SName.size));
			m_PacketFields.Add(new PacketViewItem("起動ファイル名", dhcp.File.ToString(), packet, DHCPPacket.Fields.File.offset, DHCPPacket.Fields.File.size));

			if (length < DHCPPacket.Fields.Options.offset + 4)
				return;

			m_PacketFields.Add(new PacketViewItem("オプション", dhcp.ToString(DHCPPacket.Fields.Options.offset, 4), packet, DHCPPacket.Fields.Options.offset, 4));

			var pos = DHCPPacket.length + 4;
			while (pos < length) {
				var code = packet[pos++];
				if (code == 255)
					break;
				int len = packet[pos++];
				switch (code) {
				// Subnet Mask
				case 1:
					if (len == 4)
						m_PacketFields.Add(new PacketViewItem("Subnet Mask", GetPcAddr(lwip.lwip_ntohl((uint)new pointer(packet, pos))), packet, pos, len));
					else
						m_PacketFields.Add(new PacketViewItem("Subnet Mask", "Size NG", packet, pos, len));
					break;
				// Router Option
				case 3:
					if (len % 4 == 0)
						m_PacketFields.Add(new PacketViewItem("Router Option", GetPcAddr(lwip.lwip_ntohl((uint)new pointer(packet, pos))), packet, pos, len));
					else
						m_PacketFields.Add(new PacketViewItem("Router Option", "Size NG", packet, pos, len));
					break;
				// Domain Name Server Option
				case 6:
					if (len % 4 == 0)
						m_PacketFields.Add(new PacketViewItem("Domain Name Server Option", GetPcAddr(lwip.lwip_ntohl((uint)new pointer(packet, pos))), packet, pos, len));
					else
						m_PacketFields.Add(new PacketViewItem("Domain Name Server Option", "Size NG", packet, pos, len));
					break;
				// Host Name Option
				case 12:
					m_PacketFields.Add(new PacketViewItem("Host Name Option", packet.ToString(pos, len), packet, pos, len));
					break;
				// Domain Name
				case 15:
					m_PacketFields.Add(new PacketViewItem("Domain Name", packet.ToString(pos, len), packet, pos, len));
					break;
				// Requested IP Address
				case 50:
					if (len == 4)
						m_PacketFields.Add(new PacketViewItem("Requested IP Address", GetPcAddr(lwip.lwip_ntohl((uint)new pointer(packet, pos))), packet, pos, len));
					else
						m_PacketFields.Add(new PacketViewItem("Requested IP Address", "Size NG", packet, pos, len));
					break;
				// IP Address Lease Time
				case 51:
					if (len == 4)
						m_PacketFields.Add(new PacketViewItem("IP Address Lease Time", (lwip.lwip_ntohl((uint)new pointer(packet, pos))).ToString() + "[sec]", packet, pos, len));
					else
						m_PacketFields.Add(new PacketViewItem("IP Address Lease Time", "Size NG", packet, pos, len));
					break;
				// DHCP Message Type
				case 53:
					if (len == 1)
						m_PacketFields.Add(new PacketViewItem("DHCP Message Type", GetDhcpMessageType(packet[pos]), packet, pos, len));
					else
						m_PacketFields.Add(new PacketViewItem("DHCP Message Type", "Size NG", packet, pos, len));
					break;
				// Server Identifier
				case 54:
					if (len == 4)
						m_PacketFields.Add(new PacketViewItem("Server Identifier", GetPcAddr(lwip.lwip_ntohl((uint)new pointer(packet, pos))), packet, pos, len));
					else
						m_PacketFields.Add(new PacketViewItem("Server Identifier", "Size NG", packet, pos, len));
					break;
				// Parameter Request List
				case 55:
					m_PacketFields.Add(new PacketViewItem("Parameter Request List", GetHexString(packet, pos, len, ""), packet, pos, len));
					break;
				// Vender Class Identifier
				case 60:
					m_PacketFields.Add(new PacketViewItem("Vender Class Identifier", GetHexString(packet, pos, len, ""), packet, pos, len));
					break;
				// Client-Identifier
				case 61:
					m_PacketFields.Add(new PacketViewItem("Client-Identifier", GetHexString(packet, pos, len, ""), packet, pos, len));
					break;
				// FQDN Option
				case 81:
					m_PacketFields.Add(new PacketViewItem("FQDN Option", GetHexString(packet, pos, len, ""), packet, pos, len));
					break;
				// Auto-Configure Option
				case 116:
					if (len == 1)
						m_PacketFields.Add(new PacketViewItem("Auto-Configure Option", GetHexString(packet, pos, len, ""), packet, pos, len));
					else
						m_PacketFields.Add(new PacketViewItem("Auto-Configure Option", "Size NG", packet, pos, len));
					break;
				default:
					m_PacketFields.Add(new PacketViewItem("Unkown", "", packet, pos, len));
					break;
				}
				pos += len;
			}
		}

		private void AnalyzeTcp(TCPPacket packet, int length)
		{
			m_PacketFields.Add(new PacketViewItem("送信元ポート番号", packet.SourcePort.ToString(), packet, TCPPacket.Fields.SourcePort.offset, TCPPacket.Fields.SourcePort.size));
			m_PacketFields.Add(new PacketViewItem("宛先ポート番号", packet.DestinationPort.ToString(), packet, TCPPacket.Fields.DestinationPort.offset, TCPPacket.Fields.DestinationPort.size));
			m_PacketFields.Add(new PacketViewItem("TCPシーケンス番号", packet.SequenceNo.ToString(), packet, TCPPacket.Fields.SequenceNo.offset, TCPPacket.Fields.SequenceNo.size));
			m_PacketFields.Add(new PacketViewItem("TCPアクノリッジ番号", packet.AcknowledgeNo.ToString(), packet, TCPPacket.Fields.AcknowledgeNo.offset, TCPPacket.Fields.AcknowledgeNo.size));
			m_PacketFields.Add(new PacketViewItem("データオフセット", packet.DataOffset.ToString(), packet, TCPPacket.Fields.DataOffset.offset, TCPPacket.Fields.DataOffset.size));
			//m_PacketFields.Add(new PacketViewItem("緊急フラグ", packet.Urgent.ToString(), packet, TCPPacket.Fields.Urgent.offset, TCPPacket.Fields.Urgent.size));
			//m_PacketFields.Add(new PacketViewItem("アクノリッジフラグ", packet.Acknowledge.ToString(), packet, TCPPacket.Fields.Acknowledge.offset, TCPPacket.Fields.Acknowledge.size));
			//m_PacketFields.Add(new PacketViewItem("プッシュフラグ", packet.Push.ToString(), packet, TCPPacket.Fields.Push.offset, TCPPacket.Fields.Push.size));
			//m_PacketFields.Add(new PacketViewItem("リセットフラグ", packet.Reset.ToString(), packet, TCPPacket.Fields.Reset.offset, TCPPacket.Fields.Reset.size));
			//m_PacketFields.Add(new PacketViewItem("同期フラグ", packet.Synchronize.ToString(), packet, TCPPacket.Fields.Synchronize.offset, TCPPacket.Fields.Synchronize.size));
			//m_PacketFields.Add(new PacketViewItem("終了フラグ", packet.Finis.ToString(), packet, TCPPacket.Fields.Finis.offset, TCPPacket.Fields.Finis.size));
			var flags = new List<string>();
			if (packet.Urgent) flags.Add("URG");
			if (packet.Acknowledge) flags.Add("ACK");
			if (packet.Push) flags.Add("PSH");
			if (packet.Reset) flags.Add("RST");
			if (packet.Synchronize) flags.Add("SYC");
			if (packet.Finis) flags.Add("FIN");
			m_PacketFields.Add(new PacketViewItem("フラグ", String.Join(", ", flags), packet, TCPPacket.Fields.Finis.offset, TCPPacket.Fields.Finis.size));
			m_PacketFields.Add(new PacketViewItem("ウィンドウサイズ", packet.WindowSize.ToString(), packet, TCPPacket.Fields.WindowSize.offset, TCPPacket.Fields.WindowSize.size));
			m_PacketFields.Add(new PacketViewItem("TCPチェックサム", packet.TCPChecksum.ToString(), packet, TCPPacket.Fields.TCPChecksum.offset, TCPPacket.Fields.TCPChecksum.size));
			m_PacketFields.Add(new PacketViewItem("緊急ポインター", packet.UrgentPointer.ToString(), packet, TCPPacket.Fields.UrgentPointer.offset, TCPPacket.Fields.UrgentPointer.size));
			m_PacketFields.Add(new PacketViewItem("データ", "サイズ：" + (length - TCPPacket.length).ToString(), packet, TCPPacket.length, length - TCPPacket.length));
		}

		private string GetHexString(pointer packet, int pos, int len, string p)
		{
			var sb = new StringBuilder();
			for (var i = pos; i < pos + len; i++) {
				sb.Append(packet[i].ToString("X02"));
				sb.Append(p);
			}
			return sb.ToString();
		}

		private string GetDhcpMessageType(byte p)
		{
			switch (p) {
			case 1: return "DHCPDISCOVER";
			case 2: return "DHCPOFFER";
			case 3: return "DHCPREQUEST";
			case 4: return "DHCPDECLINE";
			case 5: return "DHCPACK";
			case 6: return "DHCPNAK";
			case 7: return "DHCPRELEASE";
			case 8: return "DHCPINFORM";
			default: return "Unknown";
			}
		}

		private void AnalyzeEchonet(pointer packet, int length)
		{
		}
	}

	public class PacketViewItem
	{
		private byte[] m_Data;
		private readonly int m_Offset;
		private readonly int m_Size;

		public PacketViewItem(string name, string value, pointer packet, int offset, int size)
		{
			Name = name;
			Value = value;
			m_Data = packet.data;
			m_Offset = packet.offset + offset;
			m_Size = size;
		}

		public string Name { get; }

		public string Value { get; }

		public string Data {
			get {
				var size = m_Size;

				if (m_Offset + size > m_Data.Length)
					size = m_Data.Length - m_Offset;

				if (size <= 0)
					return "";

				return BitConverter.ToString(m_Data, m_Offset, size);
			}
		}
	}
}

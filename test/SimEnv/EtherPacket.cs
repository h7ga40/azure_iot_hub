//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: EtherPacket.cs 1970 2019-07-10 00:36:30Z coas-nagasima $
using uITron3;

namespace SimEnv
{
	public enum ArpOperation
	{
		Request = 1,
		Reply = 2
	}

	public class EtherPacket : pointer
	{
		public struct Fields
		{
			public static readonly bit_field_info<ulong> DestinationMac = new bit_field_info<ulong>(0, 0, 48, true);
			public static readonly bit_field_info<ulong> SourceMac = new bit_field_info<ulong>(6, 0, 48, true);
			public static readonly value_field_info<ushort> TypeLen = new value_field_info<ushort>(12, true);
		}
		public new const int length = 14;

		public EtherPacket(byte[] data, int offset = 0) : base(data, offset)
		{
			Length = data.Length - offset;
			if (Length < length) {
				throw new System.ArgumentException();
			}
		}

		public EtherPacket(pointer data, int offset = 0) : base(data, offset)
		{
			Length = data.data.Length - offset;
			if (Length < length) {
				throw new System.ArgumentException();
			}
		}

		/// <summary>宛先MACアドレス</summary>
		public ulong DestinationMac { get => GetFieldValue(Fields.DestinationMac); set => SetFieldValue(Fields.DestinationMac, value); }
		/// <summary>送信元MACアドレス</summary>
		public ulong SourceMac { get => GetFieldValue(Fields.SourceMac); set => SetFieldValue(Fields.SourceMac, value); }
		/// <summary>イーサネットパケットタイプ／パケット長</summary>
		public ushort TypeLen { get => GetFieldValue(Fields.TypeLen); set => SetFieldValue(Fields.TypeLen, value); }

		public int Length { get; set; }

		public const ushort ETHTYPE_ARP = 0x0806;
		public const ushort ETHTYPE_IP = 0x0800;
		public const ushort ETHTYPE_IPV6 = 0x86DD;

		public static int Compare(byte[] buf1, int ofs1, byte[] buf2, int ofs2, int len)
		{
			int end = ofs1 + len;
			int ret = 0;

			for (; ofs1 < end; ofs1++, ofs2++) {
				ret = buf2[ofs2] - buf1[ofs1];
				if (ret != 0)
					break;
			}

			return ret;
		}
	}

	public class IPv4Packet : pointer
	{
		public struct Fields
		{
			public static readonly bit_field_info<byte> Version = new bit_field_info<byte>(0, 4, 4);
			public static readonly bit_field_info<byte> InternetHeaderLength = new bit_field_info<byte>(0, 0, 4);
			public static readonly value_field_info<byte> TypeOfServices = new value_field_info<byte>(1);
			public static readonly value_field_info<ushort> DatagramLength = new value_field_info<ushort>(2, true);
			public static readonly value_field_info<ushort> Identification = new value_field_info<ushort>(4, true);
			public static readonly bit_field_info<byte> Flags = new bit_field_info<byte>(6, 5, 3);
			public static readonly bit_field_info<ushort> FragmentOffset = new bit_field_info<ushort>(6, 3, 13, true);
			public static readonly value_field_info<byte> TimeToLive = new value_field_info<byte>(8);
			public static readonly value_field_info<byte> Protocol = new value_field_info<byte>(9);
			public static readonly value_field_info<ushort> HeaderChecksum = new value_field_info<ushort>(10/*, true*/);
			public static readonly value_field_info<uint> Source = new value_field_info<uint>(12, true);
			public static readonly value_field_info<uint> Destination = new value_field_info<uint>(16, true);
		}
		public new const int length = 20;

		public IPv4Packet(byte[] data, int offset = 0) : base(data, offset) { }
		public IPv4Packet(pointer data, int offset = 0) : base(data, offset) { }

		/// <summary>IPヴァージョン:4</summary>
		public byte Version { get => GetFieldValue(Fields.Version); set => SetFieldValue(Fields.Version, value); }
		/// <summary>IPヘッダー長:5</summary>
		public byte InternetHeaderLength { get => GetFieldValue(Fields.InternetHeaderLength); set => SetFieldValue(Fields.InternetHeaderLength, value); }
		/// <summary>サービスタイプ:?</summary>
		public byte TypeOfServices { get => GetFieldValue(Fields.TypeOfServices); set => SetFieldValue(Fields.TypeOfServices, value); }
		/// <summary>データグラム長</summary>
		public ushort DatagramLength { get => GetFieldValue(Fields.DatagramLength); set => SetFieldValue(Fields.DatagramLength, value); }
		/// <summary>データ識別番号</summary>
		public ushort Identification { get => GetFieldValue(Fields.Identification); set => SetFieldValue(Fields.Identification, value); }
		/// <summary>フラグ:4</summary>
		public byte Flags { get => GetFieldValue(Fields.Flags); set => SetFieldValue(Fields.Flags, value); }
		/// <summary>フラグメントオフセット:5</summary>
		public ushort FragmentOffset { get => GetFieldValue(Fields.FragmentOffset); set => SetFieldValue(Fields.FragmentOffset, value); }
		/// <summary>TTL</summary>
		public byte TimeToLive { get => GetFieldValue(Fields.TimeToLive); set => SetFieldValue(Fields.TimeToLive, value); }
		/// <summary>プロトコル</summary>
		public byte Protocol { get => GetFieldValue(Fields.Protocol); set => SetFieldValue(Fields.Protocol, value); }
		/// <summary>ヘッダーチェックサム</summary>
		public ushort HeaderChecksum { get => GetFieldValue(Fields.HeaderChecksum); set => SetFieldValue(Fields.HeaderChecksum, value); }
		/// <summary>送信元IPアドレス</summary>
		public uint Source { get => GetFieldValue(Fields.Source); set => SetFieldValue(Fields.Source, value); }
		/// <summary>宛先IPアドレス</summary>
		public uint Destination { get => GetFieldValue(Fields.Destination); set => SetFieldValue(Fields.Destination, value); }

		public const byte IP_PROTO_ICMP = 1;
		public const byte IP_PROTO_IGMP = 2;
		public const byte IP_PROTO_UDP = 17;
		public const byte IP_PROTO_UDPLITE = 170;
		public const byte IP_PROTO_TCP = 6;

		public void SetIPCheckSum()
		{
			ushort chksum = HeaderChecksum;

			HeaderChecksum = 0;

			chksum = lwip.inet_chksum(this, (ushort)(InternetHeaderLength * 4));

			HeaderChecksum = chksum;
		}
	}

	public class IPv6Packet : pointer
	{
		public struct Fields
		{
			public static readonly bit_field_info<byte> Version = new bit_field_info<byte>(0, 4, 4);
			public static readonly bit_field_info<ushort> TrafficClass = new bit_field_info<ushort>(0, 4, 8, true);
			public static readonly bit_field_info<uint> FlowLabel = new bit_field_info<uint>(0, 12, 20, true);
			public static readonly value_field_info<ushort> PaylaodLength = new value_field_info<ushort>(4, true);
			public static readonly value_field_info<byte> NextHeader = new value_field_info<byte>(6);
			public static readonly value_field_info<byte> HopLimit = new value_field_info<byte>(7);
			public static readonly array_field_info<byte> SourceAddress = new array_field_info<byte>(8, 16);
			public static readonly array_field_info<byte> DestinationAddress = new array_field_info<byte>(24, 16);
		}
		public new const int length = 40;

		public IPv6Packet(byte[] data, int offset = 0) : base(data, offset) { }
		public IPv6Packet(pointer data, int offset = 0) : base(data, offset) { }

		/// <summary>IPヴァージョン:6</summary>
		public byte Version { get => GetFieldValue(Fields.Version); set => SetFieldValue(Fields.Version, value); }
		/// <summary>トラフィッククラス</summary>
		public ushort TrafficClass { get => GetFieldValue(Fields.TrafficClass); set => SetFieldValue(Fields.TrafficClass, value); }
		/// <summary>フローラベル</summary>
		public uint FlowLabel { get => GetFieldValue(Fields.FlowLabel); set => SetFieldValue(Fields.FlowLabel, value); }
		/// <summary>ペイロード長</summary>
		public ushort PaylaodLength { get => GetFieldValue(Fields.PaylaodLength); set => SetFieldValue(Fields.PaylaodLength, value); }
		/// <summary>次ヘッダ</summary>
		public byte NextHeader { get => GetFieldValue(Fields.NextHeader); set => SetFieldValue(Fields.NextHeader, value); }
		/// <summary>ホップ制限</summary>
		public byte HopLimit { get => GetFieldValue(Fields.HopLimit); set => SetFieldValue(Fields.HopLimit, value); }
		/// <summary>送信元IPアドレス</summary>
		public array<byte> SourceAddress => GetArrayField(Fields.SourceAddress);
		/// <summary>宛先IPアドレス</summary>
		public array<byte> DestinationAddress => GetArrayField(Fields.DestinationAddress);

		public const byte IP_PROTO_ICMPV6 = 58;
	}

	public class ICMPPacket : pointer
	{
		public struct Fields
		{
			public static readonly value_field_info<byte> ICMPType = new value_field_info<byte>(0);
			public static readonly value_field_info<byte> ICMPCode = new value_field_info<byte>(1);
			public static readonly value_field_info<ushort> ICMPChecksum = new value_field_info<ushort>(2/*, true*/);
		}
		public new const int length = 4;

		public ICMPPacket(byte[] data, int offset = 0) : base(data, offset) { }
		public ICMPPacket(pointer data, int offset = 0) : base(data, offset) { }

		/// <summary>ICMPタイプ</summary>
		public byte ICMPType { get => GetFieldValue(Fields.ICMPType); set => SetFieldValue(Fields.ICMPType, value); }
		/// <summary>ICMPコード</summary>
		public byte ICMPCode { get => GetFieldValue(Fields.ICMPCode); set => SetFieldValue(Fields.ICMPCode, value); }
		/// <summary>ICMPチェックサム</summary>
		public ushort ICMPChecksum { get => GetFieldValue(Fields.ICMPChecksum); set => SetFieldValue(Fields.ICMPChecksum, value); }
	}

	public class ICMPv6Packet : pointer
	{
		public struct Fields
		{
			public static readonly value_field_info<byte> ICMPType = new value_field_info<byte>(0);
			public static readonly value_field_info<byte> ICMPCode = new value_field_info<byte>(1);
			public static readonly value_field_info<ushort> ICMPChecksum = new value_field_info<ushort>(2/*, true*/);
		}
		public new const int length = 4;

		public ICMPv6Packet(byte[] data, int offset = 0) : base(data, offset) { }
		public ICMPv6Packet(pointer data, int offset = 0) : base(data, offset) { }

		/// <summary>ICMPタイプ</summary>
		public byte ICMPType { get => GetFieldValue(Fields.ICMPType); set => SetFieldValue(Fields.ICMPType, value); }
		/// <summary>ICMPコード</summary>
		public byte ICMPCode { get => GetFieldValue(Fields.ICMPCode); set => SetFieldValue(Fields.ICMPCode, value); }
		/// <summary>ICMPチェックサム</summary>
		public ushort ICMPChecksum { get => GetFieldValue(Fields.ICMPChecksum); set => SetFieldValue(Fields.ICMPChecksum, value); }

		internal void SetICMPChecksum(ushort len, ip6_addr source, ip6_addr destination)
		{
			ushort chksum = ICMPChecksum;

			ICMPChecksum = 0;

			pbuf p = new pbuf(null, this) {
				payload = this,
				len = len
			};

			chksum = lwip.inet6_chksum_pseudo(p, source, destination, IPv6Packet.IP_PROTO_ICMPV6, p.len);

			ICMPChecksum = chksum;
		}
	}

	/// <summary>ルーター要請</summary>
	public class RouterSolicitationPacket : pointer
	{
		public struct Fields
		{
			public static readonly value_field_info<uint> Reserved = new value_field_info<uint>(0);
		}
		public new const int length = 4;

		public RouterSolicitationPacket(byte[] data, int offset = 0) : base(data, offset) { }
		public RouterSolicitationPacket(pointer data, int offset = 0) : base(data, offset) { }

		/// <summary>予約領域</summary>
		public uint Reserved { get => GetFieldValue(Fields.Reserved); set => SetFieldValue(Fields.Reserved, value); }
	}

	/// <summary>ルーター広告</summary>
	public class RouterAdvertisementPacket : pointer
	{
		public struct Fields
		{
			public static readonly value_field_info<byte> CurrentHopLimit = new value_field_info<byte>(0);
			public static readonly bit_field_info<byte> ManagedAddressConfiguration = new bit_field_info<byte>(1, 7, 1);
			public static readonly bit_field_info<byte> OtherConfiguration = new bit_field_info<byte>(1, 6, 1);
			public static readonly bit_field_info<byte> Reserved = new bit_field_info<byte>(1, 5, 6);
			public static readonly value_field_info<ushort> RouterLifetime = new value_field_info<ushort>(2);
			public static readonly value_field_info<uint> ReachableTime = new value_field_info<uint>(4);
			public static readonly value_field_info<uint> RetransTimer = new value_field_info<uint>(8);
		}
		public new const int length = 12;

		public RouterAdvertisementPacket(byte[] data, int offset = 0) : base(data, offset) { }
		public RouterAdvertisementPacket(pointer data, int offset = 0) : base(data, offset) { }

		/// <summary>現在の最大ホップ数</summary>
		public byte CurrentHopLimit { get => GetFieldValue(Fields.CurrentHopLimit); set => SetFieldValue(Fields.CurrentHopLimit, value); }
		/// <summary>管理対象アドレス設定フラグ</summary>
		public byte ManagedAddressConfiguration { get => GetFieldValue(Fields.ManagedAddressConfiguration); set => SetFieldValue(Fields.ManagedAddressConfiguration, value); }
		/// <summary>管理対象アドレス設定フラグ</summary>
		public byte OtherConfiguration { get => GetFieldValue(Fields.OtherConfiguration); set => SetFieldValue(Fields.OtherConfiguration, value); }
		/// <summary>予約領域</summary>
		public byte Reserved { get => GetFieldValue(Fields.Reserved); set => SetFieldValue(Fields.Reserved, value); }
		/// <summary>ルーター有効期間</summary>
		public ushort RouterLifetime { get => GetFieldValue(Fields.RouterLifetime); set => SetFieldValue(Fields.RouterLifetime, value); }
		/// <summary>到達可能時間</summary>
		public uint ReachableTime { get => GetFieldValue(Fields.ReachableTime); set => SetFieldValue(Fields.ReachableTime, value); }
		/// <summary>再送タイマー</summary>
		public uint RetransTimer { get => GetFieldValue(Fields.RetransTimer); set => SetFieldValue(Fields.RetransTimer, value); }
	}

	/// <summary>近隣要請</summary>
	public class NeighborSolicitationPacket : pointer
	{
		public struct Fields
		{
			public static readonly value_field_info<uint> Reserved = new value_field_info<uint>(0);
			public static readonly array_field_info<byte> TargetAddress = new array_field_info<byte>(4, 16);
		}
		public new const int length = 20;

		public NeighborSolicitationPacket(byte[] data, int offset = 0) : base(data, offset) { }
		public NeighborSolicitationPacket(pointer data, int offset = 0) : base(data, offset) { }

		/// <summary>予約領域</summary>
		public uint Reserved { get => GetFieldValue(Fields.Reserved); set => SetFieldValue(Fields.Reserved, value); }
		/// <summary>ターゲットアドレス</summary>
		public array<byte> TargetAddress => GetArrayField(Fields.TargetAddress);
	}

	/// <summary>近隣広告</summary>
	public class NeighborAdvertisementPacket : pointer
	{
		public struct Fields
		{
			public static readonly bit_field_info<byte> RouterFlag = new bit_field_info<byte>(0, 7, 1);
			public static readonly bit_field_info<byte> SolicitedFlag = new bit_field_info<byte>(0, 6, 1);
			public static readonly bit_field_info<byte> OverrideFlag = new bit_field_info<byte>(0, 5, 1);
			public static readonly bit_field_info<uint> Reserved = new bit_field_info<uint>(0, 4, 29, true);
			public static readonly array_field_info<byte> TargetAddress = new array_field_info<byte>(4, 16);
		}
		public new const int length = 20;

		public NeighborAdvertisementPacket(byte[] data, int offset = 0) : base(data, offset) { }
		public NeighborAdvertisementPacket(pointer data, int offset = 0) : base(data, offset) { }

		/// <summary>ルーターフラグ</summary>
		public byte RouterFlag { get => GetFieldValue(Fields.RouterFlag); set => SetFieldValue(Fields.RouterFlag, value); }
		/// <summary>要請フラグ</summary>
		public byte SolicitedFlag { get => GetFieldValue(Fields.SolicitedFlag); set => SetFieldValue(Fields.SolicitedFlag, value); }
		/// <summary>上書きフラグ</summary>
		public byte OverrideFlag { get => GetFieldValue(Fields.OverrideFlag); set => SetFieldValue(Fields.OverrideFlag, value); }
		/// <summary>予約領域</summary>
		public uint Reserved { get => GetFieldValue(Fields.Reserved); set => SetFieldValue(Fields.Reserved, value); }
		/// <summary>ターゲットアドレス</summary>
		public array<byte> TargetAddress => GetArrayField(Fields.TargetAddress);
	}

	/// <summary>ICMPv6オプション</summary>
	public class ICMPv6OptionPacket : pointer
	{
		public struct Fields
		{
			public static readonly value_field_info<byte> Type = new value_field_info<byte>(0);
			public static readonly value_field_info<byte> Length = new value_field_info<byte>(1);
		}
		public new const int length = 2;

		public ICMPv6OptionPacket(byte[] data, int offset = 0) : base(data, offset) { }
		public ICMPv6OptionPacket(pointer data, int offset = 0) : base(data, offset) { }

		/// <summary>タイプ</summary>
		public byte Type { get => GetFieldValue(Fields.Type); set => SetFieldValue(Fields.Type, value); }
		/// <summary>データ長</summary>
		public byte Length { get => GetFieldValue(Fields.Length); set => SetFieldValue(Fields.Length, value); }
	}

	/// <summary>Ethernetアドレス</summary>
	public class EthernetAddressPacket : pointer
	{
		public struct Fields
		{
			public static readonly array_field_info<byte> LinkLayerAddress = new array_field_info<byte>(0, 6);
		}
		public new const int length = 6;

		public EthernetAddressPacket(byte[] data, int offset = 0) : base(data, offset) { }
		public EthernetAddressPacket(pointer data, int offset = 0) : base(data, offset) { }

		/// <summary>リンク層アドレス</summary>
		public array<byte> LinkLayerAddress => GetArrayField(Fields.LinkLayerAddress);
	}

	/// <summary>プレフィックス情報</summary>
	public class PrefixInformationPacket : pointer
	{
		public struct Fields
		{
			public static readonly value_field_info<byte> PrefixLength = new value_field_info<byte>(0);
			public static readonly bit_field_info<byte> OnLinkFlag = new bit_field_info<byte>(1, 7, 1);
			public static readonly bit_field_info<byte> AutonomousAddressConfigurationFlag = new bit_field_info<byte>(1, 6, 1);
			public static readonly bit_field_info<byte> Reserved1 = new bit_field_info<byte>(1, 0, 5);
			public static readonly value_field_info<uint> ValidLifetime = new value_field_info<uint>(2);
			public static readonly value_field_info<uint> PreferredLifetime = new value_field_info<uint>(6);
			public static readonly value_field_info<uint> Reserved2 = new value_field_info<uint>(10);
			public static readonly array_field_info<byte> Prefix = new array_field_info<byte>(14, 16);
		}
		public new const int length = 20;

		public PrefixInformationPacket(byte[] data, int offset = 0) : base(data, offset) { }
		public PrefixInformationPacket(pointer data, int offset = 0) : base(data, offset) { }

		/// <summary>プレフィックス長</summary>
		public byte PrefixLength { get => GetFieldValue(Fields.PrefixLength); set => SetFieldValue(Fields.PrefixLength, value); }
		/// <summary>同一リンクフラグ</summary>
		public byte OnLinkFlag { get => GetFieldValue(Fields.OnLinkFlag); set => SetFieldValue(Fields.OnLinkFlag, value); }
		/// <summary>自動アドレス設定フラグ</summary>
		public byte AutonomousAddressConfigurationFlag { get => GetFieldValue(Fields.AutonomousAddressConfigurationFlag); set => SetFieldValue(Fields.AutonomousAddressConfigurationFlag, value); }
		/// <summary>予約領域</summary>
		public byte Reserved1 { get => GetFieldValue(Fields.Reserved1); set => SetFieldValue(Fields.Reserved1, value); }
		/// <summary>有効期間</summary>
		public uint ValidLifetime { get => GetFieldValue(Fields.ValidLifetime); set => SetFieldValue(Fields.ValidLifetime, value); }
		/// <summary>参照可能期間</summary>
		public uint PreferredLifetime { get => GetFieldValue(Fields.PreferredLifetime); set => SetFieldValue(Fields.PreferredLifetime, value); }
		/// <summary>予約領域</summary>
		public uint Reserved2 { get => GetFieldValue(Fields.Reserved2); set => SetFieldValue(Fields.Reserved2, value); }
		/// <summary>プレフィックス</summary>
		public array<byte> Prefix => GetArrayField(Fields.Prefix);
	}

	/// <summary>リダイレクトヘッダー</summary>
	public class RedirectedHeaderPacket : pointer
	{
		public struct Fields
		{
			public static readonly array_field_info<byte> Reserved = new array_field_info<byte>(0, 6);
			public static readonly array_field_info<byte> IPHeaderData = new array_field_info<byte>(6, 0);
		}
		public new const int length = 6;

		public RedirectedHeaderPacket(byte[] data, int offset = 0) : base(data, offset) { }
		public RedirectedHeaderPacket(pointer data, int offset = 0) : base(data, offset) { }

		/// <summary>予約領域</summary>
		public array<byte> Reserved => GetArrayField(Fields.Reserved);
		/// <summary>IPヘッダー＋データ</summary>
		public array<byte> IPHeaderData => GetArrayField(Fields.IPHeaderData);
	}

	public class MTUPacket : pointer
	{
		public struct Fields
		{
			public static readonly value_field_info<ushort> Reserved = new value_field_info<ushort>(0);
			public static readonly value_field_info<uint> MTU = new value_field_info<uint>(2);
		}
		public new const int length = 2;

		public MTUPacket(byte[] data, int offset = 0) : base(data, offset) { }
		public MTUPacket(pointer data, int offset = 0) : base(data, offset) { }

		/// <summary>予約領域</summary>
		public ushort Reserved { get => GetFieldValue(Fields.Reserved); set => SetFieldValue(Fields.Reserved, value); }
		/// <summary>MTU</summary>
		public uint MTU { get => GetFieldValue(Fields.MTU); set => SetFieldValue(Fields.MTU, value); }
	}

	public class IPAddressPrefixPacket : pointer
	{
		public struct Fields
		{
			public static readonly value_field_info<byte> OptionCode = new value_field_info<byte>(0);
			public static readonly value_field_info<byte> PrefixLength = new value_field_info<byte>(1);
			public static readonly value_field_info<uint> Reserved = new value_field_info<uint>(2);
			public static readonly array_field_info<byte> IPv6Address = new array_field_info<byte>(6, 16);
		}
		public new const int length = 22;

		public IPAddressPrefixPacket(byte[] data, int offset = 0) : base(data, offset) { }
		public IPAddressPrefixPacket(pointer data, int offset = 0) : base(data, offset) { }

		/// <summary>オプションコード</summary>
		public byte OptionCode { get => GetFieldValue(Fields.OptionCode); set => SetFieldValue(Fields.OptionCode, value); }
		/// <summary>プレフィックス長</summary>
		public byte PrefixLength { get => GetFieldValue(Fields.PrefixLength); set => SetFieldValue(Fields.PrefixLength, value); }
		/// <summary>予約領域</summary>
		public uint Reserved { get => GetFieldValue(Fields.Reserved); set => SetFieldValue(Fields.Reserved, value); }
		/// <summary>IPv6アドレス</summary>
		public byte IPv6Address { get => GetFieldValue(Fields.IPv6Address); set => SetFieldValue(Fields.IPv6Address, value); }
	}

	public class LinkLayerAddressPacket : pointer
	{
		public struct Fields
		{
			public static readonly value_field_info<byte> OptionCode = new value_field_info<byte>(0);
			public static readonly array_field_info<byte> LinkLayerAddress = new array_field_info<byte>(1, 6);
		}
		public new const int length = 10;

		public LinkLayerAddressPacket(byte[] data, int offset = 0) : base(data, offset) { }
		public LinkLayerAddressPacket(pointer data, int offset = 0) : base(data, offset) { }

		/// <summary>オプションコード</summary>
		public byte OptionCode { get => GetFieldValue(Fields.OptionCode); set => SetFieldValue(Fields.OptionCode, value); }
		/// <summary>リンク層アドレス</summary>
		public array<byte> LinkLayerAddress => GetArrayField(Fields.LinkLayerAddress);
	}

	public class NeighborAdvertisementAcknowledgmentPacket : pointer
	{
		public struct Fields
		{
			public static readonly value_field_info<byte> OptionCode = new value_field_info<byte>(0);
			public static readonly value_field_info<byte> Status = new value_field_info<byte>(1);
			public static readonly value_field_info<uint> Reserved = new value_field_info<uint>(2);
		}
		public new const int length = 6;

		public NeighborAdvertisementAcknowledgmentPacket(byte[] data, int offset = 0) : base(data, offset) { }
		public NeighborAdvertisementAcknowledgmentPacket(pointer data, int offset = 0) : base(data, offset) { }

		/// <summary>オプションコード</summary>
		public byte OptionCode { get => GetFieldValue(Fields.OptionCode); set => SetFieldValue(Fields.OptionCode, value); }
		/// <summary>Status</summary>
		public byte Status { get => GetFieldValue(Fields.Status); set => SetFieldValue(Fields.Status, value); }
		/// <summary>予約領域</summary>
		public uint Reserved { get => GetFieldValue(Fields.Reserved); set => SetFieldValue(Fields.Reserved, value); }
	}

	public class UDPPacket : pointer
	{
		public struct Fields
		{
			public static readonly value_field_info<ushort> SourcePort = new value_field_info<ushort>(0, true);
			public static readonly value_field_info<ushort> DestinationPort = new value_field_info<ushort>(2, true);
			public static readonly value_field_info<ushort> UDPLength = new value_field_info<ushort>(4, true);
			public static readonly value_field_info<ushort> UDPChecksum = new value_field_info<ushort>(6/*, true*/);
			public static readonly value_field_info<byte> UDPPayload = new value_field_info<byte>(8);
		}
		public new const int length = 8;

		public UDPPacket(byte[] data, int offset = 0) : base(data, offset) { }
		public UDPPacket(pointer data, int offset = 0) : base(data, offset) { }

		/// <summary>送信元ポート番号</summary>
		public ushort SourcePort { get => GetFieldValue(Fields.SourcePort); set => SetFieldValue(Fields.SourcePort, value); }
		/// <summary>宛先ポート番号</summary>
		public ushort DestinationPort { get => GetFieldValue(Fields.DestinationPort); set => SetFieldValue(Fields.DestinationPort, value); }
		/// <summary>UDPデータ長</summary>
		public ushort UDPLength { get => GetFieldValue(Fields.UDPLength); set => SetFieldValue(Fields.UDPLength, value); }
		/// <summary>UDPチェックサム</summary>
		public ushort UDPChecksum { get => GetFieldValue(Fields.UDPChecksum); set => SetFieldValue(Fields.UDPChecksum, value); }

		public void SetUDPChecksum(ushort len, ip_addr source, ip_addr destination)
		{
			ushort chksum = UDPChecksum;

			UDPChecksum = 0;

			pbuf p = new pbuf(null, this) {
				payload = this,
				len = len
			};

			chksum = lwip.inet_chksum_pseudo(p, source, destination, IPv4Packet.IP_PROTO_UDP, p.len);

			UDPChecksum = chksum;
		}

		internal void SetUDPChecksum(ushort len, ip6_addr source, ip6_addr destination)
		{
			ushort chksum = UDPChecksum;

			UDPChecksum = 0;

			pbuf p = new pbuf(null, this) {
				payload = this,
				len = len
			};

			chksum = lwip.inet6_chksum_pseudo(p, source, destination, IPv4Packet.IP_PROTO_UDP, p.len);

			UDPChecksum = chksum;
		}
	}

	public class TCPPacket : pointer
	{
		public struct Fields
		{
			public static readonly value_field_info<ushort> SourcePort = new value_field_info<ushort>(0, true);
			public static readonly value_field_info<ushort> DestinationPort = new value_field_info<ushort>(2, true);
			public static readonly value_field_info<uint> SequenceNo = new value_field_info<uint>(4, true);
			public static readonly value_field_info<uint> AcknowledgeNo = new value_field_info<uint>(8, true);
			public static readonly value_field_info<byte> DataOffset = new value_field_info<byte>(12);
			public static readonly bool_field_info Urgent = new bool_field_info(13, 5);
			public static readonly bool_field_info Acknowledge = new bool_field_info(13, 4);
			public static readonly bool_field_info Push = new bool_field_info(13, 3);
			public static readonly bool_field_info Reset = new bool_field_info(13, 2);
			public static readonly bool_field_info Synchronize = new bool_field_info(13, 1);
			public static readonly bool_field_info Finis = new bool_field_info(13, 0);
			public static readonly value_field_info<ushort> WindowSize = new value_field_info<ushort>(14, true);
			public static readonly value_field_info<ushort> TCPChecksum = new value_field_info<ushort>(16/*, true*/);
			public static readonly value_field_info<ushort> UrgentPointer = new value_field_info<ushort>(18, true);
		}
		public new const int length = 20;

		public TCPPacket(byte[] data, int offset = 0) : base(data, offset) { }
		public TCPPacket(pointer data, int offset = 0) : base(data, offset) { }

		/// <summary>送信元ポート番号</summary>
		public ushort SourcePort { get => GetFieldValue(Fields.SourcePort); set => SetFieldValue(Fields.SourcePort, value); }
		/// <summary>宛先ポート番号</summary>
		public ushort DestinationPort { get => GetFieldValue(Fields.DestinationPort); set => SetFieldValue(Fields.DestinationPort, value); }
		/// <summary>TCPシーケンス番号</summary>
		public uint SequenceNo { get => GetFieldValue(Fields.SequenceNo); set => SetFieldValue(Fields.SequenceNo, value); }
		/// <summary>TCPアクノリッジ番号</summary>
		public uint AcknowledgeNo { get => GetFieldValue(Fields.AcknowledgeNo); set => SetFieldValue(Fields.AcknowledgeNo, value); }
		/// <summary>データオフセット</summary>
		public byte DataOffset { get => GetFieldValue(Fields.DataOffset); set => SetFieldValue(Fields.DataOffset, value); }
		/// <summary>緊急フラグ</summary>
		public bool Urgent { get => GetFieldValue(Fields.Urgent); set => SetFieldValue(Fields.Urgent, value); }
		/// <summary>アクノリッジフラグ</summary>
		public bool Acknowledge { get => GetFieldValue(Fields.Acknowledge); set => SetFieldValue(Fields.Acknowledge, value); }
		/// <summary>プッシュフラグ</summary>
		public bool Push { get => GetFieldValue(Fields.Push); set => SetFieldValue(Fields.Push, value); }
		/// <summary>リセットフラグ</summary>
		public bool Reset { get => GetFieldValue(Fields.Reset); set => SetFieldValue(Fields.Reset, value); }
		/// <summary>同期フラグ</summary>
		public bool Synchronize { get => GetFieldValue(Fields.Synchronize); set => SetFieldValue(Fields.Synchronize, value); }
		/// <summary>終了フラグ</summary>
		public bool Finis { get => GetFieldValue(Fields.Finis); set => SetFieldValue(Fields.Finis, value); }
		/// <summary>ウィンドウサイズ</summary>
		public ushort WindowSize { get => GetFieldValue(Fields.WindowSize); set => SetFieldValue(Fields.WindowSize, value); }
		/// <summary>TCPチェックサム</summary>
		public ushort TCPChecksum { get => GetFieldValue(Fields.TCPChecksum); set => SetFieldValue(Fields.TCPChecksum, value); }
		/// <summary>緊急ポインター</summary>
		public ushort UrgentPointer { get => GetFieldValue(Fields.UrgentPointer); set => SetFieldValue(Fields.UrgentPointer, value); }

		public void SetTCPChecksum(ushort len, ip_addr source, ip_addr destination)
		{
			ushort chksum = TCPChecksum;

			TCPChecksum = 0;

			pbuf p = new pbuf(null, this) {
				payload = this,
				len = len
			};

			chksum = lwip.inet_chksum_pseudo(p, source, destination, IPv4Packet.IP_PROTO_TCP, p.len);

			TCPChecksum = chksum;
		}
	}

	public class ARPPacket : pointer
	{
		public struct Fields
		{
			public static readonly value_field_info<ushort> HardwareType = new value_field_info<ushort>(0, true);
			public static readonly value_field_info<ushort> ProtocolType = new value_field_info<ushort>(2, true);
			public static readonly value_field_info<byte> HwAddrLength = new value_field_info<byte>(4);
			public static readonly value_field_info<byte> PcAddrLength = new value_field_info<byte>(5);
			public static readonly value_field_info<ushort> Operation = new value_field_info<ushort>(6, true);
			public static readonly bit_field_info<ulong> SrcHwAddr = new bit_field_info<ulong>(8, 0, 48, true);
			public static readonly value_field_info<uint> SrcPcAddr = new value_field_info<uint>(14, true);
			public static readonly bit_field_info<ulong> DstHwAddr = new bit_field_info<ulong>(18, 0, 48, true);
			public static readonly value_field_info<uint> DstPcAddr = new value_field_info<uint>(24, true);
		}
		public new const int length = 28;

		public ARPPacket(byte[] data, int offset = 0) : base(data, offset) { }
		public ARPPacket(pointer data, int offset = 0) : base(data, offset) { }

		/// <summary>ハードウェアタイプ</summary>
		public ushort HardwareType { get => GetFieldValue(Fields.HardwareType); set => SetFieldValue(Fields.HardwareType, value); }
		/// <summary>プロトコルタイプ</summary>
		public ushort ProtocolType { get => GetFieldValue(Fields.ProtocolType); set => SetFieldValue(Fields.ProtocolType, value); }
		/// <summary>ハードウェアアドレス長</summary>
		public byte HwAddrLength { get => GetFieldValue(Fields.HwAddrLength); set => SetFieldValue(Fields.HwAddrLength, value); }
		/// <summary>プロトコルアドレス長</summary>
		public byte PcAddrLength { get => GetFieldValue(Fields.PcAddrLength); set => SetFieldValue(Fields.PcAddrLength, value); }
		/// <summary>操作種別</summary>
		public ArpOperation Operation { get => (ArpOperation)GetFieldValue(Fields.Operation); set => SetFieldValue(Fields.Operation, (ushort)value); }
		/// <summary>送信元MACアドレス</summary>
		public ulong SrcHwAddr { get => GetFieldValue(Fields.SrcHwAddr); set => SetFieldValue(Fields.SrcHwAddr, value); }
		/// <summary>送信元IPアドレス</summary>
		public uint SrcPcAddr { get => GetFieldValue(Fields.SrcPcAddr); set => SetFieldValue(Fields.SrcPcAddr, value); }
		/// <summary>宛先MACアドレス</summary>
		public ulong DstHwAddr { get => GetFieldValue(Fields.DstHwAddr); set => SetFieldValue(Fields.DstHwAddr, value); }
		/// <summary>宛先IPアドレス</summary>
		public uint DstPcAddr { get => GetFieldValue(Fields.DstPcAddr); set => SetFieldValue(Fields.DstPcAddr, value); }

		public void SetArpPacket(ArpOperation operation, ulong srcHwAddr, uint srcProtocolAddr,
			ulong dstHwAddr, uint dstProtocolAddr)
		{
			HardwareType = 1;
			ProtocolType = EtherPacket.ETHTYPE_IP;
			HwAddrLength = 6;
			PcAddrLength = 4;
			Operation = operation;
			SrcHwAddr = srcHwAddr;
			SrcPcAddr = srcProtocolAddr;
			DstHwAddr = dstHwAddr;
			DstPcAddr = dstProtocolAddr;
		}
	}

	public class DHCPPacket : pointer
	{
		public struct Fields
		{
			public static readonly value_field_info<byte> OP = new value_field_info<byte>(0);
			public static readonly value_field_info<byte> HwAddrType = new value_field_info<byte>(1);
			public static readonly value_field_info<byte> HwAddrLength = new value_field_info<byte>(2);
			public static readonly value_field_info<byte> HOpt = new value_field_info<byte>(3);
			public static readonly value_field_info<uint> XId = new value_field_info<uint>(4, true);
			public static readonly value_field_info<ushort> Secs = new value_field_info<ushort>(8, true);
			public static readonly value_field_info<ushort> Flags = new value_field_info<ushort>(10, true);
			public static readonly value_field_info<uint> CIAddr = new value_field_info<uint>(12, true);
			public static readonly value_field_info<uint> YIAddr = new value_field_info<uint>(16, true);
			public static readonly value_field_info<uint> SIAddr = new value_field_info<uint>(20, true);
			public static readonly value_field_info<uint> GIAddr = new value_field_info<uint>(24, true);
			public static readonly array_field_info<byte> CHAddr = new array_field_info<byte>(28, 16);
			public static readonly array_field_info<byte> SName = new array_field_info<byte>(44, 64);
			public static readonly array_field_info<byte> File = new array_field_info<byte>(108, 128);
			public static readonly array_field_info<byte> Options = new array_field_info<byte>(236, 0);
		}
		public new const int length = 236;

		public DHCPPacket(byte[] data, int offset = 0) : base(data, offset) { }
		public DHCPPacket(pointer data, int offset = 0) : base(data, offset) { }

		/// <summary>メッセージタイプ</summary>
		public byte OP { get => GetFieldValue(Fields.OP); set => SetFieldValue(Fields.OP, value); }
		/// <summary>ハードウェアアドレスタイプ</summary>
		public byte HwAddrType { get => GetFieldValue(Fields.HwAddrType); set => SetFieldValue(Fields.HwAddrType, value); }
		/// <summary>ハードウェアアドレス長</summary>
		public byte HwAddrLength { get => GetFieldValue(Fields.HwAddrLength); set => SetFieldValue(Fields.HwAddrLength, value); }
		/// <summary>ホップカウント</summary>
		public byte HOpt { get => GetFieldValue(Fields.HOpt); set => SetFieldValue(Fields.HOpt, value); }
		/// <summary>トランザクションID</summary>
		public uint XId { get => GetFieldValue(Fields.XId); set => SetFieldValue(Fields.XId, value); }
		/// <summary>秒数</summary>
		public ushort Secs { get => GetFieldValue(Fields.Secs); set => SetFieldValue(Fields.Secs, value); }
		/// <summary>フラグ</summary>
		public ushort Flags { get => GetFieldValue(Fields.Flags); set => SetFieldValue(Fields.Flags, value); }
		/// <summary>クライアントIPアドレス</summary>
		public uint CIAddr { get => GetFieldValue(Fields.CIAddr); set => SetFieldValue(Fields.CIAddr, value); }
		/// <summary>ユーザーIPアドレス</summary>
		public uint YIAddr { get => GetFieldValue(Fields.YIAddr); set => SetFieldValue(Fields.YIAddr, value); }
		/// <summary>サーバーIPアドレス</summary>
		public uint SIAddr { get => GetFieldValue(Fields.SIAddr); set => SetFieldValue(Fields.SIAddr, value); }
		/// <summary>ゲートウェイIPアドレス</summary>
		public uint GIAddr { get => GetFieldValue(Fields.GIAddr); set => SetFieldValue(Fields.GIAddr, value); }
		/// <summary>クライアントハードウェアアドレス</summary>
		public array<byte> CHAddr => GetArrayField(Fields.CHAddr);
		/// <summary>サーバーホスト名</summary>
		public array<byte> SName => GetArrayField(Fields.SName);
		/// <summary>起動ファイル名</summary>
		public array<byte> File => GetArrayField(Fields.File);
		/// <summary>オプション</summary>
		public array<byte> Options => GetArrayField(Fields.Options);
	}
}

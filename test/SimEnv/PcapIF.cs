//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: PcapIF.cs 1970 2019-07-10 00:36:30Z coas-nagasima $
using System;
using System.Collections.Generic;
using System.Net.NetworkInformation;
using System.Runtime.InteropServices;
using System.Threading;
using uITron3;

namespace SimEnv
{
	[StructLayout(LayoutKind.Sequential)]
	internal struct pcap_if
	{
		public IntPtr next;             // pcap_if *
		public string name;
		public string description;
		public IntPtr addresses;        // pcap_addr *
		public int flags;
	}

	[StructLayout(LayoutKind.Sequential)]
	internal struct pcap_addr
	{
		public IntPtr next;             // pcap_addr *
		public IntPtr addr;             // sockaddr *
		public IntPtr netmask;          // sockaddr *
		public IntPtr broadaddr;        // sockaddr *
		public IntPtr dstaddr;          // sockaddr *
	}

	[StructLayout(LayoutKind.Sequential)]
	internal struct sockaddr
	{
		public short sa_family;
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 14)]
		public byte[] sa_data;
	}

	[StructLayout(LayoutKind.Sequential)]
	internal struct pcap_pkthdr
	{
		public timeval ts;
		public int caplen;
		public int len;
	}

	[StructLayout(LayoutKind.Sequential)]
	internal struct timeval
	{
		public int tv_sec;
		public int tv_usec;
	}

	public class PcapIF : IDisposable, IHub
	{
		[DllImport("kernel32", CharSet = CharSet.Unicode)]
		private static extern IntPtr LoadLibrary(string lpFileName);

		[DllImport("kernel32")]
		private static extern bool FreeLibrary(IntPtr hModule);

		[DllImport("kernel32", CharSet = CharSet.Ansi)]
		private static extern IntPtr GetProcAddress(IntPtr hModule, string lpProcName);

		private const int PCAP_ERRBUF_SIZE = 256;
		private const int PCAP_OPENFLAG_PROMISCUOUS = 1;

		private IntPtr m_Module;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate int Tpcap_findalldevs(ref IntPtr alldevsp, IntPtr errbuf);
		private Tpcap_findalldevs pcap_findalldevs;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate void Tpcap_freealldevs(IntPtr alldevs);
		private Tpcap_freealldevs pcap_freealldevs;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate IntPtr Tpcap_open_live(string device, int snaplen, int promise, int to_ms, IntPtr ebuf);
		private Tpcap_open_live pcap_open_live;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate int Tpcap_next_ex(IntPtr p, ref IntPtr pkt_header, ref IntPtr pkt_data);
		private Tpcap_next_ex pcap_next_ex;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate int Tpcap_sendpacket(IntPtr p, IntPtr buf, int size);
		private Tpcap_sendpacket pcap_sendpacket;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate void Tpcap_breakloop(IntPtr p);
		private Tpcap_breakloop pcap_breakloop;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate void Tpcap_close(IntPtr p);
		private Tpcap_close pcap_close;
		private uint m_Subnet;
		private uint m_SubnetMask;

		private struct Packet
		{
			public IEtherConnection Sender;
			public EtherPacket Packect;

			public Packet(IEtherConnection sender, EtherPacket packet)
			{
				Sender = sender;
				Packect = packet;
			}
		}

		private Thread m_Thread;
		private AutoResetEvent m_Event;
		private LinkedList<IEtherConnection> m_Connections = new LinkedList<IEtherConnection>();
		private LinkedList<Packet> m_EtherPackets = new LinkedList<Packet>();
		private bool m_Terminate;

		public PcapIF(bool pcap)
		{
			if (pcap) {
				const string dllName = "wpcap";
				m_Module = LoadLibrary(dllName);
			}
			else {
				m_Module = IntPtr.Zero;
			}

			if (m_Module == IntPtr.Zero) {
				m_Event = new AutoResetEvent(false);
				m_Thread = new Thread(new ThreadStart(Broadcast)) {
					Name = "PcapIFLoop"
				};
				return;
			}

			pcap_findalldevs = (Tpcap_findalldevs)Marshal.GetDelegateForFunctionPointer(GetProcAddress(m_Module, "pcap_findalldevs"), typeof(Tpcap_findalldevs));
			pcap_freealldevs = (Tpcap_freealldevs)Marshal.GetDelegateForFunctionPointer(GetProcAddress(m_Module, "pcap_freealldevs"), typeof(Tpcap_freealldevs));
			pcap_open_live = (Tpcap_open_live)Marshal.GetDelegateForFunctionPointer(GetProcAddress(m_Module, "pcap_open_live"), typeof(Tpcap_open_live));
			pcap_next_ex = (Tpcap_next_ex)Marshal.GetDelegateForFunctionPointer(GetProcAddress(m_Module, "pcap_next_ex"), typeof(Tpcap_next_ex));
			pcap_sendpacket = (Tpcap_sendpacket)Marshal.GetDelegateForFunctionPointer(GetProcAddress(m_Module, "pcap_sendpacket"), typeof(Tpcap_sendpacket));
			pcap_breakloop = (Tpcap_breakloop)Marshal.GetDelegateForFunctionPointer(GetProcAddress(m_Module, "pcap_breakloop"), typeof(Tpcap_breakloop));
			pcap_close = (Tpcap_close)Marshal.GetDelegateForFunctionPointer(GetProcAddress(m_Module, "pcap_close"), typeof(Tpcap_close));

			DeviceList = GetDeviceList();

			m_Thread = new Thread(new ThreadStart(MainLoop)) {
				Name = "PcapIFLoop"
			};
		}

		public void Dispose()
		{
			pcap_findalldevs = null;
			pcap_freealldevs = null;
			pcap_open_live = null;
			pcap_next_ex = null;
			pcap_sendpacket = null;
			pcap_breakloop = null;
			pcap_close = null;

			if (m_Module != null) {
				if (FreeLibrary(m_Module)) {
					m_Module = IntPtr.Zero;
				}
			}
		}

		private List<DeviceInfo> DeviceList { get; }

		private DeviceInfo Device { get; set; }

		public void Init(uint ipAddr, uint mask)
		{
			m_Subnet = ipAddr & mask;
			m_SubnetMask = mask;

			if (m_Thread == null)
				return;

			m_Thread.Start();
		}

		public void Terminate()
		{
			m_Terminate = true;

			if (m_Event != null)
				m_Event.Set();
		}

		private void Broadcast()
		{
			while (!m_Terminate) {
				while (m_EtherPackets.Count > 0) {
					Packet msg;

					lock (m_EtherPackets) {
						msg = m_EtherPackets.First.Value;
						m_EtherPackets.RemoveFirst();
					}

					for (var connection = m_Connections.First;
						connection != null; connection = connection.Next) {
						if (msg.Sender != connection.Value)
							connection.Value.Receive(msg.Packect);
					}
				}

				if (!m_Event.WaitOne())
					break;
			}
		}

		public void Add(IEtherConnection connection)
		{
			if (connection.Hub != null)
				throw new Exception("すでに繋がってます。");

			connection.Hub = this;
			lock (m_Connections) {
				m_Connections.AddLast(connection);
			}
		}

		public void Remove(IEtherConnection connection)
		{
			connection.Hub = null;
			lock (m_Connections) {
				m_Connections.Remove(connection);
			}
		}

		public void Send(IEtherConnection sender, EtherPacket packet)
		{
			if (Device == null || Device.Hendle == IntPtr.Zero) {
				if (m_Event != null) {
					lock (m_EtherPackets) {
						m_EtherPackets.AddLast(new Packet(sender, packet));
					}
					m_Event.Set();
				}
				return;
			}

			var buff = new byte[packet.Length];
			Buffer.BlockCopy(packet.data, packet.offset, buff, 0, buff.Length);

			var hResult = GCHandle.Alloc(buff, GCHandleType.Pinned);
			try {
				var buffer = Marshal.UnsafeAddrOfPinnedArrayElement(buff, 0);

				pcap_sendpacket(Device.Hendle, buffer, buff.Length);
			}
			finally {
				hResult.Free();
			}
		}

		private void MainLoop()
		{
			var device = Device;

			if (device == null) {
				DeviceInfo linked = null, match = null;

				foreach (var di in DeviceList) {
					var ni = di.NetworkInterface;
					if (ni == null)
						continue;

					if (ni.OperationalStatus != OperationalStatus.Up/* ||
					ni.NetworkInterfaceType == NetworkInterfaceType.Loopback ||
					ni.NetworkInterfaceType == NetworkInterfaceType.Tunnel*/)
						continue;

					if (linked == null)
						linked = di;

					if (di.Addresses.Find(new Predicate<Addresses>(addr => {
						if (addr.Address.sa_family != 2)
							return false;

						var ipAddr = lwip.lwip_htonl(BitConverter.ToUInt32(addr.Address.sa_data, 2));
						if ((ipAddr & m_SubnetMask) != m_Subnet)
							return false;

						IPAddress = ipAddr;
						return true;
					})) == null)
						continue;

					match = di;
					break;
				}

				if (match == null)
					if (linked == null) {
						m_Event = new AutoResetEvent(false);
						Broadcast();
						return;
					}
					else
						device = linked;
				else
					device = match;
			}

			var handle = OpenDevice(device);
			device.Hendle = handle;

			var adapterProperties = device.NetworkInterface.GetIPProperties();
			foreach (var addr in adapterProperties.UnicastAddresses) {
				if (addr.Address.AddressFamily != System.Net.Sockets.AddressFamily.InterNetworkV6)
					continue;

				Buffer.BlockCopy(addr.Address.GetAddressBytes(), 0, IPv6Address, 0, IPv6Address.Length);
			}

			Device = device;

			try {
				var pHeader = IntPtr.Zero;
				var pData = IntPtr.Zero;
				int res;
				var header = new pcap_pkthdr();
				while ((res = pcap_next_ex(handle, ref pHeader, ref pData)) >= 0) {
					if (m_Terminate)
						break;

					if (res == 0)
						continue;

					header = (pcap_pkthdr)Marshal.PtrToStructure(pHeader, typeof(pcap_pkthdr));
					var buf = new byte[header.len];
					Marshal.Copy(pData, buf, 0, buf.Length);
					var packet = new EtherPacket(buf, 0) {
						Length = buf.Length
					};

					if (m_Module != IntPtr.Zero)
						SetCheckSum(packet);

					foreach (var lc in m_Connections) {
						lc.Receive(packet);
					}

					pHeader = IntPtr.Zero;
					pData = IntPtr.Zero;
				}
			}
			finally {
				Device.Hendle = IntPtr.Zero;
				CloseDevice(handle);
			}
		}

		private class Addresses
		{
			public sockaddr Address;
			public sockaddr NetMask;
			public sockaddr BroadcastAddress;
			public sockaddr DestinationAddress;     // Point-to-Pointインタフェースの時
		}

		private class DeviceInfo
		{
			public string Name;
			public string Description;
			public List<Addresses> Addresses = new List<Addresses>();
			public NetworkInterface NetworkInterface;
			public IntPtr Hendle;
		}

		private List<DeviceInfo> GetDeviceList()
		{
			var allDevices = IntPtr.Zero;
			var handle = IntPtr.Zero;
			var buff = new byte[PCAP_ERRBUF_SIZE + 1];

			var hbuf = GCHandle.Alloc(buff, GCHandleType.Pinned);
			try {
				var errbuf = Marshal.UnsafeAddrOfPinnedArrayElement(buff, 0);

				pcap_findalldevs(ref allDevices, errbuf);
			}
			finally {
				hbuf.Free();
			}

			var devices = new List<DeviceInfo>();
			try {
				var dp = allDevices;
				while (dp != IntPtr.Zero) {
					var dev = (pcap_if)Marshal.PtrToStructure(dp, typeof(pcap_if));
					var device = new DeviceInfo {
						Name = dev.name,
						Description = dev.description
					};
					if (dev.addresses != IntPtr.Zero) {
						var ap = dev.addresses;
						while (ap != IntPtr.Zero) {
							var addresses = (pcap_addr)Marshal.PtrToStructure(ap, typeof(pcap_addr));
							var ad = new Addresses {
								Address = GetAddress(addresses.addr),
								NetMask = GetAddress(addresses.netmask),
								BroadcastAddress = GetAddress(addresses.broadaddr),
								DestinationAddress = GetAddress(addresses.dstaddr)
							};
							device.Addresses.Add(ad);
							ap = addresses.next;
						}
					}
					devices.Add(device);
					dp = dev.next;
				}
			}
			finally {
				pcap_freealldevs(allDevices);
			}

			var adapters = NetworkInterface.GetAllNetworkInterfaces();
			foreach (var adapter in adapters) {
				var device = devices.Find(new Predicate<DeviceInfo>(delegate (DeviceInfo p) {
					return p.Name.ToUpper().Contains(adapter.Id.ToUpper());
				}));
				if (device != null)
					device.NetworkInterface = adapter;
			}

			return devices;
		}

		private static sockaddr GetAddress(IntPtr paddr)
		{
			if (paddr != IntPtr.Zero) {
				return (sockaddr)Marshal.PtrToStructure(paddr, typeof(sockaddr));
			}
			return new sockaddr();
		}

		private IntPtr OpenDevice(DeviceInfo device)
		{
			var handle = IntPtr.Zero;
			var buff = new byte[PCAP_ERRBUF_SIZE + 1];

			var hbuf = GCHandle.Alloc(buff, GCHandleType.Pinned);
			try {
				var errbuf = Marshal.UnsafeAddrOfPinnedArrayElement(buff, 0);

				handle = pcap_open_live(device.Name, 65536, PCAP_OPENFLAG_PROMISCUOUS, 1, errbuf);
			}
			finally {
				hbuf.Free();
			}

			return handle;
		}

		private void SetCheckSum(EtherPacket packet)
		{
			if (EtherPacket.Compare(Device.NetworkInterface.GetPhysicalAddress().GetAddressBytes(), 0, packet.data, 6, 6) != 0)
				return;

			switch (packet.TypeLen) {
			case EtherPacket.ETHTYPE_IP:
				var ipv4 = new IPv4Packet(packet, EtherPacket.length);

				if (ipv4.Version == 4) {
					ipv4.SetIPCheckSum();

					switch (ipv4.Protocol) {
					case IPv4Packet.IP_PROTO_UDP:
					case IPv4Packet.IP_PROTO_UDPLITE: {
						var udp = new UDPPacket(ipv4, IPv4Packet.length);
						if (udp.UDPChecksum != 0)
							udp.SetUDPChecksum(udp.UDPLength,
								new ip_addr(ipv4, IPv4Packet.Fields.Source.offset),
								new ip_addr(ipv4, IPv4Packet.Fields.Destination.offset));
						break;
					}
					case IPv4Packet.IP_PROTO_TCP: {
						var tcp = new TCPPacket(ipv4, IPv4Packet.length);
						if (ipv4.DatagramLength >= IPv4Packet.length) {
							tcp.SetTCPChecksum((ushort)(ipv4.DatagramLength - IPv4Packet.length),
									new ip_addr(ipv4, IPv4Packet.Fields.Source.offset),
									new ip_addr(ipv4, IPv4Packet.Fields.Destination.offset));
						}
						else {
							tcp.TCPChecksum = 0;
						}
						break;
					}
					}
				}
				break;
			case EtherPacket.ETHTYPE_IPV6:
				break;
			}
		}

		private void CloseDevice(IntPtr handle)
		{
			pcap_breakloop(handle);
			pcap_close(handle);
		}

		public bool IsStarted => (Device != null) || (m_Event != null);

		public byte[] MacAddress => (Device == null) ? new byte[6] : Device.NetworkInterface.GetPhysicalAddress().GetAddressBytes();

		public uint IPAddress { get; private set; }

		public byte[] IPv6Address { get; } = new byte[16];
	}
}

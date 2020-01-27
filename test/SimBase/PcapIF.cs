//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: PcapIF.cs 1970 2019-07-10 00:36:30Z coas-nagasima $
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Threading;
using System.Net.NetworkInformation;

namespace SimBase
{
	[StructLayout(LayoutKind.Sequential)]
	struct pcap_if
	{
		public IntPtr next;				// pcap_if *
		public string name;
		public string description;
		public IntPtr addresses;		// pcap_addr *
		public int flags;
	}

	[StructLayout(LayoutKind.Sequential)]
	struct pcap_addr
	{
		public IntPtr next;				// pcap_addr *
		public IntPtr addr;				// sockaddr *
		public IntPtr netmask;			// sockaddr *
		public IntPtr broadaddr;		// sockaddr *
		public IntPtr dstaddr;			// sockaddr *
	}

	[StructLayout(LayoutKind.Sequential)]
	struct sockaddr
	{
		public Int16 sa_family;
		[MarshalAs(UnmanagedType.ByValArray, SizeConst = 14)]
		public byte[] sa_data;
	}

	[StructLayout(LayoutKind.Sequential)]
	struct pcap_pkthdr
	{
		public timeval ts;
		public int caplen;
		public int len;
	}

	[StructLayout(LayoutKind.Sequential)]
	struct timeval
	{
		public int tv_sec;
		public int tv_usec;
	}

	public class PcapIF : IDisposable, IHub
	{
		[DllImport("kernel32", CharSet = CharSet.Unicode)]
		private extern static IntPtr LoadLibrary(string lpFileName);

		[DllImport("kernel32")]
		private extern static bool FreeLibrary(IntPtr hModule);

		[DllImport("kernel32", CharSet = CharSet.Ansi)]
		private extern static IntPtr GetProcAddress(IntPtr hModule, string lpProcName);

		const int PCAP_ERRBUF_SIZE = 256;
		const int PCAP_OPENFLAG_PROMISCUOUS = 1;

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

		private uint m_IPAddr;
		private uint m_Subnet;
		private uint m_SubnetMask;
		byte[] m_IPv6Addr = new byte[16];
		List<DeviceInfo> m_DeviceList;
		DeviceInfo m_Device;

		struct Packet
		{
			public IEtherConnection Sender;
			public EtherPacket Packect;

			public Packet(IEtherConnection sender, EtherPacket packet)
			{
				Sender = sender;
				Packect = packet;
			}
		}

		Thread m_Thread;
		AutoResetEvent m_Event;
		LinkedList<IEtherConnection> m_Connections = new LinkedList<IEtherConnection>();
		LinkedList<Packet> m_EtherPackets = new LinkedList<Packet>();
		bool m_Terminate;

		public PcapIF()
		{
			const string dllName = "wpcap";
			m_Module = LoadLibrary(dllName);

			if (m_Module == IntPtr.Zero) {
				m_Event = new AutoResetEvent(false);
				m_Thread = new Thread(new ThreadStart(Broadcast));
				m_Thread.Name = "PcapIFLoop";
				return;
			}

			pcap_findalldevs = (Tpcap_findalldevs)Marshal.GetDelegateForFunctionPointer(GetProcAddress(m_Module, "pcap_findalldevs"), typeof(Tpcap_findalldevs));
			pcap_freealldevs = (Tpcap_freealldevs)Marshal.GetDelegateForFunctionPointer(GetProcAddress(m_Module, "pcap_freealldevs"), typeof(Tpcap_freealldevs));
			pcap_open_live = (Tpcap_open_live)Marshal.GetDelegateForFunctionPointer(GetProcAddress(m_Module, "pcap_open_live"), typeof(Tpcap_open_live));
			pcap_next_ex = (Tpcap_next_ex)Marshal.GetDelegateForFunctionPointer(GetProcAddress(m_Module, "pcap_next_ex"), typeof(Tpcap_next_ex));
			pcap_sendpacket = (Tpcap_sendpacket)Marshal.GetDelegateForFunctionPointer(GetProcAddress(m_Module, "pcap_sendpacket"), typeof(Tpcap_sendpacket));
			pcap_breakloop = (Tpcap_breakloop)Marshal.GetDelegateForFunctionPointer(GetProcAddress(m_Module, "pcap_breakloop"), typeof(Tpcap_breakloop));
			pcap_close = (Tpcap_close)Marshal.GetDelegateForFunctionPointer(GetProcAddress(m_Module, "pcap_close"), typeof(Tpcap_close));

			m_DeviceList = GetDeviceList();

			m_Thread = new Thread(new ThreadStart(MainLoop));
			m_Thread.Name = "PcapIFLoop";
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

		private List<DeviceInfo> DeviceList { get { return m_DeviceList; } }

		private DeviceInfo Device
		{
			get { return m_Device; }
			set { m_Device = value; }
		}

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

					for (LinkedListNode<IEtherConnection> connection = m_Connections.First;
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
			if (m_Device == null || m_Device.Hendle == IntPtr.Zero) {
				if (m_Event != null) {
					lock (m_EtherPackets) {
						m_EtherPackets.AddLast(new Packet(sender, packet));
					}
					m_Event.Set();
				}
				return;
			}

			byte[] buff = new byte[packet.Length];
			Buffer.BlockCopy(packet.data, packet.offset, buff, 0, buff.Length);

			GCHandle hResult = GCHandle.Alloc(buff, GCHandleType.Pinned);
			try {
				IntPtr buffer = Marshal.UnsafeAddrOfPinnedArrayElement(buff, 0);

				pcap_sendpacket(m_Device.Hendle, buffer, buff.Length);
			}
			finally {
				hResult.Free();
			}
		}

		private void MainLoop()
		{
			DeviceInfo device = m_Device;

			if (device == null) {
				DeviceInfo linked = null, match = null;

				foreach (DeviceInfo di in m_DeviceList) {
					NetworkInterface ni = di.NetworkInterface;
					if (ni == null)
						continue;

					if (ni.OperationalStatus != OperationalStatus.Up/* ||
					ni.NetworkInterfaceType == NetworkInterfaceType.Loopback ||
					ni.NetworkInterfaceType == NetworkInterfaceType.Tunnel*/)
						continue;

					if (linked == null)
						linked = di;

					if (di.Addresses.Find(new Predicate<Addresses>(addr =>
					{
						if (addr.Address.sa_family != 2)
							return false;

						uint ipAddr = lwip.lwip_htonl(BitConverter.ToUInt32(addr.Address.sa_data, 2));
						if ((ipAddr & m_SubnetMask) != m_Subnet)
							return false;

						m_IPAddr = ipAddr;
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

			IntPtr handle = OpenDevice(device);
			device.Hendle = handle;

			IPInterfaceProperties adapterProperties = device.NetworkInterface.GetIPProperties();
			foreach (var addr in adapterProperties.UnicastAddresses) {
				if (addr.Address.AddressFamily != System.Net.Sockets.AddressFamily.InterNetworkV6)
					continue;

				Buffer.BlockCopy(addr.Address.GetAddressBytes(), 0, m_IPv6Addr, 0, m_IPv6Addr.Length);
			}

			m_Device = device;

			try {
				IntPtr pHeader = IntPtr.Zero;
				IntPtr pData = IntPtr.Zero;
				int res;
				pcap_pkthdr header = new pcap_pkthdr();
				while ((res = pcap_next_ex(handle, ref pHeader, ref pData)) >= 0) {
					if (m_Terminate)
						break;

					if (res == 0)
						continue;

					header = (pcap_pkthdr)Marshal.PtrToStructure(pHeader, typeof(pcap_pkthdr));
					byte[] buf = new byte[header.len];
					Marshal.Copy(pData, buf, 0, buf.Length);
					EtherPacket packet = new EtherPacket(buf, 0);
					packet.Length = buf.Length;

					SetCheckSum(packet);

					foreach (IEtherConnection lc in m_Connections) {
						lc.Receive(packet);
					}

					pHeader = IntPtr.Zero;
					pData = IntPtr.Zero;
				}
			}
			finally {
				m_Device.Hendle = IntPtr.Zero;
				CloseDevice(handle);
			}
		}

		private class Addresses
		{
			public sockaddr Address;
			public sockaddr NetMask;
			public sockaddr BroadcastAddress;
			public sockaddr DestinationAddress;		// Point-to-Pointインタフェースの時
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
			IntPtr allDevices = IntPtr.Zero;
			IntPtr handle = IntPtr.Zero;
			byte[] buff = new byte[PCAP_ERRBUF_SIZE + 1];

			GCHandle hbuf = GCHandle.Alloc(buff, GCHandleType.Pinned);
			try {
				IntPtr errbuf = Marshal.UnsafeAddrOfPinnedArrayElement(buff, 0);

				pcap_findalldevs(ref allDevices, errbuf);
			}
			finally {
				hbuf.Free();
			}

			List<DeviceInfo> devices = new List<DeviceInfo>();
			try {
				IntPtr dp = allDevices;
				while (dp != IntPtr.Zero) {
					pcap_if dev = (pcap_if)Marshal.PtrToStructure(dp, typeof(pcap_if));
					DeviceInfo device = new DeviceInfo();
					device.Name = dev.name;
					device.Description = dev.description;
					if (dev.addresses != IntPtr.Zero) {
						IntPtr ap = dev.addresses;
						while (ap != IntPtr.Zero) {
							pcap_addr addresses = (pcap_addr)Marshal.PtrToStructure(ap, typeof(pcap_addr));
							Addresses ad = new Addresses();
							ad.Address = GetAddress(addresses.addr);
							ad.NetMask = GetAddress(addresses.netmask);
							ad.BroadcastAddress = GetAddress(addresses.broadaddr);
							ad.DestinationAddress = GetAddress(addresses.dstaddr);
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

			NetworkInterface[] adapters = NetworkInterface.GetAllNetworkInterfaces();
			foreach (NetworkInterface adapter in adapters) {
				DeviceInfo device = devices.Find(new Predicate<DeviceInfo>(delegate(DeviceInfo p)
				{
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

		IntPtr OpenDevice(DeviceInfo device)
		{
			IntPtr handle = IntPtr.Zero;
			byte[] buff = new byte[PCAP_ERRBUF_SIZE + 1];

			GCHandle hbuf = GCHandle.Alloc(buff, GCHandleType.Pinned);
			try {
				IntPtr errbuf = Marshal.UnsafeAddrOfPinnedArrayElement(buff, 0);

				handle = pcap_open_live(device.Name, 65536, PCAP_OPENFLAG_PROMISCUOUS, 1, errbuf);
			}
			finally {
				hbuf.Free();
			}

			return handle;
		}

		private void SetCheckSum(EtherPacket packet)
		{
			if (EtherPacket.Compare(m_Device.NetworkInterface.GetPhysicalAddress().GetAddressBytes(), 0, packet.data, 6, 6) != 0)
				return;

			switch (packet.TypeLen) {
			case EtherPacket.ETHTYPE_IP:
				IPv4Packet ipv4 = new IPv4Packet(packet, EtherPacket.length);

				if (ipv4.Version == 4) {
					ipv4.SetIPCheckSum();

					switch (ipv4.Protocol) {
					case IPv4Packet.IP_PROTO_UDP:
					case IPv4Packet.IP_PROTO_UDPLITE: {
							UDPPacket udp = new UDPPacket(ipv4, IPv4Packet.length);
							if (udp.UDPChecksum != 0)
								udp.SetUDPChecksum(udp.UDPLength,
									new ip_addr(ipv4, IPv4Packet.Fields.Source.offset),
									new ip_addr(ipv4, IPv4Packet.Fields.Destination.offset));
						}
						break;
					case IPv4Packet.IP_PROTO_TCP: {
							TCPPacket tcp = new TCPPacket(ipv4, IPv4Packet.length);
							tcp.SetTCPChecksum((ushort)(ipv4.DatagramLength - IPv4Packet.length),
									new ip_addr(ipv4, IPv4Packet.Fields.Source.offset),
									new ip_addr(ipv4, IPv4Packet.Fields.Destination.offset));
						}
						break;
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

		public bool IsStarted
		{
			get { return (m_Device != null) || (m_Event != null); }
		}

		public byte[] MacAddress
		{
			get { return (m_Device == null) ? new byte[6] : m_Device.NetworkInterface.GetPhysicalAddress().GetAddressBytes(); }
		}

		public uint IPAddress
		{
			get { return m_IPAddr; }
		}

		public byte[] IPv6Address { get { return m_IPv6Addr; } }
	}
}

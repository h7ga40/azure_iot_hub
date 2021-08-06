using SimBase;
using System;
using System.Collections.Generic;
using System.Text;
using uITron3;

namespace SimEnv
{
	public class Unit : IUnitSim
	{
		private Target m_Taget;
		private List<IUnitInterface> m_Interfaces = new List<IUnitInterface>();
		lwip m_lwIP;
		netif m_Netif;
		PacketBridge m_IPPacketBridge;
		eth_addr m_MacAddr;

		public Unit(string unitName, Target main, ulong macAddr, uint addr, uint subNet)
		{
			UnitName = unitName;
			m_Taget = main;
			Itron = new Itron(m_Taget);

			m_Taget.OnSetEvent = UnitOnSetEvent;
			m_Taget.OnStart = UnitOnStart;
			m_Taget.OnTerminate = UnitOnTerminate;
			m_Taget.OnIdle = UnitOnIdle;
			m_Taget.OnOutput = UnitOnOutput;

			m_lwIP = m_Taget.lwip;
			m_MacAddr = new eth_addr(macAddr);
			var local_addr = new ip_addr(0xC0A80164);
			var subnet = new ip_addr(0xFFFFFF00);
			var gate_way_addr = new ip_addr(0xC0A80101);

			etharp.etharp_init(m_lwIP);

			m_Netif = new netif(m_lwIP, "eth0", m_lwIP.etharp.etharp_output, netif_linkoutput);

			m_Taget.AddNetIf(m_Netif, local_addr, subnet, gate_way_addr, m_Netif, netif_init, m_lwIP.etharp.ethernet_input);

			m_IPPacketBridge = new PacketBridge(m_Taget, 256);
			m_IPPacketBridge.InputDataHandler = InputDataHandler;
			m_Taget.PacketBridges.Add(m_IPPacketBridge);
			EtherIF = new EtherIF(this, "TEthernet", "EtherIF", m_IPPacketBridge.IFKind, macAddr);
			EtherIF.Init(addr, subNet);

			m_Interfaces.Add(EtherIF);

			CommandIF = new CommandIF(this, "TSIO", "CommandIF", 257);
			m_Interfaces.Add(CommandIF);

			Itron.Init();
		}

		private ER InputDataHandler(byte[] packet)
		{
			pbuf p = m_lwIP.pbuf_alloc(pbuf_layer.PBUF_RAW, (ushort)packet.Length, pbuf_type.PBUF_POOL);
			int pos = 0, rest = packet.Length;

			for (pbuf q = p; q != null; q = q.next) {
				int len = rest;
				if (len > q.len)
					len = q.len;

				Buffer.BlockCopy(packet, pos, q.payload.data, q.payload.offset, len);
				pos += len;
				rest -= len;
			}

			m_Netif.input(p, m_Netif);

			return ER.E_OK;
		}

		private err_t netif_init(netif netif)
		{
			pointer.memcpy(netif.hwaddr, m_MacAddr, netif.hwaddr_len);

			return err_t.ERR_OK;
		}

		private err_t netif_linkoutput(netif netif, pbuf p)
		{
			int pos = 0, rest = p.tot_len;
			byte[] packet = new byte[rest];

			for (pbuf q = p; q != null; q = q.next) {
				int len = rest;
				if (len > q.len)
					len = q.len;

				Buffer.BlockCopy(q.payload.data, q.payload.offset, packet, pos, len);
				pos += len;
				rest -= len;
			}

			m_IPPacketBridge.OutputData(packet);

			return err_t.ERR_OK;
		}

		public string UnitName { get; set; }

		public Itron Itron { get; }

		public EtherIF EtherIF { get; }
		public CommandIF CommandIF { get; }

		public IList<IUnitInterface> Interfaces => m_Interfaces.AsReadOnly();

		public void Prepare()
		{
		}

		public void Start()
		{
			m_Taget.Start(false);
			m_Taget.UnmaskeInterrupt(0);
		}

		public void Exit()
		{
			m_Taget.Terminate();
		}

		public bool IsAlive => m_Taget.IsAlive;

		public void Interrupt(int intNo)
		{
			m_Taget.Interrupt(intNo);
		}

		public int ReadAddr(uint addr, byte[] dst)
		{
			return m_Taget.ReadAddr(addr, dst, 0, dst.Length);
		}

		public int WriteAddr(uint addr, byte[] src)
		{
			return m_Taget.WriteAddr(addr, src, 0, src.Length);
		}

		public void SetBit(uint addr, int bit, bool value)
		{
			var temp = m_Taget.GetByte(addr);
			if (value)
				temp |= (byte)(0x01 << bit);
			else
				temp &= (byte)(~(0x01 << bit));
			m_Taget.SetByte(addr, temp);
		}

		public bool GetBit(uint addr, int bit)
		{
			var temp = m_Taget.GetByte(addr);
			return (temp & (0x01 << bit)) != 0;
		}

		public void Input(int kind, byte[] data)
		{
			m_Taget.Input(kind, data);
		}

		public long GetTimer()
		{
			return m_Taget.GetTimer();
		}

		public void Progress(long interval)
		{
			m_Taget.Progress(new TMO(interval));
		}

		public void CallTimeOut()
		{
			m_Taget.CallTimeOut();
		}

		public bool ProcessEvent()
		{
			return m_Taget.ProcessEvent();
		}

		public event UnitEventHandler UnitSetEvent;

		private void UnitOnSetEvent()
		{
			if (UnitSetEvent == null)
				return;

			UnitSetEvent(this);
		}

		public event UnitEventHandler UnitStart;

		private void UnitOnStart()
		{
			if (UnitStart == null)
				return;

			UnitStart(this);
		}

		public event UnitEventHandler UnitExit;

		private void UnitOnTerminate()
		{
			if (UnitExit == null)
				return;

			UnitExit(this);
		}

		public event UnitEventHandler UnitIdle;

		private void UnitOnIdle()
		{
			if (UnitIdle == null)
				return;

			UnitIdle(this);
		}

		public event UnitOutputEventHandler UnitOutput;

		private void UnitOnOutput(int kind, byte[] data)
		{
			if (kind == -1) {
				System.Diagnostics.Debug.WriteLine(UnitName + " : " + Encoding.Unicode.GetString(data));
			}
			else if (kind < 0) {
				UnitOutput?.Invoke(this, -kind, data);
			}
			else {
				Push(kind, data);
			}
		}

		protected void Push(int kind, byte[] data)
		{
			foreach (var uif in m_Interfaces) {
				if (uif.IFKind != kind)
					continue;
				uif.Push(data);
			}
		}

		public event UnitGetSystemTimeEventHandler UnitGetSystemTime;

		private void UnitOnGetSystemTime(ref long now)
		{
			if (UnitGetSystemTime == null)
				return;

			UnitGetSystemTime(this, ref now);
		}

		public event UnitInterfaceEventHandler AddInterface;

		public event UnitInterfaceEventHandler RemoveInterface;

		public event UnitInterfaceEventHandler UpdateInterface;

		protected void DoAddInterface(IUnitInterface item)
		{
			AddInterface?.Invoke(this, item);
		}

		protected void DoRemoveInterface(IUnitInterface item)
		{
			RemoveInterface?.Invoke(this, item);
		}

		protected void DoUpdateInterface(IUnitInterface item)
		{
			UpdateInterface?.Invoke(this, item);
		}

		protected void CallLog(int kind, byte[] data)
		{
			UnitOutput?.Invoke(this, kind, data);
		}
	}
}

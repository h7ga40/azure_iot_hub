using SimBase;
using System.Collections.Generic;
using System.Text;
using uITron3;

namespace SimEnv
{
	public class Unit : IUnitSim
	{
		private Target m_Taget;
		private List<IUnitInterface> m_Interfaces = new List<IUnitInterface>();

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

			main.SetIPv4Addr(addr, subNet);
			main.IPPacketBridge = new PacketBridge(main, 256);
			m_Taget.PacketBridges.Add(main.IPPacketBridge);
			EtherIF = new EtherIF(this, "TEthernet", "EtherIF", main.IPPacketBridge.IFKind, macAddr);
			EtherIF.Init(addr, subNet);

			m_Interfaces.Add(EtherIF);

			CommandIF = new CommandIF(this, "TSIO", "CommandIF", 257);
			m_Interfaces.Add(CommandIF);

			Itron.Init();
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

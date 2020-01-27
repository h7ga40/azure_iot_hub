using System;
using System.Collections.Generic;
using uITron3;

namespace SimEnv
{
	public abstract class Target : Kernel
	{
		private class IntCtrl
		{
			private uint m_Flag = 0x80000000u;

			public bool Mask {
				get => (m_Flag & 0x80000000u) != 0;
				set { if (value) m_Flag |= 0x80000000u; else m_Flag &= ~0x80000000u; }
			}

			public bool Invoke {
				get => (m_Flag & 0x40000000u) != 0;
				set { if (value) m_Flag |= 0x40000000u; else m_Flag &= ~0x40000000u; }
			}

			public byte Icrcmd {
				get => (byte)(m_Flag & 0x000000FFu);
				set { m_Flag &= ~0x000000FFu; m_Flag |= value; }
			}
		}

		private IntCtrl[] m_IntCtrl = new IntCtrl[8];
		private List<ISysTimerSync> m_SysTimerSyncs = new List<ISysTimerSync>();

		public Target()
			: base(45000000, 0, new TMO(1))
		{
			for (var i = 0; i < m_IntCtrl.Length; i++)
				m_IntCtrl[i] = new IntCtrl();

			m_IntHandler = new T_DINT[m_IntCtrl.Length];
			m_InProcIntr = new bool[m_IntCtrl.Length];

			var timer = new Timer(this, 0);
			m_SysTimerSyncs.Add(timer);
		}

		protected override bool InterruptUnmasked(int intNo)
		{
			return !m_IntCtrl[intNo].Mask && m_IntCtrl[intNo].Invoke;
		}

		protected override void SetInterrupt(int intNo)
		{
			m_IntCtrl[intNo].Invoke = true;
		}

		protected override void ClearInterrupt(int intNo)
		{
			m_IntCtrl[intNo].Invoke = false;
		}

		public override ER DefineInterruptHandler(uint dintno, ref T_DINT pk_dint)
		{
			if ((dintno < 0) || (dintno >= m_IntHandler.Length))
				return ER.E_PAR;

			m_IntHandler[dintno] = pk_dint;

			return ER.E_OK;
		}

		public override ER ChangeInterruptControlRegister(uint dintno, byte icrcmd)
		{
			if ((dintno < 0) || (dintno >= m_IntHandler.Length))
				return ER.E_PAR;

			m_IntCtrl[dintno].Icrcmd = icrcmd;

			return ER.E_OK;
		}

		public override void Input(int kind, byte[] data)
		{
			foreach (var pb in m_PacketBridges) {
				if ((kind == pb.IFKind) && (pb.InputDataHandler != null)) {
					pb.InputDataHandler(data);
					return;
				}
			}

			throw new NotImplementedException();
		}

		public override TMO GetTimer()
		{
			var result = TMO.TMO_FEVR;

			foreach (var sts in m_SysTimerSyncs) {
				var temp = new TMO(sts.GetTimer());
				if (temp == TMO.TMO_FEVR)
					continue;

				if ((result == TMO.TMO_FEVR) || (temp < result))
					result = temp;
			}

			return result;
		}

		public override void Progress(TMO interval)
		{
			foreach (var sts in m_SysTimerSyncs) {
				sts.Progress(interval);
			}
		}

		public override void CallTimeOut()
		{
			foreach (var sts in m_SysTimerSyncs) {
				sts.CallTimeOut();
			}
		}

		public void UnmaskeInterrupt(int intNo)
		{
			m_IntCtrl[intNo].Mask = false;
		}

		public void MaskeInterrupt(int intNo)
		{
			m_IntCtrl[intNo].Mask = true;
		}

		public List<ISysTimerSync> SysTimerSyncs => m_SysTimerSyncs;

		public List<PacketBridge> PacketBridges => m_PacketBridges;
	}
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace uITron3
{
	public delegate ER PacketBridgeInputData(byte[] data);

	public class PacketBridge
	{
		Kernel m_Kernel;
		int m_Kind;
		PacketBridgeInputData m_InputData;
		LinkedList<byte[]> m_DataQueue = new LinkedList<byte[]>();

		public PacketBridge(Kernel pKernel, int kind)
		{
			m_Kernel = pKernel;
			m_Kind = kind;
		}

		public int IFKind { get { return m_Kind; } }

		public PacketBridgeInputData InputDataHandler
		{
			get { return m_InputData; }
			set { m_InputData = value; }
		}

		public void OutputData(byte[] data)
		{
			m_Kernel.Output(m_Kind, data, data.Length);
		}

		public void InputData(byte[] data)
		{
			lock (m_DataQueue) {
				m_DataQueue.AddLast(data);
			}
			m_Kernel.Interrupt(this);
		}

		internal void DoInputData()
		{
			byte[] data;
			lock (m_DataQueue) {
				data = m_DataQueue.First.Value;
				m_DataQueue.RemoveFirst();
			}
			m_InputData(data);
		}
	}
}

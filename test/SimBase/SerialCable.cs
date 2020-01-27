//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: SerialCable.cs 1970 2019-07-10 00:36:30Z coas-nagasima $
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimBase
{
	public interface ISerialCable
	{
		void Send(ISerialPort sender, byte[] data);
		bool Connect(ISerialPort serialPort);
		bool Disconnect(ISerialPort serialPort);
	}

	public interface ISerialPort
	{
		void Receive(ISerialCable cable, byte[] data);
	}

	public class SerialCable : ISerialCable
	{
		ISerialPort m_Port1;
		ISerialPort m_Port2;

		public SerialCable()
		{
		}

		public ISerialPort Port1
		{
			get { return m_Port1; }
		}

		public ISerialPort Port2
		{
			get { return m_Port2; }
		}

		public void Send(ISerialPort sender, byte[] data)
		{
			if (m_Port1 == sender) {
				if (m_Port2 != null)
					m_Port2.Receive(this, data);
			}
			else if (m_Port2 == sender) {
				if (m_Port1 != null)
					m_Port1.Receive(this, data);
			}
		}

		public bool Connect(ISerialPort serialPort)
		{
			if (m_Port1 == null) {
				m_Port1 = serialPort;
				return true;
			}
			else if (m_Port2 == null) {
				m_Port2 = serialPort;
				return true;
			}
			return false;
		}

		public bool Disconnect(ISerialPort serialPort)
		{
			if (m_Port1 == serialPort) {
				m_Port1 = null;
				return true;
			}
			else if (m_Port2 == serialPort) {
				m_Port2 = null;
				return true;
			}
			return false;
		}
	}
}

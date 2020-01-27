//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: SerialCable.cs 1970 2019-07-10 00:36:30Z coas-nagasima $

namespace SimEnv
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
		public SerialCable()
		{
		}

		public ISerialPort Port1 { get; private set; }

		public ISerialPort Port2 { get; private set; }

		public void Send(ISerialPort sender, byte[] data)
		{
			if (Port1 == sender) {
				if (Port2 != null)
					Port2.Receive(this, data);
			}
			else if (Port2 == sender) {
				if (Port1 != null)
					Port1.Receive(this, data);
			}
		}

		public bool Connect(ISerialPort serialPort)
		{
			if (Port1 == null) {
				Port1 = serialPort;
				return true;
			}
			else if (Port2 == null) {
				Port2 = serialPort;
				return true;
			}
			return false;
		}

		public bool Disconnect(ISerialPort serialPort)
		{
			if (Port1 == serialPort) {
				Port1 = null;
				return true;
			}
			else if (Port2 == serialPort) {
				Port2 = null;
				return true;
			}
			return false;
		}
	}
}

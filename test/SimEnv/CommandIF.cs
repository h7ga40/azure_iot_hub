using SimBase;
using System;

namespace SimEnv
{
	public class CommandIF : IUnitInterface
	{
		public CommandIF(IUnitSim unitSim, string type, string name, int ifKind)
		{
			UnitSim = unitSim;
			TypeName = type;
			InterfaceName = name;
			IFKind = ifKind;
		}

		public string TypeName { get; }

		public string InterfaceName { get; }

		public IUnitSim UnitSim { get; }

		public int IFKind { get; }

		public Action<IUnitInterface, byte[]> Output { get; set; }

		public void Input(byte[] data)
		{
			UnitSim.Input(IFKind, data);
		}

		public void Push(byte[] data)
		{
			Output?.Invoke(this, data);
		}
	}
}

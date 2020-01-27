using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimBase
{
	public interface IEnvironment
	{
		IUnitConsole CreateUnitConsole(string ctorText);
		IUnitSim CreateUnit(string ctorText);
		void OnTerminated();
		void Log(IUnitSim sender, int kind, byte[] data);
		void UnitExit(IUnitSim sender);
	}
}

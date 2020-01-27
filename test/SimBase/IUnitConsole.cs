using SimBase;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimBase
{
	public interface IUnitConsole
	{
		IUnitSim Device { get; }
		string Text { get; }
		IUnitSim Prepare();
		void Activate(bool active);
	}
}

using System.Runtime.InteropServices;

namespace SimEnv
{
	[ComVisible(true)]
	public interface IUnitDllExporter
	{
		void Prepare(IUnitDllImporter importer);
		void Start();
		void Exit();
		void Interrupt(int IntNo);
		string GetUnitName();
		void SetUnitName([MarshalAs(UnmanagedType.LPWStr), In]string value);
		int ReadAddr(uint addr, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2), Out]byte[] buffer, int count);
		int WriteAddr(uint addr, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2), In]byte[] buffer, int count);
		bool GetBit(uint addr, int bit);
		void SetBit(uint addr, int bit, bool value);
		bool ProcessEvent();
		void Input(int kind, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2), In]byte[] data, int size);
		long GetTimer();
		void Progress(long timer);
		void CallTimeOut();
	}

	[ComVisible(true)]
	public interface IUnitDllImporter
	{
		void SetEvent();
		void Start();
		void Terminate();
		void Idle();
		void Output(int kind, [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 2), In]byte[] data, int size);
		void GetSystemTime(ref long now, ref long frequency);
		void AddInterface([MarshalAs(UnmanagedType.LPWStr), In]string type, [MarshalAs(UnmanagedType.LPWStr), In]string name, int kind);
		void RemoveInterface([MarshalAs(UnmanagedType.LPWStr), In]string type, [MarshalAs(UnmanagedType.LPWStr), In]string name, int kind);
	}
}

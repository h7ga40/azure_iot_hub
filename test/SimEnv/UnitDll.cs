//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: UnitDll.cs 1970 2019-07-10 00:36:30Z coas-nagasima $
using Microsoft.CSharp;
using SimBase;
using System;
using System.CodeDom;
using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;

namespace SimEnv
{
	internal static class DllImport
	{
		[DllImport("kernel32", CharSet = CharSet.Unicode)]
		public static extern IntPtr LoadLibrary(string lpFileName);

		[DllImport("kernel32")]
		public static extern bool FreeLibrary(IntPtr hModule);

		[DllImport("kernel32", CharSet = CharSet.Ansi)]
		public static extern IntPtr GetProcAddress(IntPtr hModule, string lpProcName);

		[DllImport("kernel32", CharSet = CharSet.Unicode)]
		public static extern IntPtr FindResource(IntPtr hModule, int lpName, string lpType);

		[DllImport("kernel32", CharSet = CharSet.Unicode)]
		public static extern int SizeofResource(IntPtr hModule, IntPtr hResInfo);

		[DllImport("kernel32", CharSet = CharSet.Unicode)]
		public static extern IntPtr LoadResource(IntPtr hModule, IntPtr hResInfo);

		[DllImport("kernel32", CharSet = CharSet.Unicode)]
		public static extern IntPtr LockResource(IntPtr hResData);

		public static Delegate GetFunction<T>(IntPtr module, string name)
		{
			var addr = GetProcAddress(module, name);
			if (addr == IntPtr.Zero) {
				return null;
			}
			return Marshal.GetDelegateForFunctionPointer(addr, typeof(T));
		}
	}

	public abstract class DllUnitSim : IDisposable, IUnitSim, ISystemCall
	{
		[UnmanagedFunctionPointer(CallingConvention.StdCall)]
		private delegate void TKernelEvent(IntPtr obj);
		[UnmanagedFunctionPointer(CallingConvention.StdCall)]
		private delegate void TOutputEvent(IntPtr obj, int kind, IntPtr data, int size);
		[UnmanagedFunctionPointer(CallingConvention.StdCall)]
		private delegate void TGetSystemTimeEvent(IntPtr obj, ref long now);
		[UnmanagedFunctionPointer(CallingConvention.StdCall)]
		private delegate void TInterfaceEvent(IntPtr obj, [MarshalAs(UnmanagedType.LPWStr)]string type, [MarshalAs(UnmanagedType.LPWStr)]string name, int kind);
		[UnmanagedFunctionPointer(CallingConvention.StdCall)]
		private delegate void TStartExternalEvent(IntPtr obj, int argc, [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPStr, SizeParamIndex = 1), In]string[] argv);

		private readonly string m_DllName;
		private IntPtr m_Object;
		private IntPtr m_Module;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate void TPrepare();
		private TPrepare m_Prepare;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate void TStart();
		private TStart m_Start;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate void TExit();
		private TExit m_Exit;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate void TInterrupt(int IntNo);
		private TInterrupt m_Interrupt;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate void TGetUnitName(IntPtr value, int size);
		private TGetUnitName m_GetUnitName;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate void TSetUnitName(IntPtr value);
		private TSetUnitName m_SetUnitName;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate void TAssignOnSetEvent(TKernelEvent onSetEvent, IntPtr objOfOnSetEvent);
		private readonly TAssignOnSetEvent m_AssignOnSetEvent;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate void TAssignOnStart(TKernelEvent onStart, IntPtr objOfOnStart);
		private TAssignOnStart m_AssignOnStart;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate void TAssignOnTerminate(TKernelEvent onTerminate, IntPtr objOfOnTerminate);
		private TAssignOnTerminate m_AssignOnTerminate;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate void TAssignOnIdle(TKernelEvent onIdle, IntPtr objOfOnIdle);
		private TAssignOnIdle m_AssignOnIdle;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate void TAssignOnOutput(TOutputEvent onLog, IntPtr objOfOnLog);
		private TAssignOnOutput m_AssignOnOutput;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate void TAssignOnGetSystemTime(TGetSystemTimeEvent OnGetSystemTime, IntPtr ObjOfOnGetSystemTime);
		private readonly TAssignOnGetSystemTime m_AssignOnGetSystemTime;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate void TAssignOnAddInterface(TInterfaceEvent OnAddInterface, IntPtr ObjOfOnAddInterface);
		private TAssignOnAddInterface m_AssignOnAddInterface;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate void TAssignOnRemoveInterface(TInterfaceEvent OnRemoveInterface, IntPtr ObjOfOnRemoveInterface);
		private TAssignOnRemoveInterface m_AssignOnRemoveInterface;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate void TAssignOnStartExternal(TStartExternalEvent OnStartExternal, IntPtr ObjOfOnStartExternal);
		private readonly TAssignOnStartExternal m_AssignOnStartExternal;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate int TReadAddr(uint addr, IntPtr buffer, int count);
		private TReadAddr m_ReadAddr;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate int TWriteAddr(uint addr, IntPtr buffer, int count);
		private TWriteAddr m_WriteAddr;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate bool TGetBit(uint addr, int bit);
		private TGetBit m_GetBit;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate void TSetBit(uint addr, int bit, bool value);
		private TSetBit m_SetBit;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate bool TProcessEvent();
		private TProcessEvent m_ProcessEvent;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate void TInput(int kind, IntPtr data, int size);
		private TInput m_Input;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate long TGetTimer();
		private TGetTimer m_GetTimer;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate void TProgress(long timer);
		private TProgress m_Progress;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate void TCallTimeOut();
		private TCallTimeOut m_CallTimeOut;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate int TSystemCall(int n, int a, int b, int c, int d, int e, int f);
		private readonly TSystemCall m_SystemCall;

		private readonly TKernelEvent m_UnitOnSetEvent;
		private readonly TKernelEvent m_UnitOnStart;
		private readonly TKernelEvent m_UnitOnTerminate;
		private readonly TKernelEvent m_UnitOnIdle;
		private readonly TOutputEvent m_UnitOnOutput;
		private TGetSystemTimeEvent m_UnitOnGetSystemTime;
		private TInterfaceEvent m_UnitOnAddInterface;
		private TInterfaceEvent m_UnitOnRemoveInterface;
		private readonly TStartExternalEvent m_UnitOnStartExternal;

		private readonly string m_TraceLogResource;

		public DllUnitSim(string dllName)
		{
			m_DllName = dllName;
			m_Module = DllImport.LoadLibrary(dllName);

			if (m_Module == IntPtr.Zero)
				return;

			m_Prepare = (TPrepare)DllImport.GetFunction<TPrepare>(m_Module, "Prepare");
			m_Start = (TStart)DllImport.GetFunction<TStart>(m_Module, "Start");
			m_Exit = (TExit)DllImport.GetFunction<TExit>(m_Module, "Exit");
			m_Interrupt = (TInterrupt)DllImport.GetFunction<TInterrupt>(m_Module, "Interrupt");
			m_GetUnitName = (TGetUnitName)DllImport.GetFunction<TGetUnitName>(m_Module, "GetUnitName");
			m_SetUnitName = (TSetUnitName)DllImport.GetFunction<TSetUnitName>(m_Module, "SetUnitName");
			m_AssignOnSetEvent = (TAssignOnSetEvent)DllImport.GetFunction<TAssignOnSetEvent>(m_Module, "AssignOnSetEvent");
			m_AssignOnStart = (TAssignOnStart)DllImport.GetFunction<TAssignOnStart>(m_Module, "AssignOnStart");
			m_AssignOnTerminate = (TAssignOnTerminate)DllImport.GetFunction<TAssignOnTerminate>(m_Module, "AssignOnTerminate");
			m_AssignOnIdle = (TAssignOnIdle)DllImport.GetFunction<TAssignOnIdle>(m_Module, "AssignOnIdle");
			m_AssignOnOutput = (TAssignOnOutput)DllImport.GetFunction<TAssignOnOutput>(m_Module, "AssignOnOutput");
			m_AssignOnGetSystemTime = (TAssignOnGetSystemTime)DllImport.GetFunction<TAssignOnGetSystemTime>(m_Module, "AssignOnGetSystemTime");
			m_AssignOnAddInterface = (TAssignOnAddInterface)DllImport.GetFunction<TAssignOnAddInterface>(m_Module, "AssignOnAddInterface");
			m_AssignOnRemoveInterface = (TAssignOnRemoveInterface)DllImport.GetFunction<TAssignOnRemoveInterface>(m_Module, "AssignOnRemoveInterface");
			m_ReadAddr = (TReadAddr)DllImport.GetFunction<TReadAddr>(m_Module, "ReadAddr");
			m_WriteAddr = (TWriteAddr)DllImport.GetFunction<TWriteAddr>(m_Module, "WriteAddr");
			m_GetBit = (TGetBit)DllImport.GetFunction<TGetBit>(m_Module, "GetBit");
			m_SetBit = (TSetBit)DllImport.GetFunction<TSetBit>(m_Module, "SetBit");
			m_ProcessEvent = (TProcessEvent)DllImport.GetFunction<TProcessEvent>(m_Module, "ProcessEvent");
			m_Input = (TInput)DllImport.GetFunction<TInput>(m_Module, "Input");
			m_GetTimer = (TGetTimer)DllImport.GetFunction<TGetTimer>(m_Module, "GetTimer");
			m_Progress = (TProgress)DllImport.GetFunction<TProgress>(m_Module, "Progress");
			m_CallTimeOut = (TCallTimeOut)DllImport.GetFunction<TCallTimeOut>(m_Module, "CallTimeOut");
			// Option
			m_AssignOnStartExternal = (TAssignOnStartExternal)DllImport.GetFunction<TAssignOnStartExternal>(m_Module, "AssignOnStartExternal");
			m_SystemCall = (TSystemCall)DllImport.GetFunction<TSystemCall>(m_Module, "SystemCall");

			if ((m_Prepare == null) || (m_Start == null) || (m_Exit == null) || (m_Interrupt == null)
				|| (m_GetUnitName == null) || (m_SetUnitName == null) || (m_AssignOnSetEvent == null)
				|| (m_AssignOnStart == null) || (m_AssignOnTerminate == null) || (m_AssignOnIdle == null)
				|| (m_AssignOnOutput == null) || (m_AssignOnGetSystemTime == null) || (m_AssignOnAddInterface == null)
				|| (m_AssignOnRemoveInterface == null) || (m_ReadAddr == null) || (m_WriteAddr == null)
				|| (m_GetBit == null) || (m_SetBit == null) || (m_ProcessEvent == null) || (m_Input == null)
				|| (m_GetTimer == null) || (m_Progress == null) || (m_CallTimeOut == null)) {
				var module = m_Module;
				m_Module = IntPtr.Zero;
				DllImport.FreeLibrary(module);
				return;
			}

			m_Object = Marshal.GetIUnknownForObject(this);

			m_UnitOnSetEvent = new TKernelEvent(UnitOnSetEvent);
			m_AssignOnSetEvent(m_UnitOnSetEvent, m_Object);
			m_UnitOnStart = new TKernelEvent(UnitOnStart);
			m_AssignOnStart(m_UnitOnStart, m_Object);
			m_UnitOnTerminate = new TKernelEvent(UnitOnTerminate);
			m_AssignOnTerminate(m_UnitOnTerminate, m_Object);
			m_UnitOnIdle = new TKernelEvent(UnitOnIdle);
			m_AssignOnIdle(m_UnitOnIdle, m_Object);
			m_UnitOnOutput = new TOutputEvent(UnitOnOutput);
			m_AssignOnOutput(m_UnitOnOutput, m_Object);
			m_UnitOnGetSystemTime = new TGetSystemTimeEvent(UnitOnGetSystemTime);
			m_AssignOnGetSystemTime(m_UnitOnGetSystemTime, m_Object);
			m_UnitOnAddInterface = new TInterfaceEvent(UnitOnAddInterface);
			m_AssignOnAddInterface(m_UnitOnAddInterface, m_Object);
			m_UnitOnRemoveInterface = new TInterfaceEvent(UnitOnRemoveInterface);
			m_AssignOnRemoveInterface(m_UnitOnRemoveInterface, m_Object);
			if (m_AssignOnStartExternal != null) {
				m_UnitOnStartExternal = new TStartExternalEvent(UnitOnStartExternal);
				m_AssignOnStartExternal(m_UnitOnStartExternal, m_Object);
			}

			var hResInfo = DllImport.FindResource(m_Module, 1, "TRACELOG_RESOURCE");
			if (hResInfo == IntPtr.Zero)
				return;

			var size = DllImport.SizeofResource(m_Module, hResInfo);
			var hGlobal = DllImport.LoadResource(m_Module, hResInfo);

			var buf = new byte[size];
			Marshal.Copy(DllImport.LockResource(hGlobal), buf, 0, buf.Length);

			m_TraceLogResource = Encoding.Default.GetString(buf);
		}

		public virtual void Dispose()
		{
			var module = m_Module;
			m_Module = IntPtr.Zero;

			m_Prepare = null;
			m_Start = null;
			m_Exit = null;
			m_Interrupt = null;
			m_GetUnitName = null;
			m_SetUnitName = null;
			m_AssignOnStart = null;
			m_AssignOnTerminate = null;
			m_AssignOnIdle = null;
			m_AssignOnOutput = null;
			m_AssignOnAddInterface = null;
			m_AssignOnRemoveInterface = null;
			m_UnitOnGetSystemTime = null;
			m_UnitOnAddInterface = null;
			m_UnitOnRemoveInterface = null;
			m_ReadAddr = null;
			m_WriteAddr = null;
			m_GetBit = null;
			m_SetBit = null;
			m_ProcessEvent = null;
			m_Input = null;
			m_GetTimer = null;
			m_Progress = null;
			m_CallTimeOut = null;

			if (m_Object != IntPtr.Zero) {
				Marshal.Release(m_Object);
				m_Object = IntPtr.Zero;
			}
			if (module != IntPtr.Zero) {
				DllImport.FreeLibrary(module);
			}
		}

		public bool IsAlive => (m_Module != IntPtr.Zero);

		public event UnitEventHandler UnitSetEvent;
		public event UnitEventHandler UnitStart;
		public event UnitEventHandler UnitExit;
		public event UnitEventHandler UnitIdle;
		public event UnitOutputEventHandler UnitOutput;
		public event UnitGetSystemTimeEventHandler UnitGetSystemTime;

		private static void UnitOnSetEvent(IntPtr Obj)
		{
			var u = (DllUnitSim)Marshal.GetObjectForIUnknown(Obj);

			if (u.UnitSetEvent == null)
				return;

			u.UnitSetEvent(u);
		}

		private static void UnitOnStart(IntPtr Obj)
		{
			var u = (DllUnitSim)Marshal.GetObjectForIUnknown(Obj);

			if (u.UnitStart == null)
				return;

			u.UnitStart(u);
		}

		private static void UnitOnTerminate(IntPtr Obj)
		{
			var u = (DllUnitSim)Marshal.GetObjectForIUnknown(Obj);

			if (u.UnitExit == null)
				return;

			u.UnitExit(u);
		}

		private static void UnitOnIdle(IntPtr obj)
		{
			var u = (DllUnitSim)Marshal.GetObjectForIUnknown(obj);

			if (u.UnitIdle == null)
				return;

			u.UnitIdle(u);
		}

		private static void UnitOnOutput(IntPtr obj, int kind, IntPtr data, int size)
		{
			var u = (DllUnitSim)Marshal.GetObjectForIUnknown(obj);

			var temp = new byte[size];

			Marshal.Copy(data, temp, 0, size);

			if (kind == 0) {
				System.Diagnostics.Debug.WriteLine(u.UnitName + " : " + Encoding.Unicode.GetString(temp));
			}
			else if (kind > 0) {
				u.Push(kind, temp);
			}
			else {
				u.UnitOutput?.Invoke(u, -kind, temp);
			}
		}

		private static void UnitOnGetSystemTime(IntPtr obj, ref long now)
		{
			var u = (DllUnitSim)Marshal.GetObjectForIUnknown(obj);

			if (u.UnitGetSystemTime == null)
				return;

			u.UnitGetSystemTime(u, ref now);
		}

		protected abstract void AddInterfaceFromDll(string name, string type, int kind);

		private static void UnitOnAddInterface(IntPtr obj, string name, string type, int kind)
		{
			var u = (DllUnitSim)Marshal.GetObjectForIUnknown(obj);

			u.AddInterfaceFromDll(name, type, kind);
		}

		protected abstract void RemoveInterfaceFromDll(string name, string type, int kind);

		private static void UnitOnRemoveInterface(IntPtr obj, string name, string type, int kind)
		{
			var u = (DllUnitSim)Marshal.GetObjectForIUnknown(obj);

			u.RemoveInterfaceFromDll(name, type, kind);
		}

		protected virtual void UnitStartExternal(string[] arg)
		{
			throw new NotImplementedException();
		}

		private void UnitOnStartExternal(IntPtr obj, int argc, string[] argv)
		{
			var u = (DllUnitSim)Marshal.GetObjectForIUnknown(obj);

			u.UnitStartExternal(argv);
		}

		public int SystemCall(int n, int a, int b, int c, int d, int e, int f)
		{
			return m_SystemCall(n, a, b, c, d, e, f);
		}

		public void Prepare()
		{
			m_Prepare();
		}

		public void Start()
		{
			m_Start();
		}

		public void Exit()
		{
			m_Exit();
		}

		public void Interrupt(int IntNo)
		{
			m_Interrupt(IntNo);
		}

		public int ReadAddr(uint Addr, byte[] dst)
		{
			int result;

			var hResult = GCHandle.Alloc(dst, GCHandleType.Pinned);
			try {
				var buffer = Marshal.UnsafeAddrOfPinnedArrayElement(dst, 0);

				result = m_ReadAddr(Addr, buffer, dst.Length);
			}
			finally {
				hResult.Free();
			}

			return result;
		}

		public int WriteAddr(uint Addr, byte[] src)
		{
			int result;

			var hResult = GCHandle.Alloc(src, GCHandleType.Pinned);
			try {
				var buffer = Marshal.UnsafeAddrOfPinnedArrayElement(src, 0);

				result = m_WriteAddr(Addr, buffer, src.Length);
			}
			finally {
				hResult.Free();
			}

			return result;
		}

		public void SetBit(uint addr, int bit, bool value)
		{
			m_SetBit(addr, bit, value);
		}

		public bool GetBit(uint addr, int bit)
		{
			return m_GetBit(addr, bit);
		}

		public void Input(int Kind, byte[] Data)
		{
			var hResult = GCHandle.Alloc(Data, GCHandleType.Pinned);
			try {
				var buffer = Marshal.UnsafeAddrOfPinnedArrayElement(Data, 0);

				m_Input(Kind, buffer, Data.Length);
			}
			finally {
				hResult.Free();
			}
		}

		public long GetTimer()
		{
			return m_GetTimer();
		}

		public void Progress(long timer)
		{
			m_Progress(timer);
		}

		public void CallTimeOut()
		{
			m_CallTimeOut();
		}

		public string UnitName {
			get {
				var result = new char[256];
				var hResult = GCHandle.Alloc(result, GCHandleType.Pinned);
				try {
					var buffer = Marshal.UnsafeAddrOfPinnedArrayElement(result, 0);

					m_GetUnitName(buffer, result.Length);
				}
				finally {
					hResult.Free();
				}
				var last = Array.IndexOf<char>(result, '\0');
				if (last < 0)
					last = result.Length;
				return new string(result, 0, last);
			}
			set {
				var result = value.ToCharArray();
				var hResult = GCHandle.Alloc(result, GCHandleType.Pinned);
				try {
					var buffer = Marshal.UnsafeAddrOfPinnedArrayElement(result, 0);

					m_SetUnitName(buffer);
				}
				finally {
					hResult.Free();
				}
			}
		}

		public bool ProcessEvent()
		{
			if (m_ProcessEvent != null)
				return m_ProcessEvent();
			return false;
		}

		protected abstract void Push(int kind, byte[] Data);

		public abstract IList<IUnitInterface> Interfaces { get; }

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

	public class DllUnitSimApp : IDisposable, IUnitSimApp
	{
		[UnmanagedFunctionPointer(CallingConvention.StdCall)]
		private delegate void TSystemCallEvent(IntPtr obj, int n, int a, int b, int c, int d, int e, int f);

		private readonly string m_DllName;
		private IntPtr m_Object;
		private IntPtr m_Module;

		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate int TStartExternal(int argc, [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPStr, SizeParamIndex = 0), In]string[] argv);
		private TStartExternal m_StartExternal;
		[UnmanagedFunctionPointer(CallingConvention.Cdecl)]
		private delegate void TAssignOnSystemCall(TSystemCallEvent OnSystemCall, IntPtr ObjOfOnSystemCall);
		private TAssignOnSystemCall m_AssignOnSystemCall;

		private readonly TSystemCallEvent m_UnitOnSystemCall;

		public DllUnitSimApp(string dllName)
		{
			m_DllName = dllName;
			m_Module = DllImport.LoadLibrary(dllName);

			if (m_Module == IntPtr.Zero)
				return;

			m_StartExternal = (TStartExternal)DllImport.GetFunction<TStartExternal>(m_Module, "StartExternal");
			m_AssignOnSystemCall = (TAssignOnSystemCall)DllImport.GetFunction<TAssignOnSystemCall>(m_Module, "AssignOnSystemCall");

			if ((m_StartExternal == null) || (m_AssignOnSystemCall == null)) {
				var module = m_Module;
				m_Module = IntPtr.Zero;
				DllImport.FreeLibrary(module);
				return;
			}

			m_Object = Marshal.GetIUnknownForObject(this);

			m_UnitOnSystemCall = new TSystemCallEvent(UnitOnSystemCall);
			m_AssignOnSystemCall(m_UnitOnSystemCall, m_Object);
		}

		public virtual void Dispose()
		{
			var module = m_Module;
			m_Module = IntPtr.Zero;

			m_StartExternal = null;
			m_AssignOnSystemCall = null;

			if (m_Object != IntPtr.Zero) {
				Marshal.Release(m_Object);
				m_Object = IntPtr.Zero;
			}
			if (module != IntPtr.Zero) {
				DllImport.FreeLibrary(module);
			}
		}

		public bool IsAlive => (m_Module != IntPtr.Zero);

		public ISystemCall SystemCall { get; set; }

		private void UnitOnSystemCall(IntPtr obj, int n, int a, int b, int c, int d, int e, int f)
		{
			var u = (DllUnitSimApp)Marshal.GetObjectForIUnknown(obj);

			u.UnitSystemCall(n, a, b, c, d, e, f);
		}

		protected virtual void UnitSystemCall(int n, int a, int b, int c, int d, int e, int f)
		{
			SystemCall.SystemCall(n, a, b, c, d, e, f);
		}

		public void StartExternal(string[] arg)
		{
			m_StartExternal(arg.Length, arg);
		}
	}

	public delegate IUnitInterface GetInterfaceCallback(IUnitSim owner, IUnitInterface item);

	public class UnitSimAdpt<T> : IDisposable, IUnitSim, IUnitSimAdpt
		where T : class, IUnitSim
	{
		private Type m_Type;
		protected T m_UnitSim;
		private IInterfaceHub m_InterfaceHub;
		private List<IUnitInterface> m_Interfaces = new List<IUnitInterface>();
		private string m_UnitName;

		public UnitSimAdpt(T unitSim, IInterfaceHub hub)
		{
			m_Type = unitSim.GetType();
			m_UnitSim = unitSim;
			Init(hub);
		}

		public UnitSimAdpt(IInterfaceHub hub)
		{
			m_Type = typeof(T);
			Create(hub);
		}

		protected Type Type => m_Type;

		protected virtual void Create(IInterfaceHub hub)
		{
			var ctor = m_Type.GetConstructor(new Type[] { typeof(IInterfaceHub) });
			m_UnitSim = (T)ctor.Invoke(new object[] { hub });
			Init(hub);
		}

		public void Dispose()
		{
			foreach (var item1 in m_Interfaces) {
				var item2 = (UnitInterfaceAdpt)item1;
				item2.Target = null;
			}

			m_UnitSim = default(T);
			if (m_UnitSim is IDisposable dis)
				dis.Dispose();
		}

		public void Restart()
		{
			Dispose();

			Create(m_InterfaceHub);
		}

		protected void Init(IInterfaceHub hub)
		{
			m_InterfaceHub = hub;
			m_UnitName = m_UnitSim.UnitName;
			m_UnitSim.UnitSetEvent += new UnitEventHandler(UnitSim_UnitSetEvent);
			m_UnitSim.UnitStart += new UnitEventHandler(UnitSim_UnitStart);
			m_UnitSim.UnitExit += new UnitEventHandler(UnitSim_UnitExit);
			m_UnitSim.UnitIdle += new UnitEventHandler(UnitSim_UnitIdle);
			m_UnitSim.UnitOutput += new UnitOutputEventHandler(UnitSim_Log);
			m_UnitSim.UnitGetSystemTime += new UnitGetSystemTimeEventHandler(UnitSim_UnitGetSystemTime);
			m_UnitSim.AddInterface += new UnitInterfaceEventHandler(UnitSim_AddInterface);
			m_UnitSim.RemoveInterface += new UnitInterfaceEventHandler(UnitSim_RemoveInterface);
			m_UnitSim.UpdateInterface += new UnitInterfaceEventHandler(UnitSim_UpdateInterface);
			foreach (var item in m_UnitSim.Interfaces) {
				var oldItem = (UnitInterfaceAdpt)m_Interfaces.Find(
					delegate (IUnitInterface item1) {
						return ((UnitInterfaceAdpt)item1).InterfaceName == item.InterfaceName;
					});

				if (oldItem != null) {
					oldItem.Target = item;
					continue;
				}

				var adpt = GetOrGenerateInterface(item);
				m_Interfaces.Add(adpt);
			}
		}

		public IUnitInterface FindInterface(string name)
		{
			return m_Interfaces.Find(delegate (IUnitInterface cmp) {
				return cmp.InterfaceName == name;
			});
		}

		public static GetInterfaceCallback GetInterface;

		private IUnitInterface GetOrGenerateInterface(IUnitInterface item)
		{
			IUnitInterface uif = null;

			if (GetInterface != null)
				uif = GetInterface(this, item);

			if (uif == null)
				uif = GenerateInterface(item);

			return uif;
		}

		/// <summary>
		/// デフォルト値の式を返す
		/// </summary>
		/// <param name="type"></param>
		/// <returns></returns>
		private CodeExpression GetDefaultValue(Type type)
		{
			if (type == typeof(string))
				return new CodePrimitiveExpression("");
			else if (type == typeof(bool))
				return new CodePrimitiveExpression(false);
			else if (type.IsPrimitive)
				return new CodePrimitiveExpression(0);
			else if (type.IsEnum)
				return new CodeCastExpression(type, new CodePrimitiveExpression(0));
			else if (type.IsValueType) {
				var ret = new CodeObjectCreateExpression {
					CreateType = new CodeTypeReference(type)
				};
				return ret;
			}
			else
				return new CodePrimitiveExpression(null);
		}

		private static Dictionary<Type, Type> m_AdapterTypes = new Dictionary<Type, Type>();

		/// <summary>
		/// 動的にアダプタークラスを作成する
		/// </summary>
		/// <param name="item"></param>
		/// <returns></returns>
		private IUnitInterface GenerateInterface(IUnitInterface item)
		{
			var itemType = item.GetType();
			var itemInterfaces = itemType.GetInterfaces();

			if (!m_AdapterTypes.TryGetValue(itemType, out var adptType)) {
				var assembles = new List<string>();
				var execAssembly = Assembly.GetExecutingAssembly();
				assembles.Add(execAssembly.Location);

				var nullExp = new CodePrimitiveExpression(null);

				// 名前空間
				var nameSpace = new CodeNamespace("console");

				// クラス
				var className = "UnitIFAdapter" + (m_AdapterTypes.Count + 1).ToString();
				var newType = new CodeTypeDeclaration(className);
				nameSpace.Types.Add(newType);

				newType.BaseTypes.Add(new CodeTypeReference(typeof(UnitInterfaceAdpt)));

				// コンストラクタ
				{
					var ctor = new CodeConstructor();
					newType.Members.Add(ctor);

					ctor.Attributes = MemberAttributes.Public;

					var param1 = new CodeParameterDeclarationExpression {
						Type = new CodeTypeReference(itemType),
						Name = "unitIf"
					};
					ctor.Parameters.Add(param1);

					var param2 = new CodeParameterDeclarationExpression {
						Type = new CodeTypeReference(typeof(IUnitSim)),
						Name = "unitSim"
					};
					ctor.Parameters.Add(param2);

					ctor.BaseConstructorArgs.Add(new CodeArgumentReferenceExpression("unitIf"));
					ctor.BaseConstructorArgs.Add(new CodeArgumentReferenceExpression("unitSim"));
				}

				var this_Target = new CodeFieldReferenceExpression {
					TargetObject = new CodeThisReferenceExpression(),
					FieldName = "Target"
				};

				// インターフェイスを実装
				foreach (var ifType in itemInterfaces) {
					if ((ifType == typeof(IUnitInterface) || (ifType == typeof(IUnitInterfaceAdpt))))
						continue;

					var assemblyLocation = Assembly.GetAssembly(ifType).Location;
					if (!assembles.Contains(assemblyLocation))
						assembles.Add(assemblyLocation);

					newType.BaseTypes.Add(new CodeTypeReference(ifType));

					var if_Target = new CodeCastExpression {
						Expression = this_Target,
						TargetType = new CodeTypeReference(ifType)
					};

					// ラッパープロパティの実装
					foreach (var pInfo in ifType.GetProperties()) {
						var prop = new CodeMemberProperty();
						newType.Members.Add(prop);

						prop.PrivateImplementationType = new CodeTypeReference(ifType);
						prop.Attributes = 0;
						prop.Name = pInfo.Name;
						prop.Type = new CodeTypeReference(pInfo.PropertyType);

						assemblyLocation = Assembly.GetAssembly(pInfo.PropertyType).Location;
						if (!assembles.Contains(assemblyLocation))
							assembles.Add(assemblyLocation);

						var indexes = new List<CodeExpression>();

						var paramInfos = pInfo.GetIndexParameters();
						foreach (var paramInfo in paramInfos) {
							var param = new CodeParameterDeclarationExpression();
							prop.Parameters.Add(param);

							var dir = (FieldDirection)0;
							if (paramInfo.IsIn)
								dir |= FieldDirection.In;
							if (paramInfo.IsOut)
								dir |= FieldDirection.Out;
							if (paramInfo.IsRetval)
								dir |= FieldDirection.Ref;
							param.Direction = dir;
							param.Type = new CodeTypeReference(paramInfo.ParameterType);
							param.Name = paramInfo.Name;

							assemblyLocation = Assembly.GetAssembly(paramInfo.ParameterType).Location;
							if (!assembles.Contains(assemblyLocation))
								assembles.Add(assemblyLocation);

							indexes.Add(new CodeArgumentReferenceExpression(paramInfo.Name));
						}

						prop.HasGet = pInfo.GetGetMethod() != null;
						if (prop.HasGet) {
							var ifState = new CodeConditionStatement();
							prop.GetStatements.Add(ifState);

							var ifStateCondition = new CodeBinaryOperatorExpression {
								Operator = CodeBinaryOperatorType.IdentityEquality,
								Left = this_Target,
								Right = nullExp
							};
							ifState.Condition = ifStateCondition;

							var trueRetState = new CodeMethodReturnStatement();
							ifState.TrueStatements.Add(trueRetState);
							trueRetState.Expression = GetDefaultValue(pInfo.PropertyType);

							var falseRetState = new CodeMethodReturnStatement();
							ifState.FalseStatements.Add(falseRetState);
							if (paramInfos.Length > 0) {
								falseRetState.Expression = new CodeArrayIndexerExpression(if_Target, indexes.ToArray());
							}
							else
								falseRetState.Expression = new CodeFieldReferenceExpression(if_Target, pInfo.Name);
						}
						prop.HasSet = pInfo.GetSetMethod() != null;
						if (prop.HasSet) {
							var ifState = new CodeConditionStatement();
							prop.SetStatements.Add(ifState);

							var ifStateCondition = new CodeBinaryOperatorExpression {
								Operator = CodeBinaryOperatorType.IdentityInequality,
								Left = this_Target,
								Right = nullExp
							};
							ifState.Condition = ifStateCondition;

							var trueRetState = new CodeAssignStatement();
							ifState.TrueStatements.Add(trueRetState);
							if (paramInfos.Length > 0) {
								trueRetState.Left = new CodeArrayIndexerExpression(if_Target, indexes.ToArray());
							}
							else
								trueRetState.Left = new CodeFieldReferenceExpression(if_Target, pInfo.Name);
							trueRetState.Right = new CodePropertySetValueReferenceExpression();
						}
					}

					// ラッパーメソッドの実装
					foreach (var mInfo in ifType.GetMethods()) {
						if (mInfo.IsSpecialName)
							continue;

						var method = new CodeMemberMethod();
						newType.Members.Add(method);

						method.PrivateImplementationType = new CodeTypeReference(ifType);
						method.Attributes = 0;
						method.Name = mInfo.Name;
						method.ReturnType = new CodeTypeReference(mInfo.ReturnType);

						assemblyLocation = Assembly.GetAssembly(mInfo.ReturnType).Location;
						if (!assembles.Contains(assemblyLocation))
							assembles.Add(assemblyLocation);

						var prms = new List<CodeExpression>();

						var paramInfos = mInfo.GetParameters();
						foreach (var paramInfo in paramInfos) {
							var param = new CodeParameterDeclarationExpression();
							method.Parameters.Add(param);

							var dir = (FieldDirection)0;
							if (paramInfo.IsIn)
								dir |= FieldDirection.In;
							if (paramInfo.IsOut)
								dir |= FieldDirection.Out;
							if (paramInfo.IsRetval)
								dir |= FieldDirection.Ref;
							param.Direction = dir;
							param.Type = new CodeTypeReference(paramInfo.ParameterType);
							param.Name = paramInfo.Name;

							assemblyLocation = Assembly.GetAssembly(paramInfo.ParameterType).Location;
							if (!assembles.Contains(assemblyLocation))
								assembles.Add(assemblyLocation);

							var prm = new CodeArgumentReferenceExpression(paramInfo.Name);
							if (dir != 0)
								prms.Add(new CodeDirectionExpression(dir, prm));
							else
								prms.Add(prm);
						}

						var cmre = new CodeMethodReferenceExpression(if_Target, method.Name);
						if (mInfo.ReturnType == typeof(void)) {
							var ifState = new CodeConditionStatement();
							method.Statements.Add(ifState);

							var ifStateCondition = new CodeBinaryOperatorExpression {
								Operator = CodeBinaryOperatorType.IdentityInequality,
								Left = this_Target,
								Right = nullExp
							};
							ifState.Condition = ifStateCondition;

							ifState.TrueStatements.Add(new CodeMethodInvokeExpression(cmre, prms.ToArray()));
						}
						else {
							var ifState = new CodeConditionStatement();
							method.Statements.Add(ifState);

							var ifStateCondition = new CodeBinaryOperatorExpression {
								Operator = CodeBinaryOperatorType.IdentityEquality,
								Left = this_Target,
								Right = nullExp
							};
							ifState.Condition = ifStateCondition;

							var trueRetState = new CodeMethodReturnStatement();
							ifState.TrueStatements.Add(trueRetState);
							trueRetState.Expression = GetDefaultValue(mInfo.ReturnType);

							var falseRetState = new CodeMethodReturnStatement();
							ifState.FalseStatements.Add(falseRetState);
							falseRetState.Expression = new CodeMethodInvokeExpression(cmre, prms.ToArray());
						}
					}
				}

				var compileUnit = new CodeCompileUnit();
				compileUnit.Namespaces.Add(nameSpace);

				compileUnit.ReferencedAssemblies.AddRange(assembles.ToArray());

				var provider = new CSharpCodeProvider();

				var cplparam = new CompilerParameters {
					GenerateInMemory = true
				};

				CodeCompileUnit[] units = { compileUnit };
				var result = provider.CompileAssemblyFromDom(cplparam, units);

				if (result.Errors.Count > 0) {
					var option = new CodeGeneratorOptions();
					var sb = new StringBuilder();
					TextWriter tw = new StringWriter(sb);

					provider.GenerateCodeFromCompileUnit(compileUnit, tw, option);

					tw.Close();

					System.Diagnostics.Debug.WriteLine(sb);
					foreach (CompilerError text in result.Errors)
						System.Diagnostics.Debug.WriteLine(text.ErrorText);

					return new UnitInterfaceAdpt(item, this);
				}

				var assembly = result.CompiledAssembly;

				adptType = assembly.GetType("console." + className);

				m_AdapterTypes.Add(itemType, adptType);
			}

			var ctorInfo = adptType.GetConstructor(new Type[] { itemType, typeof(IUnitSim) });
			return (IUnitInterface)ctorInfo.Invoke(new object[] { item, this });
		}

		public T Target => m_UnitSim;

		IUnitSim IUnitSimAdpt.Target => m_UnitSim;

		public static T GetTarget(IUnitSim unit)
		{
			if (!(unit is IUnitSimAdpt adpt)) {
				if (unit is T bare)
					return bare;
				return null;
			}

			return adpt.Target as T;
		}

		private void UnitSim_UnitSetEvent(IUnitSim sender)
		{
			UnitSetEvent?.Invoke(this);
		}

		private void UnitSim_UnitStart(IUnitSim sender)
		{
			UnitStart?.Invoke(this);
		}

		private void UnitSim_UnitExit(IUnitSim sender)
		{
			UnitExit?.Invoke(this);
		}

		private void UnitSim_UnitIdle(IUnitSim sender)
		{
			UnitIdle?.Invoke(this);
		}

		private void UnitSim_Log(IUnitSim sender, int Kind, byte[] Data)
		{
			UnitOutput?.Invoke(this, Kind, Data);
		}

		private void UnitSim_UnitGetSystemTime(IUnitSim sender, ref long now)
		{
			UnitGetSystemTime?.Invoke(this, ref now);
		}

		private void UnitSim_AddInterface(IUnitSim sender, IUnitInterface item)
		{
			var adpt = GetOrGenerateInterface(item);
			m_Interfaces.Add(adpt);
			AddInterface?.Invoke(this, adpt);
		}

		private void UnitSim_RemoveInterface(IUnitSim sender, IUnitInterface item)
		{
			foreach (var item1 in m_Interfaces) {
				var item2 = (UnitInterfaceAdpt)item1;
				if (item2.Target == item) {
					item2.Target = null;
					break;
				}
			}
		}

		private void UnitSim_UpdateInterface(IUnitSim sender, IUnitInterface item)
		{
			if (UpdateInterface != null) {
				foreach (var item1 in m_Interfaces) {
					var item2 = (UnitInterfaceAdpt)item1;
					if (item2.Target == item) {
						UpdateInterface(this, item2);
						break;
					}
				}
			}
		}

		#region IUnitSim メンバ

		public string UnitName {
			get { if (m_UnitSim != null) m_UnitName = m_UnitSim.UnitName; return m_UnitName; }
			set { m_UnitName = value; if (m_UnitSim != null) m_UnitSim.UnitName = value; }
		}

		public bool IsAlive {
			get { if (m_UnitSim == null) return false; return m_UnitSim.IsAlive; }
		}

		public event UnitEventHandler UnitSetEvent;

		public event UnitEventHandler UnitStart;

		public event UnitEventHandler UnitExit;

		public event UnitEventHandler UnitIdle;

		public event UnitOutputEventHandler UnitOutput;

		public event UnitGetSystemTimeEventHandler UnitGetSystemTime;

		public event UnitInterfaceEventHandler AddInterface;

		public event UnitInterfaceEventHandler RemoveInterface;

		public event UnitInterfaceEventHandler UpdateInterface;

		public void Prepare()
		{
			if (m_UnitSim != null)
				m_UnitSim.Prepare();
		}

		public void Start()
		{
			if (m_UnitSim != null)
				m_UnitSim.Start();
		}

		public void Exit()
		{
			if (m_UnitSim != null)
				m_UnitSim.Exit();
		}

		public IList<IUnitInterface> Interfaces => m_Interfaces.AsReadOnly();

		public void Interrupt(int IntNo)
		{
			if (m_UnitSim != null)
				m_UnitSim.Interrupt(IntNo);
		}

		public void Input(int kind, byte[] Data)
		{
			if (m_UnitSim != null)
				m_UnitSim.Input(kind, Data);
		}

		public int ReadAddr(uint addr, byte[] dst)
		{
			if (m_UnitSim != null)
				return m_UnitSim.ReadAddr(addr, dst);
			return 0;
		}

		public int WriteAddr(uint addr, byte[] src)
		{
			if (m_UnitSim != null)
				return m_UnitSim.WriteAddr(addr, src);
			return 0;
		}

		public bool GetBit(uint addr, int bit)
		{
			if (m_UnitSim != null)
				return m_UnitSim.GetBit(addr, bit);
			return false;
		}

		public void SetBit(uint addr, int bit, bool value)
		{
			if (m_UnitSim != null)
				m_UnitSim.SetBit(addr, bit, value);
		}

		public long GetTimer()
		{
			if (m_UnitSim != null)
				return m_UnitSim.GetTimer();
			return -1;
		}

		public void Progress(long timer)
		{
			if (m_UnitSim != null)
				m_UnitSim.Progress(timer);
		}

		public void CallTimeOut()
		{
			if (m_UnitSim != null)
				m_UnitSim.CallTimeOut();
		}

		public bool ProcessEvent()
		{
			if (m_UnitSim != null)
				return m_UnitSim.ProcessEvent();
			return false;
		}

		#endregion
	}

	public class UnitInterfaceAdpt : IUnitInterface, IUnitInterfaceAdpt
	{
		public UnitInterfaceAdpt(IUnitInterface unitInterface, IUnitSim unitSim)
		{
			Target = unitInterface;
			TypeName = unitInterface.TypeName;
			InterfaceName = unitInterface.InterfaceName;
			UnitSim = unitSim;
			IFKind = unitInterface.IFKind;
		}

		public IUnitInterface Target { get; set; }

		#region IUnitInterface メンバ

		public string TypeName { get; }

		public string InterfaceName { get; }

		public IUnitSim UnitSim { get; }

		public int IFKind { get; }

		public Action<IUnitInterface, byte[]> Output { get; set; }

		public void Push(byte[] data)
		{
			Output?.Invoke(this, data);
		}

		public void Input(byte[] data)
		{
			UnitSim.Input(IFKind, data);
		}

		#endregion

		public static bool Is<C>(IUnitInterface item) where C : class
		{
			if (!(item is C cast)) {
				var adpt = item as IUnitInterfaceAdpt;
				cast = adpt.Target as C;
			}

			return cast != null;
		}

		public static C As<C>(IUnitInterface item) where C : class
		{
			if (!(item is C cast)) {
				var adpt = item as IUnitInterfaceAdpt;
				cast = adpt.Target as C;
			}

			return cast;
		}
	}
}

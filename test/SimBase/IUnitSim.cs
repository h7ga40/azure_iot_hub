//------------------------------------------------------------------------------
// Embedded Software Simulation Base Classes
// Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: IUnitSim.cs 1970 2019-07-10 00:36:30Z coas-nagasima $
using System;
using System.Collections.Generic;

namespace SimBase
{
	public interface IUnitInterface
	{
		string TypeName { get; }
		string InterfaceName { get; }
		IUnitSim UnitSim { get; }
		int IFKind { get; }
		Action<IUnitInterface, byte[]> Output { get; set; }
		void Push(byte[] data);
		void Input(byte[] data);
	}

	public interface IInterfaceHub
	{
		bool Send(IUnitInterface sender, object message);
	}

	public delegate void UnitEventHandler(IUnitSim sender);
	public delegate void UnitInterfaceEventHandler(IUnitSim sender, IUnitInterface item);
	public delegate void UnitOutputEventHandler(IUnitSim sender, int kind, byte[] data);
	public delegate void UnitGetSystemTimeEventHandler(IUnitSim sender, ref long now);

	public interface IUnitSim
	{
		void Prepare();
		void Start();
		void Exit();
		bool IsAlive { get; }
		void Interrupt(int intNo);
		int ReadAddr(uint addr, byte[] dst);
		int WriteAddr(uint addr, byte[] src);
		void SetBit(uint addr, int bit, bool value);
		bool GetBit(uint addr, int bit);
		void Input(int kind, byte[] Data);
		long GetTimer();
		void Progress(long interval);
		void CallTimeOut();
		bool ProcessEvent();
		string UnitName { get; set; }
		event UnitEventHandler UnitSetEvent;
		event UnitEventHandler UnitStart;
		event UnitEventHandler UnitExit;
		event UnitEventHandler UnitIdle;
		event UnitOutputEventHandler UnitOutput;
		event UnitGetSystemTimeEventHandler UnitGetSystemTime;
		IList<IUnitInterface> Interfaces { get; }
		event UnitInterfaceEventHandler AddInterface;
		event UnitInterfaceEventHandler RemoveInterface;
		event UnitInterfaceEventHandler UpdateInterface;
	}

	public interface IUnitSimAdpt
	{
		IUnitSim Target { get; }
		void Restart();
	}

	public interface IUnitInterfaceAdpt
	{
		IUnitInterface Target { get; set; }
	}

	public interface IUnitSimApp
	{
		void StartExternal(string[] arg);
	}

	public interface ISystemCall
	{
		int SystemCall(int n, int a, int b, int c, int d, int e, int f);
	}
}

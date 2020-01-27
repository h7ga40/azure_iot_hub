using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace SimBase
{
	public class SimRunner
	{
		IEnvironment m_Environment;
		UnitSimList m_UnitList = new UnitSimList();
		Thread m_Thread;
		AutoResetEvent m_Event;
		bool m_Terminated;

		public UnitSimList Units { get { return m_UnitList; } }

		public SimRunner(IEnvironment environment)
		{
			m_Environment = environment;
			m_Thread = new Thread(new ThreadStart(TimerProc));
			m_Thread.Name = "TimerProc";
			m_Event = new AutoResetEvent(false);
			m_Terminated = false;
		}

		public void Start()
		{
			m_Thread.Start();
		}

		public void Terminate()
		{
			m_Terminated = true;
			m_Event.Set();
		}

		public IUnitSim CreateUnit(string ctorText)
		{
			IUnitConsole console = m_Environment.CreateUnitConsole(ctorText);
			IUnitSim unit = console.Prepare();
			if (unit == null)
				return null;

			m_UnitList.Add(unit);
			unit.UnitExit += UnitSim_UnitExit;
			unit.UnitOutput += UnitSim_Log;
			unit.UnitSetEvent += Unit_UnitSetEvent;
			unit.UnitGetSystemTime += Unit_UnitGetSystemTime;

			return unit;
		}

		void UnitSim_UnitExit(IUnitSim sender)
		{
			m_UnitList.Remove(sender);
			m_Environment.UnitExit(sender);
		}

		protected void UnitSim_Log(IUnitSim sender, int kind, byte[] data)
		{
			m_Environment.Log(sender, kind, data);
		}

		public const long Frequency = 10000000/*[100nsec]=1[sec]*/;

		void TimerProc()
		{
			long prev, now, sim, esc;

			now = DateTime.Now.Ticks;
			sim = now;
			esc = -Frequency / 1000;

			IUnitSim[] units = m_UnitList.ToArray();

			while (!m_Terminated) {
				// タイムアウト処理があればを実行する
				CallTimeOut(units);

				// タイマーの最小値を取得
				long timer = GetTimer(units);
				bool signal = false;

				if (timer >= 0) {
					bool taskSwitch = timer == 0;
					// タイムアウト後の時刻から、タイマー時間を計算
					long target = sim + timer;
					long diff = target - now;
					if (diff < 0) {
						// 差が拡がり過ぎたらシミュレーション時刻を現在時刻にする
						if (diff < esc)
							sim = now;
						diff = 0;
					}

					if ((diff > 0) || taskSwitch)
						signal = m_Event.WaitOne(new TimeSpan(diff));
				}
				else
					signal = m_Event.WaitOne();

				prev = now;
				now = DateTime.Now.Ticks;

				// 経過時間の計算、シグナルで起こされた場合、測定値を使用
				long interval = (signal || (timer <= 0)) ? (now - prev) : timer;

				sim += interval;

				units = m_UnitList.ToArray();

				// 時間が進んだことをメッセージ処理に知らせる
				Progress(units, interval);

				// イベントを処理する
				ProcessEvent(units);
			}

			m_Environment.OnTerminated();
		}

		private long GetTimer(IUnitSim[] units)
		{
			long result, timer;

			result = -1;

			foreach (IUnitSim unit in units) {
				if (!unit.IsAlive)
					continue;

				timer = unit.GetTimer();
				if (timer != -1) {
					if (result != -1) {
						if (timer < result)
							result = timer;
					}
					else
						result = timer;
				}
			}

			return result;
		}

		private void Progress(IUnitSim[] units, long timer)
		{
			foreach (IUnitSim unit in units) {
				if (unit.IsAlive)
					unit.Progress(timer);
			}
		}

		private void CallTimeOut(IUnitSim[] units)
		{
			foreach (IUnitSim unit in units) {
				if (unit.IsAlive)
					unit.CallTimeOut();
			}
		}

		private void ProcessEvent(IUnitSim[] units)
		{
			foreach (IUnitSim unit in units) {
				if (unit.IsAlive)
					unit.ProcessEvent();
			}
		}

		void Unit_UnitSetEvent(IUnitSim sender)
		{
			m_Event.Set();
		}

		static long m_Start = (new DateTime(1970, 1, 1)).Ticks;

		void Unit_UnitGetSystemTime(IUnitSim sender, ref long now)
		{
			now = DateTime.Now.Ticks - m_Start;
		}
	}
}

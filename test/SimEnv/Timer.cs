using uITron3;

namespace SimEnv
{
	internal class Timer : ISysTimerSync
	{
		private Kernel m_Kernel;
		private readonly int m_IntNo;
		private long m_Timer;
		private readonly long m_Interval;
		private bool m_TimeOut;

		public Timer(Kernel kernel, int intNo)
		{
			m_Kernel = kernel;
			m_IntNo = intNo;
			m_Interval = SimBase.SimRunner.Frequency / 1000;    // 1/1000[sec]=1ms
			m_Timer = m_Interval;
		}

		public long GetTimer()
		{
			return m_Timer;
		}

		public void Progress(long interval)
		{
			m_Timer -= interval;
			if (m_Timer <= 0) {
				//m_Timer += m_Interval;
				m_Timer = m_Interval;
				m_TimeOut = true;
			}
		}

		public void CallTimeOut()
		{
			if (m_TimeOut) {
				m_TimeOut = false;
				m_Kernel.Interrupt(m_IntNo);
			}
		}
	}
}

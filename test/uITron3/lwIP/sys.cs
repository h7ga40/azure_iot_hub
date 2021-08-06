using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace uITron3
{
	internal partial class sys
	{
		lwip lwip;
		Nucleus m_Nucleus;
		static Random m_Random = new Random();
		public static int LWIP_RAND() { return m_Random.Next(); }

		public int old_level;
		public int lev;

		/** Return code for timeouts from sys_arch_mbox_fetch and sys_arch_sem_wait */
		public const int SYS_ARCH_TIMEOUT = unchecked((int)0xffffffff);

		/** sys_mbox_tryfetch() returns SYS_MBOX_EMPTY if appropriate.
		 * For now we use the same magic value, but we allow this to change in future.
		 */
		public const int SYS_MBOX_EMPTY = -1;

		public sys(lwip lwip, Nucleus nucleus)
		{
			this.lwip = lwip;
			m_Nucleus = nucleus;
		}

		internal uint sys_now()
		{
			SYSTIME time = new SYSTIME();

			m_Nucleus.GetSystemTime(out time);

			return (uint)(time.Value);
		}

		public void SYS_ARCH_PROTECT(int lvl)
		{
			m_Nucleus.LockCPU();
		}

		public void SYS_ARCH_UNPROTECT(int lvl)
		{
			m_Nucleus.UnlockCPU();
		}

		public void SYS_ARCH_DECL_PROTECT(int lvl)
		{
		}

		internal err_t sys_mutex_new(sys_mutex_t mem_mutex)
		{
			return err_t.ERR_OK;
		}

		internal void sys_mutex_lock(sys_mutex_t mem_mutex)
		{
			Monitor.Enter(mem_mutex);
		}

		internal void sys_mutex_unlock(sys_mutex_t mem_mutex)
		{
			Monitor.Exit(mem_mutex);
		}

		internal int sys_arch_mbox_fetch<P>(sys_mbox_t<P> mbox, out P msg, int tmout) where P : class
		{
			return mbox.fetch(out msg, tmout);
		}

		internal bool sys_mbox_valid<P>(sys_mbox_t<P> mbox) where P : class
		{
			return mbox != null;
		}

		internal err_t sys_mbox_post<P>(sys_mbox_t<P> mbox, P msg) where P : class
		{
			return mbox.post(msg);
		}

		internal err_t sys_mbox_trypost<P>(sys_mbox_t<P> mbox, P msg) where P : class
		{
			return mbox.trypost(msg);
		}

		internal static void sys_init(lwip lwip, Nucleus nucleus)
		{
			lwip.sys = new sys(lwip, nucleus);
		}
	}

	partial class lwip
	{
		internal sys sys;
	}

	public class sys_mutex_t
	{
	}

	public class sys_mbox_t<P> where P : class
	{
		Mutex mutex = new Mutex();
		Queue<P> msgs = new Queue<P>();
		AutoResetEvent @event = new AutoResetEvent(false);

		internal int fetch(out P msg, int tmout)
		{
			if (tmout == 0) {
				tmout = Timeout.Infinite;
			}

			mutex.WaitOne();
			if (msgs.Count == 0) {
				mutex.ReleaseMutex();

				if (tmout == 0) {
					msg = null;
					return sys.SYS_MBOX_EMPTY;
				}
				else if (!@event.WaitOne(tmout)) {
					msg = null;
					return sys.SYS_ARCH_TIMEOUT;
				}

				mutex.WaitOne();
			}
			msg = msgs.Dequeue();
			mutex.ReleaseMutex();

			return 0;
		}

		internal err_t post(P msg)
		{
			mutex.WaitOne();

			msgs.Enqueue(msg);

			mutex.ReleaseMutex();

			@event.Set();

			return err_t.ERR_OK;
		}

		internal err_t trypost(P msg)
		{
			mutex.WaitOne();

			msgs.Enqueue(msg);

			mutex.ReleaseMutex();

			@event.Set();

			return err_t.ERR_OK;
		}
	}
}

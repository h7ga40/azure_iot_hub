using System.Threading;

namespace uITron3
{
	enum tcpip_msg_type
	{
#if LWIP_NETCONN
		TCPIP_MSG_API,
#endif // LWIP_NETCONN
		TCPIP_MSG_INPKT,
#if LWIP_NETIF_API
		TCPIP_MSG_NETIFAPI,
#endif // LWIP_NETIF_API
#if LWIP_TCPIP_TIMEOUT
		TCPIP_MSG_TIMEOUT,
		TCPIP_MSG_UNTIMEOUT,
#endif // LWIP_TCPIP_TIMEOUT
		TCPIP_MSG_CALLBACK,
		TCPIP_MSG_CALLBACK_STATIC,
		End
	};

	public delegate void tcpip_callback_fn(object arg);

	class tcpip_msg : memp
	{
		public tcpip_msg_type type;

		public tcpip_msg(lwip lwip)
			: base(lwip)
		{
		}

		public class cb_t
		{
			public tcpip_callback_fn function;
			public object ctx;
		}

		public class msg_t
		{
			public cb_t cb { get; } = new cb_t();
		}

		public msg_t msg { get; } = new msg_t();
	}

	public class tcpip
	{
		private lwip lwip;
		private Thread thread;
		private sys_mbox_t<tcpip_msg> mbox = new sys_mbox_t<tcpip_msg>();
		private int thread_state;

		public tcpip(lwip lwip)
		{
			this.lwip = lwip;

			thread = new Thread(tcpip_thread);
			thread_state = 0;
			thread.Start();
		}

		internal static void tcpip_init(lwip lwip)
		{
			lwip.tcpip = new tcpip(lwip);
		}

		internal void LWIP_TCPIP_THREAD_ALIVE() { return; }

		private sys_mutex_t lock_tcpip_core = new sys_mutex_t();

		internal void LOCK_TCPIP_CORE() => lwip.sys.sys_mutex_lock(lock_tcpip_core);

		internal void UNLOCK_TCPIP_CORE() => lwip.sys.sys_mutex_unlock(lock_tcpip_core);

		/**
		 * Call a specific function in the thread context of
		 * tcpip_thread for easy access synchronization.
		 * A function called in that way may access lwIP core code
		 * without fearing concurrent access.
		 *
		 * @param f the function to call
		 * @param ctx parameter passed to f
		 * @param block 1 to block until the request is posted, 0 to non-blocking mode
		 * @return ERR_OK if the function was called, another err_t if not
		 */
		public err_t tcpip_callback_with_block(tcpip_callback_fn function, object ctx, byte block)
		{
			tcpip_msg msg;

			if (lwip.sys.sys_mbox_valid(mbox)) {
				msg = (tcpip_msg)lwip.memp_malloc(memp_t.MEMP_TCPIP_MSG_API);
				if (msg == null) {
					return err_t.ERR_MEM;
				}

				msg.type = tcpip_msg_type.TCPIP_MSG_CALLBACK;
				msg.msg.cb.function = function;
				msg.msg.cb.ctx = ctx;
				if (block != 0) {
					lwip.sys.sys_mbox_post(mbox, msg);
				}
				else {
					if (lwip.sys.sys_mbox_trypost(mbox, msg) != err_t.ERR_OK) {
						lwip.memp_free(memp_t.MEMP_TCPIP_MSG_API, msg);
						return err_t.ERR_MEM;
					}
				}
				return err_t.ERR_OK;
			}
			return err_t.ERR_VAL;
		}

		private void tcpip_thread()
		{
			tcpip_msg msg;

			while (thread_state == 0) {
				lwip.sys.sys_timeouts_mbox_fetch(mbox, out msg);

				switch (msg.type) {
				case tcpip_msg_type.End:
					goto end;
				}
			}

		end:
			thread_state = 2;
		}

		public void Terminate()
		{
			thread_state = 1;

			lwip.sys.sys_mbox_post(mbox, new tcpip_msg(lwip) { type = tcpip_msg_type.End });

			thread.Join();

			lwip.tcpip = null;
		}
	}

	partial class lwip
	{
		internal tcpip tcpip;
	}
}

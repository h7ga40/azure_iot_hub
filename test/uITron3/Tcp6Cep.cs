using System;
using System.Collections.Generic;

namespace uITron3
{
	public struct T_TCP6_CCEP
	{
		public const ushort TCP_PORTANY = 0;

		public ATR cepatr;
		public CepCallback callback;
		public object exinf;
	}

	public struct T_TCP6_CREP
	{
		public ATR repatr;              /* 受付口属性		*/
		public T_IPV6EP myaddr;         /* 自分のアドレス	*/
		public object exinf;
	}

	internal class Tcp6Cep : StateMachine<bool>
	{
		private ID m_CepID;
		private T_TCP6_CCEP m_ctcp;
		private Nucleus m_Nucleus;
		private lwip m_lwIP;

		public Tcp6Cep(ID cepid, ref T_TCP6_CCEP pk_ccep, Nucleus nucleus, lwip lwIP)
		{
			m_CepID = cepid;
			m_ctcp = pk_ccep;
			m_Nucleus = nucleus;
			this.m_lwIP = lwIP;
		}

		public ID CepID { get { return m_CepID; } }

		public Nucleus Nucleus { get { return m_Nucleus; } }

		public T_TCP6_CCEP ctcp { get { return m_ctcp; } }


		internal ER Accept(Tcp6Rep tcpRep, T_IPV6EP p_dstaddr, TMO tmout)
		{
			throw new NotImplementedException();
		}

		internal ER SendBuffer(int len)
		{
			throw new NotImplementedException();
		}

		internal ER Connect(T_IPV6EP p_myaddr, T_IPV6EP p_dstaddr, TMO tmout)
		{
			throw new NotImplementedException();
		}

		internal ER Shutdown()
		{
			throw new NotImplementedException();
		}

		internal ER Close(TMO tmout)
		{
			throw new NotImplementedException();
		}

		internal ER SendData(pointer data, int len, TMO tmout)
		{
			throw new NotImplementedException();
		}

		internal ER ReceiveDataNblk(pointer data, int len)
		{
			throw new NotImplementedException();
		}

		internal ER ReceiveData(pointer data, int len, TMO tmout)
		{
			throw new NotImplementedException();
		}

		internal ER ReceiveBuffer(ref pointer p_buf, TMO tmout)
		{
			throw new NotImplementedException();
		}

		internal ER ReleaseBuffer(int len)
		{
			throw new NotImplementedException();
		}

		internal ER SendUrgentData(pointer data, int len, TMO tmout)
		{
			throw new NotImplementedException();
		}

		internal ER ReceiveUrgentData(pointer data, int len)
		{
			throw new NotImplementedException();
		}

		internal ER SetOption(int optname, pointer optval, int optlen)
		{
			throw new NotImplementedException();
		}

		internal ER GetOption(int optname, pointer optval, int optlen)
		{
			throw new NotImplementedException();
		}

		internal ER GetBuffer(ref pointer p_buf, TMO tmout)
		{
			throw new NotImplementedException();
		}

		internal ER Cancel(FN fncd)
		{
			throw new NotImplementedException();
		}
	}

	internal class Tcp6Rep : StateMachine<bool>
	{
		private ID m_RepID;
		private T_TCP6_CREP m_ctcp;
		private Nucleus m_Nucleus;
		private lwip m_lwIP;

		public Tcp6Rep(ID tcpid, ref T_TCP6_CREP pk_ctcp, Nucleus nucleus, lwip lwIP)
		{
			m_RepID = tcpid;
			m_ctcp = pk_ctcp;
			m_Nucleus = nucleus;
			m_lwIP = lwIP;
		}

		public ID RepID { get { return m_RepID; } }

		public Nucleus Nucleus { get { return m_Nucleus; } }

		public T_TCP6_CREP ctcp { get { return m_ctcp; } }
	}
}

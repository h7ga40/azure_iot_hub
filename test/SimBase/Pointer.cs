using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimBase
{
	public struct pvoid
	{
		public static readonly pvoid Null = new pvoid();
		private byte[] m_Memory;
		private int m_Offset;

		public pvoid(byte[] memory, int offset = 0)
		{
			m_Memory = memory;
			m_Offset = offset;
		}

		public byte[] Memory { get { return m_Memory; } }
		public int Offset { get { return m_Offset; } }
		public bool IsNull { get { return m_Memory == null; } }

		public static bool operator ==(pvoid a, pvoid b)
		{
			if (a.m_Memory != b.m_Memory)
				throw new ArgumentException();

			return a.m_Offset == b.m_Offset;
		}

		public static bool operator !=(pvoid a, pvoid b)
		{
			if (a.m_Memory != b.m_Memory)
				throw new ArgumentException();

			return a.m_Offset != b.m_Offset;
		}

		public override bool Equals(object obj)
		{
			if (!(obj is pvoid))
				return base.Equals(obj);

			return m_Memory == ((pvoid)obj).m_Memory && m_Offset == ((pvoid)obj).m_Offset;
		}

		public override int GetHashCode()
		{
			return m_Memory.GetHashCode() + m_Offset.GetHashCode();
		}

		public static bool operator >(pvoid a, pvoid b)
		{
			if (a.m_Memory != b.m_Memory)
				throw new ArgumentException();

			return a.m_Offset > b.m_Offset;
		}

		public static bool operator >=(pvoid a, pvoid b)
		{
			if (a.m_Memory != b.m_Memory)
				throw new ArgumentException();

			return a.m_Offset >= b.m_Offset;
		}

		public static bool operator <(pvoid a, pvoid b)
		{
			if (a.m_Memory != b.m_Memory)
				throw new ArgumentException();

			return a.m_Offset < b.m_Offset;
		}

		public static bool operator <=(pvoid a, pvoid b)
		{
			if (a.m_Memory != b.m_Memory)
				throw new ArgumentException();

			return a.m_Offset <= b.m_Offset;
		}

		public static explicit operator int(pvoid This)
		{
			return This.m_Offset;
		}

		internal void AddOffset(int v)
		{
			m_Offset += v;
		}

		internal void SubOffset(int v)
		{
			m_Offset -= v;
		}
	}
}

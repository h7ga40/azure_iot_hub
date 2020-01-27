using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimBase
{
	public struct pushort
	{
		public static readonly pushort Null = new pushort();

		pvoid p;

		public pushort(byte[] memory, int offset = 0)
		{
			p = new pvoid(memory, offset);
		}

		public pushort(pvoid pushort, int offset = 0)
		{
			p = new pvoid(pushort.Memory, pushort.Offset + offset);
		}

		public byte[] Memory { get { return p.Memory; } }
		public int Offset { get { return p.Offset; } }
		public bool IsNull { get { return p.Memory == null; } }

		public static implicit operator pvoid(pushort a)
		{
			return a.p;
		}

		public ushort this[int index]
		{
			get { return BitConverter.ToUInt16(p.Memory, p.Offset + sizeof(ushort) * index); }
			set { Buffer.BlockCopy(BitConverter.GetBytes(value), 0, p.Memory, p.Offset + sizeof(ushort) * index, sizeof(ushort)); }
		}
		public static bool operator ==(pushort a, pushort b)
		{
			if (a.p.Memory != b.p.Memory)
				throw new ArgumentException();

			return a.p.Offset == b.p.Offset;
		}

		public static bool operator !=(pushort a, pushort b)
		{
			if (a.p.Memory != b.p.Memory)
				throw new ArgumentException();

			return a.p.Offset != b.p.Offset;
		}

		public override bool Equals(object obj)
		{
			if (!(obj is pushort))
				return base.Equals(obj);

			return p.Memory == ((pushort)obj).p.Memory && p.Offset == ((pushort)obj).p.Offset;
		}

		public override int GetHashCode()
		{
			return p.Memory.GetHashCode() + p.Offset.GetHashCode();
		}

		public static bool operator >(pushort a, pushort b)
		{
			if (a.p.Memory != b.p.Memory)
				throw new ArgumentException();

			return a.p.Offset > b.p.Offset;
		}

		public static bool operator >=(pushort a, pushort b)
		{
			if (a.p.Memory != b.p.Memory)
				throw new ArgumentException();

			return a.p.Offset >= b.p.Offset;
		}

		public static bool operator <(pushort a, pushort b)
		{
			if (a.p.Memory != b.p.Memory)
				throw new ArgumentException();

			return a.p.Offset < b.p.Offset;
		}

		public static bool operator <=(pushort a, pushort b)
		{
			if (a.p.Memory != b.p.Memory)
				throw new ArgumentException();

			return a.p.Offset <= b.p.Offset;
		}

		public static explicit operator int(pushort This)
		{
			return This.p.Offset;
		}

		public static pushort operator +(pushort a, int b)
		{
			return new pushort(a, b * sizeof(ushort));
		}

		public static pushort operator -(pushort a, int b)
		{
			return new pushort(a, -b * sizeof(ushort));
		}

		public static int operator -(pushort a, pushort b)
		{
			if (a.p.Memory != b.p.Memory)
				throw new ArgumentException();

			return a.p.Offset - b.p.Offset;
		}

		public static pushort operator ++(pushort a)
		{
			a.p.AddOffset(sizeof(ushort));
			return a;
		}

		public static pushort operator --(pushort a)
		{
			a.p.SubOffset(sizeof(ushort));
			return a;
		}
	}
}

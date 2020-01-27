using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SimBase
{
	public struct pbyte
	{
		public static readonly pbyte Null = new pbyte();

		pvoid p;

		public pbyte(byte[] memory, int offset = 0)
		{
			p = new pvoid(memory, offset);
		}

		public pbyte(pvoid pbyte, int offset = 0)
		{
			p = new pvoid(pbyte.Memory, pbyte.Offset + offset);
		}

		public byte[] Memory { get { return p.Memory; } }
		public int Offset { get { return p.Offset; } }
		public bool IsNull { get { return p.Memory == null; } }

		public static implicit operator pvoid(pbyte a)
		{
			return a.p;
		}

		public byte this[int index]
		{
			get { return p.Memory[p.Offset + index]; }
			set { p.Memory[p.Offset + index] = value; }
		}

		public static bool operator ==(pbyte a, pbyte b)
		{
			if (a.p.Memory != b.p.Memory)
				throw new ArgumentException();

			return a.p.Offset == b.p.Offset;
		}

		public static bool operator !=(pbyte a, pbyte b)
		{
			if (a.p.Memory != b.p.Memory)
				throw new ArgumentException();

			return a.p.Offset != b.p.Offset;
		}

		public override bool Equals(object obj)
		{
			if (!(obj is pbyte))
				return base.Equals(obj);

			return p.Memory == ((pbyte)obj).p.Memory && p.Offset == ((pbyte)obj).p.Offset;
		}

		public override int GetHashCode()
		{
			return p.Memory.GetHashCode() + p.Offset.GetHashCode();
		}

		public static bool operator >(pbyte a, pbyte b)
		{
			if (a.p.Memory != b.p.Memory)
				throw new ArgumentException();

			return a.p.Offset > b.p.Offset;
		}

		public static bool operator >=(pbyte a, pbyte b)
		{
			if (a.p.Memory != b.p.Memory)
				throw new ArgumentException();

			return a.p.Offset >= b.p.Offset;
		}

		public static bool operator <(pbyte a, pbyte b)
		{
			if (a.p.Memory != b.p.Memory)
				throw new ArgumentException();

			return a.p.Offset < b.p.Offset;
		}

		public static bool operator <=(pbyte a, pbyte b)
		{
			if (a.p.Memory != b.p.Memory)
				throw new ArgumentException();

			return a.p.Offset <= b.p.Offset;
		}

		public static explicit operator int(pbyte This)
		{
			return This.p.Offset;
		}

		public static pbyte operator +(pbyte a, int b)
		{
			return new pbyte(a, b);
		}

		public static pbyte operator -(pbyte a, int b)
		{
			return new pbyte(a, -b);
		}

		public static int operator -(pbyte a, pbyte b)
		{
			if (a.p.Memory != b.p.Memory)
				throw new ArgumentException();

			return a.p.Offset - b.p.Offset;
		}

		public static pbyte operator ++(pbyte a)
		{
			a.p.AddOffset(sizeof(byte));
			return a;
		}

		public static pbyte operator --(pbyte a)
		{
			a.p.SubOffset(sizeof(byte));
			return a;
		}
	}
}

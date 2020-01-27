using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace SimBase
{
	public struct p<T> where T : struct
	{
		public static readonly p<T> Null = new p<T>();

		pvoid pv;

		public p(byte[] memory, int offset = 0)
		{
			pv = new pvoid(memory, offset);
		}

		public p(pvoid pointer, int offset = 0)
		{
			pv = new pvoid(pointer.Memory, pointer.Offset + offset);
		}

		public byte[] Memory { get { return pv.Memory; } }
		public int Offset { get { return pv.Offset; } }
		public bool IsNull { get { return pv.Memory == null; } }

		public static implicit operator pvoid(p<T> a)
		{
			return a.pv;
		}

		public T this[int index]
		{
			get
			{
				T result;
				GCHandle hValue = GCHandle.Alloc(pv.Memory, GCHandleType.Pinned);
				try {
					IntPtr pdata = Marshal.UnsafeAddrOfPinnedArrayElement(pv.Memory, pv.Offset + index * Marshal.SizeOf(typeof(T)));
					result = (T)Marshal.PtrToStructure(pdata, typeof(T));
				}
				finally {
					hValue.Free();
				}
				return result;
			}
			set
			{
				GCHandle hValue = GCHandle.Alloc(pv.Memory, GCHandleType.Pinned);
				try {
					IntPtr pdata = Marshal.UnsafeAddrOfPinnedArrayElement(pv.Memory, pv.Offset + index * Marshal.SizeOf(typeof(T)));
					Marshal.StructureToPtr(value, pdata, false);
				}
				finally {
					hValue.Free();
				}
			}
		}
		public static bool operator ==(p<T> a, p<T> b)
		{
			if (a.pv.Memory != b.pv.Memory)
				throw new ArgumentException();

			return a.pv.Offset == b.pv.Offset;
		}

		public static bool operator !=(p<T> a, p<T> b)
		{
			if (a.pv.Memory != b.pv.Memory)
				throw new ArgumentException();

			return a.pv.Offset != b.pv.Offset;
		}

		public override bool Equals(object obj)
		{
			if (!(obj is p<T>))
				return base.Equals(obj);

			return pv.Memory == ((p<T>)obj).pv.Memory && pv.Offset == ((p<T>)obj).pv.Offset;
		}

		public override int GetHashCode()
		{
			return pv.Memory.GetHashCode() + pv.Offset.GetHashCode();
		}

		public static bool operator >(p<T> a, p<T> b)
		{
			if (a.pv.Memory != b.pv.Memory)
				throw new ArgumentException();

			return a.pv.Offset > b.pv.Offset;
		}

		public static bool operator >=(p<T> a, p<T> b)
		{
			if (a.pv.Memory != b.pv.Memory)
				throw new ArgumentException();

			return a.pv.Offset >= b.pv.Offset;
		}

		public static bool operator <(p<T> a, p<T> b)
		{
			if (a.pv.Memory != b.pv.Memory)
				throw new ArgumentException();

			return a.pv.Offset < b.pv.Offset;
		}

		public static bool operator <=(p<T> a, p<T> b)
		{
			if (a.pv.Memory != b.pv.Memory)
				throw new ArgumentException();

			return a.pv.Offset <= b.pv.Offset;
		}

		public static explicit operator int(p<T> This)
		{
			return This.pv.Offset;
		}

		public static p<T> operator +(p<T> a, int b)
		{
			return new p<T>(a, b * Marshal.SizeOf(typeof(T)));
		}

		public static p<T> operator -(p<T> a, int b)
		{
			return new p<T>(a, -b * Marshal.SizeOf(typeof(T)));
		}

		public static int operator -(p<T> a, p<T> b)
		{
			if (a.pv.Memory != b.pv.Memory)
				throw new ArgumentException();

			return a.pv.Offset - b.pv.Offset;
		}

		public static p<T> operator ++(p<T> a)
		{
			a.pv.AddOffset(Marshal.SizeOf(typeof(T)));
			return a;
		}

		public static p<T> operator --(p<T> a)
		{
			a.pv.SubOffset(Marshal.SizeOf(typeof(T)));
			return a;
		}
	}
}

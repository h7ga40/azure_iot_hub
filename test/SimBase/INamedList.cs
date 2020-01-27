using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;

namespace SimBase
{
	public interface INamedList<T> : IEnumerable<T>, IEnumerable
	{
		int Count { get; }
		T this[string name] { get; }
		void Lock();
		void Unlock();
		T[] ToArray();
	}
}

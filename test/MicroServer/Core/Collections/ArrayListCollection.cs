using System;
using System.Collections;

namespace MicroServer.Collections
{
	public class ArrayListCollection : IList
	{
		protected ArrayList _arrayList;

		#region IList

		public int Add(object value)
		{
			return _arrayList.Add(value);
		}

		public void Clear()
		{
			_arrayList.Clear();
		}

		public bool Contains(object value)
		{
			return _arrayList.Contains(value);
		}

		public int IndexOf(object value)
		{
			return _arrayList.IndexOf(value);
		}

		public void Insert(int index, object value)
		{
			_arrayList.Insert(index, value);
		}

		public bool IsFixedSize => _arrayList.IsFixedSize;

		public bool IsReadOnly => _arrayList.IsReadOnly;

		public void Remove(object value)
		{
			_arrayList.Remove(value);
		}

		public void RemoveAt(int index)
		{
			_arrayList.RemoveAt(index);
		}

		public object this[int index] {
			get => _arrayList[index];
			set => _arrayList.Insert(index, value);
		}

		public void CopyTo(Array array, int index)
		{
			_arrayList.CopyTo(array, index);
		}

		public int Count => _arrayList.Count;

		public bool IsSynchronized => _arrayList.IsSynchronized;

		public object SyncRoot => _arrayList.SyncRoot;

		public IEnumerator GetEnumerator()
		{
			return _arrayList.GetEnumerator();
		}

		#endregion IList
	}
}

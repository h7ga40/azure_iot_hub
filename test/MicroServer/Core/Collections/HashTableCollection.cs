using System;
using System.Collections;

namespace MicroServer.Collections
{
	public class HashTableCollection : ICloneable, IDictionary, ICollection, IEnumerable
	{
		protected Hashtable _hashTable;

		#region ICloneable

		public object Clone()
		{
			return _hashTable.Clone();
		}

		#endregion ICloneable

		#region IDictionary

		public void Add(object key, object value)
		{
			_hashTable.Add(key, value);
		}

		public void Clear()
		{
			_hashTable.Clear();
		}

		public bool Contains(object key)
		{
			return _hashTable.Contains(key);
		}

		public bool IsFixedSize => _hashTable.IsFixedSize;

		public bool IsReadOnly => _hashTable.IsReadOnly;

		public ICollection Keys => _hashTable.Keys;

		public void Remove(object key)
		{
			_hashTable.Remove(key);
		}

		public ICollection Values => _hashTable.Values;

		public object this[object key] {
			get => _hashTable[key];
			set => _hashTable.Add(key, value);
		}

		public void CopyTo(Array array, int index)
		{
			_hashTable.CopyTo(array, index);
		}

		public int Count => _hashTable.Count;

		public bool IsSynchronized => _hashTable.IsSynchronized;

		public object SyncRoot => _hashTable.SyncRoot;

		public IDictionaryEnumerator GetEnumerator()
		{
			return ((IDictionary)_hashTable).GetEnumerator();
		}

		#endregion IDictionary

		#region IEnumerable

		IEnumerator IEnumerable.GetEnumerator()
		{
			return _hashTable.GetEnumerator();
		}

		#endregion IEnumerable
	}
}

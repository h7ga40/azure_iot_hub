using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace SimBase
{
	public class UnitSimList : INamedList<IUnitSim>
	{
		private LinkedList<IUnitSim> m_List = new LinkedList<IUnitSim>();

		public UnitSimList()
		{
		}

		#region INamedList<IUnitSim> メンバ

		public int Count
		{
			get { return m_List.Count; }
		}

		public IUnitSim this[string name]
		{
			get
			{
				foreach (IUnitSim unit in m_List) {
					if (unit.UnitName == name)
						return unit;
				}

				return null;
			}
		}

		public void Lock()
		{
			Monitor.Enter(m_List);
		}

		public void Unlock()
		{
			Monitor.Exit(m_List);
		}

		public IUnitSim[] ToArray()
		{
			IUnitSim[] result = new IUnitSim[m_List.Count];
			int i = 0;

			lock (m_List) {
				foreach (IUnitSim unit in m_List) {
					result[i++] = unit;
				}
			}

			return result;
		}

		#endregion

		#region IEnumerable<IUnitSim> メンバ

		public IEnumerator<IUnitSim> GetEnumerator()
		{
			return m_List.GetEnumerator();
		}

		#endregion

		#region IEnumerable メンバ

		System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
		{
			return m_List.GetEnumerator();
		}

		#endregion

		public void Clear()
		{
			lock (m_List) {
				m_List.Clear();
			}
		}

		public void Add(IUnitSim unit)
		{
			lock (m_List) {
				m_List.AddLast(unit);
			}
		}

		public void Remove(IUnitSim unit)
		{
			lock (m_List) {
				m_List.Remove(unit);
			}
		}
	}
}

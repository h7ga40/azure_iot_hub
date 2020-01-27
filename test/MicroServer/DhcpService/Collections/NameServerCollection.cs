using MicroServer.Collections;
using System.Collections;

namespace MicroServer.Net.Dhcp
{
	public class NameServerCollection : ArrayListCollection
	{

		public NameServerCollection()
		{
			_arrayList = new ArrayList();
		}

		public NameServerCollection(string preferredAddress)
		{
			_arrayList = new ArrayList();
			Add(new InternetAddress(preferredAddress));
		}

		public NameServerCollection(string preferredAddress, string alternetAddress)
		{
			_arrayList = new ArrayList();
			Add(new InternetAddress(preferredAddress));
			Add(new InternetAddress(alternetAddress));
		}

		public int Add(InternetAddress item)
		{
			return _arrayList.Add(item);
		}

		public int Add(string value)
		{
			return _arrayList.Add(new InternetAddress(value));
		}

		public bool Contains(InternetAddress item)
		{
			return _arrayList.Contains(item);
		}

		public void Remove(InternetAddress item)
		{
			_arrayList.Remove(item);
		}
	}
}

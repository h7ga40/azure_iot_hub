using MicroServer.Collections;
using System.Collections;

namespace MicroServer.Net.Dhcp
{
	public class ReservationCollection : HashTableCollection
	{
		public ReservationCollection()
		{
			_hashTable = new Hashtable();
		}

		public InternetAddress this[PhysicalAddress macAddress] {
			get => (InternetAddress)_hashTable[macAddress];
			set => _hashTable[macAddress] = value;
		}

		public void Add(InternetAddress ipAddress, PhysicalAddress macAddress)
		{
			_hashTable[ipAddress] = macAddress;
		}

		public void Add(string IpAddress, string macAddress)
		{
			_hashTable.Add(PhysicalAddress.Parse(macAddress), InternetAddress.Parse(IpAddress));
		}

		public bool Contains(PhysicalAddress macAddress)
		{
			return _hashTable.Contains(macAddress);
		}

		public void Remove(PhysicalAddress macAddress)
		{
			_hashTable.Remove(macAddress);
		}
	}
}

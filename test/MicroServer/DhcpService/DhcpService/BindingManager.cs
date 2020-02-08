using MicroServer.Logging;
using MicroServer.Utilities;
using System;
using System.Collections;
using System.IO;
using System.Text;
using uITron3;

namespace MicroServer.Net.Dhcp
{
	public class BindingManager
	{
		private static readonly Random RANDOM = new Random();

		private readonly object _bindingSync = new object();
		private readonly HNO _reaperTimer = new HNO(1);
		private string _storageRoot;
		private string _filePath;

		private Hashtable _reservationTable = new Hashtable();
		private Hashtable _unassignedTable = new Hashtable();
		private Hashtable _assignedTable = new Hashtable();

		// time is configured using seconds
		private int _offerTimeout = 60 * 2;   // 2 Minutes
		private int _declinedTimeout = 60 * 60;  // 60 minutes

		private int _leaseDuration = 60 * 60 * 24; //  1 Day
		private int _leaseRenewal = 60 * 60 * 6; //  6 Hours
		private int _leaseRebinding = 60 * 60 * 18; //  18 Hours

		private int _reaperDelay;
		private int _reaperInterval;
		Itron _itron;

		public BindingManager(int reaperDelay = 30000, int reaperInterval = 30000)
		{
			_reaperDelay = reaperDelay;
			_reaperInterval = reaperInterval;
		}

		public void Start(Itron itron)
		{ 
			_itron = itron;
			T_DCYC dcyc = new T_DCYC();
			dcyc.cycact = CYCACT.TCY_ON;
			dcyc.cychdr = Reaper;
			dcyc.cyctim = _reaperInterval;
			_itron.def_cyc(_reaperTimer, ref dcyc);
		}

		public string StorageRoot {
			get => _storageRoot;
			set {
				try {
					_storageRoot = Path.GetFullPath(value);
					_filePath = Path.GetFullPath(_storageRoot + @"\dhcp.dat");
				}
				catch {
					_storageRoot = null;
					throw new ArgumentException("Invalid file path");
				}
			}
		}

		public int OfferTimeOut {
			get => _offerTimeout;
			set => _offerTimeout = value;
		}

		public int DeclinedTimeOut {
			get => _declinedTimeout;
			set => _declinedTimeout = value;
		}

		public int LeaseDuration {
			get => _leaseDuration;
			set => _leaseDuration = value;
		}

		public int LeaseRenewal {
			get => _leaseRenewal;
			set => _leaseRenewal = value;
		}

		public int LeaseRebinding {
			get => _leaseRebinding;
			set => _leaseRebinding = value;
		}

		public void Save()
		{
			if (!StringUtility.IsNullOrEmpty(StorageRoot)) {
				try {
					using (var fileStream = new FileStream(_filePath, FileMode.Create)) {
						if (this != null) {
							lock (_bindingSync) {
								var store = new FileStore();
								foreach (BindingLease lease in _assignedTable.Values) {
									var item = new FileStore {
										ClientId = lease.ClientId,
										Owner = lease.Owner.ToArray(),
										Address = lease.Address.ToArray(),
										HostName = lease.HostName,
										Expiration = lease.Expiration,
										SessionId = lease.SessionId,
										State = lease.State
									};
									store.Add(item);
								}
								var serializedData = Reflection.Serialize(store, typeof(FileStore));
								fileStream.Write(serializedData, 0, serializedData.Length);
								fileStream.Close();
							}
						}
					}
				}
				catch (Exception ex) {
					Logger.WriteError(this, "Error Message:" + ex.Message.ToString(), ex);
				}
			}
		}

		public void Load()
		{
			if (!StringUtility.IsNullOrEmpty(_filePath)) {
				try {
					if (File.Exists(_filePath)) {
						Logger.WriteInfo(this, "Binding file is enabled using " + _filePath);
						using (var fileStream = new FileStream(_filePath, FileMode.Open)) {
							var data = new byte[10000];
							var readCount = fileStream.Read(data, 0, data.Length);

							var deserializedData = (FileStore)Reflection.Deserialize((data), typeof(FileStore));

							if (deserializedData != null && deserializedData.Count > 0) {
								lock (_bindingSync) {
									foreach (FileStore endPoint in deserializedData) {
										var lease = new BindingLease(
											new PhysicalAddress(endPoint.Owner),
											new InternetAddress(endPoint.Address)) {
											ClientId = endPoint.ClientId,
											Expiration = endPoint.Expiration,
											State = endPoint.State,
											SessionId = endPoint.SessionId,
											HostName = endPoint.HostName
										};

										if (!_assignedTable.Contains(lease.Address)) {
											_assignedTable.Add(lease.Address, lease);
											Logger.WriteDebug("Binding Loaded | " + lease.ToString());
										}
									}
								}
							}
						}
					}
					else {
						Logger.WriteDebug(this, "Bindings file is disabled. Failed to accesss file " + _filePath);
					}
				}
				catch (Exception ex) {
					Logger.WriteError(this, "Error Message:" + ex.Message.ToString(), ex);
				}
			}
			else {
				Logger.WriteInfo(this, "Binding file is disabled");
			}
		}

		public void PoolRange(InternetAddress startAddress, InternetAddress endAddress)
		{
			lock (_bindingSync) {
				for (var address = startAddress.Copy(); address.CompareTo(endAddress) <= 0; address = address.NextAddress()) {
					if (!_unassignedTable.Contains(address)) {
						_unassignedTable.Add(address, new BindingLease(RANDOM.Next(Int32.MaxValue).ToString(), new PhysicalAddress(), address, new byte[0], DateTime.MaxValue, 0, LeaseState.Unassigned));
					}
				}
			}
		}

		public void Reservation(InternetAddress address, PhysicalAddress macAddress)
		{
			lock (_bindingSync) {
				if (_reservationTable.Contains(address)) {
					_reservationTable[address] = new BindingLease(RANDOM.Next(Int32.MaxValue).ToString(), macAddress, address, new byte[0], DateTime.MaxValue, 0, LeaseState.Static);
				}
				else {
					_reservationTable.Add(address, new BindingLease(RANDOM.Next(Int32.MaxValue).ToString(), macAddress, address, new byte[0], DateTime.MaxValue, 0, LeaseState.Static));

				}
			}
		}

		public BindingLease GetOffer(BindingLease binding)
		{
			// offer reservations binding
			if (_reservationTable.Contains(binding.Owner)) {
				var ipAddress = (InternetAddress)_reservationTable[binding.Owner];
				var lease = (BindingLease)_reservationTable[ipAddress];

				if (lease.Owner.Equals(binding.Owner)
					&& lease.State.Equals(LeaseState.Static)) {
					binding.Address = lease.Address;
					binding.Expiration = DateTime.MaxValue;
					binding.State = LeaseState.Static;
					return SaveBinding(binding);
				}
			}

			// offer discovery request with existing ip binding
			if (!binding.Address.Equals(InternetAddress.Any)) {
				// check for active address
				if (_assignedTable.Contains(binding.ClientId)) {
					var lease = (BindingLease)_assignedTable[binding.ClientId];

					if (lease.State.Equals(LeaseState.Released) || lease.State.Equals(LeaseState.Offered)) {
						binding.Address = lease.Address;
						binding.Expiration = DateTime.Now.AddSeconds(_offerTimeout);
						binding.State = LeaseState.Offered;

						return SaveBinding(binding);
					}
					else {
						RemoveBinding(binding);
					}
				}
				// check for unassigned address
				else if (_unassignedTable.Contains(binding.Address)) {
					binding.Expiration = DateTime.Now.AddSeconds(_offerTimeout);
					binding.State = LeaseState.Offered;

					return CreateBinding(binding, binding.Address);
				}
			}

			// offer next open binding address for invalid or no request
			foreach (BindingLease lease in _unassignedTable.Values) {
				binding.Address = lease.Address;
				binding.Expiration = DateTime.Now.AddSeconds(_offerTimeout);
				binding.State = LeaseState.Offered;

				return CreateBinding(binding, lease.Address);
			}

			return null;
		}

		public BindingLease GetAssigned(BindingLease binding)
		{
			// assign reservations binding
			if (_reservationTable.Contains(binding.Owner)) {
				var ipAddress = (InternetAddress)_reservationTable[binding.Owner];

				var lease = (BindingLease)_reservationTable[ipAddress];

				if (lease.Address.Equals(binding.Address)
					&& lease.Owner.Equals(binding.Owner)
					&& lease.State.Equals(LeaseState.Static)) {
					binding.Expiration = DateTime.MaxValue;
					binding.State = LeaseState.Static;

					return SaveBinding(binding);
				}
			}

			// assign binding
			if (_assignedTable.Contains(binding.ClientId)) {
				var lease = (BindingLease)_assignedTable[binding.ClientId];

				if (lease.Owner.Equals(binding.Owner)) {
					binding.Expiration = DateTime.Now.AddSeconds(_leaseDuration);
					binding.State = LeaseState.Assigned;

					return SaveBinding(binding);
				}
			}

			return null;
		}

		public BindingLease SetDeclined(BindingLease binding)
		{
			if (!binding.Address.Equals(InternetAddress.Any)) {
				if (_assignedTable.Contains(binding.ClientId)) {
					var lease = (BindingLease)_assignedTable[binding.ClientId];
					if (lease.Owner.Equals(binding.Owner) && lease.State != LeaseState.Static) {
						binding.ClientId = "-1";
						binding.Expiration = DateTime.Now.AddSeconds(_declinedTimeout);
						binding.State = LeaseState.Declined;
						return SaveBinding(binding);
					}
				}
			}
			return null;
		}

		public BindingLease SetReleased(BindingLease binding)
		{
			if (!binding.Address.Equals(InternetAddress.Any)) {
				if (_assignedTable.Contains(binding.ClientId)) {
					var lease = (BindingLease)_assignedTable[binding.ClientId];
					if (lease.Owner.Equals(binding.Owner)) {
						if (lease.State == LeaseState.Static) {
							return lease;
						}
						else {
							binding.Expiration = lease.Expiration;
							binding.State = LeaseState.Released;
							return SaveBinding(binding);
						}
					}
				}
			}
			return null;
		}

		public override string ToString()
		{
			var sb = new StringBuilder();

			foreach (BindingLease lease in _assignedTable.Values) {
				sb.AppendLine(lease.ToString());
			}

			return sb.ToString();
		}

		private BindingLease CreateBinding(BindingLease binding, InternetAddress address)
		{
			if (!_assignedTable.Contains(binding.ClientId) && _unassignedTable.Contains(address)) {
				lock (_bindingSync) {
					_unassignedTable.Remove(address);
					_assignedTable.Add(binding.ClientId, binding);
				}
				Save();

				return (BindingLease)_assignedTable[binding.ClientId];
			}

			return null;
		}

		private BindingLease SaveBinding(BindingLease binding)
		{
			if (_assignedTable.Contains(binding.ClientId)) {
				lock (_bindingSync) {
					_assignedTable[binding.ClientId] = binding;
				}
				Save();
				return (BindingLease)_assignedTable[binding.ClientId];
			}
			return null;
		}

		private void RemoveBinding(BindingLease binding)
		{
			if (_assignedTable.Contains(binding.ClientId)) {
				lock (_bindingSync) {
					_assignedTable.Remove(binding.ClientId);

					binding.ClientId = RANDOM.Next(Int32.MaxValue).ToString();
					binding.SessionId = 0;
					binding.HostName = new byte[0];
					binding.Owner = new PhysicalAddress(0, 0, 0, 0, 0, 0);
					binding.State = LeaseState.Unassigned;
					binding.Expiration = DateTime.MaxValue;
					if (!_unassignedTable.Contains(binding.Address)) {
						_unassignedTable.Add(binding.Address, binding);
					}
				}
				//this.Save();
			}
		}

		private ID Reaper()
		{
			Logger.WriteDebug(this, "Binding manager processed " + _assignedTable.Count.ToString() + " records");
			foreach (BindingLease lease in _assignedTable.Values) {
				//Logger.WriteDebug("  Binding | " + lease.ToString());

				if (lease.Expiration < DateTime.Now) {
					RemoveBinding(lease);
					Logger.WriteDebug("  Perged Binding | " + lease.ToString());
				}
				else {
					if (lease.State != LeaseState.Unassigned) {
						Logger.WriteDebug("  Active Binding | " + lease.ToString());
					}
				}
			}
			Save();

			// debug table list
			//foreach (BindingLease lease in _unassignedTable.Values)
			//{
			//    Logger.WriteDebug("  UNASSIGNED | " + lease.ToString());
			//}

			//foreach (BindingLease lease in _assignedTable.Values)
			//{
			//    Logger.WriteDebug("    ASSIGNED | " + lease.ToString());
			//}

			//foreach (BindingLease lease in _reservationTable.Values)
			//{
			//    Logger.WriteDebug(" RESERVATION | " + lease.ToString());
			//}
			return ID.NULL;
		}

		[Serializable]
		public class FileStore : ArrayList
		{
			public string ClientId;
			public byte[] Owner;
			public byte[] Address;
			public byte[] HostName;
			public DateTime Expiration;
			public uint SessionId;
			public LeaseState State;
		}
	}
}

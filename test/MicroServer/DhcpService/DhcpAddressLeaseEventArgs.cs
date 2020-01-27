using System;

namespace MicroServer.Net.Dhcp
{
	public class DhcpLeaseEventArgs : EventArgs
	{
		/// <summary>
		///     The remote client IP address.
		/// </summary>
		public string Address { get; private set; }

		/// <summary>
		///     Binding lease for the connected client.
		/// </summary>
		public BindingLease Lease { get; private set; }

		/// <summary>
		///     Initializes a new instance of the <see cref="DhcpLeaseEventArgs"/> class.
		/// </summary>
		/// <param name="address">The IP addres of remote client.</param>
		/// <param name="lease">The binding lease of the remote client.</param>
		public DhcpLeaseEventArgs(InternetAddress address, BindingLease lease)
		{
			Address = address.ToString();
			Lease = lease;
		}
	}
}

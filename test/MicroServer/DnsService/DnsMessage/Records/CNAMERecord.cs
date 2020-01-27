using System;

namespace MicroServer.Net.Dns
{
	/// <summary>
	/// CNAME Resource Record (RFC 1035 3.3.1)
	/// </summary>
	[Serializable]
	public class CNAMERecord : RecordBase
	{
		private readonly string _domain;

		#region Public Properties

		/// <summary>
		/// A &lt;domain-name&gt; which specifies the canonical or primary
		/// name for the owner. The owner name is an alias.
		/// </summary>
		public string Domain => _domain;

		#endregion

		public CNAMERecord(DnsReader br)
		{
			_domain = br.ReadDomain();
		}

		public override string ToString()
		{
			return "    canonical name = " + _domain;
		}
	}
}
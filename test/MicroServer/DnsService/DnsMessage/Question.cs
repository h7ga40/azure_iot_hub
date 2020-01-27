using System.Net;
using System.Text;

namespace MicroServer.Net.Dns
{
	/// <summary>
	/// A Question (RFC 1035 4.1.2)
	/// </summary>
	public class Question
	{
		private string _domain;     // QNAME
		private RecordType _qtype;
		private RecordClass _qclass;

		#region Public Properties

		/// <summary>
		/// The domain name to ask for.
		/// </summary>
		public string Domain {
			get => _domain;
			set => _domain = value;
		}

		/// <summary>
		/// The type of the query.
		/// </summary>
		public RecordType Type {
			get => _qtype;
			set => _qtype = value;
		}

		/// <summary>
		/// The class of the query.
		/// </summary>
		public RecordClass Class {
			get => _qclass;
			set => _qclass = value;
		}

		#endregion

		internal Question(DnsReader br)
		{
			_domain = br.ReadDomain();
			_qtype = (RecordType)br.ReadInt16();
			_qclass = (RecordClass)br.ReadInt16();
		}

		public Question()
		{
		}

		public Question(string domain, RecordType qtype, RecordClass qclass)
		{
			if (qtype == RecordType.PTR) {
				var addr = IPAddress.Parse(domain);

				var sb = new StringBuilder();

				var addrBytes = addr.GetAddressBytes();
				for (var i = addrBytes.Length - 1; i >= 0; i--)
					sb.Append((int)addrBytes[i] + ".");

				sb.Append("in-addr.arpa");

				_domain = sb.ToString();
			}
			else
				_domain = domain;

			_qtype = qtype;
			_qclass = qclass;
		}

		internal void Write(DnsWriter bw)
		{
			bw.WriteDomain(Domain);
			bw.Write((short)Type);
			bw.Write((short)Class);
		}
	}
}

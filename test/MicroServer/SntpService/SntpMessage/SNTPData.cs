using System;

namespace MicroServer.Net.Sntp
{
	public class SntpMessage
	{
		public int VersionNumber { get; internal set; }
		public Stratum Stratum { get; internal set; }
		public int PollInterval { get; internal set; }
		public double Precision { get; internal set; }
		public DateTime ReferenceDateTime { get; internal set; }
		public DateTime ReceiveDateTime { get; internal set; }
		public string ReferenceIdentifier { get; internal set; }
		public DateTime TransmitDateTime { get; internal set; }
		internal Mode Mode { get; set; }
		internal LeapIndicator LeapIndicator { get; set; }
		public byte[] Data { get; }

		public static implicit operator byte[] (SntpMessage data)
		{
			return data.Data;
		}
	}
}

using MicroServer.Logging;
using System;

namespace MicroServer.Net.Sntp
{
	/// <summary>
	///     Used by <see cref="TimeChanged.OnTimeChanged" />.
	/// </summary>
	public class TimeChangedEventArgs : EventArgs
	{
		public SntpMessage ResponseMessage { get; internal set; }
		public DateTime DeviceTime { get; internal set; }
		public DateTime NetworkTime { get; internal set; }

		public double ClockOffset { get; internal set; }

		/// <summary>
		///     Initializes a new instance of the <see cref="TimeChangedEventArgs" /> class.
		/// </summary>
		/// <param name="message">Response Message.</param>
		public TimeChangedEventArgs(SntpMessage message)
		{
			ResponseMessage = message;
			DeviceTime = DateTime.Now.ToUniversalTime();
			NetworkTime = message.TransmitDateTime;
			ClockOffset = message.LocalClockOffset;

			//Debug.Print(TimeZone.CurrentTimeZone.GetUtcOffset(responseMessage.TransmitDateTime).ToString());
			//Debug.Print(TimeZone.CurrentTimeZone.GetUtcOffset(this.NetworkTime).ToString());

			//Utility.SetLocalTime(message.TransmitDateTime.AddHours(-8));
			Logger.WriteInfo(this, "Device time changed from " + DeviceTime.ToString() + " to " + DateTime.Now.ToString() + " using network time");

		}
	}
}

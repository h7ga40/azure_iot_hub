using System;

namespace MicroServer.Utilities
{
	/// <summary>
	/// Provides additional parsing operations
	/// </summary>
	public abstract class ParseUtility
	{
		/// <summary>
		/// Attempt to parse the provided string value.
		/// </summary>
		/// <param name="value">String value to be parsed</param>
		/// <param name="i">Variable to set successfully parsed value to</param>
		/// <returns>True if parsing was successful</returns>
		public static bool TryParseInt(string value, out int i)
		{
			i = 0;
			try {
				i = Int32.Parse(value);
				return true;
			}
			catch {
				return false;
			}
		}

		/// <summary>
		/// Attempt to parse the provided string value.
		/// </summary>
		/// <param name="value">String value to be parsed</param>
		/// <param name="i">Variable to set successfully parsed value to</param>
		/// <returns>True if parsing was successful</returns>
		public static bool TryParseShort(string value, out short i)
		{
			i = 0;
			try {
				i = Int16.Parse(value);
				return true;
			}
			catch {
				return false;
			}
		}

		/// <summary>
		/// Attempt to parse the provided string value.
		/// </summary>
		/// <param name="value">String value to be parsed</param>
		/// <param name="i">Variable to set successfully parsed value to</param>
		/// <returns>True if parsing was successful</returns>
		public static bool TryParseLong(string value, out long i)
		{
			i = 0;
			try {
				i = Int64.Parse(value);
				return true;
			}
			catch {
				return false;
			}
		}

		/// <summary>
		/// Attempt to parse the provided string value.
		/// </summary>
		/// <param name="value">String value to be parsed</param>
		/// <param name="i">Variable to set successfully parsed value to</param>
		/// <returns>True if parsing was successful</returns>
		public static bool TryParseDouble(string value, out double i)
		{
			i = 0;
			try {
				i = Double.Parse(value);
				return true;
			}
			catch {
				return false;
			}
		}

		/// <summary>
		/// Attempt to parse the provided string value.
		/// </summary>
		/// <param name="value">String value to be parsed</param>
		/// <param name="val">Variable to set successfully parsed value to</param>
		/// <returns>True if parsing was successful</returns>
		public static bool TryParseBool(string value, out bool val)
		{
			val = false;
			try {
				if (value == "1" || value.ToUpper() == Boolean.TrueString.ToUpper()) {
					val = true;

					return true;
				}
				else if (value == "0" || value.ToUpper() == Boolean.FalseString.ToUpper()) {
					val = false;

					return true;
				}

				return false;

			}
			catch {
				return false;
			}
		}

		/// <summary>
		/// Attempt to parse the provided string value.
		/// </summary>
		/// <param name="value">String value to be parsed</param>
		/// <param name="i">Variable to set successfully parsed value to</param>
		/// <returns>True if parsing was successful</returns>
		public static bool TryParseUInt(string value, out uint i)
		{
			i = 0;
			try {
				i = UInt32.Parse(value);
				return true;
			}
			catch {
				return false;
			}
		}

		/// <summary>
		/// Attempt to parse the provided string value.
		/// </summary>
		/// <param name="value">String value to be parsed</param>
		/// <param name="i">Variable to set successfully parsed value to</param>
		/// <returns>True if parsing was successful</returns>
		public static bool TryParseUShort(string value, out ushort i)
		{
			i = 0;
			try {
				i = UInt16.Parse(value);
				return true;
			}
			catch {
				return false;
			}
		}

		/// <summary>
		/// Attempt to parse the provided string value.
		/// </summary>
		/// <param name="value">String value to be parsed</param>
		/// <param name="i">Variable to set successfully parsed value to</param>
		/// <returns>True if parsing was successful</returns>
		public static bool TryParseULong(string value, out ulong i)
		{
			i = 0;
			try {
				i = UInt64.Parse(value);
				return true;
			}
			catch {
				return false;
			}
		}

		/// <summary>
		/// Attempt to parse the provided datetime value.
		/// </summary>
		/// <param name="datetime">Datetime value to be parsed</param>
		/// <returns>Datetime if parsing was successful</returns>
		public static DateTime TryParseDateTime(string datetime)
		{
			// Converts an ISO 8601 time/date format string, which is used by JSON and others,
			// into a DateTime object. 

			//Check to see if format contains the timezone ID, or contains UTC reference
			// Neither means it's localtime
			var utc = datetime.EndsWith("Z");

			var parts = datetime.Split(new char[] { 'T', 'Z', ':', '-', '.', '+', });

			// We now have the time string to parse, and we'll adjust
			// to UTC or timezone after parsing
			var year = parts[0];
			var month = (parts.Length > 1) ? parts[1] : "1";
			var day = (parts.Length > 2) ? parts[2] : "1";
			var hour = (parts.Length > 3) ? parts[3] : "0";
			var minute = (parts.Length > 4) ? parts[4] : "0";
			var second = (parts.Length > 5) ? parts[5] : "0";
			var ms = (parts.Length > 6) ? parts[6] : "0";

			var dt = new DateTime(Convert.ToInt32(year), Convert.ToInt32(month), Convert.ToInt32(day), Convert.ToInt32(hour), Convert.ToInt32(minute), Convert.ToInt32(second), Convert.ToInt32(ms));

			// If a time offset was specified instead of the UTC marker,
			// add/subtract in the hours/minutes
			if ((utc == false) && (parts.Length >= 9)) {
				// There better be a timezone offset
				var hourOffset = (parts.Length > 7) ? parts[7] : "";
				var minuteOffset = (parts.Length > 8) ? parts[8] : "";
				if (datetime.Contains("+")) {
					dt = dt.AddHours(Convert.ToDouble(hourOffset));
					dt = dt.AddMinutes(Convert.ToDouble(minuteOffset));
				}
				else {
					dt = dt.AddHours(-(Convert.ToDouble(hourOffset)));
					dt = dt.AddMinutes(-(Convert.ToDouble(minuteOffset)));
				}
			}

			if (utc) {
				// Convert the Kind to DateTimeKind.Utc if string Z present
				dt = new DateTime(0, DateTimeKind.Utc).AddTicks(dt.Ticks);
			}

			return dt;

			//string[] dt = datetime.Split(' ');
			//string[] date = dt[0].Split('/');
			//string[] time = dt[1].Split(':');
			//return new DateTime(int.Parse(date[2]), int.Parse(date[0]), int.Parse(date[1]), int.Parse(time[0]), int.Parse(time[1]), int.Parse (time[2]));
		}

		#region TryParseHex Method

		private static string hexValues = "0123456789ABCDEF";

		/// <summary>
		/// Converts a hex string to a byte value
		/// </summary>
		/// <param name="value">Byte value as output parameter</param>
		/// <returns>True if successful</returns>
		public static bool TryParseHex(string source, out byte value)
		{
			value = 0;

			if (source == null)
				return false;

			source = source.Trim();
			if (source.Length != 2)
				return false;

			source = source.ToUpper();
			var value0 = hexValues.IndexOf(source[0]);
			var value1 = hexValues.IndexOf(source[1]);
			if (value0 < 0 || value1 < 0)
				return false;

			value = (byte)((value0 << 4) + value1);

			return true;
		}

		/// <summary>
		/// Converts a hex string to an Int16 value
		/// </summary>
		/// <param name="value">Int16 value as output parameter</param>
		/// <returns>True if successful</returns>
		public static bool TryParseHex(string source, out short value)
		{
			value = 0;

			if (source == null)
				return false;

			source = source.Trim();
			if (source.Length != 4)
				return false;

			source = source.ToUpper();
			var value0 = hexValues.IndexOf(source[0]);
			var value1 = hexValues.IndexOf(source[1]);
			var value2 = hexValues.IndexOf(source[2]);
			var value3 = hexValues.IndexOf(source[3]);
			if (value0 < 0 || value1 < 0 || value2 < 0 || value3 < 0)
				return false;

			value = (short)((value0 << 12) + (value1 << 8) + (value2 << 4) + value3);

			return true;
		}

		/// <summary>
		/// Converts a hex string to an Int32 value
		/// </summary>
		/// <param name="value">Int32 value as output parameter</param>
		/// <returns>True if successful</returns>
		public static bool TryParseHex(string source, out int value)
		{
			value = 0;

			if (source == null)
				return false;

			source = source.Trim();
			if (source.Length != 8)
				return false;

			source = source.ToUpper();
			var value0 = hexValues.IndexOf(source[0]);
			var value1 = hexValues.IndexOf(source[1]);
			var value2 = hexValues.IndexOf(source[2]);
			var value3 = hexValues.IndexOf(source[3]);
			var value4 = hexValues.IndexOf(source[4]);
			var value5 = hexValues.IndexOf(source[5]);
			var value6 = hexValues.IndexOf(source[6]);
			var value7 = hexValues.IndexOf(source[7]);
			if (value0 < 0 || value1 < 0 || value2 < 0 || value3 < 0 || value4 < 0 || value5 < 0 || value6 < 0 || value7 < 0)
				return false;

			value = (value0 << 28) + (value1 << 24) + (value2 << 20) + (value3 << 16) + (value4 << 12) + (value5 << 8) + (value6 << 4) + value7;

			return true;
		}

		/// <summary>
		/// Converts a hex string to an UInt16 value
		/// </summary>
		/// <param name="value">UInt16 value as output parameter</param>
		/// <returns>True if successful</returns>
		public static bool TryParseHex(string source, out ushort value)
		{
			value = 0;

			if (source == null)
				return false;

			source = source.Trim();
			if (source.Length != 4)
				return false;

			source = source.ToUpper();
			var value0 = hexValues.IndexOf(source[0]);
			var value1 = hexValues.IndexOf(source[1]);
			var value2 = hexValues.IndexOf(source[2]);
			var value3 = hexValues.IndexOf(source[3]);
			if (value0 < 0 || value1 < 0 || value2 < 0 || value3 < 0)
				return false;

			value = (ushort)((value0 << 12) + (value1 << 8) + (value2 << 4) + value3);

			return true;
		}

		/// <summary>
		/// Converts a hex string to an UInt32 value
		/// </summary>
		/// <param name="value">UInt32 value as output parameter</param>
		/// <returns>True if successful</returns>
		public static bool TryParseHex(string source, out uint value)
		{
			value = 0;

			if (source == null)
				return false;

			source = source.Trim();
			if (source.Length != 8)
				return false;

			source = source.ToUpper();
			var value0 = hexValues.IndexOf(source[0]);
			var value1 = hexValues.IndexOf(source[1]);
			var value2 = hexValues.IndexOf(source[2]);
			var value3 = hexValues.IndexOf(source[3]);
			var value4 = hexValues.IndexOf(source[4]);
			var value5 = hexValues.IndexOf(source[5]);
			var value6 = hexValues.IndexOf(source[6]);
			var value7 = hexValues.IndexOf(source[7]);
			if (value0 < 0 || value1 < 0 || value2 < 0 || value3 < 0 || value4 < 0 || value5 < 0 || value6 < 0 || value7 < 0)
				return false;

			value = (uint)((value0 << 28) + (value1 << 24) + (value2 << 20) + (value3 << 16) + (value4 << 12) + (value5 << 8) + (value6 << 4) + value7);

			return true;
		}

		#endregion TryParseHex Functions
	}
}

namespace MicroServer.Net.Sntp
{
	/// <summary>
	/// Contains global variables for project.
	/// </summary>
	public static class Constants
	{
		public static int SNTP_SERVICE_PORT => 123;
		public static int SNTP_CLIENT_PORT => 123;
		public static int SNTP_MAX_MESSAGE_SIZE => 68;
		public static int SNTP_MIN_MESSAGE_SIZE => 48;
		public static int SNTP_RECEIVE_TIMEOUT => -1;
		public static int SNTP_SEND_TIMEOUT => -1;
		public static string SNTP_DEFAULT_PRIMARY_SERVER => "0.pool.ntp.org";
		public static string SNTP_DEFAULT_ALTERNATE_SERVER => "time.windows.com";
	}
}

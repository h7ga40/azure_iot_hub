namespace MicroServer.Net.Dhcp
{
	/// <summary>
	/// Contains global variables for project.
	/// </summary>
	public static class Constants
	{
		public static int DHCP_SERVICE_PORT => 67;
		public static int DHCP_CLIENT_PORT => 68;
		public static int DHCP_MIN_MESSAGE_SIZE => 236;
		public static int DHCP_MAX_MESSAGE_SIZE => 1024;
		public static int DHCP_RECEIVE_TIMEOUT => -1;
		public static int DHCP_SEND_TIMEOUT => -1;
		public static uint DHCP_OPTIONS_MAGIC_NUMBER => 1669485411;
		public static uint DHCP_WIN_OPTIONS_MAGIC_NUMBER => 1666417251;
	}
}

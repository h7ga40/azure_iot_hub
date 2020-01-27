namespace MicroServer.Net.Http
{
	public static class Constants
	{
		public static int HTTP_SERVER_PORT => 80;
		public static int HTTP_LISTEN_BACKLOG => 10;
		public static int HTTP_MAX_MESSAGE_SIZE => 65535;
		public static int HTTP_MIN_MESSAGE_SIZE => 0;
		public static int HTTP_RECEIVE_TIMEOUT => -1;
		public static int HTTP_SEND_TIMEOUT => -1;
		public static string HTTP_WEB_ROOT_FOLDER => "WebRoot";
		public static string HTTP_WEB_VIEWS_FOLDER => "Views";
	}
}

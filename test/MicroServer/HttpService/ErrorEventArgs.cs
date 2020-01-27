using System;


namespace MicroServer.Net.Http
{
	/// <summary>
	///     Used by <see cref="ChannelTcpListener.ClientConnected" />.
	/// </summary>
	public class ErrorEventArgs : EventArgs
	{
		/// <summary>
		///     Initializes a new instance of the <see cref="ErrorEventArgs" /> class.
		/// </summary>
		/// <param name="exception">The execption.</param>
		public ErrorEventArgs(Exception exception) { }
	}
}
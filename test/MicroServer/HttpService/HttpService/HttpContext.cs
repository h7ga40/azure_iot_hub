using MicroServer.Net.Sockets;
using System;

namespace MicroServer.Net.Http
{
	/// <summary>
	/// Request context
	/// </summary>
	public class HttpContext : IHttpContext
	{
		/// <summary>
		/// Initializes a new instance of the <see cref="HttpContext" /> class.
		/// </summary>
		public HttpContext()
		{
			RouteData = new MemoryItemStorage();
		}

		#region IHttpContext Members

		/// <summary>
		/// Incoming request
		/// </summary>
		public IHttpRequest Request { get; set; }

		/// <summary>
		/// Response to send back
		/// </summary>
		public IHttpResponse Response { get; set; }

		/// <summary>
		/// Can be used to store items through this request
		/// </summary>
		public IItemStorage Items { get; set; }

		/// <summary>
		/// Used to store items for the entire application.
		/// </summary>
		public IItemStorage Application { get; set; }

		/// <summary>
		/// USed to store items for the current session (if a session has been started)
		/// </summary>
		/// <remarks>Will be null if a session has not been started.</remarks>
		//public IItemStorage Session { get; set; }

		/// <summary>
		/// All exceptions will be logged by the system, but we generally do only keep track of the last one.
		/// </summary>
		public Exception LastException { get; set; }

		/// <summary>
		/// Gets or sets currently logged in user.
		/// </summary>
		//public IPrincipal User { get; set; }

		/// <summary>
		/// Channel connected
		/// </summary>
		internal SocketChannel Channel { get; set; }

		/// <summary>
		/// Gets information stored for the route.
		/// </summary>
		/// <remarks>For instance used to convert the URI into parameters.</remarks>
		/// <seealso cref="IRequestRouter"/>
		public IItemStorage RouteData { get; private set; }

		/// <summary>
		/// Gets or sets currently logged in user.
		/// </summary>
		public string User { get; set; }

		/// <summary>
		/// Performs application-defined tasks associated with freeing, releasing, or resetting unmanaged resources.
		/// </summary>
		public void Dispose()
		{
			Dispose(true);
			GC.SuppressFinalize(this);
		}

		/// <summary>
		/// Handles object cleanup
		/// </summary>
		/// <param name="disposing">True if called from Dispose(); false if called from GC finalization.</param>
		protected virtual void Dispose(bool disposing)
		{
			if (disposing) {

			}
		}

		#endregion
	}
}
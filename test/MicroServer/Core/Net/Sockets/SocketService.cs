using System;
using System.Net;


namespace MicroServer.Net.Sockets
{
	/// <summary>
	/// Implementation of <see cref="SocketService"/> that serves socket requests from remote clients.
	/// </summary>
	public abstract class SocketService : IDisposable
	{
		#region Private Properties

		private IPAddress _interfaceAddress = IPAddress.Parse("192.168.137.1")/*GetDefaultLocalAddress()*/;
		private int _servicePort = 0;

		#endregion Private Properties

		#region Public Properties

		/// <summary>
		///   Gets or sets the ip address for receiving data
		/// </summary>
		public IPAddress InterfaceAddress {
			get => _interfaceAddress;
			set => _interfaceAddress = value;
		}

		/// <summary>
		///   Gets or sets the port for receiving data
		/// </summary>
		public int ServicePort {
			get => _servicePort;
			set => _servicePort = value;
		}

		#endregion Public Properties

		#region Constructors / Deconstructors

		/// <summary>
		/// Initializes a new instance of the <see cref="SocketService"/> class.
		/// </summary>
		public SocketService() { }

		/// <summary>
		/// Handles object cleanup for GC finalization.
		/// </summary>
		~SocketService()
		{
			Dispose(false);
		}

		/// <summary>
		/// Handles object cleanup.
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

		#endregion  Constructors / Deconstructors

		#region Methods

		/// <summary>
		///  Starts the service listener if it is in a stopped state.
		/// </summary>
		public abstract bool Start();

		/// <summary>
		///  Stops the service listener if in started state.
		/// </summary>
		public abstract bool Stop();

		/// <summary>
		///  Restarts the service listener if in a started state.
		/// </summary>
		public bool Restart()
		{
			Stop();
			return Start();
		}

		#endregion Methods
	}
}

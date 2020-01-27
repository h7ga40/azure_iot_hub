using MicroServer.Logging;
using MicroServer.Net.Dhcp;
using MicroServer.Net.Dns;
using MicroServer.Net.Http;
using MicroServer.Net.Http.Files;
using MicroServer.Net.Http.Modules;
using MicroServer.Net.Http.Mvc.Controllers;
using MicroServer.Net.Http.Routing;
using MicroServer.Net.Sntp;
using MicroServer.Utilities;
using System;
using System.Diagnostics;
using System.Net;

namespace MicroServer.Service
{
	public class ServiceManager : IDisposable
	{
		#region Private Properties

		private string _storageRoot = null;
		private static ServiceManager _service;

		private DhcpService _dhcpService = new DhcpService();
		private bool _dhcpEnabled = false;

		private DnsService _dnsService = new DnsService();
		private bool _dnsEnabled = false;

		private SntpService _sntpService = new SntpService();
		private bool _sntpEnabled = false;

		private HttpService _httpService = new HttpService(null);
		private bool _httpEnabled = true;
		private bool _allowListing = false;

		private IPAddress _interfaceAddress = IPAddress.Any;
		private string _dnsSuffix;
		private string _serverName;

		#endregion Private Properties

		#region Constructors / Deconstructors

		public ServiceManager()
			: this(LogType.Output, LogLevel.Error, null) { }

		public ServiceManager(LogType logtype, LogLevel loglevel, string storageRoot)
		{
			_storageRoot = storageRoot;

			try {
				LoggerLevel loggerLevel;

				switch (loglevel) {
				case LogLevel.Info:
					loggerLevel = LoggerLevel.Info;
					break;

				case LogLevel.Error:
					loggerLevel = LoggerLevel.Error;
					break;

				default:
					loggerLevel = LoggerLevel.Debug;
					break;
				}

				switch (logtype) {
				case LogType.File:

					var FileLogger = new FileLogger();
					FileLogger.Open(_storageRoot, "log", "txt");
					Logger.Initialize(FileLogger, loggerLevel);
					Debug.Print(String.Concat("[", DateTime.Now.ToString("MM/dd/yyyy HH:mm:ss"), "] ", ToString(), " | Info | ", "File logging enabled on: ", _storageRoot));
					break;

				default:
					Logger.Initialize(new DebugLogger(), loggerLevel);
					Debug.Print(String.Concat("[", DateTime.Now.ToString("MM/dd/yyyy HH:mm:ss"), "] ", ToString(), " | Info | ", "Output windows logging enabled"));
					break;
				}

			}
			catch (Exception ex) {
				Debug.Print("Error: Logging faild to initialize" + ex.ToString());
			}
		}

		~ServiceManager()
		{
			Dispose(false);
		}

		public void Dispose()
		{
			Dispose(true);
			GC.SuppressFinalize(this);
		}

		protected virtual void Dispose(bool disposing)
		{
			if (disposing) {
				_dhcpService.Dispose();
				_dnsService.Dispose();
				_sntpService.Dispose();
				_httpService.Dispose();
			}
		}

		#endregion  Constructors / Deconstructors

		#region Public Properties

		/// <summary>
		/// Gets current service manager.
		/// </summary>
		/// <remarks>
		/// Only valid once service has been started.
		/// </remarks>
		public static ServiceManager Current => _service;

		public DhcpService DhcpService {
			get => _dhcpService;
			set => _dhcpService = value;
		}

		public bool DhcpEnabled {
			get => _dhcpEnabled;
			set => _dhcpEnabled = value;
		}

		public DnsService DnsService {
			get => _dnsService;
			set => _dnsService = value;
		}

		public bool DnsEnabled {
			get => _dnsEnabled;
			set => _dnsEnabled = value;
		}

		public SntpService SntpService {
			get => _sntpService;
			set => _sntpService = value;
		}

		public bool SntpEnabled {
			get => _sntpEnabled;
			set => _sntpEnabled = value;
		}

		public HttpService HttpService {
			get => _httpService;
			set => _httpService = value;
		}

		public bool HttpEnabled {
			get => _httpEnabled;
			set => _httpEnabled = value;
		}

		public string InterfaceAddress {
			get => _interfaceAddress.ToString();
			set => _interfaceAddress = IPAddress.Parse(value);
		}

		public string DnsSuffix {
			get => _dnsSuffix;
			set => _dnsSuffix = value;
		}

		public string ServerName {
			get => _serverName;
			set => _serverName = value;
		}

		public string StorageRoot {
			get => _storageRoot;
			set => _storageRoot = value;
		}

		public bool AllowListing {
			get => _allowListing;
			set => _allowListing = value;
		}

		public LogLevel LogLevel {
			get {
				switch (Logger.GetLogLevel()) {
				case LoggerLevel.Info:
					return LogLevel.Info;

				case LoggerLevel.Error:
					return LogLevel.Error;

				default:
					return LogLevel.Debug;
				}
			}
			set {
				switch (value) {
				case LogLevel.Info:
					Logger.SetLogLevel(LoggerLevel.Info);
					break;

				case LogLevel.Error:
					Logger.SetLogLevel(LoggerLevel.Error);
					break;

				default:
					Logger.SetLogLevel(LoggerLevel.Debug);
					break;
				}
			}
		}

		#endregion Public Properties

		#region Methods

		public void StartAll()
		{
			_service = this;

			// DHCP Service
			if (_dhcpEnabled == true) {
				_dhcpService.InterfaceAddress = _interfaceAddress;
				_dhcpService.ServerName = _serverName;
				_dhcpService.DnsSuffix = _dnsSuffix;
				_dhcpService.StorageRoot = _storageRoot;

				if (_sntpEnabled == true) {
					_dhcpService.RemoveOption(DhcpOption.NTPServer);
					_dhcpService.AddOption(DhcpOption.NTPServer, _interfaceAddress.GetAddressBytes());
				}
				_dhcpService.Start();
			}

			// DNS Service
			if (_dnsEnabled == true) {
				_dnsService.InterfaceAddress = _interfaceAddress;
				_dnsService.ServerName = _serverName;
				_dnsService.DnsSuffix = _dnsSuffix;

				if (!StringUtility.IsNullOrEmpty(_serverName) || !StringUtility.IsNullOrEmpty(_dnsSuffix)) {
					var record = new Answer {
						Domain = String.Concat(_serverName, ".", _dnsSuffix),
						Class = RecordClass.IN,
						Type = RecordType.A,
						Ttl = 60,
						Record = new ARecord(_interfaceAddress.GetAddressBytes())
					};

					_service.DnsService.ZoneFile.Add(record);

					Logger.WriteInfo(this, "Device registered with dns:  " + record.Domain);
				}

				_dnsService.Start();
			}

			// SNTP Service
			if (_sntpEnabled == true) {
				_sntpService.InterfaceAddress = _interfaceAddress;
				_sntpService.Start();
			}

			// HTTP Service
			if (_httpEnabled == true) {
				var _moduleManager = new ModuleManager();

				// Add the router module as the fist module to pipeline
				_moduleManager.Add(new RouterModule());

				if (StorageRoot != null) {
					// Create disk file service for the root storage
					var fileService = new DiskFileService("/", StorageRoot + @"\" + MicroServer.Net.Http.Constants.HTTP_WEB_ROOT_FOLDER + @"\");

					// Add the file module to pipeline
					_moduleManager.Add(new FileModule(fileService) { AllowListing = _allowListing });
				}

				// Add the controller module to pipeline
				_moduleManager.Add(new ControllerModule());

				// Add the error module as the last module to pipeline
				_moduleManager.Add(new ErrorModule());

				//  Create the Http service
				_httpService = new HttpService(_moduleManager) {
					InterfaceAddress = _interfaceAddress
				};
				_httpService.Start();
			}

			Logger.WriteInfo(this, "Service Manager started all services");
		}

		public void StopAll()
		{
			if (_dhcpEnabled == true)
				_dhcpService.Stop();

			if (_dnsEnabled == true)
				_dnsService.Stop();

			if (_sntpEnabled == true)
				_sntpService.Stop();

			if (_httpEnabled == true)
				_httpService.Stop();

			Logger.WriteInfo(this, "Service Manager stopped all services");
		}

		public void RestartAll()
		{
			if (_dhcpEnabled == true)
				_dhcpService.Restart();

			if (_dnsEnabled == true)
				_dnsService.Restart();

			if (_sntpEnabled == true)
				_sntpService.Restart();

			if (_httpEnabled == true)
				_httpService.Restart();

			Logger.WriteInfo(this, "Service Manager restarted all services");
		}

		public void LogError(object source, string message, Exception ex)
		{
			Logger.WriteError(source, message, ex);
		}

		public void LogInfo(object source, string message)
		{
			Logger.WriteInfo(source, message);
		}

		public void LogDebug(object source, string message)
		{
			Logger.WriteDebug(source, message);
		}

		#endregion Methods
	}
}
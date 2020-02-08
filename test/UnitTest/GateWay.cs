using MicroServer.Net.Dhcp;
using MicroServer.Net.Dns;
using MicroServer.Net.Sntp;
using MicroServer.Service;
using SimEnv;
using System.Diagnostics;
using uITron3;

namespace SystemSim
{
	internal class GateWay : Target
	{
		private ServiceManager Server;
		private Itron m_Itron;
		ID MAIN_TASK;

		public GateWay(uint addr, uint subNet)
		{
			Addr = addr;
			SubNet = subNet;
		}

		public uint Addr { get; }
		public uint SubNet { get; }

		public override void Init(Itron itron)
		{
			m_Itron = itron;

			T_CTSK ctsk = new T_CTSK();
			ctsk.task = main;
			ER er = m_Itron.cre_tsk(ID.ID_AUTO, ref ctsk, out MAIN_TASK);
		}

		protected override void Start()
		{
			m_Itron.sta_tsk(MAIN_TASK, 0);
		}

		private void main(object exinf)
		{
			Server = new ServiceManager(LogType.Output, LogLevel.Info, @"\winfs") {

				//INITIALIZING : Server Services
				InterfaceAddress = "192.168.137.1",
				ServerName = "example",
				DnsSuffix = "iot.local"
			};

			// SERVICES:  disable services 
			//Server.HttpEnabled = false;

			// SERVICES INIT: quickly enable/disable services
			HttpInit();
			DhcpInit();
			DnsInit();
			SntpInit();

			// SERVICES: Start all services
			Server.StartAll(m_Itron);

			// enables basic authentication on server using AccountService class for user name and password
			//Server.HttpService.Add(new AuthenticationModule(new BasicAuthentication(new AccountService(), Server.ServerName)));
		}

		#region Protocol Initialization

		private void HttpInit()
		{
			//SERVICE: HTTP
			Server.HttpEnabled = true;
			Server.AllowListing = false;

			var url = System.String.Concat("http://", Server.HttpService.InterfaceAddress, ":", "80");
			Server.LogInfo("Test.Harness", "Launch Web Browser: " + url);
		}

		private void DhcpInit()
		{
			//SERVICE: DHCP
			Server.DhcpEnabled = true;

			// event handlers for dhcp service
			Server.DhcpService.OnDhcpMessageReceived += DhcpService_OnDhcpMessageReceived;
			Server.DhcpService.OnDhcpMessageSent += DhcpService_OnDhcpMessageSent;
			Server.DhcpService.OnLeaseAcknowledged += DhcpService_OnLeaseAcknowledged;
			Server.DhcpService.OnLeaseDeclined += DhcpService_OnLeaseDeclined;
			Server.DhcpService.OnLeaseReleased += DhcpService_OnLeaseReleased;

			Server.DhcpService.PoolRange("192.168.137.100", "192.168.137.254");
			Server.DhcpService.GatewayAddress = "192.168.137.1";
			Server.DhcpService.SubnetMask = "255.255.255.0";
		}

		private void DnsInit()
		{
			//SERVICE: DNS
			Server.DnsEnabled = true;

			// event handlers for dns service
			Server.DnsService.OnDnsMessageReceived += DnsService_OnDnsMessageReceived;
			Server.DnsService.OnDnsMessageSent += DnsService_OnDnsMessageSent;
		}

		private void SntpInit()
		{
			//SERVICE: SNTP
			Server.SntpEnabled = true;

			// event handlers for sntp service
			Server.SntpService.OnSntpMessageReceived += SntpService_OnSntpMessageReceived;
			Server.SntpService.OnSntpMessageSent += SntpService_OnSntpMessageSent;
		}

		#endregion Protocol Initialization

		#region Event Handlers

		private void DhcpService_OnDhcpMessageReceived(object sender, DhcpMessageEventArgs args)
		{
			Debug.Print("Test Harness: DHCP Message Received " + args.RequestMessage.ToString());
		}

		private void DhcpService_OnDhcpMessageSent(object sender, DhcpMessageEventArgs args)
		{
			Debug.Print("Test Harness: DHCP Message Sent " + args.RequestMessage.ToString());
		}

		private void DhcpService_OnLeaseAcknowledged(object sender, DhcpLeaseEventArgs args)
		{
			Debug.Print("Test Harness: DHCP Lease Acknowledged " + args.Lease.ToString());
		}

		private void DhcpService_OnLeaseDeclined(object sender, DhcpLeaseEventArgs args)
		{
			Debug.Print("Test Harness: DHCP Lease Declined " + args.Lease.ToString());
		}

		private void DhcpService_OnLeaseReleased(object sender, DhcpLeaseEventArgs args)
		{
			Debug.Print("Test Harness: DHCP Lease Released " + args.Lease.ToString());
		}

		private void DnsService_OnDnsMessageReceived(object sender, DnsMessageEventArgs args)
		{
			Debug.Print("Test Harness: DNS Message Received " + args.RequestMessage.ToString());
		}

		private void DnsService_OnDnsMessageSent(object sender, DnsMessageEventArgs args)
		{
			Debug.Print("Test Harness: DNS Message Sent " + args.RequestMessage.ToString());
		}

		private void SntpService_OnSntpMessageReceived(object sender, SntpMessageEventArgs args)
		{
			Debug.Print("Test Harness: SNTP Message Received " + args.RequestMessage.ToString());
		}

		private void SntpService_OnSntpMessageSent(object sender, SntpMessageEventArgs args)
		{
			Debug.Print("Test Harness: SNTP Message Sent " + args.RequestMessage.ToString());
		}

		#endregion Event Handlers

	}
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace uITron3
{
	/** Function prototype for netif init functions. Set up flags and output/linkoutput
	 * callback functions in this function.
	 *
	 * @param netif The netif to initialize
	 */
	public delegate err_t netif_init_fn(netif netif);
	/** Function prototype for netif.input functions. This function is saved as 'input'
	 * callback function in the netif struct. Call it when a packet has been received.
	 *
	 * @param p The received packet, copied into a pbuf
	 * @param inp The netif which received the packet
	 */
	public delegate err_t netif_input_fn(pbuf p, netif inp);
	/** Function prototype for netif.output functions. Called by lwIP when a packet
	 * shall be sent. For ethernet netif, set this to 'etharp_output' and set
	 * 'linkoutput'.
	 *
	 * @param netif The netif which shall send a packet
	 * @param p The packet to send (p.payload points to IP header)
	 * @param ipaddr The IP address to which the packet shall be sent
	 */
	public delegate err_t netif_output_t(netif netif, pbuf p, ip_addr ipaddr);
	/** Function prototype for netif.linkoutput functions. Only used for ethernet
	 * netifs. This function is called by ARP when a packet shall be sent.
	 *
	 * @param netif The netif which shall send a packet
	 * @param p The packet to send (raw ethernet packet)
	 */
	public delegate err_t netif_linkoutput_fn(netif netif, pbuf p);

	public class netif
	{
		lwip lwip;

		/** Whether the network interface is 'up'. This is
		 * a software flag used to control whether this network
		 * interface is enabled and processes traffic.
		 * It is set by the startup code (for static IP configuration) or
		 * by dhcp/autoip when an address has been assigned.
		 */
		public const byte NETIF_FLAG_UP = (byte)0x01U;
		/** If set, the netif has broadcast capability.
		 * Set by the netif driver in its init function. */
		public const byte NETIF_FLAG_BROADCAST = (byte)0x02U;
		/** If set, the netif is one end of a point-to-point connection.
		 * Set by the netif driver in its init function. */
		public const byte NETIF_FLAG_POINTTOPOINT = (byte)0x04U;
		/** If set, the interface is configured using DHCP.
		 * Set by the DHCP code when starting or stopping DHCP. */
		public const byte NETIF_FLAG_DHCP = (byte)0x08U;
		/** If set, the interface has an active link
		 *  (set by the network interface driver).
		 * Either set by the netif driver in its init function (if the link
		 * is up at that time) or at a later point once the link comes up
		 * (if link detection is supported by the hardware). */
		public const byte NETIF_FLAG_LINK_UP = (byte)0x10U;
		/** If set, the netif is an ethernet device using ARP.
		 * Set by the netif driver in its init function.
		 * Used to check input packet types and use of DHCP. */
		public const byte NETIF_FLAG_ETHARP = (byte)0x20U;
		/** If set, the netif is an ethernet device. It might not use
		 * ARP or TCP/IP if it is used for PPPoE only.
		 */
		public const byte NETIF_FLAG_ETHERNET = (byte)0x40U;
		/** If set, the netif has IGMP capability.
		 * Set by the netif driver in its init function. */
		public const byte NETIF_FLAG_IGMP = (byte)0x80U;

		public netif next;

		public ushort mtu = 1500;
		public ip_addr ip_addr = new ip_addr(new byte[ip_addr.length]);
		public ip_addr netmask = new ip_addr(new byte[ip_addr.length]);
		public ip_addr gw = new ip_addr(new byte[ip_addr.length]);
		public byte flags = NETIF_FLAG_BROADCAST;
		public netif_input_fn input;
		public netif_linkoutput_fn linkoutput;
		public object addr_hint;
		public eth_addr hwaddr = new eth_addr(0, 0, 0, 0, 0, 0);
		public int hwaddr_len = 6;
		public object state;
		public int num;
		public string name;
		public netif_output_t output;

		public netif(lwip lwip, string name, netif_output_t output, netif_linkoutput_fn linkoutput)
		{
			this.lwip = lwip;
			this.name = name;
			this.output = output;
			this.linkoutput = linkoutput;
		}

		private void set_ipaddr(ip_addr ipaddr)
		{
			ip_addr.ip_addr_set(this.ip_addr, ipaddr);

			lwip.LWIP_DEBUGF(opt.NETIF_DEBUG | lwip.LWIP_DBG_TRACE | lwip.LWIP_DBG_STATE, "netif: IP address of interface {0}{1} set to {2}.{3}.{4}.{5}\n",
				name[0], name[1],
				ip_addr.ip4_addr1_16(this.ip_addr),
				ip_addr.ip4_addr2_16(this.ip_addr),
				ip_addr.ip4_addr3_16(this.ip_addr),
				ip_addr.ip4_addr4_16(this.ip_addr));
		}

		public void set_gw(ip_addr gw)
		{
			ip_addr.ip_addr_set(this.gw, gw);
			lwip.LWIP_DEBUGF(opt.NETIF_DEBUG | lwip.LWIP_DBG_TRACE | lwip.LWIP_DBG_STATE, "netif: GW address of interface {0}{1} set to {2}.{3}.{4}.{5}\n",
				name[0], name[1],
				ip_addr.ip4_addr1_16(this.gw),
				ip_addr.ip4_addr2_16(this.gw),
				ip_addr.ip4_addr3_16(this.gw),
				ip_addr.ip4_addr4_16(this.gw));
		}

		public void set_netmask(ip_addr netmask)
		{
			ip_addr.ip_addr_set(this.netmask, netmask);
			lwip.LWIP_DEBUGF(opt.NETIF_DEBUG | lwip.LWIP_DBG_TRACE | lwip.LWIP_DBG_STATE, "netif: netmask of interface {0}{1} set to {2}.{3}.{4}.{5}\n",
				name[0], name[1],
				ip_addr.ip4_addr1_16(this.netmask),
				ip_addr.ip4_addr2_16(this.netmask),
				ip_addr.ip4_addr3_16(this.netmask),
				ip_addr.ip4_addr4_16(this.netmask));
		}

		/** Ask if an interface is up */
		public static bool netif_is_up(netif netif) => (netif.flags & NETIF_FLAG_UP) != 0;

		public static void NETIF_SET_HWADDRHINT(netif netif, object hint)
		{
			netif.addr_hint = hint;
		}

		/**
		 * Change IP address configuration for a network interface (including netmask
		 * and default gateway).
		 *
		 * @param netif the network interface to change
		 * @param ipaddr the new IP address
		 * @param netmask the new netmask
		 * @param gw the new default gateway
		 */
		public static void netif_set_addr(netif netif, ip_addr ipaddr, ip_addr netmask, ip_addr gw)
		{
			netif.set_ipaddr(ipaddr);
			netif.set_netmask(netmask);
			netif.set_gw(gw);
		}
	}

	partial class lwip
	{
		internal netif netif_default;
		internal netif netif_list;
		internal int netif_num;

		/**
		 * Add a network interface to the list of lwIP netifs.
		 *
		 * @param netif a pre-allocated netif structure
		 * @param ipaddr IP address for the new netif
		 * @param netmask network mask for the new netif
		 * @param gw default gateway IP address for the new netif
		 * @param state opaque data passed to the new netif
		 * @param init callback function that initializes the interface
		 * @param input callback function that is called to pass
		 * ingress packets up in the protocol layer stack.
		 *
		 * @return netif, or null if failed.
		 */
		public netif netif_add(netif netif, ip_addr ipaddr, ip_addr netmask,
			ip_addr gw, object state, netif_init_fn init, netif_input_fn input)
		{
			LWIP_ASSERT("No init function given", init != null);

			/* reset new interface configuration state */
			ip_addr.ip_addr_set_zero(netif.ip_addr);
			ip_addr.ip_addr_set_zero(netif.netmask);
			ip_addr.ip_addr_set_zero(netif.gw);
			netif.flags = 0;
#if LWIP_DHCP
			/* netif not under DHCP control by default */
			netif.dhcp = null;
#endif // LWIP_DHCP
#if LWIP_AUTOIP
			/* netif not under AutoIP control by default */
			netif.autoip = null;
#endif // LWIP_AUTOIP
#if LWIP_NETIF_STATUS_CALLBACK
			netif.status_callback = null;
#endif // LWIP_NETIF_STATUS_CALLBACK
#if LWIP_NETIF_LINK_CALLBACK
			netif.link_callback = null;
#endif // LWIP_NETIF_LINK_CALLBACK
#if LWIP_IGMP
			netif.igmp_mac_filter = null;
#endif // LWIP_IGMP
#if ENABLE_LOOPBACK
			netif.loop_first = null;
			netif.loop_last = null;
#endif // ENABLE_LOOPBACK

			/* remember netif specific state information data */
			netif.state = state;
			netif.num = netif_num++;
			netif.input = input;
			netif.NETIF_SET_HWADDRHINT(netif, null);
#if ENABLE_LOOPBACK && LWIP_LOOPBACK_MAX_PBUFS
			netif.loop_cnt_current = 0;
#endif // ENABLE_LOOPBACK && LWIP_LOOPBACK_MAX_PBUFS */

			netif.netif_set_addr(netif, ipaddr, netmask, gw);

			/* call user specified initialization function for netif */
			if (init(netif) != err_t.ERR_OK) {
				return null;
			}

			/* add this netif to the list */
			netif.next = netif_list;
			netif_list = netif;
			//snmp_inc_iflist();

#if LWIP_IGMP
			/* start IGMP processing */
			if (netif.flags & NETIF_FLAG_IGMP) {
				igmp_start(netif);
			}
#endif // LWIP_IGMP */

			LWIP_DEBUGF(opt.NETIF_DEBUG, "netif: added interface {0}{1} IP addr ",
				netif.name[0], netif.name[1]);
			ip_addr.ip_addr_debug_print(opt.NETIF_DEBUG, ipaddr);
			LWIP_DEBUGF(opt.NETIF_DEBUG, (" netmask "));
			ip_addr.ip_addr_debug_print(opt.NETIF_DEBUG, netmask);
			LWIP_DEBUGF(opt.NETIF_DEBUG, (" gw "));
			ip_addr.ip_addr_debug_print(opt.NETIF_DEBUG, gw);
			LWIP_DEBUGF(opt.NETIF_DEBUG, ("\n"));
			return netif;
		}

		/**
		 * Set a network interface as the default network interface
		 * (used to output all packets for which no specific route is found)
		 *
		 * @param netif the default network interface
		 */
		internal void netif_set_default(netif netif)
		{
			this.netif_default = netif;
		}

		/**
		 * Bring an interface up, available for processing
		 * traffic.
		 * 
		 * @note: Enabling DHCP on a down interface will make it come
		 * up once configured.
		 * 
		 * @see dhcp_start()
		 */
		public void netif_set_up(netif netif)
		{
			if ((netif.flags & netif.NETIF_FLAG_UP) == 0) {
				netif.flags |= netif.NETIF_FLAG_UP;

#if LWIP_SNMP
				snmp_get_sysuptime(&netif.ts);
#endif // LWIP_SNMP

				//NETIF_STATUS_CALLBACK(netif);

				if ((netif.flags & netif.NETIF_FLAG_LINK_UP) != 0) {
#if LWIP_ARP
					/* For Ethernet network interfaces, we would like to send a "gratuitous ARP" */
					if ((netif.flags & (netif.NETIF_FLAG_ETHARP)) != 0) {
						etharp.etharp_gratuitous(netif);
					}
#endif // LWIP_ARP

#if LWIP_IGMP
					/* resend IGMP memberships */
					if (netif.flags & NETIF_FLAG_IGMP) {
						igmp_report_groups( netif);
					}
#endif // LWIP_IGMP
				}
			}
		}

		/**
		 * Bring an interface down, disabling any traffic processing.
		 *
		 * @note: Enabling DHCP on a down interface will make it come
		 * up once configured.
		 * 
		 * @see dhcp_start()
		 */
		void netif_set_down(netif netif)
		{
			if ((netif.flags & netif.NETIF_FLAG_UP) != 0) {
				netif.flags &= unchecked((byte)~netif.NETIF_FLAG_UP);
#if LWIP_SNMP
				snmp_get_sysuptime(&netif.ts);
#endif

#if LWIP_ARP
				if ((netif.flags & netif.NETIF_FLAG_ETHARP) != 0) {
					etharp.etharp_cleanup_netif(netif);
				}
#endif // LWIP_ARP
				//NETIF_STATUS_CALLBACK(netif);
			}
		}
	}
}

#define CHECKSUM_GEN_IP
#define CHECKSUM_CHECK_IP
/**
 * @file
 * This is the IPv4 layer implementation for incoming and outgoing IP traffic.
 * 
 * @see ip_frag.c
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace uITron3
{
	public class ip_hdr : pointer
	{
		public new const int length = 20;

		public const ushort IP_RF = (ushort)0x8000U;        /* reserved fragment flag */
		public const ushort IP_DF = (ushort)0x4000U;        /* dont fragment flag */
		public const ushort IP_MF = (ushort)0x2000U;        /* more fragments flag */
		public const ushort IP_OFFMASK = (ushort)0x1fffU;   /* mask for fragmenting bits */

		public ip_hdr(pointer src, int offset = 0)
			: base(src, offset)
		{
		}

		public ip_hdr(byte[] data, int offset = 0)
			: base(data, offset)
		{
		}

		public byte v {
			get { return (byte)(data[offset] >> 4); }
			set { data[offset] = (byte)((data[offset] & 0x0F) | (value << 4)); }
		}

		public byte hl {
			get { return (byte)(data[offset] & 0xF); }
			set { data[offset] = (byte)((data[offset] & 0xF0) | (value & 0xF)); }
		}

		public byte tos {
			get { return data[offset + 1]; }
			set { data[offset + 1] = value; }
		}

		public ushort len {
			get { return BitConverter.ToUInt16(data, offset + 2); }
			set { Buffer.BlockCopy(BitConverter.GetBytes(value), 0, data, offset + 2, sizeof(ushort)); }
		}

		public ushort id {
			get { return BitConverter.ToUInt16(data, offset + 4); }
			set { Buffer.BlockCopy(BitConverter.GetBytes(value), 0, data, offset + 4, sizeof(ushort)); }
		}

		public byte flag {
			get { return (byte)(data[offset + 6] >> 5); }
			set { data[offset] = (byte)((data[offset + 6] & 0x1F) | (value << 5)); }
		}

		public ushort fgm_ofs {
			get { return BitConverter.ToUInt16(data, offset + 6); }
			set { Buffer.BlockCopy(BitConverter.GetBytes(value), 0, data, offset + 6, sizeof(ushort)); }
		}

		public byte ttl {
			get { return data[offset + 8]; }
			set { data[offset + 8] = value; }
		}

		public byte proto {
			get { return data[offset + 9]; }
			set { data[offset + 9] = value; }
		}

		public ushort chksum {
			get { return BitConverter.ToUInt16(data, offset + 10); }
			set { Buffer.BlockCopy(BitConverter.GetBytes(value), 0, data, offset + 10, sizeof(ushort)); }
		}

		public ip_addr src {
			get { return new ip_addr(data, offset + 12); }
			set { Buffer.BlockCopy(value.data, value.offset, data, offset + 12, ip_addr.length); }
		}

		public ip_addr dest {
			get { return new ip_addr(data, offset + 16); }
			set { Buffer.BlockCopy(value.data, value.offset, data, offset + 16, ip_addr.length); }
		}
	}

	public class ip_pcb : memp
	{
		public ip_pcb(lwip lwip)
			: base(lwip)
		{
		}

		/* Common members of all PCB types */
		/* ip addresses in network byte order */
		private ip_addr _local_ip = new ip_addr(new byte[ip_addr.length]);
		private ip_addr _remote_ip = new ip_addr(new byte[ip_addr.length]);
		public ip_addr local_ip { get { return _local_ip; } }
		public ip_addr remote_ip { get { return _remote_ip; } }
		/* Socket options */
		public byte so_options;
		/* Type Of Service */
		public byte tos;
		/* Time To Live */
		public byte ttl;
		/* link layer address resolution hint */
#if LWIP_NETIF_HWADDRHINT
		public byte addr_hint;
#endif // LWIP_NETIF_HWADDRHINT
	}

	/*
	 * Option flags per-socket. These are the same like SO_XXX.
	 */
	public static class sof
	{
		/*public const byte SOF_DEBUG     = 0x01;     Unimplemented: turn on debugging info recording */
		public const byte SOF_ACCEPTCONN = 0x02;  /* socket has had listen() */
		public const byte SOF_REUSEADDR = 0x04;  /* allow local address reuse */
		public const byte SOF_KEEPALIVE = 0x08;  /* keep connections alive */
		/*public const uint SOF_DONTROUTE = 0x10U;     Unimplemented: just use interface addresses */
		public const byte SOF_BROADCAST = 0x20;  /* permit to send and to receive broadcast messages (see IP_SOF_BROADCAST option) */
		/*public const uint SOF_USELOOPBACK = 0x40U;     Unimplemented: bypass hardware when possible */
		public const byte SOF_LINGER = 0x80;  /* linger on close if data present */
		/*public const uint SOF_OOBINLINE = 0x0100U;   Unimplemented: leave received OOB data in line */
		/*public const uint SOF_REUSEPORT = 0x0200U;   Unimplemented: allow local address & port reuse */

		/* These flags are inherited (e.g. from a listen-pcb to a connection-pcb): */
		public const byte SOF_INHERITED = (SOF_REUSEADDR | SOF_KEEPALIVE | SOF_LINGER);/*|SOF_DEBUG|SOF_DONTROUTE|SOF_OOBINLINE*/
	}

	public class ip
	{
		lwip lwip;

		public const int IP_HLEN = ip_hdr.length;

		public const int IP_PROTO_ICMP = 1;
		public const int IP_PROTO_IGMP = 2;
		public const int IP_PROTO_UDP = 17;
		public const int IP_PROTO_UDPLITE = 136;
		public const int IP_PROTO_TCP = 6;
		public const byte NETIF_FLAG_BROADCAST = (byte)0x02U;

		public static ip_addr IP_HDRINCL = null;

		public ushort mtu = 1500;
		public ip_addr ip_addr = new ip_addr(0x00000000);
		public ip_addr netmask = new ip_addr(0xFFFFFFFF);
		public byte flags = NETIF_FLAG_BROADCAST;

		public static ushort LWIP_MAKE_U16(byte a, byte b) => (ushort)((a << 8) | b);

		public ip(lwip lwip)
		{
			this.lwip = lwip;
		}

		internal ip_addr ip_current_src_addr()
		{
			return current_iphdr_src;
		}

		internal ip_addr ip_current_dest_addr()
		{
			return current_iphdr_dest;
		}

		internal static bool ip_get_option(ip_pcb pcb, byte flag)
		{
			return (pcb.so_options & flag) != 0;
		}

		internal static void ip_set_option(ip_pcb pcb, byte flag)
		{
			pcb.so_options |= flag;
		}

		/**
		 * The interface that provided the packet for the current callback
		 * invocation.
		 */
		netif current_netif;

		/**
		 * Header of the input packet currently being processed.
		 */
		ip_hdr current_header;
		/** Source IP address of current_header */
		internal ip_addr current_iphdr_src = new ip_addr(new byte[ip_addr.length]);
		/** Destination IP address of current_header */
		internal ip_addr current_iphdr_dest = new ip_addr(new byte[ip_addr.length]);

		/** The IP header ID of the next outgoing IP packet */
		private ushort ip_id;

		/**
		 * Finds the appropriate network interface for a given IP address. It
		 * searches the list of network interfaces linearly. A match is found
		 * if the masked IP address of the network interface equals the masked
		 * IP address given to the function.
		 *
		 * @param dest the destination IP address for which to find the route
		 * @return the netif on which to send to reach dest
		 */
		internal netif ip_route(ip_addr dest)
		{
			netif netif;

#if LWIP_HOOK_IP4_ROUTE
			netif = LWIP_HOOK_IP4_ROUTE(dest);
			if (netif != null) {
				return netif;
			}
#endif

			/* iterate through netifs */
			for (netif = lwip.netif_list; netif != null; netif = netif.next) {
				/* network mask matches? */
				if (netif.netif_is_up(netif)) {
					if (ip_addr.ip_addr_netcmp(dest, netif.ip_addr, netif.netmask)) {
						/* return netif on which to forward IP packet */
						return netif;
					}
				}
			}
			if ((lwip.netif_default == null) || (!netif.netif_is_up(lwip.netif_default))) {
				lwip.LWIP_DEBUGF(opt.IP_DEBUG | lwip.LWIP_DBG_LEVEL_SERIOUS, "ip_route: No route to {0}.{1}.{2}.{3}\n",
				  ip_addr.ip4_addr1_16(dest), ip_addr.ip4_addr2_16(dest), ip_addr.ip4_addr3_16(dest), ip_addr.ip4_addr4_16(dest));
				//IP_STATS_INC(ip.rterr);
				//snmp_inc_ipoutnoroutes();
				return null;
			}
			/* no matching netif found, use default netif */
			return lwip.netif_default;
		}

		/**
		 * This function is called by the network interface device driver when
		 * an IP packet is received. The function does the basic checks of the
		 * IP header such as packet size being at least larger than the header
		 * size etc. If the packet was not destined for us, the packet is
		 * forwarded (using ip_forward). The IP checksum is always checked.
		 *
		 * Finally, the packet is sent to the upper layer protocol input function.
		 * 
		 * @param p the received IP packet (p.payload points to IP header)
		 * @param inp the netif on which this packet was received
		 * @return ERR_OK if the packet was processed (could return ERR_* if it wasn't
		 *         processed, but currently always returns ERR_OK)
		 */
		internal err_t ip_input(pbuf p, netif inp)
		{
			ip_hdr iphdr;
			netif netif;
			ushort iphdr_hlen;
			ushort iphdr_len;
#if IP_ACCEPT_LINK_LAYER_ADDRESSING
			int check_ip_src = 1;
#endif // IP_ACCEPT_LINK_LAYER_ADDRESSING */

			//IP_STATS_INC(ip.recv);
			//snmp_inc_ipinreceives();

			/* identify the IP header */
			iphdr = new ip_hdr(p.payload);
			if (iphdr.v != 4) {
				lwip.LWIP_DEBUGF(opt.IP_DEBUG | lwip.LWIP_DBG_LEVEL_WARNING, "IP packet dropped due to bad version number {0}\n", iphdr.v);
				ip_debug_print(p);
				lwip.pbuf_free(p);
				//IP_STATS_INC(ip.err);
				//IP_STATS_INC(ip.drop);
				//snmp_inc_ipinhdrerrors();
				return err_t.ERR_OK;
			}

#if LWIP_HOOK_IP4_INPUT
			if (LWIP_HOOK_IP4_INPUT(p, inp)) {
				/* the packet has been eaten */
				return err_t.ERR_OK;
			}
#endif

			/* obtain IP header length in number of 32-bit words */
			iphdr_hlen = iphdr.hl;
			/* calculate IP header length in bytes */
			iphdr_hlen *= 4;
			/* obtain ip length in bytes */
			iphdr_len = lwip.lwip_ntohs(iphdr.len);

			/* header length exceeds first pbuf length, or ip length exceeds total pbuf length? */
			if ((iphdr_hlen > p.len) || (iphdr_len > p.tot_len)) {
				if (iphdr_hlen > p.len) {
					lwip.LWIP_DEBUGF(opt.IP_DEBUG | lwip.LWIP_DBG_LEVEL_SERIOUS,
						"IP header (len {0}) does not fit in first pbuf (len {1}), IP packet dropped.\n",
						iphdr_hlen, p.len);
				}
				if (iphdr_len > p.tot_len) {
					lwip.LWIP_DEBUGF(opt.IP_DEBUG | lwip.LWIP_DBG_LEVEL_SERIOUS,
						"IP (len {0}) is longer than pbuf (len {1}), IP packet dropped.\n",
						iphdr_len, p.tot_len);
				}
				/* free (drop) packet pbufs */
				lwip.pbuf_free(p);
				//IP_STATS_INC(ip.lenerr);
				//IP_STATS_INC(ip.drop);
				//snmp_inc_ipindiscards();
				return err_t.ERR_OK;
			}

			/* verify checksum */
#if CHECKSUM_CHECK_IP
			if (lwip.inet_chksum(iphdr, iphdr_hlen) != 0) {

				lwip.LWIP_DEBUGF(opt.IP_DEBUG | lwip.LWIP_DBG_LEVEL_SERIOUS,
				  "Checksum (0x{0}) failed, IP packet dropped.\n", lwip.inet_chksum(iphdr, iphdr_hlen));
				ip_debug_print(p);
				lwip.pbuf_free(p);
				//IP_STATS_INC(ip.chkerr);
				//IP_STATS_INC(ip.drop);
				//snmp_inc_ipinhdrerrors();
				return err_t.ERR_OK;
			}
#endif

			/* Trim pbuf. This should have been done at the netif layer,
			 * but we'll do it anyway just to be sure that its done. */
			lwip.pbuf_realloc(p, iphdr_len);

			/* copy IP addresses to aligned ip_addr */
			ip_addr.ip_addr_copy(current_iphdr_dest, iphdr.dest);
			ip_addr.ip_addr_copy(current_iphdr_src, iphdr.src);

			/* match packet against an interface, i.e. is this packet for us? */
#if LWIP_IGMP
			if (ip_addr.ip_addr_ismulticast(current_iphdr_dest)) {
				if ((inp.flags & netif.NETIF_FLAG_IGMP) && (igmp.igmp_lookfor_group(inp, current_iphdr_dest))) {
					netif = inp;
				}
				else {
					netif = null;
				}
			}
			else
#endif // LWIP_IGMP
			{
				/* start trying with inp. if that's not acceptable, start walking the
				   list of configured netifs.
				   'first' is used as a boolean to mark whether we started walking the list */
				int first = 1;
				netif = inp;
				do {
					lwip.LWIP_DEBUGF(opt.IP_DEBUG, "ip_input: iphdr.dest 0x{0:X} netif.ip_addr 0x{1:X} (0x{2:X}, 0x{3:X}, 0x{4:X})\n",
						ip_addr.ip4_addr_get_u32(iphdr.dest), ip_addr.ip4_addr_get_u32(netif.ip_addr),
						ip_addr.ip4_addr_get_u32(iphdr.dest) & ip_addr.ip4_addr_get_u32(netif.netmask),
						ip_addr.ip4_addr_get_u32(netif.ip_addr) & ip_addr.ip4_addr_get_u32(netif.netmask),
						ip_addr.ip4_addr_get_u32(iphdr.dest) & ~ip_addr.ip4_addr_get_u32(netif.netmask));

					/* interface is up and configured? */
					if ((netif.netif_is_up(netif)) && (!ip_addr.ip_addr_isany(netif.ip_addr))) {
						/* unicast to this interface address? */
						if (ip_addr.ip_addr_cmp(current_iphdr_dest, netif.ip_addr) ||
							/* or broadcast on this interface network address? */
							ip_addr.ip_addr_isbroadcast(current_iphdr_dest, netif)) {
							lwip.LWIP_DEBUGF(opt.IP_DEBUG, "ip_input: packet accepted on interface {0}{1}\n",
								netif.name[0], netif.name[1]);
							/* break out of for loop */
							break;
						}
#if LWIP_AUTOIP
						/* connections to link-local addresses must persist after changing
						   the netif's address (RFC3927 ch. 1.9) */
						if ((netif.autoip != null) &&
							ip_addr.ip_addr_cmp(current_iphdr_dest, netif.autoip.llipaddr)) {
							lwip.LWIP_DEBUGF(opt.IP_DEBUG, "ip_input: LLA packet accepted on interface {0}{1}\n",
								netif.name[0], netif.name[1]);
							/* break out of for loop */
							break;
						}
#endif // LWIP_AUTOIP
					}
					if (first != 0) {
						first = 0;
						netif = lwip.netif_list;
					}
					else {
						netif = netif.next;
					}
					if (netif == inp) {
						netif = netif.next;
					}
				} while (netif != null);
			}

#if IP_ACCEPT_LINK_LAYER_ADDRESSING
			/* Pass DHCP messages regardless of destination address. DHCP traffic is addressed
			 * using link layer addressing (such as Ethernet MAC) so we must not filter on IP.
			 * According to RFC 1542 section 3.1.1, referred by RFC 2131).
			 *
			 * If you want to accept private broadcast communication while a netif is down,
			 * define LWIP_IP_ACCEPT_UDP_PORT(dst_port), e.g.:
			 *
			 * #define LWIP_IP_ACCEPT_UDP_PORT(dst_port) ((dst_port) == PP_NTOHS(12345))
			 */
			if (netif == null) {
				/* remote port is DHCP server? */
				if (iphdr.proto == IP_PROTO_UDP) {
					udp_hdr udphdr = new udp_hdr(iphdr + iphdr_hlen);
					lwip.LWIP_DEBUGF(opt.IP_DEBUG | lwip.LWIP_DBG_TRACE, "ip_input: UDP packet to DHCP client port {0}\n",
					  lwip.lwip_ntohs(udphdr.dest));
					if (IP_ACCEPT_LINK_LAYER_ADDRESSED_PORT(udphdr.dest)) {
						lwip.LWIP_DEBUGF(opt.IP_DEBUG | lwip.LWIP_DBG_TRACE, ("ip_input: DHCP packet accepted.\n"));
						netif = inp;
						check_ip_src = 0;
					}
				}
			}
#endif // IP_ACCEPT_LINK_LAYER_ADDRESSING


			/* broadcast or multicast packet source address? Compliant with RFC 1122: 3.2.1.3 */
#if IP_ACCEPT_LINK_LAYER_ADDRESSING
			/* DHCP servers need 0.0.0.0 to be allowed as source address (RFC 1.1.2.2: 3.2.1.3/a) */
			if (check_ip_src != 0 && !ip_addr.ip_addr_isany(lwip.ip.current_iphdr_src))
#endif // IP_ACCEPT_LINK_LAYER_ADDRESSING
			{
				if (ip_addr.ip_addr_isbroadcast(current_iphdr_src, inp) ||
					ip_addr.ip_addr_ismulticast(current_iphdr_src)) {
					/* packet source is not valid */
					lwip.LWIP_DEBUGF(opt.IP_DEBUG | lwip.LWIP_DBG_TRACE | lwip.LWIP_DBG_LEVEL_WARNING, "ip_input: packet source is not valid.\n");
					/* free (drop) packet pbufs */
					lwip.pbuf_free(p);
					++lwip.lwip_stats.ip.drop;
					//snmp.snmp_inc_ipinaddrerrors();
					//snmp.snmp_inc_ipindiscards();
					return err_t.ERR_OK;
				}
			}

			/* packet not for us? */
			if (netif == null) {
				/* packet not for us, route or discard */
				lwip.LWIP_DEBUGF(opt.IP_DEBUG | lwip.LWIP_DBG_TRACE, "ip_input: packet not for us.\n");
#if IP_FORWARD
				/* non-broadcast packet? */
				if (!ip_addr.ip_addr_isbroadcast(current_iphdr_dest, inp)) {
					/* try to forward IP packet on (other) interfaces */
					ip_forward(p, iphdr, inp);
				}
				else
#endif // IP_FORWARD
				{
					//snmp_inc_ipinaddrerrors();
					//snmp_inc_ipindiscards();
				}
				lwip.pbuf_free(p);
				return err_t.ERR_OK;
			}
			/* packet consists of multiple fragments? */
			if ((iphdr.fgm_ofs & lwip.PP_HTONS(ip_hdr.IP_OFFMASK | ip_hdr.IP_MF)) != 0) {
#if IP_REASSEMBLY // packet fragment reassembly code present?
				lwip.LWIP_DEBUGF(opt.IP_DEBUG, "IP packet is a fragment (id=0x{0:X04} tot_len={1:X04} len={2} MF={3} offset={4}), calling ip_reass()\n",
				  lwip.lwip_ntohs(iphdr.id), p.tot_len, lwip.lwip_ntohs(iphdr.len), (iphdr.fgm_ofs & lwip.PP_HTONS(ip_hdr.IP_MF)) != 0, (lwip.lwip_ntohs(iphdr.fgm_ofs) & ip_hdr.IP_OFFMASK) * 8);
				/* reassemble the packet*/
				p = ip_reass(p);
				/* packet not fully reassembled yet? */
				if (p == null) {
					return err_t.ERR_OK;
				}
				iphdr = new ip_hdr(p.payload);
#else // IP_REASSEMBLY == 0, no packet fragment reassembly code present
				lwip.pbuf_free(p);
				lwip.LWIP_DEBUGF(opt.IP_DEBUG | lwip.LWIP_DBG_LEVEL_SERIOUS, "IP packet dropped since it was fragmented (0x{0:X}) (while IP_REASSEMBLY == 0).\n",
				  lwip.lwip_ntohs(iphdr.fgm_ofs));
				//IP_STATS_INC(ip.opterr);
				//IP_STATS_INC(ip.drop);
				/* unsupported protocol feature */
				//snmp_inc_ipinunknownprotos();
				return err_t.ERR_OK;
#endif // IP_REASSEMBLY
			}

#if !IP_OPTIONS_ALLOWED // no support for IP options in the IP header?

#if LWIP_IGMP
			/* there is an extra "router alert" option in IGMP messages which we allow for but do not police */
			if((iphdr_hlen > IP_HLEN) &&  (IPH_PROTO(iphdr) != IP_PROTO_IGMP)) {
#else
			if (iphdr_hlen > IP_HLEN) {
#endif // LWIP_IGMP
				lwip.LWIP_DEBUGF(opt.IP_DEBUG | lwip.LWIP_DBG_LEVEL_SERIOUS, "IP packet dropped since there were IP options (while IP_OPTIONS_ALLOWED == 0).\n");
				lwip.pbuf_free(p);
				//IP_STATS_INC(ip.opterr);
				//IP_STATS_INC(ip.drop);
				/* unsupported protocol feature */
				//snmp_inc_ipinunknownprotos();
				return err_t.ERR_OK;
			}
#endif // IP_OPTIONS_ALLOWED == 0

			/* send to upper layers */
			lwip.LWIP_DEBUGF(opt.IP_DEBUG, "ip_input: \n");
			ip_debug_print(p);
			lwip.LWIP_DEBUGF(opt.IP_DEBUG, "ip_input: p.len {0} p.tot_len {1}\n", p.len, p.tot_len);

			current_netif = inp;
			current_header = iphdr;

#if LWIP_RAW
			/* raw input did not eat the packet? */
			if (raw.raw_input(p, inp) == 0)
#endif // LWIP_RAW
			{
				switch (iphdr.proto) {
#if LWIP_UDP
				case ip.IP_PROTO_UDP:
#if LWIP_UDPLITE
				case ip.IP_PROTO_UDPLITE:
#endif // LWIP_UDPLITE
					//snmp.snmp_inc_ipindelivers();
					lwip.udp.udp_input(p, inp, iphdr.proto == ip.IP_PROTO_UDPLITE);
					break;
#endif // LWIP_UDP
#if LWIP_TCP
				case ip.IP_PROTO_TCP:
					//snmp.snmp_inc_ipindelivers();
					lwip.tcp.tcp_input(p, inp);
					break;
#endif // LWIP_TCP
#if LWIP_ICMP
				case lwip.IP_PROTO_ICMP:
					//snmp.snmp_inc_ipindelivers();
					lwip.icmp.icmp_input(p, inp);
					break;
#endif // LWIP_ICMP
#if LWIP_IGMP
				case lwip.IP_PROTO_IGMP:
					lwip.igmp.igmp_input(p, inp, lwip.current_iphdr_dest);
					break;
#endif // LWIP_IGMP
				default:
#if LWIP_ICMP
					/* send ICMP destination protocol unreachable unless is was a broadcast */
					if (!ip_addr.ip_addr_isbroadcast(lwip.current_iphdr_dest, inp) &&
						!ip_addr.ip_addr_ismulticast(lwip.current_iphdr_dest))
					{
						p.payload = iphdr;
						lwip.icmp.icmp_dest_unreach(p, icmp_dur_type.ICMP_DUR_PROTO);
					}
#endif // LWIP_ICMP
					lwip.pbuf_free(p);

					lwip.LWIP_DEBUGF(opt.IP_DEBUG | lwip.LWIP_DBG_LEVEL_SERIOUS, "Unsupported transport protocol {0}\n", iphdr.proto);

					++lwip.lwip_stats.ip.proterr;
					++lwip.lwip_stats.ip.drop;
					//snmp_inc_ipinunknownprotos();
					break;
				}
			}

			//lwip.current_header = null;
			ip_addr.ip_addr_set_any(current_iphdr_src);
			ip_addr.ip_addr_set_any(current_iphdr_dest);

			return err_t.ERR_OK;
		}

		/**
		 * Sends an IP packet on a network interface. This function constructs
		 * the IP header and calculates the IP header checksum. If the source
		 * IP address is null, the IP address of the outgoing network
		 * interface is filled in as source address.
		 * If the destination IP address is IP_HDRINCL, p is assumed to already
		 * include an IP header and p.payload points to it instead of the data.
		 *
		 * @param p the packet to send (p.payload points to the data, e.g. next
					protocol header; if dest == IP_HDRINCL, p already includes an IP
					header and p.payload points to that IP header)
		 * @param src the source IP address to send from (if src == IP_ADDR_ANY, the
		 *         IP  address of the netif used to send is used as source address)
		 * @param dest the destination IP address to send the packet to
		 * @param ttl the TTL value to be set in the IP header
		 * @param tos the TOS value to be set in the IP header
		 * @param proto the PROTOCOL to be set in the IP header
		 * @param netif the netif on which to send this packet
		 * @return ERR_OK if the packet was sent OK
		 *         ERR_BUF if p doesn't have enough space for IP/LINK headers
		 *         returns errors returned by netif.output
		 *
		 * @note ip_id: RFC791 "some host may be able to simply use
		 *  unique identifiers independent of destination"
		 */
		internal err_t ip_output_if(pbuf p, ip_addr src, ip_addr dest, byte ttl, byte tos, byte proto, netif netif)
		{
			return ip_output_if_opt(p, src, dest, ttl, tos, proto, netif, null);
		}

		/**
		 * Same as ip_output_if() but with the possibility to include IP options:
		 *
		 * @ param ip_options pointer to the IP options, copied into the IP header
		 * @ param optlen length of ip_options
		 */
		internal err_t ip_output_if_opt(pbuf p, ip_addr src, ip_addr dest, byte ttl, byte tos, byte proto,
			netif netif, byte[] ip_options)
		{
			int optlen = ip_options == null ? 0 : ip_options.Length;
			ip_hdr iphdr;
			ip_addr dest_addr = new ip_addr(0);
#if CHECKSUM_GEN_IP_INLINE
			uint chk_sum = 0;
#endif // CHECKSUM_GEN_IP_INLINE */

			/* pbufs passed to IP must have a ref-count of 1 as their payload pointer
			   gets altered as the packet is passed down the stack */
			lwip.LWIP_ASSERT("p.ref == 1", p.@ref == 1);

			//snmp_inc_ipoutrequests();

			/* Should the IP header be generated or is it already included in p? */
			if (dest != ip.IP_HDRINCL) {
				short ip_hlen = ip.IP_HLEN;
				short optlen_aligned = 0;
				if (optlen != 0) {
#if CHECKSUM_GEN_IP_INLINE
				int i;
#endif // CHECKSUM_GEN_IP_INLINE
					/* round up to a multiple of 4 */
					optlen_aligned = (short)((optlen + 3) & ~3);
					ip_hlen += optlen_aligned;
					/* First write in the IP options */
					if (lwip.pbuf_header(p, optlen_aligned) != 0) {
						lwip.LWIP_DEBUGF(opt.IP_DEBUG | lwip.LWIP_DBG_LEVEL_SERIOUS, ("ip_output_if_opt: not enough room for IP options in pbuf\n"));
						//ip.IP_STATS_INC(ip.err);
						//snmp_inc_ipoutdiscards();
						return err_t.ERR_BUF;
					}
					pointer.memcpy(p.payload, new pointer(ip_options), optlen);
					if (optlen < optlen_aligned) {
						/* zero the remaining bytes */
						pointer.memset(p.payload + optlen, 0, optlen_aligned - optlen);
					}
#if CHECKSUM_GEN_IP_INLINE
					for (i = 0; i < optlen_aligned/2; i++) {
						chk_sum += ((ushort*)p.payload)[i];
					}
#endif // CHECKSUM_GEN_IP_INLINE
				}
				/* generate IP header */
				if (lwip.pbuf_header(p, ip.IP_HLEN) != 0) {
					lwip.LWIP_DEBUGF(opt.IP_DEBUG | lwip.LWIP_DBG_LEVEL_SERIOUS, ("ip_output: not enough room for IP header in pbuf\n"));

					//ip.IP_STATS_INC(ip.err);
					//snmp_inc_ipoutdiscards();
					return err_t.ERR_BUF;
				}

				iphdr = new ip_hdr(p.payload);
				lwip.LWIP_ASSERT("check that first pbuf can hold struct ip_hdr",
						   (p.len >= ip_hdr.length));

				iphdr.ttl = ttl;
				iphdr.proto = proto;
#if CHECKSUM_GEN_IP_INLINE
				chk_sum += LWIP_MAKE_U16(proto, ttl);
#endif // CHECKSUM_GEN_IP_INLINE

				/* dest cannot be null here */
				ip_addr.ip_addr_copy(iphdr.dest, dest);
#if CHECKSUM_GEN_IP_INLINE
				chk_sum += ip_addr.ip4_addr_get_u32(iphdr.dest) & 0xFFFF;
				chk_sum += ip_addr.ip4_addr_get_u32(iphdr.dest) >> 16;
#endif // CHECKSUM_GEN_IP_INLINE

				iphdr.v = 4;
				iphdr.hl = (byte)(ip_hlen / 4);
				iphdr.tos = tos;
#if CHECKSUM_GEN_IP_INLINE
				chk_sum += ip.LWIP_MAKE_U16(tos, (byte)(iphdr.v << 4 | iphdr.hl));
#endif // CHECKSUM_GEN_IP_INLINE
				iphdr.len = lwip.lwip_htons(p.tot_len);
#if CHECKSUM_GEN_IP_INLINE
				chk_sum += iphdr.len;
#endif // CHECKSUM_GEN_IP_INLINE
				iphdr.fgm_ofs = 0;
				iphdr.id = lwip.lwip_htons(ip_id);
#if CHECKSUM_GEN_IP_INLINE
				chk_sum += iphdr.id;
#endif // CHECKSUM_GEN_IP_INLINE
				ip_id++;

				if (ip_addr.ip_addr_isany(src)) {
					ip_addr.ip_addr_copy(iphdr.src, netif.ip_addr);
				}
				else {
					/* src cannot be null here */
					ip_addr.ip_addr_copy(iphdr.src, src);
				}

#if CHECKSUM_GEN_IP_INLINE
				chk_sum += ip_addr.ip4_addr_get_u32(iphdr.src) & 0xFFFF;
				chk_sum += ip_addr.ip4_addr_get_u32(iphdr.src) >> 16;
				chk_sum = (chk_sum >> 16) + (chk_sum & 0xFFFF);
				chk_sum = (chk_sum >> 16) + chk_sum;
				chk_sum = ~chk_sum;
				iphdr.chksum = (ushort)chk_sum; /* network order */
#else // CHECKSUM_GEN_IP_INLINE
				iphdr.chksum = 0;
#if CHECKSUM_GEN_IP
				iphdr.chksum = lwip.inet_chksum(iphdr, (ushort)ip_hlen);
#endif
#endif // CHECKSUM_GEN_IP_INLINE
			}
			else {
				/* IP header already included in p */
				iphdr = new ip_hdr(p.payload);
				ip_addr.ip_addr_copy(dest_addr, iphdr.dest);
				dest = dest_addr;
			}

			//ip.IP_STATS_INC(ip.xmit);

			lwip.LWIP_DEBUGF(opt.IP_DEBUG, "ip_output_if: {0}{1}{2}\n", netif.name[0], netif.name[1], netif.num);
#if IP_DEBUG
			ip_debug_print(p);
#endif

#if ENABLE_LOOPBACK
			if (ip_addr_cmp(dest, &netif.ip_addr)) {
				/* Packet to self, enqueue it for loopback */
				LWIP_DEBUGF(IP_DEBUG, ("netif_loop_output()"));
				return netif_loop_output(netif, p, dest);
			}
#if LWIP_IGMP
			if ((p.flags & PBUF_FLAG_MCASTLOOP) != 0) {
				netif_loop_output(netif, p, dest);
			}
#endif // LWIP_IGMP
#endif // ENABLE_LOOPBACK
#if IP_FRAG
			/* don't fragment if interface has mtu set to 0 [loopif] */
			if (netif.mtu && (p.tot_len > netif.mtu)) {
				return ip_frag(p, netif, dest);
			}
#endif // IP_FRAG

			lwip.LWIP_DEBUGF(opt.IP_DEBUG, "netif.output()");
			return netif.output(netif, p, dest);
		}


		/**
		 * Simple interface to ip_output_if. It finds the outgoing network
		 * interface and calls upon ip_output_if to do the actual work.
		 *
		 * @param p the packet to send (p.payload points to the data, e.g. next
					protocol header; if dest == ip.IP_HDRINCL, p already includes an IP
					header and p.payload points to that IP header)
		 * @param src the source IP address to send from (if src == IP_ADDR_ANY, the
		 *         IP  address of the netif used to send is used as source address)
		 * @param dest the destination IP address to send the packet to
		 * @param ttl the TTL value to be set in the IP header
		 * @param tos the TOS value to be set in the IP header
		 * @param proto the PROTOCOL to be set in the IP header
		 *
		 * @return err_t.ERR_RTE if no route is found
		 *         see ip_output_if() for more return values
		 */
		public err_t ip_output(pbuf p, ip_addr src, ip_addr dest,
			byte ttl, byte tos, byte proto)
		{
			netif netif;

			/* pbufs passed to IP must have a @ref-count of 1 as their payload pointer
			   gets altered as the packet is passed down the stack */
			lwip.LWIP_ASSERT("p.ref == 1", p.@ref == 1);

			if ((netif = ip_route(dest)) == null) {
				lwip.LWIP_DEBUGF(opt.IP_DEBUG, "ip_output: No route to {0}.{1}.{2}.{3}\n",
				  ip_addr.ip4_addr1_16(dest), ip_addr.ip4_addr2_16(dest), ip_addr.ip4_addr3_16(dest), ip_addr.ip4_addr4_16(dest));
				//ip.IP_STATS_INC(ip.rterr);
				return err_t.ERR_RTE;
			}

			return ip_output_if(p, src, dest, ttl, tos, proto, netif);
		}

#if LWIP_NETIF_HWADDRHINT
		/** Like ip.ip_output, but takes and addr_hint pointer that is passed on to netif.addr_hint
		 *  before calling ip_output_if.
		 *
		 * @param p the packet to send (p.payload points to the data, e.g. next
					protocol header; if dest == ip.IP_HDRINCL, p already includes an IP
					header and p.payload points to that IP header)
		 * @param src the source IP address to send from (if src == IP_ADDR_ANY, the
		 *         IP  address of the netif used to send is used as source address)
		 * @param dest the destination IP address to send the packet to
		 * @param ttl the TTL value to be set in the IP header
		 * @param tos the TOS value to be set in the IP header
		 * @param proto the PROTOCOL to be set in the IP header
		 * @param addr_hint address hint pointer set to netif.addr_hint before
		 *        calling ip_output_if()
		 *
		 * @return err_t.ERR_RTE if no route is found
		 *         see ip_output_if() for more return values
		 */
		public err_t ip_output_hinted(pbuf p, ip_addr src, ip_addr dest,
			byte ttl, byte tos, byte proto, object addr_hint)
		{
			netif netif;
			err_t err;

			/* pbufs passed to IP must have a @ref-count of 1 as their payload pointer
			   gets altered as the packet is passed down the stack */
			lwip.LWIP_ASSERT("p.ref == 1", p.@ref == 1);

			if ((netif = ip_route(dest)) == null) {
				lwip.LWIP_DEBUGF(opt.IP_DEBUG, "ip_output: No route to {0}.{1}.{2}.{3}\n",
				  ip_addr.ip4_addr1_16(dest), ip_addr.ip4_addr2_16(dest), ip_addr.ip4_addr3_16(dest), ip_addr.ip4_addr4_16(dest));
				//ip.IP_STATS_INC(ip.rterr);
				return err_t.ERR_RTE;
			}

			netif.NETIF_SET_HWADDRHINT(netif, addr_hint);
			err = ip_output_if(p, src, dest, ttl, tos, proto, netif);
			netif.NETIF_SET_HWADDRHINT(netif, null);

			return err;
		}
#endif // LWIP_NETIF_HWADDRHINT

		private void ip_debug_print(pbuf p)
		{
		}
	}

	public partial class lwip
	{
		internal ip ip;

		public lwip()
		{
			ip = new ip(this);
		}
	}
}

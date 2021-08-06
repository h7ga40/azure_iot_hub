/**
 * @file
 * Address Resolution Protocol module for IP over Ethernet
 *
 * Functionally, ARP is divided into two parts. The first maps an IP address
 * to a physical address when sending a packet, and the second part answers
 * requests from other machines for our physical address.
 *
 * This implementation complies with RFC 826 (Ethernet ARP). It supports
 * Gratuitious ARP from RFC3220 (IP Mobility Support for IPv4) section 4.6
 * if an interface calls etharp_gratuitous(our_netif) upon address change.
 */

/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * Copyright (c) 2003-2004 Leon Woestenberg <leon.woestenberg@axon.tv>
 * Copyright (c) 2003-2004 Axon Digital Design B.V., The Netherlands.
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
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace uITron3
{
	public class eth_addr : pointer
	{
		public new const int length = 6;

		public eth_addr(pointer src, int offset = 0)
			: base(src, offset)
		{
			addr = new addr_t(this);
		}

		public eth_addr(byte[] data, int offset = 0)
			: base(data, offset)
		{
			addr = new addr_t(this);
		}

		public eth_addr(byte v1, byte v2, byte v3, byte v4, byte v5, byte v6)
			: base(new byte[6])
		{
			data[0] = v1;
			data[1] = v2;
			data[2] = v3;
			data[3] = v4;
			data[4] = v5;
			data[5] = v6;
		}

		public eth_addr(ulong macAddr)
			: base(new byte[6])
		{
			data[0] = (byte)((macAddr >> 40) & 0xFF);
			data[1] = (byte)((macAddr >> 32) & 0xFF);
			data[2] = (byte)((macAddr >> 24) & 0xFF);
			data[3] = (byte)((macAddr >> 16) & 0xFF);
			data[4] = (byte)((macAddr >> 8) & 0xFF);
			data[5] = (byte)((macAddr >> 0) & 0xFF);
		}

		public class addr_t
		{
			eth_addr owner;

			public addr_t(eth_addr owner)
			{
				this.owner = owner;
			}

			public byte this[int index] {
				get { return owner.data[owner.offset + index]; }
				set { owner.data[owner.offset + index] = value; }
			}
		}

		public addr_t addr { get; private set; }
	}

	public class eth_hdr : pointer
	{
		public new const int length = eth_addr.length + eth_addr.length + sizeof(ushort);

		public eth_hdr(pointer src, int offset = 0)
			: base(src, offset)
		{
		}

		public eth_hdr(byte[] data, int offset = 0)
			: base(data, offset)
		{
		}

		public eth_addr dest {
			get { return new eth_addr(data, offset); }
			set { Buffer.BlockCopy(value.data, value.offset, data, offset, eth_addr.length); }
		}

		public eth_addr src {
			get { return new eth_addr(data, offset + eth_addr.length); }
			set { Buffer.BlockCopy(value.data, value.offset, data, offset + eth_addr.length, eth_addr.length); }
		}

		public ushort type {
			get { return BitConverter.ToUInt16(data, offset + eth_addr.length + eth_addr.length); }
			set { Buffer.BlockCopy(BitConverter.GetBytes(value), 0, data, offset + eth_addr.length + eth_addr.length, sizeof(ushort)); }
		}
	}

	public class etharp_hdr : pointer
	{
		public new const int length = 28;

		public etharp_hdr(pointer src, int offset = 0)
			: base(src, offset)
		{
		}

		public etharp_hdr(byte[] data, int offset = 0)
			: base(data, offset)
		{
		}

		public ushort hwtype {
			get { return BitConverter.ToUInt16(data, offset); }
			set { Buffer.BlockCopy(BitConverter.GetBytes(value), 0, data, offset, sizeof(ushort)); }
		}

		public ushort proto {
			get { return BitConverter.ToUInt16(data, offset + 2); }
			set { Buffer.BlockCopy(BitConverter.GetBytes(value), 0, data, offset + 2, sizeof(ushort)); }
		}

		public byte hwlen {
			get { return data[offset + 4]; }
			set { data[offset + 4] = value; }
		}

		public byte protolen {
			get { return data[offset + 5]; }
			set { data[offset + 5] = value; }
		}

		public ushort opcode {
			get { return BitConverter.ToUInt16(data, offset + 6); }
			set { Buffer.BlockCopy(BitConverter.GetBytes(value), 0, data, offset + 6, sizeof(ushort)); }
		}

		public eth_addr shwaddr {
			get { return new eth_addr(data, offset + 8); }
			set { Buffer.BlockCopy(value.data, value.offset, data, offset + 8, eth_addr.length); }
		}

		public ip_addr sipaddr {
			get { return new ip_addr(data, offset + 14); }
			set { Buffer.BlockCopy(value.data, value.offset, data, offset + 14, ip_addr.length); }
		}

		public eth_addr dhwaddr {
			get { return new eth_addr(data, offset + 18); }
			set { Buffer.BlockCopy(value.data, value.offset, data, offset + 18, eth_addr.length); }
		}

		public ip_addr dipaddr {
			get { return new ip_addr(data, offset + 24); }
			set { Buffer.BlockCopy(value.data, value.offset, data, offset + 24, ip_addr.length); }
		}
	}

#if LWIP_ARP || LWIP_ETHERNET
	public partial class lwip
	{
		public etharp etharp { get; internal set; }
	}

	public class etharp
	{
		const int ETHARP_HWADDR_LEN = 6;

		public const int SIZEOF_ETH_HDR = (14 + opt.ETH_PAD_SIZE);
		public const int SIZEOF_ETHARP_HDR = 28;
		public const int SIZEOF_ETHARP_PACKET = (SIZEOF_ETH_HDR + SIZEOF_ETHARP_HDR);

		/** 5 seconds period */
		public const int ARP_TMR_INTERVAL = 5000;

		public const ushort ETHTYPE_IP = 0x0800;
		public const ushort ETHTYPE_ARP = 0x0806;
		public const ushort ETHTYPE_VLAN = 0x8100;
		public const ushort ETHTYPE_PPPOEDISC = 0x8863; /* PPP Over Ethernet Discovery Stage */
		public const ushort ETHTYPE_PPPOE = 0x8864;     /* PPP Over Ethernet Session Stage */

		private void ETHADDR32_COPY(eth_addr ethaddr1, eth_addr ethaddr2)
		{
			pointer.memcpy(ethaddr1, ethaddr2, ETHARP_HWADDR_LEN);
		}

		private void ETHADDR16_COPY(eth_addr ethaddr1, eth_addr ethaddr2)
		{
			pointer.memcpy(ethaddr1, ethaddr2, ETHARP_HWADDR_LEN);
		}

		public const ushort ARP_REQUEST = 1;
		public const ushort ARP_REPLY = 2;

		public bool eth_addr_cmp(eth_addr addr1, eth_addr addr2) => (pointer.memcmp(addr1, addr2, ETHARP_HWADDR_LEN) == 0);

		public static readonly eth_addr ethbroadcast = new eth_addr(0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
		public static readonly eth_addr ethzero = new eth_addr(0, 0, 0, 0, 0, 0);

		/** The 24-bit IANA multicast OUI is 01-00-5e: */
		public const byte LL_MULTICAST_ADDR_0 = 0x01;
		public const byte LL_MULTICAST_ADDR_1 = 0x00;
		public const byte LL_MULTICAST_ADDR_2 = 0x5e;

#if LWIP_ARP // don't build if not configured for use in lwipopts.h

		/** the time an ARP entry stays valid after its last update,
		 *  for ARP_TMR_INTERVAL = 5000, this is
		 *  (240 * 5) seconds = 20 minutes.
		 */
		public const int ARP_MAXAGE = 240;
		/** Re-request a used ARP entry 1 minute before it would expire to prevent
		 *  breaking a steadily used connection because the ARP entry timed out. */
		public const int ARP_AGE_REREQUEST_USED = ARP_MAXAGE - 12;

		/** the time an ARP entry stays pending after first request,
		 *  for ARP_TMR_INTERVAL = 5000, this is
		 *  (2 * 5) seconds = 10 seconds.
		 * 
		 *  @internal Keep this number at least 2, otherwise it might
		 *  run out instantly if the timeout occurs directly after a request.
		 */
		public const int ARP_MAXPENDING = 2;

		public const int HWTYPE_ETHERNET = 1;

		enum ETHARP_STATE
		{
			EMPTY = 0,
			PENDING,
			STABLE,
			STABLE_REREQUESTING
#if ETHARP_SUPPORT_STATIC_ENTRIES
			, STATIC
#endif // ETHARP_SUPPORT_STATIC_ENTRIES
		};

		struct etharp_entry
		{
#if ARP_QUEUEING
			/** Pointer to queue of pending outgoing packets on this ARP entry. */
			public etharp_q_entry q;
#else // ARP_QUEUEING
			/** Pointer to a single pending outgoing packet on this ARP entry. */
			public pbuf q;
#endif // ARP_QUEUEING
			public ip_addr ipaddr;
			public netif netif;
			public eth_addr ethaddr;
			public ETHARP_STATE state;
			public byte ctime;
		}

		class etharp_q_entry
		{
			public etharp_q_entry next;
			public pbuf p;
		}

		etharp_entry[] arp_table = new etharp_entry[opt.ARP_TABLE_SIZE];
		private lwip lwip;

		const byte ETHARP_FLAG_TRY_HARD = 1;
		const byte ETHARP_FLAG_FIND_ONLY = 2;

		public etharp(lwip lwip)
		{
			this.lwip = lwip;

			for (int i = 0; i < arp_table.Length; i++) {
				arp_table[i].ipaddr = new ip_addr(0);
				arp_table[i].ethaddr = new eth_addr(0, 0, 0, 0, 0, 0);
			}
		}

#if ETHARP_SUPPORT_STATIC_ENTRIES
		const byte ETHARP_FLAG_STATIC_ENTRY = 4;
#endif // ETHARP_SUPPORT_STATIC_ENTRIES

		public static void etharp_init(lwip lwip)
		{
			lwip.etharp = new etharp(lwip);
		}

#if ARP_QUEUEING
		/**
		 * Free a complete queue of etharp entries
		 *
		 * @param q a qeueue of etharp_q_entry's to free
		 */
		private void free_etharp_q(etharp_q_entry q)
		{
			etharp_q_entry r;
			LWIP_ASSERT("q != null", q != null);
			LWIP_ASSERT("q.p != null", q.p != null);
			while (q != null) {
				r = q;
				q = q.next;
				LWIP_ASSERT("r.p != null", r.p != null);
				pbuf_free(r.p);
				//memp_free(memp_t.MEMP_ARP_QUEUE, r);
			}
		}
#else // ARP_QUEUEING

		/** Compatibility define: free the queued pbuf */
		private void free_etharp_q(pbuf q) => lwip.pbuf_free(q);

#endif // ARP_QUEUEING

		/** Clean up ARP table entries */
		private void etharp_free_entry(sbyte i)
		{
			/* remove from SNMP ARP index tree */
			snmp_delete_arpidx_tree(arp_table[i].netif, arp_table[i].ipaddr);
			/* and empty packet queue */
			if (arp_table[i].q != null) {
				/* remove all queued packets */
				lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG, "etharp_free_entry: freeing entry {0}, packet queue {1}.\n", (ushort)i, arp_table[i].q);
				free_etharp_q(arp_table[i].q);
				arp_table[i].q = null;
			}
			/* recycle entry for re-use */
			arp_table[i].state = ETHARP_STATE.EMPTY;
#if LWIP_DEBUG
			/* for debugging, clean out the complete entry */
			arp_table[i].ctime = 0;
			arp_table[i].netif = null;
			ip_addr_set_zero(arp_table[i].ipaddr);
			arp_table[i].ethaddr = ethzero;
#endif // LWIP_DEBUG
		}

		/**
		 * Clears expired entries in the ARP table.
		 *
		 * This function should be called every ETHARP_TMR_INTERVAL milliseconds (5 seconds),
		 * in order to expire entries in the ARP table.
		 */
		internal void etharp_tmr()
		{
			sbyte i;

			lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG, "etharp_timer\n");
			/* remove expired entries from the ARP table */
			for (i = 0; i < opt.ARP_TABLE_SIZE; ++i) {
				var state = arp_table[i].state;
				if (state != ETHARP_STATE.EMPTY
#if ETHARP_SUPPORT_STATIC_ENTRIES
					&& (state != ETHARP_STATE.STATIC)
#endif // ETHARP_SUPPORT_STATIC_ENTRIES
				) {
					arp_table[i].ctime++;
					if ((arp_table[i].ctime >= ARP_MAXAGE) ||
						((arp_table[i].state == ETHARP_STATE.PENDING) &&
						 (arp_table[i].ctime >= ARP_MAXPENDING))) {
						/* pending or stable entry has become old! */
						lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG, "etharp_timer: expired %s entry {0}.\n",
							 arp_table[i].state >= ETHARP_STATE.STABLE ? "stable" : "pending", i);
						/* clean up entries that have just been expired */
						etharp_free_entry(i);
					}
					else if (arp_table[i].state == ETHARP_STATE.STABLE_REREQUESTING) {
						/* Reset state to stable, so that the next transmitted packet will
						   re-send an ARP request. */
						arp_table[i].state = ETHARP_STATE.STABLE;
					}
#if ARP_QUEUEING
					/* still pending entry? (not expired) */
					if (arp_table[i].state == ETHARP_STATE.PENDING) {
						/* resend an ARP query here? */
					}
#endif // ARP_QUEUEING
				}
			}
		}

		/**
		 * Search the ARP table for a matching or new entry.
		 * 
		 * If an IP address is given, return a pending or stable ARP entry that matches
		 * the address. If no match is found, create a new entry with this address set,
		 * but in state ETHARP_EMPTY. The caller must check and possibly change the
		 * state of the returned entry.
		 * 
		 * If ipaddr is null, return a initialized new entry in state ETHARP_EMPTY.
		 * 
		 * In all cases, attempt to create new entries from an empty entry. If no
		 * empty entries are available and ETHARP_FLAG_TRY_HARD flag is set, recycle
		 * old entries. Heuristic choose the least important entry for recycling.
		 *
		 * @param ipaddr IP address to find in ARP cache, or to add if not found.
		 * @param flags @see definition of ETHARP_FLAG_*
		 * @param netif netif related to this address (used for NETIF_HWADDRHINT)
		 *  
		 * @return The ARP entry index that matched or is created, ERR_MEM if no
		 * entry is found or could be recycled.
		 */
		private sbyte etharp_find_entry(ip_addr ipaddr, byte flags)
		{
			sbyte old_pending = opt.ARP_TABLE_SIZE, old_stable = opt.ARP_TABLE_SIZE;
			sbyte empty = opt.ARP_TABLE_SIZE;
			sbyte i = 0; byte age_pending = 0, age_stable = 0;
			/* oldest entry with packets on queue */
			sbyte old_queue = opt.ARP_TABLE_SIZE;
			/* its age */
			byte age_queue = 0;

			/**
			 * a) do a search through the cache, remember candidates
			 * b) select candidate entry
			 * c) create new entry
			 */

			/* a) in a single search sweep, do all of this
			 * 1) remember the first empty entry (if any)
			 * 2) remember the oldest stable entry (if any)
			 * 3) remember the oldest pending entry without queued packets (if any)
			 * 4) remember the oldest pending entry with queued packets (if any)
			 * 5) search for a matching IP entry, either pending or stable
			 *    until 5 matches, or all entries are searched for.
			 */

			for (i = 0; i < opt.ARP_TABLE_SIZE; ++i) {
				var state = arp_table[i].state;
				/* no empty entry found yet and now we do find one? */
				if ((empty == opt.ARP_TABLE_SIZE) && (state == ETHARP_STATE.EMPTY)) {
					lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG, "etharp_find_entry: found empty entry {0}\n", i);
					/* remember first empty entry */
					empty = i;
				}
				else if (state != ETHARP_STATE.EMPTY) {
					lwip.LWIP_ASSERT("state == ETHARP_STATE_PENDING || state >= ETHARP_STATE_STABLE",
					  state == ETHARP_STATE.PENDING || state >= ETHARP_STATE.STABLE);
					/* if given, does IP address match IP address in ARP entry? */
					if (ipaddr != null && ip_addr.ip_addr_cmp(ipaddr, arp_table[i].ipaddr)) {
						lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE, "etharp_find_entry: found matching entry {0}\n", i);
						/* found exact IP address match, simply bail out */
						return i;
					}
					/* pending entry? */
					if (state == ETHARP_STATE.PENDING) {
						/* pending with queued packets? */
						if (arp_table[i].q != null) {
							if (arp_table[i].ctime >= age_queue) {
								old_queue = i;
								age_queue = arp_table[i].ctime;
							}
						}
						else
						/* pending without queued packets? */
						{
							if (arp_table[i].ctime >= age_pending) {
								old_pending = i;
								age_pending = arp_table[i].ctime;
							}
						}
						/* stable entry? */
					}
					else if (state >= ETHARP_STATE.STABLE) {
#if ETHARP_SUPPORT_STATIC_ENTRIES
						/* don't record old_stable for static entries since they never expire */
						if (state < ETHARP_STATE.STATIC)
#endif // ETHARP_SUPPORT_STATIC_ENTRIES
						{
							/* remember entry with oldest stable entry in oldest, its age in maxtime */
							if (arp_table[i].ctime >= age_stable) {
								old_stable = i;
								age_stable = arp_table[i].ctime;
							}
						}
					}
				}
			}
			/* { we have no match } => try to create a new entry */

			/* don't create new entry, only search? */
			if (((flags & ETHARP_FLAG_FIND_ONLY) != 0) ||
				/* or no empty entry found and not allowed to recycle? */
				((empty == opt.ARP_TABLE_SIZE) && ((flags & ETHARP_FLAG_TRY_HARD) == 0))) {
				lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE, ("etharp_find_entry: no empty entry found and not allowed to recycle\n"));
				return (sbyte)err_t.ERR_MEM;
			}

			/* b) choose the least destructive entry to recycle:
			 * 1) empty entry
			 * 2) oldest stable entry
			 * 3) oldest pending entry without queued packets
			 * 4) oldest pending entry with queued packets
			 * 
			 * { ETHARP_FLAG_TRY_HARD is set at this point }
			 */

			/* 1) empty entry available? */
			if (empty < opt.ARP_TABLE_SIZE) {
				i = empty;
				lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE, "etharp_find_entry: selecting empty entry {0}\n", (ushort)i);
			}
			else {
				/* 2) found recyclable stable entry? */
				if (old_stable < opt.ARP_TABLE_SIZE) {
					/* recycle oldest stable*/
					i = old_stable;
					lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE, "etharp_find_entry: selecting oldest stable entry {0}\n", (ushort)i);
					/* no queued packets should exist on stable entries */
					lwip.LWIP_ASSERT("arp_table[i].q == null", arp_table[i].q == null);
					/* 3) found recyclable pending entry without queued packets? */
				}
				else if (old_pending < opt.ARP_TABLE_SIZE) {
					/* recycle oldest pending */
					i = old_pending;
					lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE, "etharp_find_entry: selecting oldest pending entry {0} (without queue)\n", (ushort)i);
					/* 4) found recyclable pending entry with queued packets? */
				}
				else if (old_queue < opt.ARP_TABLE_SIZE) {
					/* recycle oldest pending (queued packets are free in etharp_free_entry) */
					i = old_queue;
					lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE, "etharp_find_entry: selecting oldest pending entry {0}, freeing packet queue {1}\n", (ushort)i, arp_table[i].q);
					/* no empty or recyclable entries found */
				}
				else {
					lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE, "etharp_find_entry: no empty or recyclable entries found\n");
					return (sbyte)err_t.ERR_MEM;
				}

				/* { empty or recyclable entry found } */
				lwip.LWIP_ASSERT("i < ARP_TABLE_SIZE", i < opt.ARP_TABLE_SIZE);
				etharp_free_entry(i);
			}

			lwip.LWIP_ASSERT("i < ARP_TABLE_SIZE", i < opt.ARP_TABLE_SIZE);
			lwip.LWIP_ASSERT("arp_table[i].state == ETHARP_STATE_EMPTY",
				arp_table[i].state == ETHARP_STATE.EMPTY);

			/* IP address given? */
			if (ipaddr != null) {
				/* set IP address */
				ip_addr.ip_addr_copy(arp_table[i].ipaddr, ipaddr);
			}
			arp_table[i].ctime = 0;
			return i;
		}

		/**
		 * Send an IP packet on the network using netif.linkoutput
		 * The ethernet header is filled in before sending.
		 *
		 * @params netif the lwIP network interface on which to send the packet
		 * @params p the packet to send, p.payload pointing to the (uninitialized) ethernet header
		 * @params src the source MAC address to be copied into the ethernet header
		 * @params dst the destination MAC address to be copied into the ethernet header
		 * @return ERR_OK if the packet was sent, any other err_t on failure
		 */
		private err_t etharp_send_ip(netif netif, pbuf p, eth_addr src, eth_addr dst)
		{
			eth_hdr ethhdr = new eth_hdr(p.payload);

			lwip.LWIP_ASSERT("netif.hwaddr_len must be the same as ETHARP_HWADDR_LEN for etharp!",
						(netif.hwaddr_len == ETHARP_HWADDR_LEN));
			ETHADDR32_COPY(ethhdr.dest, dst);
			ETHADDR16_COPY(ethhdr.src, src);
			ethhdr.type = lwip.PP_HTONS(ETHTYPE_IP);
			lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE, "etharp_send_ip: sending packet {0}\n", p);
			/* send the packet */
			return netif.linkoutput(netif, p);
		}

		private err_t etharp_update_arp_entry(netif netif, ip_addr ipaddr, eth_addr ethaddr, byte flags)
		{
			sbyte i;

			/* non-unicast address? */
			if (ip_addr.ip_addr_isany(ipaddr) ||
				ip_addr.ip_addr_isbroadcast(ipaddr, netif) ||
				ip_addr.ip_addr_ismulticast(ipaddr)) {
				lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE, "etharp_update_arp_entry: will not add non-unicast IP address to ARP cache\n");
				return err_t.ERR_ARG;
			}
			/* find or create ARP entry */
			i = etharp_find_entry(ipaddr, flags);
			/* bail out if no entry could be found */
			if (i < 0) {
				return (err_t)i;
			}

#if ETHARP_SUPPORT_STATIC_ENTRIES
			if ((flags & ETHARP_FLAG_STATIC_ENTRY) != 0) {
				/* record static type */
				arp_table[i].state = ETHARP_STATE.STATIC;
			}
			else
#endif // ETHARP_SUPPORT_STATIC_ENTRIES
			{
				/* mark it stable */
				arp_table[i].state = ETHARP_STATE.STABLE;
			}

			/* record network interface */
			arp_table[i].netif = netif;
			/* insert in SNMP ARP index tree */
			snmp_insert_arpidx_tree(netif, arp_table[i].ipaddr);

			lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE, "etharp_update_arp_entry: updating stable entry {0}\n", i);
			/* update address */
			ETHADDR32_COPY(arp_table[i].ethaddr, ethaddr);
			/* reset time stamp */
			arp_table[i].ctime = 0;
			/* this is where we will send out queued packets! */
#if ARP_QUEUEING
			while (arp_table[i].q != null) {
				pbuf p;
				/* remember remainder of queue */
				etharp_q_entry q = arp_table[i].q;
				/* pop first item off the queue */
				arp_table[i].q = q.next;
				/* get the packet pointer */
				p = q.p;
				/* now queue entry can be freed */
				//lwip.memp_free(memp_t.MEMP_ARP_QUEUE, q);
#else // ARP_QUEUEING
			if (arp_table[i].q != null) {

				pbuf p = arp_table[i].q;
				arp_table[i].q = null;
#endif // ARP_QUEUEING
				/* send the queued IP packet */
				etharp_send_ip(netif, p, new eth_addr(netif.hwaddr), ethaddr);
				/* free the queued IP packet */
				lwip.pbuf_free(p);
			}
			return err_t.ERR_OK;
		}

#if ETHARP_SUPPORT_STATIC_ENTRIES
		/** Add a new static entry to the ARP table. If an entry exists for the
		 * specified IP address, this entry is overwritten.
		 * If packets are queued for the specified IP address, they are sent out.
		 *
		 * @param ipaddr IP address for the new static entry
		 * @param ethaddr ethernet address for the new static entry
		 * @return @see return values of etharp_add_static_entry
		 */
		err_t etharp_add_static_entry(ip_addr ipaddr, eth_addr ethaddr)
		{
			netif netif;
			LWIP_DEBUGF(opt.ETHARP_DEBUG | LWIP_DBG_TRACE, "etharp_add_static_entry: {0}.{1}.{2}.{3} - {4:X02}:{5:X02}:{6:X02}:{7:X02}:{8:X02}:{9:X02}\n",
			  ip_addr.ip4_addr1_16(ipaddr), ip_addr.ip4_addr2_16(ipaddr), ip_addr.ip4_addr3_16(ipaddr), ip_addr.ip4_addr4_16(ipaddr),
			  ethaddr.addr[0], ethaddr.addr[1], ethaddr.addr[2],
			  ethaddr.addr[3], ethaddr.addr[4], ethaddr.addr[5]);

			netif = ip_route(ipaddr);
			if (netif == null) {
				return err_t.ERR_RTE;
			}

			return etharp_update_arp_entry(netif, ipaddr, ethaddr, ETHARP_FLAG_TRY_HARD | ETHARP_FLAG_STATIC_ENTRY);
		}

		/** Remove a static entry from the ARP table previously added with a call to
		 * etharp_add_static_entry.
		 *
		 * @param ipaddr IP address of the static entry to remove
		 * @return err_t.ERR_OK: entry removed
		 *         err_t.ERR_MEM: entry wasn't found
		 *         err_t.ERR_ARG: entry wasn't a static entry but a dynamic one
		 */
		err_t etharp_remove_static_entry(ip_addr ipaddr)
		{
			sbyte i;
			LWIP_DEBUGF(opt.ETHARP_DEBUG | LWIP_DBG_TRACE, "etharp_remove_static_entry: {0}.{1}.{2}.{3}\n",
				ip_addr.ip4_addr1_16(ipaddr), ip_addr.ip4_addr2_16(ipaddr), ip_addr.ip4_addr3_16(ipaddr), ip_addr.ip4_addr4_16(ipaddr));

			/* find or create ARP entry */
			i = etharp_find_entry(ipaddr, ETHARP_FLAG_FIND_ONLY);
			/* bail out if no entry could be found */
			if (i < 0) {
				return (err_t)i;
			}

			if (arp_table[i].state != ETHARP_STATE.STATIC) {
				/* entry wasn't a static entry, cannot remove it */
				return err_t.ERR_ARG;
			}
			/* entry found, free it */
			etharp_free_entry(i);
			return err_t.ERR_OK;
		}
#endif // ETHARP_SUPPORT_STATIC_ENTRIES

		/**
		 * Remove all ARP table entries of the specified netif.
		 *
		 * @param netif points to a network interface
		 */
		internal void etharp_cleanup_netif(netif netif)
		{
			sbyte i;

			for (i = 0; i < opt.ARP_TABLE_SIZE; ++i) {
				var state = arp_table[i].state;
				if ((state != ETHARP_STATE.EMPTY) && (arp_table[i].netif == netif)) {
					etharp_free_entry(i);
				}
			}
		}

		/**
		 * Finds (stable) ethernet/IP address pair from ARP table
		 * using interface and IP address index.
		 * @note the addresses in the ARP table are in network order!
		 *
		 * @param netif points to interface index
		 * @param ipaddr points to the (network order) IP address index
		 * @param eth_ret points to return pointer
		 * @param ip_ret points to return pointer
		 * @return table index if found, -1 otherwise
		 */
		sbyte etharp_find_addr(netif netif, ip_addr ipaddr, eth_addr eth_ret, ip_addr ip_ret)
		{
			sbyte i;

			lwip.LWIP_ASSERT("eth_ret != null && ip_ret != null",
				eth_ret != null && ip_ret != null);

			//LWIP_UNUSED_ARG(netif);

			i = etharp_find_entry(ipaddr, ETHARP_FLAG_FIND_ONLY);
			if ((i >= 0) && (arp_table[i].state >= ETHARP_STATE.STABLE)) {
				eth_ret = arp_table[i].ethaddr;
				ip_ret = arp_table[i].ipaddr;
				return i;
			}
			return -1;
		}

#if ETHARP_TRUST_IP_MAC
		/**
		 * Updates the ARP table using the given IP packet.
		 *
		 * Uses the incoming IP packet's source address to update the
		 * ARP cache for the local network. The function does not alter
		 * or free the packet. This function must be called before the
		 * packet p is passed to the IP layer.
		 *
		 * @param netif The lwIP network interface on which the IP packet pbuf arrived.
		 * @param p The IP packet that arrived on netif.
		 *
		 * @return null
		 *
		 * @see pbuf_free()
		 */
		private void etharp_ip_input(netif netif, pbuf p)
		{
			eth_hdr ethhdr;
			ip_hdr iphdr;
			ip_addr iphdr_src = new ip_addr(0);
			if (LWIP_ERROR("netif != null", netif != null))
				return;

			/* Only insert an entry if the source IP address of the
			   incoming IP packet comes from a host on the local network. */
			ethhdr = new eth_hdr(p.payload);
			iphdr = new ip_hdr(ethhdr, SIZEOF_ETH_HDR);
#if ETHARP_SUPPORT_VLAN
  if (ethhdr.type == PP_HTONS(ETHTYPE_VLAN)) {
    iphdr = (struct ip_hdr *)((byte*)ethhdr + SIZEOF_ETH_HDR + SIZEOF_VLAN_HDR);
  }
#endif // ETHARP_SUPPORT_VLAN

			ip_addr.ip_addr_copy(iphdr_src, iphdr.src);

			/* source is not on the local network? */
			if (!ip_addr.ip_addr_netcmp(iphdr_src, netif.ip_addr, netif.netmask)) {
				/* do nothing */
				return;
			}

			LWIP_DEBUGF(opt.ETHARP_DEBUG | LWIP_DBG_TRACE, ("etharp_ip_input: updating ETHARP table.\n"));
			/* update the source IP address in the cache, if present */
			/* @todo We could use ETHARP_FLAG_TRY_HARD if we think we are going to talk
			 * back soon (for example, if the destination IP address is ours. */
			etharp_update_arp_entry(netif, iphdr_src, ethhdr.src, ETHARP_FLAG_FIND_ONLY);
		}
#endif // ETHARP_TRUST_IP_MAC

		/**
		 * Responds to ARP requests to us. Upon ARP replies to us, add entry to cache  
		 * send out queued IP packets. Updates cache with snooped address pairs.
		 *
		 * Should be called for incoming ARP packets. The pbuf in the argument
		 * is freed by this function.
		 *
		 * @param netif The lwIP network interface on which the ARP packet pbuf arrived.
		 * @param ethaddr Ethernet address of netif.
		 * @param p The ARP packet that arrived on netif. Is freed by this function.
		 *
		 * @return null
		 *
		 * @see pbuf_free()
		 */
		public void etharp_arp_input(netif netif, eth_addr ethaddr, pbuf p)
		{
			var sipaddr = new ip_addr(0);
			var dipaddr = new ip_addr(0);
			bool for_us;
#if LWIP_AUTOIP
			byte[] ethdst_hwaddr;
#endif // LWIP_AUTOIP

			if (lwip.LWIP_ERROR("netif != null", netif != null))
				return;

			/* drop short ARP packets: we have to check for p.len instead of p.tot_len here
			   since a struct etharp_hdr is pointed to p.payload, so it musn't be chained! */
			if (p.len < SIZEOF_ETHARP_PACKET) {
				lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE | lwip.LWIP_DBG_LEVEL_WARNING,
					"etharp_arp_input: packet dropped, too short ({0}/{1})\n", p.tot_len,
					SIZEOF_ETHARP_PACKET);
				//ETHARP_STATS_INC(etharp.lenerr);
				//ETHARP_STATS_INC(etharp.drop);
				lwip.pbuf_free(p);
				return;
			}

			var ethhdr = new eth_hdr(p.payload);
			var hdr = new etharp_hdr(p.payload, SIZEOF_ETH_HDR);
#if ETHARP_SUPPORT_VLAN
			if (ethhdr.type == PP_HTONS(ETHTYPE_VLAN)) {
				hdr = (struct etharp_hdr *)(((byte*)ethhdr) + SIZEOF_ETH_HDR + SIZEOF_VLAN_HDR);
			}
#endif // ETHARP_SUPPORT_VLAN

			/* RFC 826 "Packet Reception": */
			if ((hdr.hwtype != lwip.PP_HTONS(HWTYPE_ETHERNET)) ||
				(hdr.hwlen != ETHARP_HWADDR_LEN) ||
				(hdr.protolen != ip_addr.length) ||
				(hdr.proto != lwip.PP_HTONS(ETHTYPE_IP))) {
				lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE | lwip.LWIP_DBG_LEVEL_WARNING,
					"etharp_arp_input: packet dropped, wrong hw type, hwlen, proto, protolen or ethernet type ({0}/{1}/{2}/{3})\n",
					hdr.hwtype, hdr.hwlen, hdr.proto, hdr.protolen);
				//ETHARP_STATS_INC(etharp.proterr);
				//ETHARP_STATS_INC(etharp.drop);
				lwip.pbuf_free(p);
				return;
			}
			//ETHARP_STATS_INC(etharp.recv);

#if LWIP_AUTOIP
			/* We have to check if a host already has configured our random
			 * created link local address and continously check if there is
			 * a host with this IP-address so we can detect collisions */
			autoip_arp_reply(netif, hdr);
#endif // LWIP_AUTOIP

			/* Copy struct ip_addr2 to aligned ip_addr, to support compilers without
			 * structure packing (not using structure copy which breaks strict-aliasing rules). */
			ip_addr.IPADDR2_COPY(sipaddr, hdr.sipaddr);
			ip_addr.IPADDR2_COPY(dipaddr, hdr.dipaddr);

			/* this interface is not configured? */
			if (ip_addr.ip_addr_isany(netif.ip_addr)) {
				for_us = false;
			}
			else {
				for_us = ip_addr.ip_addr_cmp(dipaddr, netif.ip_addr);
			}

			/* ARP message directed to us?
				. add IP address in ARP cache; assume requester wants to talk to us,
				   can result in directly sending the queued packets for this host.
			   ARP message not directed to us?
				.  update the source IP address in the cache, if present */
			etharp_update_arp_entry(netif, sipaddr, hdr.shwaddr,
							 for_us ? ETHARP_FLAG_TRY_HARD : ETHARP_FLAG_FIND_ONLY);

			/* now act on the message itself */
			switch (lwip.lwip_ntohs(hdr.opcode)) {
			/* ARP request? */
			case ARP_REQUEST:
				/* ARP request. If it asked for our address, we send out a
				 * reply. In any case, we time-stamp any existing ARP entry,
				 * and possiby send out an IP packet that was queued on it. */

				lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE, "etharp_arp_input: incoming ARP request\n");
				/* ARP request for our address? */
				if (for_us) {

					lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE, "etharp_arp_input: replying to ARP request for our IP address\n");
					/* Re-use pbuf to send ARP reply.
					   Since we are re-using an existing pbuf, we can't call etharp_raw since
					   that would allocate a new pbuf. */
					hdr.opcode = lwip.lwip_htons(ARP_REPLY);

					ip_addr.IPADDR2_COPY(hdr.dipaddr, hdr.sipaddr);
					ip_addr.IPADDR2_COPY(hdr.sipaddr, netif.ip_addr);

					lwip.LWIP_ASSERT("netif.hwaddr_len must be the same as ETHARP_HWADDR_LEN for etharp!",
								(netif.hwaddr_len == ETHARP_HWADDR_LEN));
#if LWIP_AUTOIP
					/* If we are using Link-Local, all ARP packets that contain a Link-Local
					 * 'sender IP address' MUST be sent using link-layer broadcast instead of
					 * link-layer unicast. (See RFC3927 Section 2.5, last paragraph) */
					ethdst_hwaddr = ip_addr.ip_addr_islinklocal(netif.ip_addr) ? ethbroadcast.addr : hdr.shwaddr.addr;
#endif // LWIP_AUTOIP

					ETHADDR16_COPY(hdr.dhwaddr, hdr.shwaddr);
#if LWIP_AUTOIP
					ETHADDR16_COPY(ethhdr.dest, ethdst_hwaddr);
#else  // LWIP_AUTOIP
					ETHADDR16_COPY(ethhdr.dest, hdr.shwaddr);
#endif // LWIP_AUTOIP
					ETHADDR16_COPY(hdr.shwaddr, ethaddr);
					ETHADDR16_COPY(ethhdr.src, ethaddr);

					/* hwtype, hwaddr_len, proto, protolen and the type in the ethernet header
					   are already correct, we tested that before */

					/* return ARP reply */
					netif.linkoutput(netif, p);
					/* we are not configured? */
				}
				else if (ip_addr.ip_addr_isany(netif.ip_addr)) {
					/* { for_us == 0 and netif.ip_addr.addr == 0 } */
					lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE, "etharp_arp_input: we are unconfigured, ARP request ignored.\n");
					/* request was not directed to us */
				}
				else {
					/* { for_us == 0 and netif.ip_addr.addr != 0 } */
					lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE, "etharp_arp_input: ARP request was not for us.\n");
				}
				break;
			case ARP_REPLY:
				/* ARP reply. We already updated the ARP cache earlier. */
				lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE, "etharp_arp_input: incoming ARP reply\n");
#if (LWIP_DHCP && DHCP_DOES_ARP_CHECK)
				/* DHCP wants to know about ARP replies from any host with an
				 * IP address also offered to us by the DHCP server. We do not
				 * want to take a duplicate IP address on a single network.
				 * @todo How should we handle redundant (fail-over) interfaces? */
				dhcp_arp_reply(netif, &sipaddr);
#endif // (LWIP_DHCP && DHCP_DOES_ARP_CHECK)
				break;
			default:
				lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE, "etharp_arp_input: ARP unknown opcode type {0}\n", lwip.lwip_htons(hdr.opcode));
				//ETHARP_STATS_INC(etharp.err);
				break;
			}
			/* free ARP packet */
			lwip.pbuf_free(p);
		}

		/** Just a small helper function that sends a pbuf to an ethernet address
		 * in the arp_table specified by the index 'arp_idx'.
		 */
		private err_t etharp_output_to_arp_index(netif netif, pbuf q, sbyte arp_idx)
		{
			lwip.LWIP_ASSERT("arp_table[arp_idx].state >= ETHARP_STATE.STABLE",
						arp_table[arp_idx].state >= ETHARP_STATE.STABLE);
			/* if arp table entry is about to expire: re-request it,
			   but only if its state is ETHARP_STATE.STABLE to prevent flooding the
			   network with ARP requests if this address is used frequently. */
			if ((arp_table[arp_idx].state == ETHARP_STATE.STABLE) &&
				(arp_table[arp_idx].ctime >= ARP_AGE_REREQUEST_USED)) {
				if (etharp_request(netif, arp_table[arp_idx].ipaddr) == err_t.ERR_OK) {
					arp_table[arp_idx].state = ETHARP_STATE.STABLE_REREQUESTING;
				}
			}

			return etharp_send_ip(netif, q, (eth_addr)(netif.hwaddr),
			  arp_table[arp_idx].ethaddr);
		}


		/**
		 * Resolve and fill-in Ethernet address header for outgoing IP packet.
		 *
		 * For IP multicast and broadcast, corresponding Ethernet addresses
		 * are selected and the packet is transmitted on the link.
		 *
		 * For unicast addresses, the packet is submitted to etharp_query(). In
		 * case the IP address is outside the local network, the IP address of
		 * the gateway is used.
		 *
		 * @param netif The lwIP network interface which the IP packet will be sent on.
		 * @param q The pbuf(s) containing the IP packet to be sent.
		 * @param ipaddr The IP address of the packet destination.
		 *
		 * @return
		 * - err_t.ERR_RTE No route to destination (no gateway to external networks),
		 * or the return type of either etharp_query() or etharp_send_ip().
		 */
		public err_t etharp_output(netif netif, pbuf q, ip_addr ipaddr)
		{
			eth_addr dest = new eth_addr(0, 0, 0, 0, 0, 0);
			eth_addr mcastaddr = new eth_addr(0, 0, 0, 0, 0, 0);
			ip_addr dst_addr = ipaddr;

			lwip.LWIP_ASSERT("netif != null", netif != null);
			lwip.LWIP_ASSERT("q != null", q != null);
			lwip.LWIP_ASSERT("ipaddr != null", ipaddr != null);

			/* make room for Ethernet header - should not fail */
			if (lwip.pbuf_header(q, SIZEOF_ETH_HDR) != 0) {
				/* bail out */
				lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE | lwip.LWIP_DBG_LEVEL_SERIOUS,
					("etharp_output: could not allocate room for header.\n"));
				//LINK_STATS_INC(link.lenerr);
				return err_t.ERR_BUF;
			}

			/* Determine on destination hardware address. Broadcasts and multicasts
			 * are special, other IP addresses are looked up in the ARP table. */

			/* broadcast destination IP address? */
			if (ip_addr.ip_addr_isbroadcast(ipaddr, netif)) {
				/* broadcast on Ethernet also */
				dest = ethbroadcast;
				/* multicast destination IP address? */
			}
			else if (ip_addr.ip_addr_ismulticast(ipaddr)) {
				/* Hash IP multicast address to MAC address.*/
				mcastaddr.addr[0] = LL_MULTICAST_ADDR_0;
				mcastaddr.addr[1] = LL_MULTICAST_ADDR_1;
				mcastaddr.addr[2] = LL_MULTICAST_ADDR_2;
				mcastaddr.addr[3] = (byte)(ip_addr.ip4_addr2(ipaddr) & 0x7f);
				mcastaddr.addr[4] = ip_addr.ip4_addr3(ipaddr);
				mcastaddr.addr[5] = ip_addr.ip4_addr4(ipaddr);
				/* destination Ethernet address is multicast */
				dest = mcastaddr;
				/* unicast destination IP address? */
			}
			else {
				sbyte i;
				/* outside local network? if so, this can neither be a global broadcast nor
				   a subnet broadcast. */
				if (!ip_addr.ip_addr_netcmp(ipaddr, netif.ip_addr, netif.netmask) &&
					!ip_addr.ip_addr_islinklocal(ipaddr)) {
#if LWIP_AUTOIP
					ip_hdr iphdr = new ip_hdr(q.payload,
					   SIZEOF_ETH_HDR);
					/* According to RFC 3297, chapter 2.6.2 (Forwarding Rules), a packet with
					   a link-local source address must always be "directly to its destination
					   on the same physical link. The host MUST NOT send the packet to any
					   router for forwarding". */
					if (!ip_addr.ip_addr_islinklocal(iphdr.src))
#endif // LWIP_AUTOIP
					{
						/* interface has default gateway? */
						if (!ip_addr.ip_addr_isany(netif.gw)) {
							/* send to hardware address of default gateway IP address */
							dst_addr = netif.gw;
							/* no default gateway available */
						}
						else {
							/* no route to destination error (default gateway missing) */
							return err_t.ERR_RTE;
						}
					}
				}
#if LWIP_NETIF_HWADDRHINT
				if (netif.addr_hint != null) {
					/* per-pcb cached entry was given */
					sbyte etharp_cached_entry = Convert.ToSByte(netif.addr_hint);
					if (etharp_cached_entry < opt.ARP_TABLE_SIZE) {
#endif // LWIP_NETIF_HWADDRHINT
						if ((arp_table[etharp_cached_entry].state >= ETHARP_STATE.STABLE) &&
							(ip_addr.ip_addr_cmp(dst_addr, arp_table[etharp_cached_entry].ipaddr))) {
							/* the per-pcb-cached entry is stable and the right one! */
							//ETHARP_STATS_INC(etharp.cachehit);
							return etharp_output_to_arp_index(netif, q, etharp_cached_entry);
						}
#if LWIP_NETIF_HWADDRHINT
					}
				}
#endif // LWIP_NETIF_HWADDRHINT

				/* find stable entry: do this here since this is a critical path for
				   throughput and etharp_find_entry() is kind of slow */
				for (i = 0; i < opt.ARP_TABLE_SIZE; i++) {
					if ((arp_table[i].state >= ETHARP_STATE.STABLE) &&
						(ip_addr.ip_addr_cmp(dst_addr, arp_table[i].ipaddr))) {
						/* found an existing, stable entry */
						//ETHARP_SET_HINT(netif, i);
						return etharp_output_to_arp_index(netif, q, i);
					}
				}
				/* no stable entry found, use the (slower) query function:
				   queue on destination Ethernet address belonging to ipaddr */
				return etharp_query(netif, dst_addr, q);
			}

			/* continuation for multicast/broadcast destinations */
			/* obtain source Ethernet address of the given interface */
			/* send packet directly on the link */
			return etharp_send_ip(netif, q, (eth_addr)(netif.hwaddr), dest);
		}

		/**
		 * Send an ARP request for the given IP address and/or queue a packet.
		 *
		 * If the IP address was not yet in the cache, a pending ARP cache entry
		 * is added and an ARP request is sent for the given address. The packet
		 * is queued on this entry.
		 *
		 * If the IP address was already pending in the cache, a new ARP request
		 * is sent for the given address. The packet is queued on this entry.
		 *
		 * If the IP address was already stable in the cache, and a packet is
		 * given, it is directly sent and no ARP request is sent out. 
		 * 
		 * If the IP address was already stable in the cache, and no packet is
		 * given, an ARP request is sent out.
		 * 
		 * @param netif The lwIP network interface on which ipaddr
		 * must be queried for.
		 * @param ipaddr The IP address to be resolved.
		 * @param q If non-null, a pbuf that must be delivered to the IP address.
		 * q is not freed by this function.
		 *
		 * @note q must only be ONE packet, not a packet queue!
		 *
		 * @return
		 * - err_t.ERR_BUF Could not make room for Ethernet header.
		 * - err_t.ERR_MEM Hardware address unknown, and no more ARP entries available
		 *   to query for address or queue the packet.
		 * - err_t.ERR_MEM Could not queue packet due to memory shortage.
		 * - err_t.ERR_RTE No route to destination (no gateway to external networks).
		 * - err_t.ERR_ARG Non-unicast address given, those will not appear in ARP cache.
		 *
		 */
		err_t
		etharp_query(netif netif, ip_addr ipaddr, pbuf q)
		{
			eth_addr srcaddr = (eth_addr)netif.hwaddr;
			err_t result = err_t.ERR_MEM;
			sbyte i; /* ARP entry index */

			/* non-unicast address? */
			if (ip_addr.ip_addr_isbroadcast(ipaddr, netif) ||
				ip_addr.ip_addr_ismulticast(ipaddr) ||
				ip_addr.ip_addr_isany(ipaddr)) {
				lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE, ("etharp_query: will not add non-unicast IP address to ARP cache\n"));
				return err_t.ERR_ARG;
			}

			/* find entry in ARP cache, ask to create entry if queueing packet */
			i = etharp_find_entry(ipaddr, ETHARP_FLAG_TRY_HARD);

			/* could not find or create entry? */
			if (i < 0) {
				lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE, ("etharp_query: could not create ARP entry\n"));
				if (q != null) {
					lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE, ("etharp_query: packet dropped\n"));
					//ETHARP_STATS_INC(etharp.memerr);
				}
				return (err_t)i;
			}

			/* mark a fresh entry as pending (we just sent a request) */
			if (arp_table[i].state == ETHARP_STATE.EMPTY) {
				arp_table[i].state = ETHARP_STATE.PENDING;
			}

			/* { i is either a STABLE or (new or existing) PENDING entry } */
			lwip.LWIP_ASSERT("arp_table[i].state == PENDING or STABLE",
			((arp_table[i].state == ETHARP_STATE.PENDING) ||
			 (arp_table[i].state >= ETHARP_STATE.STABLE)));

			/* do we have a pending entry? or an implicit query request? */
			if ((arp_table[i].state == ETHARP_STATE.PENDING) || (q == null)) {
				/* try to resolve it; send out ARP request */
				result = etharp_request(netif, ipaddr);
				if (result != err_t.ERR_OK) {
					/* ARP request couldn't be sent */
					/* We don't re-send arp request in etharp_tmr, but we still queue packets,
					   since this failure could be temporary, and the next packet calling
					   etharp_query again could lead to sending the queued packets. */
				}
				if (q == null) {
					return result;
				}
			}

			/* packet given? */
			lwip.LWIP_ASSERT("q != null", q != null);
			/* stable entry? */
			if (arp_table[i].state >= ETHARP_STATE.STABLE) {
				/* we have a valid IP.Ethernet address mapping */
				//ETHARP_SET_HINT(netif, i);
				/* send the packet */
				result = etharp_send_ip(netif, q, srcaddr, arp_table[i].ethaddr);
				/* pending entry? (either just created or already pending */
			}
			else if (arp_table[i].state == ETHARP_STATE.PENDING) {
				/* entry is still pending, queue the given packet 'q' */
				pbuf p;
				int copy_needed = 0;
				/* IF q includes a PBUF_REF, PBUF_POOL or PBUF_RAM, we have no choice but
				 * to copy the whole queue into a new PBUF_RAM (see bug #11400) 
				 * PBUF_ROMs can be left as they are, since ROM must not get changed. */
				p = q;
				while (p != null) {
					lwip.LWIP_ASSERT("no packet queues allowed!", (p.len != p.tot_len) || (p.next == null));
					if (p.type != pbuf_type.PBUF_ROM) {
						copy_needed = 1;
						break;
					}
					p = p.next;
				}
				if (copy_needed != 0) {
					/* copy the whole packet into new pbufs */
					p = lwip.pbuf_alloc(pbuf_layer.PBUF_RAW, p.tot_len, pbuf_type.PBUF_RAM);
					if (p != null) {
						if (lwip.pbuf_copy(p, q) != err_t.ERR_OK) {
							lwip.pbuf_free(p);
							p = null;
						}
					}
				}
				else {
					/* referencing the old pbuf is enough */
					p = q;
					lwip.pbuf_ref(p);
				}
				/* packet could be taken over? */
				if (p != null) {
					/* queue packet ... */
#if ARP_QUEUEING
					etharp_q_entry new_entry;
					/* allocate a new arp queue entry */
					new_entry = new etharp_q_entry(/*memp_malloc(memp_t.MEMP_ARP_QUEUE)*/);
					if (new_entry != null) {
						new_entry.next = null;
						new_entry.p = p;
						if (arp_table[i].q != null) {
							/* queue was already existent, append the new entry to the end */
							etharp_q_entry r;
							r = arp_table[i].q;
							while (r.next != null) {
								r = r.next;
							}
							r.next = new_entry;
						}
						else {
							/* queue did not exist, first item in queue */
							arp_table[i].q = new_entry;
						}
						LWIP_DEBUGF(opt.ETHARP_DEBUG | LWIP_DBG_TRACE, "etharp_query: queued packet {0} on ARP entry {1}\n", q, i);
						result = err_t.ERR_OK;
					}
					else {
						/* the pool MEMP_ARP_QUEUE is empty */
						pbuf_free(p);
						LWIP_DEBUGF(opt.ETHARP_DEBUG | LWIP_DBG_TRACE, "etharp_query: could not queue a copy of PBUF_REF packet {0} (out of memory)\n", q);
						result = err_t.ERR_MEM;
					}
#else // ARP_QUEUEING
					/* always queue one packet per ARP request only, freeing a previously queued packet */
					if (arp_table[i].q != null) {
						lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE, "etharp_query: dropped previously queued packet {0} for ARP entry {1}\n", q, i);
						lwip.pbuf_free(arp_table[i].q);
					}
					arp_table[i].q = p;
					result = err_t.ERR_OK;
					lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE, "etharp_query: queued packet {0} on ARP entry {1}\n", q, i);
#endif // ARP_QUEUEING
				}
				else {
					//ETHARP_STATS_INC(etharp.memerr);
					lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE, "etharp_query: could not queue a copy of PBUF_REF packet {0} (out of memory)\n", q);
					result = err_t.ERR_MEM;
				}
			}
			return result;
		}

		/**
		 * Send a raw ARP packet (opcode and all addresses can be modified)
		 *
		 * @param netif the lwip network interface on which to send the ARP packet
		 * @param ethsrc_addr the source MAC address for the ethernet header
		 * @param ethdst_addr the destination MAC address for the ethernet header
		 * @param hwsrc_addr the source MAC address for the ARP protocol header
		 * @param ipsrc_addr the source IP address for the ARP protocol header
		 * @param hwdst_addr the destination MAC address for the ARP protocol header
		 * @param ipdst_addr the destination IP address for the ARP protocol header
		 * @param opcode the type of the ARP packet
		 * @return err_t.ERR_OK if the ARP packet has been sent
		 *         err_t.ERR_MEM if the ARP packet couldn't be allocated
		 *         any other err_t on failure
		 */
#if !LWIP_AUTOIP
		private
#endif // LWIP_AUTOIP
		err_t etharp_raw(netif netif, eth_addr ethsrc_addr, eth_addr ethdst_addr, eth_addr hwsrc_addr,
				ip_addr ipsrc_addr, eth_addr hwdst_addr, ip_addr ipdst_addr, ushort opcode)
		{
			pbuf p;
			err_t result = err_t.ERR_OK;
			eth_hdr ethhdr;
			etharp_hdr hdr;
#if LWIP_AUTOIP
			byte[] ethdst_hwaddr;
#endif // LWIP_AUTOIP

			lwip.LWIP_ASSERT("netif != null", netif != null);

			/* allocate a pbuf for the outgoing ARP request packet */
			p = lwip.pbuf_alloc(pbuf_layer.PBUF_RAW, SIZEOF_ETHARP_PACKET, pbuf_type.PBUF_RAM);
			/* could allocate a pbuf for an ARP request? */
			if (p == null) {
				lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE | lwip.LWIP_DBG_LEVEL_SERIOUS,
				  ("etharp_raw: could not allocate pbuf for ARP request.\n"));
				//ETHARP_STATS_INC(etharp.memerr);
				return err_t.ERR_MEM;
			}
			lwip.LWIP_ASSERT("check that first pbuf can hold struct etharp_hdr",
						(p.len >= etharp_hdr.length));

			ethhdr = new eth_hdr(p.payload);
			hdr = new etharp_hdr(ethhdr, SIZEOF_ETH_HDR);
			lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE, ("etharp_raw: sending raw ARP packet.\n"));
			hdr.opcode = lwip.lwip_htons(opcode);

			lwip.LWIP_ASSERT("netif.hwaddr_len must be the same as ETHARP_HWADDR_LEN for etharp!",
						(netif.hwaddr_len == ETHARP_HWADDR_LEN));
#if LWIP_AUTOIP
			/* If we are using Link-Local, all ARP packets that contain a Link-Local
			 * 'sender IP address' MUST be sent using link-layer broadcast instead of
			 * link-layer unicast. (See RFC3927 Section 2.5, last paragraph) */
			ethdst_hwaddr = ip_addr.ip_addr_islinklocal(ipsrc_addr) ? ethbroadcast.addr : ethdst_addr.addr;
#endif // LWIP_AUTOIP
			/* Write the ARP MAC-Addresses */
			ETHADDR16_COPY(hdr.shwaddr, hwsrc_addr);
			ETHADDR16_COPY(hdr.dhwaddr, hwdst_addr);
			/* Write the Ethernet MAC-Addresses */
#if LWIP_AUTOIP
			ETHADDR16_COPY(ethhdr.dest, ethdst_hwaddr);
#else  // LWIP_AUTOIP
			ETHADDR16_COPY(ethhdr.dest, ethdst_addr);
#endif // LWIP_AUTOIP
			ETHADDR16_COPY(ethhdr.src, ethsrc_addr);
			/* Copy struct ip_addr2 to aligned ip_addr, to support compilers without
			 * structure packing. */
			ip_addr.IPADDR2_COPY(hdr.sipaddr, ipsrc_addr);
			ip_addr.IPADDR2_COPY(hdr.dipaddr, ipdst_addr);

			hdr.hwtype = lwip.PP_HTONS(HWTYPE_ETHERNET);
			hdr.proto = lwip.PP_HTONS(ETHTYPE_IP);
			/* set hwlen and protolen */
			hdr.hwlen = ETHARP_HWADDR_LEN;
			hdr.protolen = ip_addr.length;

			ethhdr.type = lwip.PP_HTONS(ETHTYPE_ARP);
			/* send ARP query */
			result = netif.linkoutput(netif, p);
			//ETHARP_STATS_INC(etharp.xmit);
			/* free ARP query packet */
			lwip.pbuf_free(p);
			p = null;
			/* could not allocate pbuf for ARP request */

			return result;
		}

		/** For Ethernet network interfaces, we might want to send "gratuitous ARP";
		 *  this is an ARP packet sent by a node in order to spontaneously cause other
		 *  nodes to update an entry in their ARP cache.
		 *  From RFC 3220 "IP Mobility Support for IPv4" section 4.6. */
		internal err_t etharp_gratuitous(netif netif)
		{
			return etharp_request(netif, netif.ip_addr);
		}

		/**
		 * Send an ARP request packet asking for ipaddr.
		 *
		 * @param netif the lwip network interface on which to send the request
		 * @param ipaddr the IP address for which to ask
		 * @return err_t.ERR_OK if the request has been sent
		 *         err_t.ERR_MEM if the ARP packet couldn't be allocated
		 *         any other err_t on failure
		 */
		err_t etharp_request(netif netif, ip_addr ipaddr)
		{
			lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE, "etharp_request: sending ARP request.\n");
			return etharp_raw(netif, (eth_addr)netif.hwaddr, ethbroadcast,
				(eth_addr)netif.hwaddr, netif.ip_addr, ethzero,
				ipaddr, ARP_REQUEST);
		}
#endif // LWIP_ARP

		/**
		 * Process received ethernet frames. Using this function instead of directly
		 * calling ip_input and passing ARP frames through etharp in ethernetif_input,
		 * the ARP cache is protected from concurrent access.
		 *
		 * @param p the recevied packet, p.payload pointing to the ethernet header
		 * @param netif the network interface on which the packet was received
		 */
		public err_t ethernet_input(pbuf p, netif netif)
		{
			eth_hdr ethhdr;
			ushort type;
#if LWIP_ARP || ETHARP_SUPPORT_VLAN
			short ip_hdr_offset = SIZEOF_ETH_HDR;
#endif // LWIP_ARP || ETHARP_SUPPORT_VLAN

			if (p.len <= SIZEOF_ETH_HDR) {
				/* a packet with only an ethernet header (or less) is not valid for us */
				//ETHARP_STATS_INC(etharp.proterr);
				//ETHARP_STATS_INC(etharp.drop);
				goto free_and_return;
			}

			/* points to packet payload, which starts with an Ethernet header */
			ethhdr = new eth_hdr(p.payload);
			lwip.LWIP_DEBUGF(opt.ETHARP_DEBUG | lwip.LWIP_DBG_TRACE,
				"ethernet_input: dest:{0:X}:{1:X}:{2:X}:{3:X}:{4:X}:{5:X}, src:{6:X}:{7:X}:{8:X}:{9:X}:{10:X}:{11:X}, type:{12:X}\n",
				ethhdr.dest.addr[0], ethhdr.dest.addr[1], ethhdr.dest.addr[2],
				ethhdr.dest.addr[3], ethhdr.dest.addr[4], ethhdr.dest.addr[5],
				ethhdr.src.addr[0], ethhdr.src.addr[1], ethhdr.src.addr[2],
				ethhdr.src.addr[3], ethhdr.src.addr[4], ethhdr.src.addr[5],
				lwip.lwip_htons(ethhdr.type));

			type = ethhdr.type;
#if ETHARP_SUPPORT_VLAN
			if (type == PP_HTONS(ETHTYPE_VLAN)) {
				eth_vlan_hdr vlan = new eth_vlan_hdr(ethhdr, SIZEOF_ETH_HDR);
				if (p.len <= SIZEOF_ETH_HDR + SIZEOF_VLAN_HDR) {
					/* a packet with only an ethernet/vlan header (or less) is not valid for us */
					//ETHARP_STATS_INC(etharp.proterr);
					//ETHARP_STATS_INC(etharp.drop);
					goto free_and_return;
				}
#if ETHARP_VLAN_CHECK || ETHARP_VLAN_CHECK_FN // if not, allow all VLANs
#if ETHARP_VLAN_CHECK_FN
				if (!ETHARP_VLAN_CHECK_FN(ethhdr, vlan)) {
#elif ETHARP_VLAN_CHECK
				if (VLAN_ID(vlan) != ETHARP_VLAN_CHECK) {
#endif
					/* silently ignore this packet: not for our VLAN */
					pbuf_free(p);
					return err_t.ERR_OK;
				}
#endif // defined(ETHARP_VLAN_CHECK) || defined(ETHARP_VLAN_CHECK_FN)
				type = vlan.tpid;
				ip_hdr_offset = SIZEOF_ETH_HDR + SIZEOF_VLAN_HDR;
			}
#endif // ETHARP_SUPPORT_VLAN

#if LWIP_ARP_FILTER_NETIF
			netif = LWIP_ARP_FILTER_NETIF_FN(p, netif, htons(type));
#endif // LWIP_ARP_FILTER_NETIF

			if ((ethhdr.dest.addr[0] & 1) != 0) {
				/* this might be a multicast or broadcast packet */
				if (ethhdr.dest.addr[0] == LL_MULTICAST_ADDR_0) {
					if ((ethhdr.dest.addr[1] == LL_MULTICAST_ADDR_1) &&
						(ethhdr.dest.addr[2] == LL_MULTICAST_ADDR_2)) {
						/* mark the pbuf as link-layer multicast */
						p.flags |= pbuf.PBUF_FLAG_LLMCAST;
					}
				}
				else if (eth_addr_cmp(ethhdr.dest, ethbroadcast)) {
					/* mark the pbuf as link-layer broadcast */
					p.flags |= pbuf.PBUF_FLAG_LLBCAST;
				}
			}

			switch (lwip.lwip_htons(type)) {
#if LWIP_ARP
			/* IP packet? */
			case ETHTYPE_IP:
				if ((netif.flags & netif.NETIF_FLAG_ETHARP) != 0) {
					goto free_and_return;
				}
#if ETHARP_TRUST_IP_MAC
				/* update ARP table */
				etharp_ip_input(netif, p);
#endif // ETHARP_TRUST_IP_MAC
				/* skip Ethernet header */
				if (lwip.pbuf_header(p, (short)-ip_hdr_offset) != 0) {
					lwip.LWIP_ASSERT("Can't move over header in packet", false);
					goto free_and_return;
				}
				else {
					/* pass to IP layer */
					lwip.ip.ip_input(p, netif);
				}
				break;

			case ETHTYPE_ARP:
				if ((netif.flags & netif.NETIF_FLAG_ETHARP) != 0) {
					goto free_and_return;
				}
				/* pass p to ARP module */
				etharp_arp_input(netif, (eth_addr)(netif.hwaddr), p);
				break;
#endif // LWIP_ARP
#if PPPOE_SUPPORT
			case ETHTYPE_PPPOEDISC: /* PPP Over Ethernet Discovery Stage */
				pppoe_disc_input(netif, p);
				break;

			case ETHTYPE_PPPOE: /* PPP Over Ethernet Session Stage */
				pppoe_data_input(netif, p);
				break;
#endif // PPPOE_SUPPORT

			default:
				//ETHARP_STATS_INC(etharp.proterr);
				//ETHARP_STATS_INC(etharp.drop);
				goto free_and_return;
			}

			/* This means the pbuf is freed or consumed,
			   so the caller doesn't have to free it again */
			return err_t.ERR_OK;

		free_and_return:
			lwip.pbuf_free(p);
			return err_t.ERR_OK;
		}

		private void snmp_insert_arpidx_tree(netif netif, ip_addr ipaddr)
		{
		}

		private void snmp_delete_arpidx_tree(netif netif, ip_addr ipaddr)
		{
		}
	}
#endif // LWIP_ARP || LWIP_ETHERNET
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) Microsoft Corporation.  All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Portions Copyright (c) CSA Engineering AG */

/* [CSA] Support for custom memory profile added */

#ifndef _NET_DECL_LWIP_H_
#define _NET_DECL_LWIP_H_

#ifndef TCPIP_LWIP
#include <lwip_selector.h>
#endif 

#ifndef TCPIP_LWIP
#error Only include net_decl_lwip.h when using LWIP stack
#endif

#include "network_defines_lwip.h"

#define NO_SYS 0
#define ERRNO  1


#ifdef PLATFORM_DEPENDENT__MEM_SIZE
#define MEM_SIZE PLATFORM_DEPENDENT__MEM_SIZE
#else
#define MEM_SIZE MEM_SIZE__default
#endif

#ifdef PLATFORM_DEPENDENT__MEMP_NUM_PBUF
#define MEMP_NUM_PBUF PLATFORM_DEPENDENT__MEMP_NUM_PBUF
#else
#define MEMP_NUM_PBUF MEMP_NUM_PBUF__default
#endif

#ifdef PLATFORM_DEPENDENT__MEMP_NUM_UDP_PCB
#define MEMP_NUM_UDP_PCB PLATFORM_DEPENDENT__MEMP_NUM_UDP_PCB
#else
#define MEMP_NUM_UDP_PCB MEMP_NUM_UDP_PCB__default
#endif

#ifdef PLATFORM_DEPENDENT__MEMP_NUM_TCP_PCB
#define MEMP_NUM_TCP_PCB PLATFORM_DEPENDENT__MEMP_NUM_TCP_PCB
#else
#define MEMP_NUM_TCP_PCB MEMP_NUM_TCP_PCB__default
#endif

#ifdef PLATFORM_DEPENDENT__MEMP_NUM_TCP_PCB_LISTEN
#define MEMP_NUM_TCP_PCB_LISTEN PLATFORM_DEPENDENT__MEMP_NUM_TCP_PCB_LISTEN
#else
#define MEMP_NUM_TCP_PCB_LISTEN MEMP_NUM_TCP_PCB_LISTEN__default
#endif

#ifdef PLATFORM_DEPENDENT__MEMP_NUM_TCP_SEG
#define MEMP_NUM_TCP_SEG PLATFORM_DEPENDENT__MEMP_NUM_TCP_SEG
#else
#define MEMP_NUM_TCP_SEG MEMP_NUM_TCP_SEG__default
#endif

#ifdef PLATFORM_DEPENDENT__MEMP_NUM_SYS_TIMEOUT
#define MEMP_NUM_SYS_TIMEOUT PLATFORM_DEPENDENT__MEMP_NUM_SYS_TIMEOUT
#else
#define MEMP_NUM_SYS_TIMEOUT MEMP_NUM_SYS_TIMEOUT__default
#endif

#ifdef PLATFORM_DEPENDENT__MEMP_NUM_NETBUF
#define MEMP_NUM_NETBUF PLATFORM_DEPENDENT__MEMP_NUM_NETBUF
#else
#define MEMP_NUM_NETBUF MEMP_NUM_NETBUF__default
#endif

#ifdef PLATFORM_DEPENDENT__MEMP_NUM_NETCONN
#define MEMP_NUM_NETCONN PLATFORM_DEPENDENT__MEMP_NUM_NETCONN
#else
#define MEMP_NUM_NETCONN MEMP_NUM_NETCONN__default
#endif

#ifdef PLATFORM_DEPENDENT__PBUF_POOL_SIZE
#define PBUF_POOL_SIZE PLATFORM_DEPENDENT__PBUF_POOL_SIZE
#else
#define PBUF_POOL_SIZE PBUF_POOL_SIZE__default
#endif

#ifdef PLATFORM_DEPENDENT__PBUF_POOL_BUFSIZE
#define PBUF_POOL_BUFSIZE PLATFORM_DEPENDENT__PBUF_POOL_BUFSIZE
#else
#define PBUF_POOL_BUFSIZE PBUF_POOL_BUFSIZE__default
#endif

#ifdef PLATFORM_DEPENDENT__TCP_MSS
#define TCP_MSS PLATFORM_DEPENDENT__TCP_MSS
#else
#define TCP_MSS TCP_MSS__default
#endif

#ifdef PLATFORM_DEPENDENT__TCP_SND_BUF
#define TCP_SND_BUF PLATFORM_DEPENDENT__TCP_SND_BUF
#else
#define TCP_SND_BUF TCP_SND_BUF__default
#endif

#ifdef PLATFORM_DEPENDENT__TCP_SND_QUEUELEN
#define TCP_SND_QUEUELEN PLATFORM_DEPENDENT__TCP_SND_QUEUELEN
#else
#define TCP_SND_QUEUELEN TCP_SND_QUEUELEN__default
#endif

#ifdef PLATFORM_DEPENDENT__TCP_WND
#define TCP_WND PLATFORM_DEPENDENT__TCP_WND
#else
#define TCP_WND TCP_WND__default
#endif

#ifdef PLATFORM_DEPENDENT__TCP_SNDLOWAT
#define TCP_SNDLOWAT PLATFORM_DEPENDENT__TCP_SNDLOWAT
#else
#define TCP_SNDLOWAT TCP_SNDLOWAT__default
#endif

//--//

/* LWIP options that are the same for all configurations */

/* MEM_ALIGNMENT: should be set to the alignment of the CPU for which
   lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
   byte alignment -> define MEM_ALIGNMENT to 2. */
#ifdef MEM_ALIGNMENT__custom
#define MEM_ALIGNMENT           MEM_ALIGNMENT__custom
#else
#define MEM_ALIGNMENT           4
#endif

/* These two control whether reclaimer functions should be compiled
   in. Should always be turned on (1). */
#ifdef MEM_RECLAIM__custom
#define MEM_RECLAIM             MEM_RECLAIM__custom
#else
#define MEM_RECLAIM             1
#endif

#ifdef MEMP_RECLAIM__custom
#define MEMP_RECLAIM            MEMP_RECLAIM__custom
#else
#define MEMP_RECLAIM            1
#endif

/* PBUF_LINK_HLEN: the number of bytes that should be allocated for a
   link level header. */
   
#ifdef PBUF_LINK_HLEN__custom
#define PBUF_LINK_HLEN          PBUF_LINK_HLEN__custom
#else
#define PBUF_LINK_HLEN          16
#endif

/* ---------- TCP options ---------- */
#ifdef LWIP_TCP__custom
#define LWIP_TCP                LWIP_TCP__custom
#else
#define LWIP_TCP                1
#endif

#ifdef TCP_TTL__custom
#define TCP_TTL                 TCP_TTL__custom
#else
#define TCP_TTL                 255
#endif

/* Controls if TCP should queue segments that arrive out of
   order. Define to 0 if your device is low on memory. */
#ifdef TCP_QUEUE_OOSEQ__custom
#define TCP_QUEUE_OOSEQ         TCP_QUEUE_OOSEQ__custom
#else
#define TCP_QUEUE_OOSEQ         1
#endif

/* Maximum number of retransmissions of data segments. */
#ifdef TCP_MAXRTX__custom
#define TCP_MAXRTX              TCP_MAXRTX__custom
#else
#define TCP_MAXRTX              12
#endif

/* Maximum number of retransmissions of SYN segments. */
#ifdef TCP_SYNMAXRTX__custom
#define TCP_SYNMAXRTX           TCP_SYNMAXRTX__custom
#else
#define TCP_SYNMAXRTX           4
#endif

/* ---------- ARP options ---------- */
#ifdef ARP_TABLE_SIZE__custom
#define ARP_TABLE_SIZE          ARP_TABLE_SIZE__custom
#else
#define ARP_TABLE_SIZE          10
#endif

#ifdef ARP_QUEUEING__custom
#define ARP_QUEUEING            ARP_QUEUEING__custom
#else
#define ARP_QUEUEING            1
#endif

/* ---------- IP options ---------- */
/* Define IP_FORWARD to 1 if you wish to have the ability to forward
   IP packets across network interfaces. If you are going to run lwIP
   on a device with only one network interface, define this to 0. */
#ifdef IP_FORWARD__custom
#define IP_FORWARD              IP_FORWARD__custom
#else
#define IP_FORWARD              1
#endif

/* If defined to 1, IP options are allowed (but not parsed). If
   defined to 0, all packets with IP options are dropped. */
#ifdef IP_OPTIONS__custom
#define IP_OPTIONS              IP_OPTIONS__custom
#else
#define IP_OPTIONS              1
#endif

/* IP reassembly and segmentation.These are orthogonal even
 * if they both deal with IP fragments */
#ifdef IP_REASSEMBLY__custom
#define IP_REASSEMBLY           IP_REASSEMBLY__custom
#else
#define IP_REASSEMBLY           1
#endif

#ifdef IP_FRAG__custom
#define IP_FRAG                 IP_FRAG__custom
#else
#define IP_FRAG                 1
#endif

/* ---------- ICMP options ---------- */
#ifdef ICMP_TTL__custom
#define ICMP_TTL                ICMP_TTL__custom
#else
#define ICMP_TTL                255
#endif

/* ---------- DHCP options ---------- */
/* Define LWIP_DHCP to 1 if you want DHCP configuration of
   interfaces. */
#ifdef LWIP_DHCP__custom
#define LWIP_DHCP               LWIP_DHCP__custom
#else
#define LWIP_DHCP               1
#endif

/* 1 if you want to do an ARP check on the offered address
   (recommended). */
#ifdef DHCP_DOES_ARP_CHECK__custom
#define DHCP_DOES_ARP_CHECK     DHCP_DOES_ARP_CHECK__custom
#else
#define DHCP_DOES_ARP_CHECK     0
#endif

/* ---------- UDP options ---------- */
#ifdef LWIP_UDP__custom
#define LWIP_UDP                LWIP_UDP__custom
#else
#define LWIP_UDP                1
#endif

#ifdef UDP_TTL__custom
#define UDP_TTL                 UDP_TTL__custom
#else
#define UDP_TTL                 255
#endif

/* ---------- Statistics options ---------- */
#ifdef LWIP_STATS__custom
#define LWIP_STATS              LWIP_STATS__custom
#else
#define LWIP_STATS              0
#endif

/** SYS_LIGHTWEIGHT_PROT
 * define SYS_LIGHTWEIGHT_PROT in lwipopts.h if you want inter-task
 * protection for certain critical regions during buffer allocation
 * and deallocation and memory allocation and deallocation.
 */
#ifdef SYS_LIGHTWEIGHT_PROT__custom
#define SYS_LIGHTWEIGHT_PROT    SYS_LIGHTWEIGHT_PROT__custom
#else
#define SYS_LIGHTWEIGHT_PROT    1
#endif

#ifdef LWIP_COMPAT_SOCKETS__custom
#define LWIP_COMPAT_SOCKETS     LWIP_COMPAT_SOCKETS__custom
#else
#define LWIP_COMPAT_SOCKETS     1
#endif

#ifdef LWIP_PROVIDE_ERRNO__custom
#define LWIP_PROVIDE_ERRNO      LWIP_PROVIDE_ERRNO__custom
#else
#define LWIP_PROVIDE_ERRNO      1
#endif

/* ---------- SNMP options ---------- */
#ifdef LWIP_SNMP__custom
#define LWIP_SNMP               LWIP_SNMP__custom
#else
#define LWIP_SNMP               0 /*LwIP 1.2.0*/
#endif

#ifdef LWIP_IGMP__custom
#define LWIP_IGMP               LWIP_IGMP__custom
#else
#define LWIP_IGMP               1 /*LwIP 1.2.0*/
#endif

// thread priorities are in VDK terms - 1 is highest, 30 is lowest
#ifdef TCPIP_THREAD_PRIO__custom
#define TCPIP_THREAD_PRIO       TCPIP_THREAD_PRIO__custom
#else
#define TCPIP_THREAD_PRIO       5
#endif

#ifdef DEFAULT_THREAD_PRIO__custom
#define DEFAULT_THREAD_PRIO     DEFAULT_THREAD_PRIO__custom
#else
#define DEFAULT_THREAD_PRIO     10
#endif

#ifdef LOW_THREAD_PRIO__custom
#define LOW_THREAD_PRIO         LOW_THREAD_PRIO__custom
#else
#define LOW_THREAD_PRIO         29
#endif

#ifdef OPENSSL_SMALL_FOOTPRINT_custom
#define OPENSSL_SMALL_FOOTPRINT OPENSSL_SMALL_FOOTPRINT_custom
#else
#define OPENSSL_SMALL_FOOTPRINT 1
#endif

//--// RAM size estimate macro

#if 0 // TODO - implement similar for LWIP
#define NETWORK_RAM_SIZE_ESTIMATE() (\
    _NETWORK_SIZEOF_NONE + \
    NETWORK_MULTICAST_LIST_SIZE        * (_NETWORK_SIZEOF_MCLISTSIZE) + \
    NETWORK_PACKET_POOL_0__NUM_PACKETS * (NETWORK_PACKET_POOL_0__PACKET_SIZE + _NETWORK_SIZEOF_DCU) + \
    NETWORK_PACKET_POOL_1__NUM_PACKETS * (NETWORK_PACKET_POOL_1__PACKET_SIZE + _NETWORK_SIZEOF_DCU) + \
    NETWORK_PACKET_POOL_2__NUM_PACKETS * (NETWORK_PACKET_POOL_2__PACKET_SIZE + _NETWORK_SIZEOF_DCU) + \
    NETWORK_PACKET_POOL_3__NUM_PACKETS * (NETWORK_PACKET_POOL_3__PACKET_SIZE + _NETWORK_SIZEOF_DCU) + \
    NETWORK_PACKET_POOL_4__NUM_PACKETS * (NETWORK_PACKET_POOL_4__PACKET_SIZE + _NETWORK_SIZEOF_DCU) + \
    NETWORK_PACKET_POOL_5__NUM_PACKETS * (NETWORK_PACKET_POOL_5__PACKET_SIZE + _NETWORK_SIZEOF_DCU) + \
    _NETWORK_TOTAL_PACKET_COUNT        * _NETWORK_SIZEOF_PACKET_OVERHEAD     + \
    NETWORK_NUM_IFACES                 * _NETWORK_SIZEOF_IFACE        + \
    NETWORK_ROUTINGTABLE_SIZE          * _NETWORK_SIZEOF_ROUTINGTABLE + \
    NETWORK_ARP_NUM_TABLE_ENTRIES      * _NETWORK_SIZEOF_ARP_ENTRY    + \
    NETWORK_TCP_NUM_PORTS__SUPPORTED   * _NETWORK_SIZEOF_TCPPORT      + \
    NETWORK_UDP_NUM_PORTS__SUPPORTED   * _NETWORK_SIZEOF_UDPPORT      + \
    NETWORK_FRAG_TABLE_SIZE            * _NETWORK_SIZEOF_FRAG_ENTRY   + \
    NETWORK_NAT_NUM_ENTRIES            * _NETWORK_SIZEOF_NAT_ENTRY )    \

typedef char NETWORK_COMPILE_TIME_ASSERT[NETWORK_MEMORY_POOL__SIZE - NETWORK_RAM_SIZE_ESTIMATE()];
#endif

#endif //_NET_DECL_LWIP_H_


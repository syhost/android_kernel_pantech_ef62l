/*
 * INET		An implementation of the TCP/IP protocol suite for the LINUX
 *		operating system.  INET is implemented using the  BSD Socket
 *		interface as the means of communication with the user level.
 *
 *		Definitions for the "ping" module.
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 */
#ifndef _PING_H
#define _PING_H

#ifdef CONFIG_LGU_DS_CTS_IPV6_PING_TEST_FAIL_FIX
#include <net/icmp.h>
#endif
#include <net/netns/hash.h>

/* PING_HTABLE_SIZE must be power of 2 */
#define PING_HTABLE_SIZE 	64
#define PING_HTABLE_MASK 	(PING_HTABLE_SIZE-1)

#define ping_portaddr_for_each_entry(__sk, node, list) \
	hlist_nulls_for_each_entry(__sk, node, list, sk_nulls_node)

/*
 * gid_t is either uint or ushort.  We want to pass it to
 * proc_dointvec_minmax(), so it must not be larger than MAX_INT
 */
#define GID_T_MAX (((gid_t)~0U) >> 1)

#ifdef CONFIG_LGU_DS_CTS_IPV6_PING_TEST_FAIL_FIX
/* Compatibility glue so we can support IPv6 when it's compiled as a module */
struct pingv6_ops {
	int (*ipv6_recv_error)(struct sock *sk, struct msghdr *msg, int len);
	int (*datagram_recv_ctl)(struct sock *sk, struct msghdr *msg,
				 struct sk_buff *skb);
	int (*icmpv6_err_convert)(u8 type, u8 code, int *err);
	void (*ipv6_icmp_error)(struct sock *sk, struct sk_buff *skb, int err,
				__be16 port, u32 info, u8 *payload);
	int (*ipv6_chk_addr)(struct net *net, const struct in6_addr *addr,
			     struct net_device *dev, int strict);
};
#endif

struct ping_table {
	struct hlist_nulls_head	hash[PING_HTABLE_SIZE];
	rwlock_t		lock;
};

struct ping_iter_state {
	struct seq_net_private  p;
	int			bucket;
};

extern struct proto ping_prot;
#ifdef CONFIG_LGU_DS_CTS_IPV6_PING_TEST_FAIL_FIX
extern struct ping_table ping_table;
#if IS_ENABLED(CONFIG_IPV6)
extern struct pingv6_ops pingv6_ops;
#endif

struct pingfakehdr {
	struct icmphdr icmph;
	struct iovec *iov;
	sa_family_t family;
	__wsum wcheck;
};

int  ping_get_port(struct sock *sk, unsigned short ident);
void ping_hash(struct sock *sk);
void ping_unhash(struct sock *sk);

int  ping_init_sock(struct sock *sk);
void ping_close(struct sock *sk, long timeout);
int  ping_bind(struct sock *sk, struct sockaddr *uaddr, int addr_len);
void ping_err(struct sk_buff *skb, int offset, u32 info);
void ping_v4_err(struct sk_buff *skb, u32 info);
int  ping_getfrag(void *from, char *to, int offset, int fraglen, int odd,
		  struct sk_buff *);

int  ping_recvmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
		  size_t len, int noblock, int flags, int *addr_len);
int  ping_common_sendmsg(int family, struct msghdr *msg, size_t len,
			 void *user_icmph, size_t icmph_len);
int  ping_v4_sendmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
		     size_t len);
int  ping_v6_sendmsg(struct kiocb *iocb, struct sock *sk, struct msghdr *msg,
		     size_t len);
int  ping_queue_rcv_skb(struct sock *sk, struct sk_buff *skb);
void ping_rcv(struct sk_buff *skb);
#else
extern void ping_rcv(struct sk_buff *);
extern void ping_err(struct sk_buff *, u32 info);
#endif

#ifdef CONFIG_PROC_FS
extern int __init ping_proc_init(void);
extern void ping_proc_exit(void);
#endif

void __init ping_init(void);
#ifdef CONFIG_LGU_DS_CTS_IPV6_PING_TEST_FAIL_FIX
int  __init pingv6_init(void);
void pingv6_exit(void);
#endif

#endif /* _PING_H */

/*
 * Interface functions.
 * Copyright (C) 1997, 98 Kunihiro Ishiguro
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2, or (at your
 * option) any later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <lib/zebra.h>

#include "lib/linklist.h"
#include "lib/vector.h"
//#include "lib/vty.h"
//#include "lib/command.h"
#include "lib/if.h"
//#include "lib/sockunion.h"
#include "lib/prefix.h"
//#include "zebra/connected.h"
#include "lib/memory.h"
#include "lib/table.h"
//#include "lib/buffer.h"
//#include "lib/str.h"
#include "lib/log.h"
#include "netinet/in.h"
/* Master list of interfaces. */
struct list *iflist;

/* One for each program.  This structure is needed to store hooks. */
struct if_master
{
  int (*if_new_hook) (struct interface *);
  int (*if_delete_hook) (struct interface *);
} if_master;

/* Create new interface structure. */
struct interface *
if_new ()
{
  struct interface *ifp;

  ifp = XMALLOC (MTYPE_IF, sizeof (struct interface));
  memset (ifp, 0, sizeof (struct interface));
  return ifp;
}

struct interface *
if_create ()
{
  struct interface *ifp;

  ifp = if_new ();

  listnode_add (iflist, ifp);
  ifp->connected = list_new ();
  ifp->connected->del = (void (*) (void *)) connected_free;
	ifp->connected->cmp = connected_same;
  if (if_master.if_new_hook)
    (*if_master.if_new_hook) (ifp);

  return ifp;
}

/* Delete and free interface structure. */
void
if_delete (struct interface *ifp)
{
  listnode_delete (iflist, ifp);

  if (if_master.if_delete_hook)
    (*if_master.if_delete_hook) (ifp);

  /* Free connected address list */
  list_delete (ifp->connected);

  XFREE (MTYPE_IF, ifp);
}

/* Add hook to interface master. */
void
if_add_hook (int type, int (*func)(struct interface *ifp))
{
  switch (type) {
  case IF_NEW_HOOK:
    if_master.if_new_hook = func;
    break;
  case IF_DELETE_HOOK:
    if_master.if_delete_hook = func;
    break;
  default:
    break;
  }
}

/* Interface existance check by index. */
struct interface *
if_lookup_by_index (unsigned int index)
{
  listnode node;
  struct interface *ifp;

  for (node = listhead (iflist); node; nextnode (node))
    {
      ifp = getdata (node);
      if (ifp->ifindex == index)
	return ifp;
    }
  return NULL;
}

char *
ifindex2ifname (unsigned int index)
{
  listnode node;
  struct interface *ifp;

  for (node = listhead (iflist); node; nextnode (node))
    {
      ifp = getdata (node);
      if (ifp->ifindex == index)
	return ifp->name;
    }
  return "unknown";
}

/* Interface existance check by interface name. */
struct interface *
if_lookup_by_name (char *name)
{
  listnode node;
  struct interface *ifp;
  if(listhead(iflist)== NULL)return NULL;
  for (node = listhead (iflist); node; nextnode (node))
    {
      ifp = getdata (node);
      if (strncmp (name, ifp->name, sizeof ifp->name) == 0)
	return ifp;
    }
  return NULL;
}



/* Lookup interface by IPv4 address. */
struct interface *
if_lookup_exact_address (struct in_addr src)
{
  listnode node;
  listnode cnode;
  struct interface *ifp;
  struct prefix *p;
  struct connected *c;

  for (node = listhead (iflist); node; nextnode (node))
    {
      ifp = getdata (node);

      for (cnode = listhead (ifp->connected); cnode; nextnode (cnode))
	{
	  c = getdata (cnode);

	  p = c->address;

	  if (p && p->family == AF_INET)
	    {
	      if (IPV4_ADDR_SAME (&p->u.prefix4, &src))
		return ifp;
	    }
	}
    }
  return NULL;
}

/* Lookup interface by IPv4 address. */
struct interface *
if_lookup_address (struct in_addr src)
{
  listnode node;
  struct prefix addr;
  struct prefix best;
  listnode cnode;
  struct interface *ifp;
  struct prefix *p;
  struct connected *c;
  struct interface *match;

  /* Zero structures - get rid of rubbish from stack */
  memset(&addr, 0, sizeof(addr));
  memset(&best, 0, sizeof(best));

  addr.family = AF_INET;
  addr.u.prefix4 = src;
  addr.prefixlen = IPV4_MAX_BITLEN;

  match = NULL;

  for (node = listhead (iflist); node; nextnode (node))
    {
      ifp = getdata (node);

      for (cnode = listhead (ifp->connected); cnode; nextnode (cnode))
	{
	  c = getdata (cnode);

	  if (if_is_pointopoint (ifp))
	    {
	      p = c->address;

	      if (p && p->family == AF_INET)
		{
#ifdef OLD_RIB	 /* PTP  links are conventionally identified
		     by the address of the far end - MAG */
		  if (IPV4_ADDR_SAME (&p->u.prefix4, &src))
		    return ifp;
#endif
		  p = c->destination;
		  if (p && IPV4_ADDR_SAME (&p->u.prefix4, &src))
		    return ifp;
		}
	    }
	  else
	    {
	      p = c->address;

	      if (p->family == AF_INET)
		{
		  if (prefix_match (p, &addr) && p->prefixlen > best.prefixlen)
		    {
		      best = *p;
		      match = ifp;
		    }
		}
	    }
	}
    }
  return match;
}

/* Get interface by name if given name interface doesn't exist create
   one. */
struct interface *
if_get_by_name (char *name)
{
  struct interface *ifp;

  ifp = if_lookup_by_name (name);
  if (ifp == NULL)
    {
      ifp = if_create ();
      strncpy (ifp->name, name, IFNAMSIZ);
    }
  return ifp;
}

/* Does interface up ? */
int
if_is_up (struct interface *ifp)
{
  return ifp->flags & IFF_UP;
}

/* Is this loopback interface ? */
int
if_is_loopback (struct interface *ifp)
{
  return ifp->flags & IFF_LOOPBACK;
}

/* Does this interface support broadcast ? */
int
if_is_broadcast (struct interface *ifp)
{
  return ifp->flags & IFF_BROADCAST;
}

/* Does this interface support broadcast ? */
int
if_is_pointopoint (struct interface *ifp)
{
  return ifp->flags & IFF_POINTOPOINT;
}

/* Does this interface support multicast ? */
int
if_is_multicast (struct interface *ifp)
{
  return ifp->flags & IFF_MULTICAST;
}



/* For debugging

/* Interface printing for all interface.


/* Allocate connected structure. */
struct connected *
connected_new ()
{
  struct connected *new = XMALLOC (MTYPE_CONNECTED, sizeof (struct connected));
  memset (new, 0, sizeof (struct connected));
  return new;
}



void connected_add (struct interface *ifp, struct connected *ip)
{
	struct list* con = ifp->connected;
	struct listnode* n= con->head;
	int flag = 0;
	for(n;n!=NULL;n=n->next)
  	{
  		if(con->cmp == 0) 
			continue;
		else flag = 1;
	}
	if(flag==0)
	listnode_add(con,ip);
	else free(ip);
}

/* Free connected structure. */
void
connected_free (struct connected *connected)
{
  if (connected->address)
    prefix_free (connected->address);

  if (connected->destination)
    prefix_free (connected->destination);

  if (connected->label)
    free (connected->label);

  XFREE (MTYPE_CONNECTED, connected);
}

///* Print if_addr structure. */
//void
//connected_log (struct connected *connected, char *str)
//{
//  struct prefix *p;
//  struct interface *ifp;
//  char logbuf[BUFSIZ];
//  char buf[BUFSIZ];
//
//  ifp = connected->ifp;
//  p = connected->address;
//
//  snprintf (logbuf, BUFSIZ, "%s interface %s %s %s/%d ",
//	    str, ifp->name, prefix_family_str (p),
//	    inet_ntop (p->family, &p->u.prefix, buf, BUFSIZ),
//	    p->prefixlen);
//
//  p = connected->destination;
//  if (p)
//    {
//      strncat (logbuf, inet_ntop (p->family, &p->u.prefix, buf, BUFSIZ),
//	       BUFSIZ - strlen(logbuf));
//    }
//  zlog (NULL, LOG_INFO, logbuf);
//}

int connected_same(struct connected* c1,struct connected* c2){
	return connected_same_prefix(c1->address,c2->address);
}



/* If two connected address has same prefix return 1. */


int
connected_same_prefix (struct prefix *p1, struct prefix *p2)
{
  if (p1->family == p2->family)
    {
      if (p1->family == AF_INET &&
	  IPV4_ADDR_SAME (&p1->u.prefix4, &p2->u.prefix4))
	return 1;
#ifdef HAVE_IPV6
      if (p1->family == AF_INET6 &&
	  IPV6_ADDR_SAME (&p1->u.prefix6, &p2->u.prefix6))
	return 1;
#endif /* HAVE_IPV6 */
    }
  return 0;
}

struct connected *
connected_delete_by_prefix (struct interface *ifp, struct prefix *p)
{
  struct listnode *node;
  struct listnode *next;
  struct connected *ifc;

  /* In case of same prefix come, replace it with new one. */
  for (node = listhead (ifp->connected); node; node = next)
    {
      ifc = getdata (node);
      next = node->next;

      if (connected_same_prefix (ifc->address, p))
	{
	  listnode_delete (ifp->connected, ifc);
	  return ifc;
	}
    }
  return NULL;
}

/* Check the connected information is PtP style or not.  */
int
ifc_pointopoint (struct connected *ifc)
{
  struct prefix *p;
  int ptp = 0;

  /* When interface has PtP flag.  */
  if (if_is_pointopoint (ifc->ifp))
    return 1;

  /* RFC3021 PtP check.  */
  p = ifc->address;

  if (p->family == AF_INET)
    ptp = (p->prefixlen >= IPV4_MAX_PREFIXLEN - 1);
#ifdef HAVE_IPV6
  if (p->family == AF_INET6)
    ptp = (p->prefixlen >= IPV6_MAX_PREFIXLEN - 1);
#endif /* HAVE_IPV6 */

  return ptp;
}

#ifndef HAVE_IF_NAMETOINDEX
unsigned int
if_nametoindex (const char *name)
{
  listnode node;
  struct interface *ifp;

  for (node = listhead (iflist); node; nextnode (node))
    {
      ifp = getdata (node);
      if (strcmp (ifp->name, name) == 0)
	return ifp->ifindex;
    }
  return 0;
}
#endif

#ifndef HAVE_IF_INDEXTONAME
char *
if_indextoname (unsigned int ifindex, char *name)
{
  listnode node;
  struct interface *ifp;

  for (node = listhead (iflist); node; nextnode (node))
    {
      ifp = getdata (node);
      if (ifp->ifindex == ifindex)
	{
	  memcpy (name, ifp->name, IFNAMSIZ);
	  return ifp->name;
	}
    }
  return NULL;
}
#endif

/* Interface looking up by interface's address. */

/* Interface's IPv4 address reverse lookup table. */
struct route_table *ifaddr_ipv4_table;
/* struct route_table *ifaddr_ipv6_table; */

/* Initialize interface list. */
void
if_init ()
{
  iflist = list_new ();
  ifaddr_ipv4_table = route_table_init ();

  if (iflist)
    return;

  memset (&if_master, 0, sizeof if_master);
}



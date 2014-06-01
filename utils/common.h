#ifndef	_PUBLIC_INCLUDE
#define	_PUBLIC_INCLUDE
#ifdef HAVE_CONFIG_H 
#include <config.h>
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>           /* waitpid()                    */
#include <sys/socket.h>
#include <arpa/inet.h>          /* inet_addr()                  */
#include <netinet/in.h>         /* INADDR_NONE                  */
#include <netinet/ip.h>         /* INADDR_NONE                  */
#include <netinet/ip6.h>
#include <netdb.h>              /* gethostbyname                */
//#include <net/if.h>             /* get_my_addresses() support   */
#include <pthread.h>            /* pthreads support             */
#include <ctype.h>
#include <fcntl.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/epoll.h>
#include <net/ethernet.h>
#include <linux/netlink.h>      /* get_my_addresses() support   */
#include <linux/rtnetlink.h>    /* get_my_addresses() support   */
#include <linux/if.h>           /* set_link_params() support    */
#include <sys/ioctl.h>          /* get_my_addresses() support   */

//#include <netinet/if_ether.h>
//#include <netinet/ether.h>
#include <net/if_arp.h>
#include <linux/netdevice.h>

//#include <pcap.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef unsigned int id_t;
#define TRUE 1
#define FALSE 0
#endif /* _PUBLIC_INCLUDE */


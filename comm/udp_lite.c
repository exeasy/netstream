#include <utils/common.h>
#include <utils/utils.h>
#include "udp_lite.h"


#define MAX_MESSAGE_LEN 10240
char recvbuf[MAX_MESSAGE_LEN];
typedef unsigned char u_char;

int create_server(int listen_port, int type){
     if(type !=0&&type!=1){
          printf("Server Type Wrong!\n");
          return -1;
     }
     struct sockaddr_in server_addr;
     int sockfd ;
     if(type == 0) //tcp
          sockfd = socket(AF_INET,SOCK_STREAM,0);
     else
          sockfd = socket(AF_INET,SOCK_DGRAM,0);
     if(sockfd < 0)
     {
          printf("can't create socket");
          return -1;
     }
     int status = 1;
     setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, &status, sizeof (status));
     bzero(&server_addr,sizeof(server_addr));
     server_addr.sin_family = AF_INET;
     server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
     server_addr.sin_port = htons(listen_port);
     int serv_len = sizeof(struct sockaddr_in);
     if(bind(sockfd, (struct sockaddr*)&server_addr, serv_len)<0)
     {
		 printf("bind error\n");
          return -1;
      }
     return sockfd;
}

int connect_server(const char* ip, int port)
{
	struct sockaddr_in srv_addr;
	int ret = socket(AF_INET, SOCK_DGRAM, 0 );
	if( ret < 0 )
	{
		DEBUG(INFO, "can't create socket");
		return -1;
	}
	bzero(&srv_addr, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = inet_addr(ip);
	srv_addr.sin_port = htons(port);
	int srv_len = sizeof( struct sockaddr_in );

	if(connect(ret,(struct sockaddr*)&srv_addr,srv_len)<0)
	{
		DEBUG(INFO,"connect error");
		return -1;
	}
	DEBUG(INFO,"successfully connected!");
	return ret;
}
int iov_count(struct iovec *iov);
int iov_totallen(struct iovec *iov);
int lsd_recvmsg(struct in_addr *src, struct in_addr *dst,
		unsigned int *ifindex, struct iovec *message, int sockfd);

int udp_receive(int sockfd, packet_handler h)
{
	memset (recvbuf, 0, MAX_MESSAGE_LEN);
	struct iovec iovector[2];
	iovector[0].iov_base = recvbuf;
	iovector[0].iov_len = MAX_MESSAGE_LEN;
	iovector[1].iov_base = NULL;
	iovector[1].iov_len = 0;
	struct in_addr src, dst;
	unsigned int ifindex;
	int len = udp_recvmsg (&src, &dst, &ifindex, iovector, sockfd);
	struct udp_pkt_info info;
	info.src = src;
	info.dst = dst;
	info.ifindex = ifindex;
	info.len = len;
	info.sockfd = sockfd;
	info.msg = recvbuf;
	return h(&info);

}
int udp_send(int sockfd, struct udp_pkt_info* info)
{
	struct sockaddr_in addr;
	struct in_addr dst = info->dst;
	bzero((char*)&addr,   (int)sizeof(addr));
	bcopy(&dst, (char*)&addr.sin_addr,sizeof(dst));
	addr.sin_family = AF_INET;
	int ret = sendto(sockfd, info->msg , info->len , 0, (struct sockaddr*)&addr,sizeof(addr));
	return ret;
}

//receive the message
int udp_recvmsg(struct in_addr *src, struct in_addr *dst,
		unsigned int *ifindex, struct iovec *message, int sockfd) {
	int retval;
	struct msghdr rmsghdr;
	struct cmsghdr *rcmsgp;
	u_char cmsgbuf[CMSG_SPACE(sizeof (struct in_pktinfo))];
	struct in_pktinfo *pktinfo;
	struct sockaddr_in src_sin;

	rcmsgp = (struct cmsghdr *) cmsgbuf;
	pktinfo = (struct in_pktinfo *) (CMSG_DATA(rcmsgp));
	memset(&src_sin, 0, sizeof(struct sockaddr_in));

	/* receive control msg */
	rcmsgp->cmsg_level = IPPROTO_IP;
	rcmsgp->cmsg_type = IP_PKTINFO;
	rcmsgp->cmsg_len = CMSG_LEN (sizeof (struct in_pktinfo));
	/* rcmsgp = CMSG_NXTHDR (&rmsghdr, rcmsgp); */

	/* receive msg hdr */
	rmsghdr.msg_iov = message;
	rmsghdr.msg_iovlen = iov_count(message);
	rmsghdr.msg_name = (caddr_t) &src_sin;
	rmsghdr.msg_namelen = sizeof(struct sockaddr_in);
	rmsghdr.msg_control = (caddr_t) cmsgbuf;
	rmsghdr.msg_controllen = sizeof(cmsgbuf);

	int status =1;
	//setsockopt(sockfd,IPPROTO_IP,IP_RECVPKTINFO,&status,sizeof(status));
	retval = recvmsg(sockfd, &rmsghdr, 0);

	/* source address */
	assert (src);
	memcpy(src, &src_sin.sin_addr, sizeof(struct in_addr));

	/* destination address */
	if (ifindex)
		*ifindex = pktinfo->ipi_ifindex;
	if (dst)
		memcpy(dst, &pktinfo->ipi_addr, sizeof(struct in_addr));

	return retval;

}
int iov_count(struct iovec *iov) {
	int i;
	for (i = 0; iov[i].iov_base; i++)
		;
	return i;
}
int iov_totallen(struct iovec *iov) {
	int i;
	int totallen = 0;
	for (i = 0; iov[i].iov_base; i++)
		totallen += iov[i].iov_len;
	return totallen;
}

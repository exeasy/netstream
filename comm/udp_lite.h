#ifndef UDP_LITE_H

#define UDP_LITE_H


struct udp_pkt_info {
	struct in_addr src;
	struct in_addr dst;
	int ifindex;
	int len;
	int sockfd;
	char* msg;
} ;
typedef int (*packet_handler)( struct udp_pkt_info* info);

//Server 
int create_server(int listen_port, int type);

//Client
int connect_server(const char* ip, int port);

//Common 
int udp_receive(int sockfd, packet_handler h);
int udp_send(int sockfd, struct udp_pkt_info* info);



#endif /* end of include guard: UDP_LITE_H */

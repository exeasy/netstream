#include <utils/common.h>
#include <utils/utils.h>
#include "comm_lite.h"

int make_socket_status( int sfd, int block)
{
	int flags, s;

	flags = fcntl (sfd, F_GETFL, 0);
	if (flags == -1)
	{
		perror ("fcntl");
		return -1;
	}
	if(block == NO_BLOCKED)
		flags |= O_NONBLOCK;
	else if(block == BLOCKED)
		flags &= ~O_NONBLOCK;
	s = fcntl (sfd, F_SETFL, flags);
	if (s == -1)
	{
		perror ("fcntl");
		return -1;
	}

	return 0;
}
int create_connect(const char* ip, int port)
{
	struct sockaddr_in server_addr;
	int server_sock;
	int connected = 0;
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(port);
	int serv_len = sizeof(struct sockaddr_in);
	printf("Trying to connect the Server...\n");
	int ret = 0;int n;
	server_sock =  socket(AF_INET,SOCK_STREAM,0);
	make_socket_status(server_sock,NO_BLOCKED);
	setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &n, sizeof(int));//make the socket reuse able 
	if(server_sock <0)
	{
		perror("Cant create socket");
		close(server_sock);
		return -1;
	}
	ret = connect(server_sock,(struct sockaddr*)&server_addr,sizeof(struct sockaddr_in));
	if(ret != 0)
	{
		if(errno != EINPROGRESS)
		{
			perror("Connect error");
			close(server_sock);return -1;
		}
		else
		{
			struct timeval tm;
			int timeout = CONNECT_TIMEOUT;

			tm.tv_sec = timeout/1000;
			tm.tv_usec = timeout%1000 * (1000);

			fd_set set ,rset;
			FD_ZERO(&set);
			FD_ZERO(&rset);
			FD_SET(server_sock,&set);
			FD_SET(server_sock,&rset);
			socklen_t len;
			int error = -1;
			int res;
			res = select(server_sock+1,&rset,&set,NULL,&tm);
			if(res < 0)
			{
				close(server_sock);
				perror("Connect error");
				return -1;
			}
			else if(res == 0)
			{
				close(server_sock);
				perror("Connect time out");
				return -1;
			}
			else
			{
				if(FD_ISSET(server_sock,&set)&&!FD_ISSET(server_sock, &rset))
				{
					printf("connect success\n");
					make_socket_status(server_sock,BLOCKED);
					return server_sock;
				}
				else
				{
					close(server_sock);
					perror("Connect Failed");
					return -1;
				}
			}
		}
	}
	else
	{
		connected = 1;
		printf("Connect success\n");
		make_socket_status(server_sock,BLOCKED);
		return server_sock;
	}
}
int close_connect(int sockfd)
{
	return (close(sockfd));
}
int update_pma_addr(int sockfd){
	struct sockaddr_in client_addr;
	int len = sizeof(client_addr);
	char localip[24];
	int localip_value;
	getsockname(sockfd,(struct sockaddr*)&client_addr,&len);
	localip_value = client_addr.sin_addr.s_addr;
	inet_ntop(AF_INET,&client_addr.sin_addr,localip,24);
	return localip_value;
}
int send_message_to_agent(char* ip, int port, int type,  const char* buff, int len){
	int ret = create_connect(ip, port);
	if( ret == -1 ){
		perror("create connect failed");
		return -1;
	}
	printf("connect successfully!\n");
	int localip = update_pma_addr(ret);
	int pkt_len = sizeof( struct pma_pms_header) + len;
	void * newbuff = malloc(pkt_len);
	struct pma_pms_header* header = (struct pma_pms_header*)newbuff;
	header->pph_pkg_type = type;
	header->pph_pkg_version = 0x00;
	header->pph_pkg_dev_type = 0x00;
	header->pph_pkg_len = htonl(pkt_len);
	header->pph_agent_id = 0x00;

	memcpy(newbuff+ sizeof(struct pma_pms_header), buff, len);
	int status = send( ret, newbuff, pkt_len, 0);
	if( status == -1 )
	{
		perror( "send packet failed");
		return -1;
	}
	printf("Send packet success! PKT Type=%d Len=%d\n", type, len);
	close(ret);
}

#ifndef COMM_LITE_H

#define COMM_LITE_H

#define CONNECT_TIMEOUT 500 //ms 500ms is enough

#define NO_BLOCKED 0
#define BLOCKED 1

#define MAXBUF 1024*50

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

struct common_header{
	u8 pkg_type;
	u8 pkg_version;
	u16 device_type;
	u32 pkg_len;
};

struct pma_pms_header{
	struct common_header com_h;
#define pph_pkg_type com_h.pkg_type
#define pph_pkg_version com_h.pkg_version
#define pph_pkg_dev_type com_h.device_type
#define pph_pkg_len com_h.pkg_len
	u32 pph_agent_id;
	struct in_addr src_addr;
	struct in_addr dst_addr;
};

struct packet{
	char	ip[INET_ADDRSTRLEN];
	int		port;
	int		sockfd;
	unsigned int	len;
	int		timeout;
	int		ops_type;
	char	*head;
	char	*data;
	char	*tail;
	char	*end;
};

int make_socket_status( int sfd, int block);

int create_connect(const char* ip, int port);

int send_message_to_agent(char* ip, int port, int type,  const char* buff, int len);

int close_connect(int sockfd);

#endif /* end of include guard: COMM_LITE_H */

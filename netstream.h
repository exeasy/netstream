#ifndef NETSTREAM_H

#define NETSTREAM_H

#define COVS(x) x = ntohs(x)
#define COVL(x) x = ntohl(x)

typedef struct StreamHeader {
	u16 version;
	u16 count;
	u32 uptime;
	u32 unix_secs;
	u32 unix_nsecs;
	u32 flow_sequence;
	u8 engine_type;
	u8 engine_id;
	u16 sample_interval;
} FlowHeader ;

typedef struct NetFlow{
	u32 sip;
	u32 dip;
	u32 nexthop;
	u16 in_ifdex;
	u16 out_ifdex;
	u32 packet_count;
	u32 byte_count;
	u32 time_at_start;
	u32 time_at_end;
	u16 sport;
	u16 dport;
	u8 pad;
	u8 tcpflag;
	u8 layer4;
	u8 tos;
	u16 sasid;
	u16 dasid;
	u8 smask;
	u8 dmask;
	u16 pad2;
} NetFlow;


#endif /* end of include guard: NETSTREAM_H */

#include <utils/common.h>
#include <utils/utils.h>
#include <comm/comm_lite.h>
#include <comm/udp_lite.h>
#include <utils/xml.h>
#include <utils/ini.h>

#include "netstream.h"

#define OPS_PMA_FLOW_INFO 13

int device_id = 0;
char routerip[24]; 
char pmaip[24];
int pmaport;

char* flow_info_to_xml(NetFlow *flow, int rate)
{
	
	int in_flow = 0, out_flow = 0;
	if ( flow->in_ifdex == 0 ) 
		out_flow = 1;
	if ( flow->out_ifdex == 0)
		in_flow = 1;
	xmlDocPtr doc = create_xml_doc();
	xmlNodePtr devnode;
	xmlNodePtr ifnode;
	xmlNodePtr flownode;
	xmlNodePtr childnode;

	char value[100];
	memset(value, 0x00, 100);
	
	xmlNodePtr rootnode = create_xml_root_node(doc, "TC_FLOW_INFO");
	struct timeval now;
	gettimeofday(&now, NULL);
	char * time = PRINTTIME(now);
	add_xml_child(rootnode, "timestamp",time); free(time);

	devnode = add_xml_child(rootnode, "ROUTER", NULL);
	char aid[24];
	int agentid = 0;
//	inet_pton(AF_INET, routerid, &agentid); 
	agentid = ntohl(agentid);

	sprintf(aid,"%d", device_id);
	add_xml_child_prop(devnode, "id",aid); 

	// Add the Element
	int ifid = 0;
	if ( in_flow )
	{
		ifid = flow->in_ifdex;
	}
	if ( out_flow )
	{
		ifid = flow->out_ifdex;
	}

	ifnode = add_xml_child(devnode, "interface", NULL);

	memset(value, 0x00, 100);
	sprintf(value,"%d",ifid);
	add_xml_child_prop(ifnode, "id", value);

	memset(value, 0x00, 100);
	sprintf(value,"GigabitEthernet0/0/%d", ifid-3);
	add_xml_child(ifnode, "interface_name", value);
	flownode  = add_xml_child(ifnode, "flow_info", NULL);
	childnode = add_xml_child(flownode, "flow",NULL);

	memset(value, 0x00, 100);
	sprintf(value, "%d",4);
	add_xml_child(childnode, "flow_type", value);

	add_xml_child(childnode, "protocol", NULL);

	memset(value, 0x00, 100);
	sprintf(value, "%d", flow->tos);
	add_xml_child(childnode, "dscp", value);

	memset(value, 0x00, 100);
	inet_ntop(AF_INET, &flow->sip, value, 24);
	add_xml_child(childnode,"src", value);

	memset(value, 0x00, 100);
	inet_ntop(AF_INET, &flow->dip, value, 24);
	add_xml_child(childnode,"dst", value);

	memset(value, 0x00, 100);
	sprintf(value,"%d", flow->sport);
	add_xml_child(childnode,"src_port", value);

	memset(value, 0x00, 100);
	sprintf(value,"%d", flow->dport);
	add_xml_child(childnode,"dst_port", value);

	if( in_flow )
	{
		memset(value, 0x00, 100);
		sprintf(value, "%d", ifid);
		add_xml_child(childnode,"input_if", value);
		add_xml_child(childnode, "output_if", "0");

		memset(value, 0x00,100);
		sprintf(value,"%d",rate);
		add_xml_child(childnode, "in_send_rate", value);
		add_xml_child(childnode, "out_send_rate", NULL);
	}
	if (out_flow )
	{
		memset(value, 0x00, 100);
		sprintf(value, "%d", ifid);
		add_xml_child(childnode,"input_if", "0");
		add_xml_child(childnode, "output_if", value);

		memset(value, 0x00,100);
		sprintf(value,"%d",rate);
		add_xml_child(childnode, "in_send_rate", NULL);
		add_xml_child(childnode, "out_send_rate", value);
	}

	u8 *xmlbuff;
	int len  = 0;
	xmlDocDumpFormatMemoryEnc( doc, &xmlbuff, &len, "UTF-8", 0 );
	char *buff = (char*)malloc(len+1);
	memcpy(buff, xmlbuff, len);
	buff[len]= 0;
	xmlFree(xmlbuff);
	xmlFreeDoc(doc);
	return buff;
}

int flow_handle(NetFlow *flow)
{
	// only one packet , then drop it
	if (flow->packet_count == 1 ) 
		return 0;
	if (flow->time_at_start == flow->time_at_end ) 
		return 0;
	
	int in_flow = 0, out_flow = 0;
	if ( flow->in_ifdex == 0 ) 
		out_flow = 1;
	if ( flow->out_ifdex == 0)
		in_flow = 1;

	//compute the flow last time
	u32 last_time = flow->time_at_end - flow->time_at_start;

	u8 dscp = flow->tos;
	DEBUG(INFO,"last time %d\n", last_time);
	DEBUG(INFO,"byte %d\n",flow->byte_count);
	double rate = ((double)flow->byte_count)*1000/last_time;
	
	char sip[24]; char dip[24];
	inet_ntop(AF_INET, &flow->sip, sip, 24);
	inet_ntop(AF_INET, &flow->dip, dip, 24);
	DEBUG(INFO,"dscp %d\n", dscp);
	DEBUG(INFO,"src %s\n", sip);
	DEBUG(INFO,"dst %s\n", dip);
	DEBUG(INFO,"src_port %d\n", flow->sport);
	DEBUG(INFO,"dst_port %d\n", flow->dport);
	DEBUG(INFO,"input_if %d\n", flow->in_ifdex);
	DEBUG(INFO,"output_if %d\n", flow->out_ifdex);
	if( in_flow )
		DEBUG(INFO,"in_send_rate %f\n", rate);
	if( out_flow )
		DEBUG(INFO,"out_send_rate %f\n", rate); 
	char * xmlbuff = flow_info_to_xml( flow, (int)rate);
	printf("%s\n",xmlbuff);
	int len = strlen(xmlbuff);
	send_message_to_agent( pmaip, pmaport, OPS_PMA_FLOW_INFO, xmlbuff, len);
}

int packet_recvier(struct udp_pkt_info* info)
{
	char sip[24];
	char dip[24];
	inet_ntop(AF_INET, &info->src, sip,24);
	inet_ntop(AF_INET, &info->dst, dip,24);
	DEBUG(INFO,"SrcIP:[%s] DstIP:[%s] Fd:[%d] Len:[%d]",
			sip, dip, info->sockfd, info->len);
	FlowHeader* header = (FlowHeader*)(info->msg);
#ifdef DEBUGALL
	DEBUG(INFO,"================================");
	DEBUG(INFO,"version %d\n", ntohs(header->version));
	DEBUG(INFO,"Flow count %d\n", ntohs(header->count));
	DEBUG(INFO,"Uptime %d\n", ntohl(header->uptime));
	DEBUG(INFO,"Sec %d\n", ntohl(header->unix_secs));
	DEBUG(INFO,"nsec %d\n", ntohl(header->unix_nsecs));
	DEBUG(INFO,"flow_sequence %d\n", ntohl(header->flow_sequence));
	DEBUG(INFO,"engine_type %d\n", header->engine_type);
	DEBUG(INFO,"engine_id %d\n", header->engine_id);
	DEBUG(INFO,"sample %d\n", htons(header->sample_interval));
#endif

	int read_size = sizeof(FlowHeader);
	char *pos = info->msg + sizeof(FlowHeader);
	while(read_size != info->len )
	{

	NetFlow * flow = (NetFlow*)(pos);
#ifdef DEBUGALL
	char fsip[24]; char fdip[24];
	inet_ntop(AF_INET, &flow->sip, fsip, 24);
	inet_ntop(AF_INET, &flow->dip, fdip, 24);
	DEBUG(INFO,"===========Flow Info==========\n");
	DEBUG(INFO,"sip %s\n", fsip);
	DEBUG(INFO,"dip %s\n", fdip);
	char fnexthop[24];
	inet_ntop(AF_INET, &flow->nexthop, fnexthop, 24);
	DEBUG(INFO,"nexthop %s\n", fnexthop);
#endif
	COVS(flow->in_ifdex);
	COVS(flow->out_ifdex);
	COVL(flow->packet_count);
	COVL(flow->byte_count);
	COVL(flow->time_at_start);
	COVL(flow->time_at_end);
	COVS(flow->sport);
	COVS(flow->dport);
#ifdef DEBUGALL
	DEBUG(INFO,"in ifdex %d\n", flow->in_ifdex);
	DEBUG(INFO,"out ifdex %d\n", flow->out_ifdex);
	DEBUG(INFO,"pkt count %d\n", flow->packet_count);
	DEBUG(INFO,"byte count %d\n", flow->byte_count);
	DEBUG(INFO,"time start %d\n", flow->time_at_start);
	DEBUG(INFO,"time end %d\n", flow->time_at_end);
	DEBUG(INFO,"source port %d\n", flow->sport);
	DEBUG(INFO,"dest port %d\n", flow->dport);
	DEBUG(INFO,"tcp flag %d\n", flow->tcpflag);
	DEBUG(INFO,"layer4 %d\n", flow->layer4);
	DEBUG(INFO,"tos %d\n", flow->tos);
	DEBUG(INFO,"source as %d\n", ntohs(flow->sasid));
	DEBUG(INFO,"dest as %d\n", ntohs(flow->dasid));
	DEBUG(INFO,"source mask %d\n", flow->smask);
	DEBUG(INFO,"dest mask %d\n", flow->dmask);
#endif
	flow_handle(flow);

	pos += sizeof(NetFlow);
	read_size += sizeof(NetFlow);
	}
}

#define IS_VAL(s,k) ({ int ret= 0; ret = (strcmp(section, s) == 0 && strcmp( key, k ) == 0) ; ret; })

int ini_parser(const char* file, int line, const char* section, char *key, char* value, void* data)
{
	if ( section != NULL && key != NULL && value != NULL )
		printf( "Section %s -> key %s:value %s\n", section, key, value);
	else return 0;
	if(IS_VAL("Router","id")){
		inet_pton(AF_INET, value, &device_id);
		device_id = ntohl(device_id);
		return 0;
	}
	if(IS_VAL("Router","ip")) {
		strcpy(routerip, value);
		return 0;
	}

	if (IS_VAL("PMA","ip")){
		strcpy(pmaip, value);	
		return 0;
	}
	if (IS_VAL("PMA","port")){
		pmaport = atoi(value);
		return 0;
	}
	return 0;
}

int main(int argc, const char *argv[])
{

	parse_ini("cfg.ini", ini_parser, NULL);

	int fd = create_server(1001, 1);
	if( fd == -1 ) return 0;
	while(1)
	{
		udp_receive(fd, packet_recvier);	
	}
	return 0;
}

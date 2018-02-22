#include    <sys/socket.h>
#include    <sys/ioctl.h>
#include    <fcntl.h>
#include    <unistd.h>
#include    <netinet/if_ether.h>
#include    <netinet/ip.h>
#include    <netinet/udp.h>
#include    <netinet/tcp.h>
#include    <net/if.h>
#include    <netdb.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <linux/if_packet.h>
#include    <linux/errno.h>

int sendfd, sock;
struct sockaddr_ll in_addr;
struct sockaddr_ll dest;
struct ifreq ifr;

int socket_init()
{
	struct    ifreq    ifr;
	int   sock;
	memset(&ifr, 0x0, sizeof(struct ifreq));

	// 创建socket 
	if((sock = socket(PF_PACKET, SOCK_RAW, htons(0x0003))) < 0){
		perror(" socket ");
		exit(0);
	}

	// 设置混乱模式
	strcpy(ifr.ifr_name, "eth0");
	if(ioctl(sock, SIOCGIFFLAGS, &ifr) == -1){
		perror("siocgifflags");
		exit(0);
	}

	ifr.ifr_flags |= IFF_PROMISC;
	if(ioctl(sock, SIOCSIFFLAGS, &ifr) == -1){
		perror("fejwaio?");
		exit(0);
	}
	return sock;
}

int showpacket(unsigned char* buffer)
{
	int i=0;
	unsigned char buff[50];
	memmove(buff,buffer,50);  
	for(i=0;i<50;i++){
		if(i==25)
			printf("\n");
		printf("%2x ",buff[i]);
	}
	printf("\n");
}

int main(int argc, char** argv)
{
	socklen_t destlen;
	struct ethhdr   *eth_hdr;
	int	recv_len;
	int	socklen;
	int flag = 0;
	int on=1;
	int n=0;
	unsigned char   buffer[65536];
	int myflag=0;
	struct packet_mreq mr;
	int state=0;
	int recv_buf_len,buflen;
	int i=0;
	
	memset(&mr,0,sizeof(mr));
	socklen = sizeof(struct sockaddr_ll);
	
	if(argc < 2){
		printf("\n invalid argument!! \n");
		exit(0);
	}

	sock = socket_init();
	sendfd=socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	
	strcpy(ifr.ifr_name,"eth0");
	ioctl(sendfd,SIOCGIFINDEX,&ifr);
	mr.mr_ifindex=ifr.ifr_ifindex;
	mr.mr_type=PACKET_MR_PROMISC;

	if(setsockopt(sendfd,SOL_PACKET,PACKET_ADD_MEMBERSHIP,&mr,sizeof(mr))==-1){
		perror(" can not set to promisc");
		exit(0);
	}
	if(strcmp(argv[1], "all") == 0)
			flag = 0;
	else if(strcmp(argv[1], "debug") == 0)
			flag = 1;
	else {
			printf("\n wrong argument!!\n");
			exit(0);
	}

	while(1){
		memset(&dest, 0x0, socklen);
		memset(&in_addr, 0x0, socklen);   
		recv_len = recvfrom(sock, buffer, sizeof(buffer), 0,
				(struct sockaddr *)&in_addr, &socklen);
		showpacket(buffer);

		if(in_addr.sll_pkttype==PACKET_OUTGOING){printf("won't send me again;\n");continue;}		   		   		  
		
		dest.sll_family=AF_PACKET;
		
		dest.sll_ifindex =ifr.ifr_ifindex;
		
		dest.sll_halen=htons(ETH_HLEN);     
		
		memmove(&dest.sll_addr,buffer,sizeof(buffer[0])*8);
		
		n=sendto(sendfd,buffer,recv_len,0,(struct sockaddr*)&dest,sizeof(dest));
		
		printf(" send n=%d\n",n);
	}
	return 0;
}


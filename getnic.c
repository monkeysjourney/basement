#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>


int main(void)
{
	int fd;						// 套接字
	int interfaceNum = 0;		// 网卡数量
	struct ifreq buf[16];		// 
	struct ifconf ifc;			// 保存接口信息
	struct ifreq ifrcopy;
	char mac[32] = {0};
	char ip[32] = {0};
	char broadAddr[32] = {0};
	char subnetMask[32] = {0};

	// 创建一个socket
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		perror("socket");
		close(fd);
		return -1;
	}


	// 数据缓冲区长度 数据缓冲区
	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = (void *)buf;

	// 获取所有接口列表
	if (!ioctl(fd, SIOCGIFCONF, (char *)&ifc)){
		// 网卡数量
		interfaceNum = ifc.ifc_len / sizeof(struct ifreq);
		printf("interface num = %d\n", interfaceNum);
		
		while (interfaceNum-- > 0){
			
			// 网卡名称
			printf("\ndevice name: %s\n", buf[interfaceNum].ifr_name);
			
			//ignore the interface that not up or not runing
			ifrcopy = buf[interfaceNum];
			
			// 获取接口标识
			if (ioctl(fd, SIOCGIFFLAGS, &ifrcopy)){
				printf("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
				close(fd);
				return -1;
			}
			else {
				printf("flags = %d", buf[interfaceNum].ifru_flags);
			}
			
			//获取mac地址
			if (!ioctl(fd, SIOCGIFHWADDR, (char *)(&buf[interfaceNum]))){
				memset(mac, 0, sizeof(mac));
				
				snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x",
						(unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[0],
						(unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[1],
						(unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[2],
						(unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[3],
						(unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[4],
						(unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[5]);
				printf("device mac: %s\n", mac);
			}
			else{
				printf("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
				close(fd);
				return -1;
			}
			
			// 获取ip地址
			if (!ioctl(fd, SIOCGIFADDR, (char *)&buf[interfaceNum])){
				snprintf(ip, sizeof(ip), "%s",
						(char *)inet_ntoa(((struct sockaddr_in *)&(buf[interfaceNum].ifr_addr))->sin_addr));
				printf("device ip: %s\n", ip);
			}
			else{
				printf("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
				close(fd);
				return -1;
			}
			
			// 获取广播地址
			if (!ioctl(fd, SIOCGIFBRDADDR, &buf[interfaceNum])){
				snprintf(broadAddr, sizeof(broadAddr), "%s",
						(char *)inet_ntoa(((struct sockaddr_in *)&(buf[interfaceNum].ifr_broadaddr))->sin_addr));
				printf("device broadAddr: %s\n", broadAddr);
			}
			else{
				printf("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
				close(fd);
				return -1;
			}
			
			// 获取子网掩码
			if (!ioctl(fd, SIOCGIFNETMASK, &buf[interfaceNum])){
				snprintf(subnetMask, sizeof(subnetMask), "%s",
						(char *)inet_ntoa(((struct sockaddr_in *)&(buf[interfaceNum].ifr_netmask))->sin_addr));
				printf("device subnetMask: %s\n", subnetMask);
			}
			else{
				printf("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
				close(fd);
				return -1;
			}

			// 获取mtu
			if( !ioctl(fd, SIOCGIFMTU, &buf[interfaceNum]) ){
				printf("mtu: %d\n", buf[interfaceNum].ifr_mtu);
			}
			else{
				printf("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
				close(fd);
				return -1;
			}
		}
	}
	else{
		printf("ioctl: %s [%s:%d]\n", strerror(errno), __FILE__, __LINE__);
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}


#if 0
// 使用ioctl函数虽然可以获取所有的信息，但是使用起来比较麻烦，如果不需要获取MAC地址，那么使用getifaddrs函数来获取更加方便与简洁。
int main(void)
{
	 struct sockaddr_in *sin = NULL;
	 struct ifaddrs *ifa = NULL, *ifList;
	 if (getifaddrs(&ifList) < 0) return -1;
	 for (ifa = ifList; ifa != NULL; ifa = ifa->ifa_next)
	 {
		  if(ifa->ifa_addr->sa_family == AF_INET)
		  {
				printf("\n>>> interfaceName: %s\n", ifa->ifa_name);
				sin = (struct sockaddr_in *)ifa->ifa_addr;
				printf(">>> ipAddress: %s\n", inet_ntoa(sin->sin_addr));
				sin = (struct sockaddr_in *)ifa->ifa_dstaddr;
				printf(">>> broadcast: %s\n", inet_ntoa(sin->sin_addr));
				sin = (struct sockaddr_in *)ifa->ifa_netmask;
				printf(">>> subnetMask: %s\n", inet_ntoa(sin->sin_addr));
		  }
	 }
	 freeifaddrs(ifList);
	 return 0;
}

#endif

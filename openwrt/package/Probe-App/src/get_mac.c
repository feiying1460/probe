#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h> 

#include "public.h"
int count = 0;//遍历mac_table
int get_pro_mac(char *recv,int recv_len)
{
	int i;
	char buf[recv_len];

	memset(buf,0,recv_len);
	if(recv_len < (TAB_LEN*20+2))
		return -1;

	strcat(buf,"[");
	for(i=0;i<TAB_LEN;i++)
	{
		strcat(buf,"\"");
		strcat(buf,test_tab[i].mac);
		strcat(buf,"\"");
		if(i<TAB_LEN-1)
			strcat(buf,",");
	}
	strcat(buf,"]");
	strcpy(recv,buf);

	return strlen(recv);
}

int get_local_mac(char * mac, int len_limit)    //·µ»ØµÊʵ¼Ê´Èchar * macµÄַû£¨²»°ü'£©
{
    struct ifreq ifreq;
    int sock;

    if ((sock = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror ("socket");
        return -1;
    }
    strcpy (ifreq.ifr_name, "eth0");    //Currently, only get eth0

    if (ioctl (sock, SIOCGIFHWADDR, &ifreq) < 0)
    {
        perror ("ioctl");
        return -1;
    }
    
    return snprintf (mac, len_limit, "%02X:%02X:%02X:%02X:%02X:%02X", (unsigned char) ifreq.ifr_hwaddr.sa_data[0], (unsigned char) ifreq.ifr_hwaddr.sa_data[1], (unsigned char) ifreq.ifr_hwaddr.sa_data[2], (unsigned char) ifreq.ifr_hwaddr.sa_data[3], (unsigned char) ifreq.ifr_hwaddr.sa_data[4], (unsigned char) ifreq.ifr_hwaddr.sa_data[5]);
}

int to_json(char *rou_mac,int rlen,char *pro_mac,int plen)
{
	int dst_len = rlen+plen+10;
	char dst[dst_len];
	memset(dst,0,sizeof(dst));
	strcat(dst,"{\"");
	strcat(dst,rou_mac);
	strcat(dst,"\":");
	strcat(dst,pro_mac);
	strcat(dst,"}");

	if(send_post(dst,sizeof(dst)) == -1)
	{
		printf("To json in send_post failed\n");
		return -1;
	}
	printf("this is prepare send data %s\n",dst);
	
	return 0;
}

int prepare(void)
{
	char route_mac_addr[MAC_LEN];
	char probe_mac_addr[TAB_LEN*(MAC_LEN+3)] = "[";

	if(get_local_mac(route_mac_addr,18)<0)
	{
		printf("Get local mac failed!!!\n");
		return -1;
	}
	if(get_pro_mac(probe_mac_addr,sizeof(probe_mac_addr)) < 0)
	{
		printf("Get local mac failed!!!\n");
		return -1;
	}
	printf("probe mac %s\n",probe_mac_addr);
	printf("local mac %s\n",route_mac_addr);
	if(to_json(route_mac_addr,sizeof(route_mac_addr),probe_mac_addr,sizeof(probe_mac_addr)) == -1)
	{
		printf("Prepare to_json failed\n");
		return -1;
	}

	return 0;
}

void init_table(void)
{
	int i;
	memset(test_tab,0,TAB_LEN*MAC_LEN);
}

int insert_mac(char*s)
{
	int i;
	if(count < TAB_LEN){

			for(i=0;i<TAB_LEN;i++)
			{
				if(strcmp(test_tab[i].mac,s)==0){
					return -1;
				}
			}
		
			if(strlen(s) == (MAC_LEN-1)){
				strncpy(test_tab[count].mac,s,strlen(s));
				count++;
			}else{
				printf("Mac addr lenght error!!\n");
				return -1;
			}
	}
	else{
		count = 0;
		if(prepare()<0)
		{
			printf("preapre send data failed!!!!\n");
			return -1;
		}
		init_table();
	}
	
	return 0;
}

/*
int main(void)
{
	char s[20];
	int i;
	init_table();
	for(i=0;i<98;i++)
	{
		sprintf(s,"%02d:%02d:%02d:%02d:%02d:%02d",i,i,i,i,i,i);
		ins_mac(s);
	}
	for(i=0;i<TAB_LEN;i++)
	{
		printf("%s\n",test_tab[i].mac);
	}

	return 0;
}
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <linux/netlink.h>

#define TAB_LEN 10  //配置mac表的长度 
#define MAC_LEN 18  //MAC地址长度

#define NETLINK_TEST 17 //定义net_link协议族
#define MSG_LEN 100  	//报文长度


#define FILE_MAX_SIZE (1024*1024)

//mac 缓存表
struct mac_table{
	char mac[MAC_LEN];
};
//net_link使用
struct msg_to_kernel
{
	struct nlmsghdr hdr;
	char data[MSG_LEN];
};

struct u_packet_info
{
	struct nlmsghdr hdr;
	char msg[MSG_LEN];
};

//初始化缓存表
void init_table(void);

int ins_mac(char *);

int send_post(char * ,int);

struct mac_table test_tab[TAB_LEN];

//日志函数

void get_local_time(char*);

long get_file_size(char*);

void write_log_file(char*,long , char* , unsigned);

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "public.h"
 
#define IPSTR "192.168.10.254"
#define PORT 82
#define BUFSIZE 1024
 
int send_post(char * post_str,int postlen)
{

		char post_send_buf[postlen];
		char buffer[32];
        int sockfd, ret, i, h;
        struct sockaddr_in servaddr;
        char str1[4096], str2[4096], buf[BUFSIZE], *str;
        socklen_t len;
        fd_set   t_set1;
        struct timeval  tv;
 
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
                printf("创建网络连接失败,本线程即将终止---socket error!\n");
                exit(0);
        };
 
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(PORT);
        if (inet_pton(AF_INET, IPSTR, &servaddr.sin_addr) <= 0 ){
                printf("创建网络连接失败,本线程即将终止--inet_pton error!\n");
                exit(0);
        };
 
        if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
                printf("连接到服务器失败,connect error!\n");
               return -1;
        }
        printf("与远端建立了连接\n");
 
        //发送数据
	memset(post_send_buf, 0,postlen);
	strcpy(post_send_buf,post_str);
	printf("send post ---------%s\n",post_send_buf);
        memset(str2, 0, 4096);
        strcat(str2, post_send_buf);
        str=(char *)malloc(128);
        len = strlen(str2);
        sprintf(str, "%d", len+5);
 
	printf("------------------beautifl---split----line-----------------------------------\n");
        memset(str1, 0, 4096);
        strcat(str1, "POST /test.php HTTP/1.1\n");
        strcat(str1, "Host: 192.168.10.254\n");
        strcat(str1, "Content-Type: application/x-www-form-urlencoded\n");
        strcat(str1, "Content-Length: ");
        strcat(str1, str);
        strcat(str1, "\n\n");
 
        strcat(str1, "data=");
        strcat(str1, str2);
        strcat(str1, "\r\n\r\n");
        printf("%s\n",str1);
		
        ret = write(sockfd,str1,strlen(str1));
        if (ret < 0) {
                sprintf(buffer,"send failed code :%d，msg:'%s'\n",errno, strerror(errno));
				return -1;
        }else{
                sprintf(buffer,"Send %d bytes！\n", ret);
        }
		
		write_log_file("/var/log/probe.log", FILE_MAX_SIZE, buffer, strlen(buffer));
	free(str);
	close(sockfd);
 
 
        return 0;
}

/*
int main(void)
{
	char *s = "{\"78:A3:51:02:BF:73\":[\"fc:64:ba:56:20:80\",\"dc:6d:cd:6d:32:b4\",\"f0:25:b7:7e:f4:c4\",\"f0:1b:6c:56:24:d6\",\"22:f4:1b:5d:a1:08\"]}";
	send_post(s,strlen(s));
	return 0;
}
*/

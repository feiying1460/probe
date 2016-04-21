#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <linux/netlink.h>

#include "public.h"

extern int count;
int main(void)
{
        char *data = "I am from user space";
        struct sockaddr_nl local;
        struct sockaddr_nl kpeer;
        int skfd, i, ret, kpeerlen = sizeof(struct sockaddr_nl);
        struct nlmsghdr *message;
        struct u_packet_info info;
		struct mac_table *mac_buf;
        char *retval;
        message = (struct nlmsghdr *)malloc(1);

	
        skfd = socket(PF_NETLINK, SOCK_RAW, NETLINK_TEST);
        if(skfd < 0){
            printf("can not create a netlink socket\n");
            return -1;
        }
        memset(&local, 0, sizeof(local));
        local.nl_family = AF_NETLINK;
        local.nl_pid = getpid();
        local.nl_groups = 0;
        if(bind(skfd, (struct sockaddr *)&local, sizeof(local)) != 0){
            printf("bind() error\n");
            return -1;
        }
        memset(&kpeer, 0, sizeof(kpeer));
        kpeer.nl_family = AF_NETLINK;
        kpeer.nl_pid = 0;
        kpeer.nl_groups = 0;

        memset(message, '\0', sizeof(struct nlmsghdr));
        message->nlmsg_len = NLMSG_SPACE(strlen(data));
        message->nlmsg_flags = 0;
        message->nlmsg_type = 0;
        message->nlmsg_seq = 0;
        message->nlmsg_pid = local.nl_pid;

        retval = memcpy(NLMSG_DATA(message), data, strlen(data));
	
		
        ret = sendto(skfd, message, message->nlmsg_len, 0,(struct sockaddr *)&kpeer, sizeof(kpeer));
        if(!ret){
            perror("send pid:");
            exit(-1);
        }
		init_table();
		while(1)
		{
			ret = recvfrom(skfd, &info, sizeof(struct u_packet_info),0, (struct sockaddr*)&kpeer, &kpeerlen);
			if(!ret){
					perror("recv form kerner:");
					exit(-1);
			}
			info.msg[17]='\0';
			insert_mac(info.msg);
			printf("**************************************************\n");
			for(i=0;i<TAB_LEN;i++)
				printf("%s\n",test_tab[i].mac);
		}

        close(skfd);
        return 0;
}

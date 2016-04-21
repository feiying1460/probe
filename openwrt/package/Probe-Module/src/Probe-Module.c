#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <net/sock.h>
#include <linux/netlink.h>

#define NETLINK_TEST 17
struct {
	__u32 pid;
}user_process;

static struct sock *netlinkfd = NULL;

int send_to_user(char *info) //发送到用户空间
{
	int size;
	struct sk_buff *skb;
	unsigned char *old_tail;
	struct nlmsghdr *nlh; //报文头
	
	int retval;

	size = NLMSG_SPACE(strlen(info)); //报文大小
	skb = alloc_skb(size, GFP_ATOMIC); //分配一个新的套接字缓存,使用GFP_ATOMIC标志进程不>会被置为睡眠

	//初始化一个netlink消息首部
	nlh = nlmsg_put(skb, 0, 0, 0, NLMSG_SPACE(strlen(info))-sizeof(struct nlmsghdr), 0);
	old_tail = skb->tail;
	memcpy(NLMSG_DATA(nlh), info, strlen(info)); //填充数据区
	nlh->nlmsg_len = skb->tail - old_tail; //设置消息长度

	//设置控制字段
	NETLINK_CB(skb).portid = 0;
	NETLINK_CB(skb).dst_group = 0;

	//printk(KERN_DEBUG "[kernel space] skb->data:%s and recv data len %d\n", (char *)NLMSG_DATA((struct nlmsghdr *)skb->data),strlen(info));

	//发送数据
	retval = netlink_unicast(netlinkfd, skb, user_process.pid, MSG_DONTWAIT);
	//printk(KERN_DEBUG "[kernel space] netlink_unicast return: %d\n", retval);
	if(retval < 0)
	{
		printk("retval %d\n",retval);
	}
	
	
	return 0;
}

void kernel_receive(struct sk_buff *__skb) //内核从用户空间接收数据
{
	struct sk_buff *skb;
	struct nlmsghdr *nlh = NULL;

	//char *data = "This is eric's test message from kernel";

	printk(KERN_DEBUG "[kernel space] begin kernel_receive\n");
	skb = skb_get(__skb);

	if(skb->len >= sizeof(struct nlmsghdr)){
    		nlh = (struct nlmsghdr *)skb->data;
    		if((nlh->nlmsg_len >= sizeof(struct nlmsghdr)) && (__skb->len >= nlh->nlmsg_len)){
        		user_process.pid = nlh->nlmsg_pid;
        		printk(KERN_DEBUG "[kernel if space] data receive from user are:%s\n", (char *)NLMSG_DATA(nlh));
        		printk(KERN_DEBUG "[kernel space] user_pid:%ld\n", user_process.pid);
        		//send_to_user(data);
    		}
	}else{
    		printk(KERN_DEBUG "[kernel else space] data receive from user are:%s\n",(char *)NLMSG_DATA(nlmsg_hdr(__skb)));
    		//send_to_user(data);
	}

	kfree_skb(skb);
}



void recv_sta_probe_req(unsigned char *sta_req_mac)
{
	
	unsigned char recv_buf[6];
	char mac_str[18];

	memcpy(recv_buf,sta_req_mac,6);
	sprintf(mac_str,"%02x:%02x:%02x:%02x:%02x:%02x",recv_buf[0],recv_buf[1],recv_buf[2],recv_buf[3],recv_buf[4],recv_buf[5]);
	//printk("send str len:%d\n",strlen(mac_str));
	send_to_user(mac_str);
}
EXPORT_SYMBOL(recv_sta_probe_req);



static int __init mod_init(void)
{

	printk(">>>>>>>>>>>>>>>>>>>>>>init wifi probe module<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
	struct netlink_kernel_cfg cfg = {
		.input = kernel_receive,
	};
	netlinkfd = netlink_kernel_create(&init_net, NETLINK_TEST,&cfg);
	if(!netlinkfd){
    		printk(KERN_ERR "can not create a netlink socket\n");
    	return -1;
	}
	return 0;

}


static void __exit mod_exit(void)
{
	printk(">>>>>>>>>>>>>>>>>>>>>>exit wifi probe module<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
	sock_release(netlinkfd->sk_socket);
	printk(KERN_DEBUG "test_netlink_exit!!\n");
	
}



module_init(mod_init);
module_exit(mod_exit);


MODULE_LICENSE("GPL");

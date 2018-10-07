 /***********************************************************************
文件名称：TCP_CLIENT.H
功    能：完成TCP的数据收发
编写时间：2013.4.25
编 写 人：赵
注    意：
***********************************************************************/
#ifndef  _TCP_CLIENT_H_
#define  _TCP_CLIENT_H_

/* MAC ADcontinue;continue;DRESS: MAC_ADDR0:MAC_ADDR1:MAC_ADDR2:MAC_ADDR3:MAC_ADDR4:MAC_ADDR5 */
#define MAC_ADDR0   2
#define MAC_ADDR1   0
#define MAC_ADDR2   0
#define MAC_ADDR3   0
#define MAC_ADDR4   0
#define MAC_ADDR5   0
 
/*Static IP ADDRESS: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   1
#define IP_ADDR3   252
   
/*NETMASK*/
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

/*Gateway Address*/
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   1
#define GW_ADDR3   1  

#define TCP_LOCAL_PORT     		1030 
#define TCP_SERVER_PORT    		1031
#define TCP_SERVER_IP   		192,168,1,101//服务器IP，特别注意：这里的IP地址必须跟自己电脑的实际IP地址一样

extern struct tcp_pcb *tcp_client_pcb;

void TCP_Client_Init(u16_t local_port,u16_t remote_port,unsigned char a,unsigned char b,unsigned char c,unsigned char d);
err_t TCP_Client_Send_Data(struct tcp_pcb *cpcb,unsigned char *buff,unsigned int length);
struct tcp_pcb *Check_TCP_Connect(void);
void Delay_s(unsigned long ulVal); /* 利用循环产生一定的延时 */
#endif


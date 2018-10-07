#include <string.h>
#include "MQTTPacket.h"
#include "transport.h"
#include "mqtt.h"
#include "Com_Setting.h"
#include <stdio.h>
extern char ASCIICCID[17];

uint8_t mqttbuf[1400];
uint8_t OnlineFlag=0;
//#define M2MECLIPSE

u8 mqtt_open(char* payload,uint16_t TxLen)
{
  char* host;
	int port;
	char mysock = 0;
	 #ifdef M2MECLIPSE
    host = "dtu.mqtt.iot.gz.baidubce.com";
    port =1884; 
  #else
    host = csr.name; //"yzk.mqtt.iot.gz.baidubce.com";
    port = csr.port; //1883; 
  #endif

    getRSSI();
    mysock = transport_open(host,port);
    if(mysock <= 0)
    {  
      return FALSE;
    } 
    return 1;
}  

u8 mqtt_connect(char* payload,uint16_t TxLen,int *len,char *idStr)
{
  MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	MQTTString topicString = MQTTString_initializer;

  char cmd[18];
	int buflen = 1400;
  int payloadlen = TxLen;//strlen(payload

  data.keepAliveInterval = 1000;
	data.cleansession = 1;
	
#ifdef M2MECLIPSE
	data.username.cstring = "dtu/dtu";
	data.password.cstring = "W/uUPcT3Qxk25PvnpSW9XYhCmD5Lfr8lAPfAt9EGgFA=";
  data.clientID.cstring=idStr;

#else
	data.username.cstring = csr.username; //"yzk/control";
	data.password.cstring = csr.password; //"wJ8Mz/vmxgWtMq7QZ4P8M7ismt+PdhYceSigVb5p5Ew=";
 	ASCIICCID[16]=0;
	data.clientID.cstring=ASCIICCID+4;

#endif
	data.MQTTVersion = 4;
  
	*len = MQTTSerialize_connect((unsigned char *)mqttbuf, buflen, &data);

	topicString.cstring =data.clientID.cstring; //"yzk/control";
	strcat((char*)topicString.cstring, "/u");
	*len+= MQTTSerialize_publish((unsigned char *)(mqttbuf+*len), buflen- *len, 0, 0, 0, 0, topicString, (unsigned char *)payload, payloadlen);
  
	//len += MQTTSerialize_disconnect((unsigned char *)(mqttbuf + len), buflen - len);
   
  sprintf(cmd, "AT+QSSLSEND=0,%d", *len); 
  if(FALSE == sendATCommand(cmd,">",2,0)) return 1;
	if(FALSE == sendPacket((const char*)mqttbuf, "SEND OK", *len))  return 2;
  
  return 0;
} 

u8 mqtt_publish(char* payload,uint16_t TxLen,int *len,char *idStr)
{
    MQTTString topicString = MQTTString_initializer;
    char cmd[18];
    int buflen = 1400;
    int payloadlen = TxLen;//strlen(payload);
    
  #ifdef M2MECLIPSE     
    topicString.cstring = idStr; //"yzk/control";
  #else
    ASCIICCID[16]=0;
    //data.clientID.cstring=ASCIICCID+4;
    topicString.cstring = ASCIICCID+4; //"yzk/control";
  #endif
   
    strcat((char*)topicString.cstring, "/u");
    *len+= MQTTSerialize_publish((unsigned char *)(mqttbuf+*len), buflen- *len, 0, 0, 0, 0, topicString, (unsigned char *)payload, payloadlen);
    
    sprintf(cmd, "AT+QSSLSEND=0,%d", *len); 
    if(FALSE == sendATCommand(cmd,">",3,3)) return 1;
    if(FALSE == sendPacket((const char*)mqttbuf, "SEND OK", *len))  return 2;
    return 0;
} 

u8 mqtt_disconnect(int *len)
{
    char cmd[18];
    int buflen = 1400;
    
    *len = MQTTSerialize_disconnect((unsigned char *)(mqttbuf ), buflen - *len);
    
    sprintf(cmd, "AT+QSSLSEND=0,%d", *len); 
    if(FALSE == sendATCommand(cmd,">",1,0)) return 1;
    if(FALSE == sendPacket((const char*)mqttbuf, "SEND OK", *len))  return 2;
  
    return 0;
} 

u8 mqtt_pub(char* payload,uint16_t TxLen)
{
	char idStr[30]="3a99c9000004";
	int rc = 0;
	int mysock = 0;
	int len = 0;
   
  if(!OnlineFlag)
  {  
      mysock=mqtt_open(payload,TxLen);
      if(mysock <= 0)
      {  
        return FALSE;
      } 
  }   
  
  if(!OnlineFlag)
  {  
      OnlineFlag=1;
     rc=mqtt_connect(payload,TxLen,&len,idStr);
  }
  else if(OnlineFlag==1)
  {  
     rc=mqtt_publish(payload,TxLen,&len,idStr);
  } 
  else  
  {  
     rc=mqtt_disconnect(&len);
     if(rc)
     {  
         if(FALSE == transport_close(  ))
         {
            rc=3;
         }
     }    
     else
     {
        OnlineFlag=0;
        mysock=mqtt_open(payload,TxLen);
        if(mysock <= 0)
        {  
          return FALSE;
        } 
        rc=mqtt_connect(payload,TxLen,&len,idStr);
        OnlineFlag=1;
     } 
  }  

	if (rc == 0)
		return TRUE;
	else
  {  
     OnlineFlag=2;
		 return FALSE;
  } 
}


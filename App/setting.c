#include "setting.h"
#include "stm320518_i2c_ee.h"
#include "string.h"
#include "stdlib.h"
#include "24cxx.h" 
#define RCVBUFSIZE 256

uint16_t pos = 20;
uint8_t counter = 0;

struct serial srl;
struct cloudServer csr;
stConfig_Base stConfigBase;
stPoint_Base  stNorPoint[25];
stPoint_Base  stEventPoint[25];

uint8_t ucSendVaule_FLAG=1;
uint8_t ucHeartSendFlag=0;
uint8_t QSN_Flag=0;
uint8_t ucNorcnt=0;
uint8_t	ucEventcnt=0; 
uint8_t ucHeart_FLAG=0;
uint8_t Heart_time;
uint8_t hCCID[6];
uint8_t num;
char ASCIICCID[17]={"iot/"};
extern uint8_t TxBuffer[];
extern uint16_t NbrOfDataToTransfer;
float str2folat(char *str);
stPoint_Base stPointBase;

void TCP_Config_TimPro(void)
{  
    static uint8_t Device_Status=0; 
    static uint16_t timercount=0;
    static uint8_t TIMCNT[2]={0};
    static uint8_t CSQ_TIM[2]={0};
    static uint16_t Heart_TIM=0;
    static uint16_t Vaule_TIM=0;
    
    if(ucHeart_FLAG)
    {
        Heart_TIM++;
        if(Heart_TIM>csr.heartbeat_cycle)
        {
            ucHeartSendFlag=1;
            Heart_TIM=0;
        }  
       
        if(ucHeart_FLAG==Heart_STATE_WAITING)
        {
            TIMCNT[1]++;
            if(TIMCNT[1]>250)
            {
               TIMCNT[1]=0;
               ucHeart_FLAG=Heart_STATE_ERROR;					 
            }
        }	
        else if(ucHeart_FLAG==Heart_STATE_ERROR)
        {
            if(!Device_Status)
            {
                Device_Status=1;
            }	
        }	
        
        if(ucHeart_FLAG==Heart_STATE_ON)
        {
            TIMCNT[1]=0;
            Heart_TIM=0;
            if(Device_Status)
            {// ucN_ch395Send_FLAG=ZX_HeartFlag;
                Device_Status=0;
            }	 
            else
            {	 
                ucHeart_FLAG=Heart_STATE_WAITING;
            } 
        }	
        else if(ucHeart_FLAG==Heart_STATE_UP)
        {
             ucHeart_FLAG=Heart_STATE_WAITING;
        } 
    }  
        
//    if(ucSendTim_FLAG==1)//发送数据扫描间隔
//    {	          
//        ucSendTim_FLAG=0;   
//        if(GPIO_ReadOutputDataBit(LED2_GPIO_PORT,LED2_PIN))
//        {
//            STM_RTU_LEDOff(LED2);
//        }                     
//    }
    if(!ucSendVaule_FLAG)
    {
        Vaule_TIM++;
        if(Vaule_TIM>csr.indvalue_cycle)
        {
           Vaule_TIM=0;
           ucSendVaule_FLAG=1;
        }  
    }  
        
    if(QSN_Flag)
    {  
        if(QSN_Flag==1)
        {  
            CSQ_TIM[0]++;
            if(CSQ_TIM[0]==200)
            {  
                CSQ_TIM[0]=0;
                if( GPIO_ReadOutputDataBit(LED3_GPIO_PORT,LED3_PIN))
                {
                    STM_RTU_LEDOff(LED3);
                }
                else
                {
                     STM_RTU_LEDOn(LED3);						 
                } 
            } 
         } 
         else if(QSN_Flag==2)
         {
            CSQ_TIM[1]++;
            if(CSQ_TIM[1]==50)
            {  
                CSQ_TIM[1]=0;
                if( GPIO_ReadOutputDataBit(LED3_GPIO_PORT,LED3_PIN))
                {
                    STM_RTU_LEDOff(LED3);
                }
                else
                {
                    STM_RTU_LEDOn(LED3);				 
                } 
             } 
         }  
     }      
}

uint8_t point_Pro(char *p,stPoint_Base code,uint8_t *tempcount,uint8_t forsetpos,uint8_t *tempcompare)
{
	 int i,k;
	 char* p1,* p2;
	 uint8_t num;
	 uint8_t n;
   char A_Modulus[10];
	 char B_Modulus[10];
	 //static uint8_t tempcompare=0;	
	
    p = (char*)strtok(NULL, "|");
		code.regAllNum = atoi(p);
		p = (char*)strtok(NULL, "|");
		n = atoi(p);
	  (*tempcompare)+=n;
		for(i = 0; i < n; i++)
		{
				memset(A_Modulus, 0, 10);
				memset(B_Modulus, 0, 10);
				p =  (char*)strtok(NULL, "|");
				p1 = p;
				code.regType = atoi(p1);
				p1 = (char*)strchr(p1, ',') + 1;
				code.slaveID = atoi(p1);
				p1 = (char*)strchr(p1, ',') + 1;
				code.startAddr = atoi(p1);
				p2=p1;
				p1 = (char*)strchr(p1, ',');
				for(k=0;k<p1-p2;k++)
				{
						if(*(p2+k)=='.')
						{										
								code.datadigit = atoi((char*)strchr(p2, '.') + 1);
								//i--;
								break;
						}	
				}
				if(k>=p1-p2)
				{
						code.datadigit=0xff;
				}	
				p1++;				
				code.dataType = atoi(p1);						
				p1 = (char*)strchr(p1, ',') + 1;
				code.dataDevice=atoi(p1);
				p1 = (char*)strchr(p1, '.') + 1;
				code.dataChannl=atoi(p1);
				p1 = (char*)strchr(p1, ',') + 1;
				p2 = (char*)strchr(p1, ',');
				num=p2-p1;
				strncpy(A_Modulus,p1,num);
				p1 = (char*)strchr(p1, ',') + 1;
				p2=p1;
				for(num=0;num<10;num++)
				{
					 if(*((uint8_t*) p2)==',')
					 {
							 break;
					 }
					 else
					 {
							B_Modulus[num]=*(uint8_t*)p2;
					 } 												 
					 p2++;
				}
				p1 = (char*)strchr(p1, ',') + 1;
				code.filter=atoi(p1);
				if(forsetpos)
        {				
            p1 = (char*)strchr(p1, ',') + 1;					
						code.EventVaule=atoi(p1);
				}	
        if(!forsetpos)
        {  
           // stNorPoint[i] = malloc(sizeof(stPoint_Base));	
            code.Afloat=str2folat(A_Modulus);
            code.Bfloat=str2folat(B_Modulus);
            memcpy(&stNorPoint[(*tempcount)+i],&code,sizeof(stPoint_Base));
            
				}
        else
        {
           code.Afloat=str2folat(A_Modulus);
           code.Bfloat=str2folat(B_Modulus);	
           memcpy(&stEventPoint[(*tempcount)+i],&code,sizeof(stPoint_Base));
        }  	
		}		
    *tempcount= (*tempcount)+ n;
   // AT24CXX_Write(forsetpos,tempcount,1);
    if(*tempcompare==code.regAllNum) return 1;
		else return 0;
}

float str2folat(char *str)
{
	  float refloati,refloatd;
	  char* p1;
	  char* p2;
	  uint8_t num,i,negative=0;
	  p2=str;
	  if((*str)=='-')
    {
				p2=str+1;
			  negative=1;
		}
    else
    {
		   if(*str=='0')
			 {
				   if(*(str+1)!='.')
			     return  0;
			 }	 		 
		}			
    
    //有问题要改
    
	  p1 = (char*)strchr(p2, '.');
		if(p1)
		{	
				num=p1-p2;
				
				switch(num)
				{
					 case 1: refloati=*p2-0x30; break;
					 case 2: refloati=(*(p2+0)-0x30)*10+(*(p2+1)-0x30); break;
					 case 3: refloati=(*(p2+0)-0x30)*100+(*(p2+1)-0x30)*10+(*(p2+2)-0x30);	break;
					 case 4: refloati=(*(p2+0)-0x30)*1000+(*(p2+1)-0x30)*100\
													 +(*(p2+2)-0x30)*10+(*(p2+3)-0x30);	 break;
					 case 5: refloati=(*p2-0x30)*10000+(*(p2+1)-0x30)*1000+(*(p2+2)-0x30)*100\
													 +(*(p2+3)-0x30)*10+(*(p2+4)-0x30);	break;				
				}	
				for(i=0;i<4;i++)
				{
						if(*(p2+num+i+1)==0||*(p2+num+i+1)=='\r')
						{
							 break; 
						}	
				}	
				switch(i)
				{	
						case 1:
						refloatd=(*(p2+num+1)-0x30)*1000;
						break;
						case 2:
						refloatd=(*(p2+num+1)-0x30)*1000+(*(p2+num+2)-0x30)*100;
						break;
						case 3:
						refloatd=(*(p2+num+1)-0x30)*1000+(*(p2+num+2)-0x30)*100+\
								(*(p2+num+3)-0x30)*10;
						break;
						case 4:
						refloatd=((*(p2+num+1)-0x30)*1000+(*(p2+num+2)-0x30)*100+\
								(*(p2+num+3)-0x30)*10+(*(p2+num+4)-0x30));
					
						break;
				}		
				refloatd/=1000;		
			  refloatd/=10;
				refloati+=refloatd;	
				if(negative) refloati*=-1;
		}	
		else
    {
			  for(num=0;num<5;num++)
				{
					 if(*((uint8_t*) p2+num)==0 || *((uint8_t*) p2+num)=='\r')
					 {
							break;
					 }	 
				}
				switch(num)
				{
					 case 1: refloati=*p2-0x30; break;
					 case 2: refloati=(*(p2+0)-0x30)*10+(*(p2+1)-0x30); break;
					 case 3: refloati=(*(p2+0)-0x30)*100+(*(p2+1)-0x30)*10+(*(p2+2)-0x30);	break;
					 case 4: refloati=(*(p2+0)-0x30)*1000+(*(p2+1)-0x30)*100\
													 +(*(p2+2)-0x30)*10+(*(p2+3)-0x30);	 break;
					 case 5: refloati=(*p2-0x30)*10000+(*(p2+1)-0x30)*1000+(*(p2+2)-0x30)*100\
													 +(*(p2+3)-0x30)*10+(*(p2+4)-0x30);	break;				
				}	
		    if(negative) refloati*=-1;
		}			
		
		return refloati;
}	

uint8_t ReceiveSetting(void)
{
	  char  rxBuf[RCVBUFSIZE] = {""};
		char tmpBuf[40];
	  char* p;
	  char id; 
		uint8_t len,k;
		uint8_t temppos;
	  time_t ts;
    stPoint_Base code;
	  uint16_t NumDataRead = 1;
   
		USART_Cmd(USART2, ENABLE);		
    USART_ITConfig(RTU_COM_2, USART_IT_RXNE, ENABLE);
    while(runningFlag);
		 USART_Cmd(USART2, ENABLE);
    runningFlag = TRUE;
    myRN = 0;
    bRecord = TRUE;
    
    memcpy(rxBuf, RxBuffer, strlen((char*)RxBuffer)-2);
		if(0 == strncmp((char*)rxBuf, "Finish\r\n", 8))
		{			
        bsp_DelayMS(500);	 
       AT24CXX_Write(0,&ucNorcnt,1);
       bsp_DelayMS(500);	 
       AT24CXX_Write(1,&ucEventcnt,1);         
			 strcpy((char*)TxBuffer, "OK\r\n");
			 NbrOfDataToTransfer = 4;
			 runningFlag = TRUE;
			 USART_ITConfig(RTU_COM_2, USART_IT_TXE, ENABLE);
			 while(runningFlag);
			 runningFlag = TRUE;
			 pos=20;
			 return 0;
		} 
		else if(0 == strncmp((char*)rxBuf, "11|",3))
		{
		    temppos=0;
			// sprintf(tmpBuf, "%d",srl.workmode);
		  // len=strlen((char*)tmpBuf);
			// temppos=len;
//			 strcpy((char*)TxBuffer,(char*)tmpBuf);		 
//			 strcat((char*)TxBuffer+temppos, "|");
//			 temppos +=len;
//			 tmpBuf[0]=0;
			
			 sprintf(tmpBuf, "%d",srl.baudRate);
			 len=strlen((char*)tmpBuf);
			 strcpy((char*)TxBuffer+temppos,(char*)tmpBuf);
     	 temppos+=len;	
			 strcat((char*)TxBuffer+temppos, "|");
			 temppos++;
			 tmpBuf[0]=0;
			
			 sprintf(tmpBuf, "%d",srl.parity);
			 len=strlen((char*)tmpBuf);
			 strcpy((char*)TxBuffer+temppos,(char*)tmpBuf);	
       temppos+=len;						
			 strcat((char*)TxBuffer+temppos, "|");
			 temppos++;
			 tmpBuf[0]=0;
			 
			 sprintf(tmpBuf, "%d",srl.wordLength);
			 len=strlen((char*)tmpBuf);
			 strcpy((char*)TxBuffer+temppos,(char*)tmpBuf);		
			 temppos+=len;			
			 strcat((char*)TxBuffer+temppos, "|");
			 temppos++;
			 tmpBuf[0]=0;
			 
			 sprintf(tmpBuf, "%d",srl.stopBits);
			 len=strlen((char*)tmpBuf);
			 strcpy((char*)TxBuffer+temppos,(char*)tmpBuf);		
			 temppos+=len;				
			 strcat((char*)TxBuffer+temppos, "\r\n");
			 temppos+=2;		
			
			 NbrOfDataToTransfer = temppos;
			 runningFlag = TRUE;
			 USART_ITConfig(RTU_COM_2, USART_IT_TXE, ENABLE);
				while(runningFlag);
				runningFlag = TRUE;
					return 1;
		}	
		else if(0 == strncmp((char*)rxBuf, "12|", 3))
    {
			  temppos=0;
			
				len=strlen((char*)csr.CCID);
        if(len>12)len=len>12?12:len;
				temppos +=len;
				strcpy((char*)TxBuffer,(char*)csr.CCID);		 
				strcpy((char*)TxBuffer+temppos, "|");
				temppos++;
			  tmpBuf[0]=0;
      
        sprintf(tmpBuf, "%d",csr.filter_flag);
				len=strlen((char*)tmpBuf);
				strcpy((char*)TxBuffer+temppos,(char*)tmpBuf);		
				temppos+=len;			
				strcat((char*)TxBuffer+temppos, "|");
				temppos++;
				tmpBuf[0]=0;
			
			  sprintf(tmpBuf, "%d",csr.heartbeat_cycle);
				len=strlen((char*)tmpBuf);
				strcpy((char*)TxBuffer+temppos,(char*)tmpBuf);		
				temppos+=len;			
				strcat((char*)TxBuffer+temppos, "|");
				temppos++;
				tmpBuf[0]=0;
			
			  sprintf(tmpBuf, "%d",csr.indvalue_cycle);
				len=strlen((char*)tmpBuf);
				strcpy((char*)TxBuffer+temppos,(char*)tmpBuf);		
				temppos+=len;			
				strcat((char*)TxBuffer+temppos, "|");
				temppos++;
				tmpBuf[0]=0;				
		
				strcpy((char*)TxBuffer+temppos,(char*)(csr.name));		
				len=strlen((char*)csr.name);		
				temppos+=len;					
				strcat((char*)TxBuffer+len, "|");		
				temppos++;
			
			  sprintf(tmpBuf, "%d",csr.port);
				len=strlen((char*)tmpBuf);
				strcpy((char*)TxBuffer+temppos,(char*)tmpBuf);		
				temppos+=len;			
				strcat((char*)TxBuffer+temppos, "|");
				temppos++;
				tmpBuf[0]=0;
							
				strcpy((char*)TxBuffer+temppos,(char*)(csr.username));		
				len=strlen((char*)csr.username);	
				temppos+=len;										
				strcat((char*)TxBuffer+len, "|");		
				temppos++;
				
				strcpy((char*)TxBuffer+temppos,(char*)(csr.password));		
				len=strlen((char*)csr.password);	
				temppos+=len;										
				strcat((char*)TxBuffer+temppos, "\r\n");
				temppos+=2;		
				
				NbrOfDataToTransfer = temppos;
				runningFlag = TRUE;
				USART_ITConfig(RTU_COM_2, USART_IT_TXE, ENABLE);
				 while(runningFlag);
				runningFlag = TRUE;
					return 1;
		}			
    p = (char*)strtok((char*)rxBuf, "|");
    id = *p;
    switch(id)
    {
    case '1'://串口格式
        {
					  p = (char*)strtok(NULL, "|");
            srl.baudRate = atoi(p);
            p = (char*)strtok(NULL, "|");
            srl.parity = atoi(p);
            p = (char*)strtok(NULL, "|");
            srl.wordLength = atoi(p);
            p = (char*)strtok(NULL, "|");
            srl.stopBits = atoi(p);
					  AT24CXX_Write(pos,(uint8_t*)&srl, sizeof(struct serial));					 
					  pos += sizeof(struct serial);			
            ucNorcnt=0;     
            ucEventcnt=0;          
            strcpy((char*)TxBuffer, "OK\r\n");
            NbrOfDataToTransfer = 4;
            runningFlag = TRUE;
            USART_ITConfig(RTU_COM_2, USART_IT_TXE, ENABLE);
            while(runningFlag);
					  runningFlag = TRUE;				  
        }
        break;
    case '2'://用户信息
        {
            memset((uint8_t*)&csr,0,199); 
					  p = (char*)strtok(NULL, "|");
            strcpy(csr.CCID, p);
            p = (char*)strtok(NULL, "|");
					  csr.filter_flag=atoi(p);
					  p = (char*)strtok(NULL, "|");
					  csr.heartbeat_cycle=atoi(p);
					  p = (char*)strtok(NULL, "|");
					  csr.indvalue_cycle=atoi(p);							
            p = (char*)strtok(NULL, "|");
            strcpy(csr.name, p);
            p = (char*)strtok(NULL, "|");
            csr.port = atoi(p);
            p = (char*)strtok(NULL, "|");
            strcpy(csr.username, p);
            p = (char*)strtok(NULL, "|");
            strcpy(csr.password, p);
					  p = (char*)strtok(NULL, "\n");				
					  AT24CXX_Write(pos,(uint8_t*)&csr, sizeof(struct cloudServer));
					  pos += sizeof(struct cloudServer);
					  strcpy((char*)TxBuffer, "OK\r\n");
            NbrOfDataToTransfer = 4;
            runningFlag = TRUE;
            USART_ITConfig(RTU_COM_2, USART_IT_TXE, ENABLE);
             while(runningFlag);
					  runningFlag = TRUE;						
        }
        break;
    case '3'://
        {
            static uint8_t tempnum1;
            uint16_t num;
					  if(point_Pro(p,code,&ucNorcnt,0,&tempnum1)) 
						{					
                num=  sizeof(stPoint_Base)*tempnum1;            
                AT24CXX_Write(pos,(uint8_t*)stNorPoint,num);
								pos+=num;
                tempnum1=0;
                strcpy((char*)TxBuffer, "OK\r\n");	
						}	
            NbrOfDataToTransfer = 4;
            runningFlag = TRUE;
            USART_ITConfig(RTU_COM_2, USART_IT_TXE, ENABLE);
             while(runningFlag);
            runningFlag = TRUE;	            
        }
        return 1;
     case '4'://
        {
					  static uint8_t tempnum2;
            uint16_t num;
					  if(point_Pro(p,code,&ucEventcnt,1,&tempnum2)) 
						{	
                num=  sizeof(stPoint_Base)*tempnum2;    
                AT24CXX_Write(pos,(uint8_t*)stEventPoint, num);
								tempnum2=0;
                strcpy((char*)TxBuffer, "OK\r\n");
						}	
            NbrOfDataToTransfer = 4;
            runningFlag = TRUE;
            USART_ITConfig(RTU_COM_2, USART_IT_TXE, ENABLE);
             while(runningFlag);
            runningFlag = TRUE;	
        }	
        return 1;
    }
}

void LoadSetting(void)
{   
	  uint8_t i,j=0;
    uint16_t NumDataRead = 1;  
  
    AT24CXX_Read(0,&ucNorcnt,NumDataRead);
	  AT24CXX_Read(1,&ucEventcnt,NumDataRead);
    NumDataRead = sizeof(struct serial);
    AT24CXX_Read( pos,(uint8_t*)&srl, NumDataRead);
    pos += sizeof(struct serial);
    NumDataRead = sizeof(struct cloudServer);
    AT24CXX_Read(pos,(uint8_t*)&csr,  NumDataRead);
    pos += sizeof(struct cloudServer);
    //WorkMode=srl.workmode;
	//ASCIICCID[]
	  csr.heartbeat_cycle=csr.heartbeat_cycle?csr.heartbeat_cycle:60;
    csr.heartbeat_cycle*=100;
    csr.indvalue_cycle=csr.indvalue_cycle?csr.indvalue_cycle:60;
    csr.indvalue_cycle*=100;
	  memcpy(ASCIICCID+4,csr.CCID,12);
	  //strcat((char*)ASCIICCID, "/u");
		for(i=0;i<12;i+=2)
    {		 
				if(csr.CCID[i]>=0x61)
				{
						hCCID[j]=csr.CCID[i]-0x57;
				}	
				else if(csr.CCID[i]>=0x41)
				{
				   	hCCID[j]=csr.CCID[i]-0x37;
				}	
				else if(csr.CCID[i]>=0x30)
				{
				   	hCCID[j]=csr.CCID[i]-0x30;
				}	
				if(csr.CCID[i+1]>=0x61)
				{  
					  hCCID[j]<<=4;
						hCCID[j]+=csr.CCID[i+1]-0x57;
				}	
				else if(csr.CCID[i+1]>=0x41)
				{
					  hCCID[j]<<=4;
				   	hCCID[j]+=csr.CCID[i+1]-0x37;
				}	
				else if(csr.CCID[i+1]>=0x30)
				{
					  hCCID[j]<<=4;
				   	hCCID[j]+=csr.CCID[i+1]-0x30;
				}	
				j++;
		}	
     for(i = 0; i < ucNorcnt; i++)
    {
        NumDataRead = sizeof(stPoint_Base);
       // stNorPoint[i] = malloc(NumDataRead);			
			  AT24CXX_Read(pos,(uint8_t*)&stNorPoint[i],NumDataRead);		 
        pos += sizeof(stPoint_Base);		
    }
		for(i = 0; i < ucEventcnt; i++)
    {
        NumDataRead = sizeof(stPoint_Base);
        //stEventPoint[i] = malloc(NumDataRead);			
			  AT24CXX_Read(pos,(uint8_t*)&stEventPoint[i],NumDataRead);		  
        pos += sizeof(stPoint_Base);		
    }	
		pos=20;
}

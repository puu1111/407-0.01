#include "Com_Setting.h"
#include <string.h>
#include <stdlib.h>

struct serial srl;
struct cloudServer csr;


stuUSARTx *MOD_Set=&stuUSART1;

stConfig_Base stConfigBase;
stPoint_Base  stNorPoint[500];
stPoint_Base  stEventPoint[500];
stTim_TaskCycle  stTimTaskCycle;
u8 tempnum[2]={0};
//u8 RxBuffer[100];
u32  Flash_Pos=20;
u8 ASCIICCID[17]={"iot/"};
u8 hexCCID[6];
u8 regAllNum;
u8 rnCount=0;
u16 ucNorcnt=0;
u16 ucEventcnt=0;
u8 temprx[300]={0};
#define TEXT_LENTH(x) sizeof(x)	 		  	//数组长度	
#define SIZE(x) (TEXT_LENTH(x)/4+((TEXT_LENTH(x)%4)?1:0))



float str2folat(char *str)
{
	  float refloati,refloatd;
	  char* p1;
	  char* p2;
	  u8 num,i,negative=0;
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
					 if(*((u8*) p2+num)==0 || *((u8*) p2+num)=='\r')
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

u8 point_Pro(char *p,stPoint_Base code,u16 *tempcount,u8 forsetpos,u8 *tempcompare)
{
	 int i,k;
	 char* p1,* p2;
	 u8 num;
	 u8 n;
   char A_Modulus[10];
	 char B_Modulus[10];
	 //static u8 tempcompare=0;	
	
    p = (char*)strtok(NULL, "|");
		regAllNum = atoi(p);
		p = (char*)strtok(NULL, "|");
		n = atoi(p);
	  *tempcompare=(*tempcompare)+n;
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
					 if(*((u8*) p2)==',')
					 {
							 break;
					 }
					 else
					 {
							B_Modulus[num]=*(u8*)p2;
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
    if(*tempcompare==regAllNum) return 1;
		else return 0;
}

u8 ReceiveSetting(void)
{
	  char  rxBuf[255] = {""};
		char tmpBuf[40];
	  char* p,*p1;
	  char id; 
		u8 len;
		u8 temppos;
	  u32 encodenum;
    stPoint_Base code;


    while(UartRunFlag);	
    UartRunFlag = TRUE;
    MOD_Set->bRecord = TRUE;
    
    memcpy(rxBuf, MOD_Set->RX_BUF, 257);
		if(0 == strncmp((char*)rxBuf, "Finish\r\n", 8))
		{			
       //delay_ms(500);	 
       STMFLASH_Write(Ponit_BASE,(u32 *)&ucNorcnt,1);
       delay_ms(5);	 
       STMFLASH_Write(Ponit_BASE+4,(u32 *)&ucEventcnt,1);       
			 strcpy((MOD_Set->TX_BUF), "OK\r\n");			 
       MYDMA_Enable(DMA2_Stream7,4);  
			 Flash_Pos=20;
			 return 0;
		} 
		else if(0 == strncmp((char*)rxBuf, "11|",3))
		{
		   temppos=0;
			
			 sprintf(tmpBuf, "%d",srl.baudRate);
			 len=strlen((char*)tmpBuf);
			 strcpy((char*)MOD_Set->TX_BUF+temppos,(char*)tmpBuf);
     	 temppos+=len;	
			 strcat((char*)MOD_Set->TX_BUF+temppos, "|");
			 temppos++;
			 tmpBuf[0]=0;
			
			 sprintf(tmpBuf, "%d",srl.parity);
			 len=strlen((char*)tmpBuf);
			 strcpy((char*)MOD_Set->TX_BUF+temppos,(char*)tmpBuf);	
       temppos+=len;						
			 strcat((char*)MOD_Set->TX_BUF+temppos, "|");
			 temppos++;
			 tmpBuf[0]=0;
			 
			 sprintf(tmpBuf, "%d",srl.wordLength);
			 len=strlen((char*)tmpBuf);
			 strcpy((char*)MOD_Set->TX_BUF+temppos,(char*)tmpBuf);		
			 temppos+=len;			
			 strcat((char*)MOD_Set->TX_BUF+temppos, "|");
			 temppos++;
			 tmpBuf[0]=0;
			 
			 sprintf(tmpBuf, "%d",srl.stopBits);
			 len=strlen((char*)tmpBuf);
			 strcpy((char*)MOD_Set->TX_BUF+temppos,(char*)tmpBuf);		
			 temppos+=len;				
			 strcat((char*)MOD_Set->TX_BUF+temppos, "\r\n");
			 temppos+=2;		
			
			 MOD_Set->TX_len[1] = temppos;
       MYDMA_Enable(DMA2_Stream7,MOD_Set->TX_len[1]);  
					return 1;
		}	
		else if(0 == strncmp((char*)rxBuf, "12|", 3))
    {
			  temppos=0;
			
				len=strlen((char*)csr.CCID);
        if(len>12)len=len>12?12:len;
				temppos +=len;
				strcpy((char*)MOD_Set->TX_BUF,(char*)csr.CCID);		 
				strcpy((char*)MOD_Set->TX_BUF+temppos, "|");
				temppos++;
			  tmpBuf[0]=0;
      
        sprintf(tmpBuf, "%d",csr.filter_flag);
				len=strlen((char*)tmpBuf);
				strcpy((char*)MOD_Set->TX_BUF+temppos,(char*)tmpBuf);		
				temppos+=len;			
				strcat((char*)MOD_Set->TX_BUF+temppos, "|");
				temppos++;
				tmpBuf[0]=0;
			
			  sprintf(tmpBuf, "%d",csr.heartbeat_cycle);
				len=strlen((char*)tmpBuf);
				strcpy((char*)MOD_Set->TX_BUF+temppos,(char*)tmpBuf);		
				temppos+=len;			
				strcat((char*)MOD_Set->TX_BUF+temppos, "|");
				temppos++;
				tmpBuf[0]=0;
			
			  sprintf(tmpBuf, "%d",csr.indvalue_cycle);
				len=strlen((char*)tmpBuf);
				strcpy((char*)MOD_Set->TX_BUF+temppos,(char*)tmpBuf);		
				temppos+=len;			
				strcat((char*)MOD_Set->TX_BUF+temppos, "|");
				temppos++;
				tmpBuf[0]=0;				
		
				strcpy((char*)MOD_Set->TX_BUF+temppos,(char*)(csr.name));		
				len=strlen((char*)csr.name);		
				temppos+=len;					
				strcat((char*)MOD_Set->TX_BUF+len, "|");		
				temppos++;
			
			  sprintf(tmpBuf, "%d",csr.port);
				len=strlen((char*)tmpBuf);
				strcpy((char*)MOD_Set->TX_BUF+temppos,(char*)tmpBuf);		
				temppos+=len;			
				strcat((char*)MOD_Set->TX_BUF+temppos, "|");
				temppos++;
				tmpBuf[0]=0;
							
				strcpy((char*)MOD_Set->TX_BUF+temppos,(char*)(csr.username));		
				len=strlen((char*)csr.username);	
				temppos+=len;										
				strcat((char*)MOD_Set->TX_BUF+len, "|");		
				temppos++;
				
				strcpy((char*)MOD_Set->TX_BUF+temppos,(char*)(csr.password));		
				len=strlen((char*)csr.password);	
				temppos+=len;										
				strcat((char*)MOD_Set->TX_BUF+temppos, "\r\n");
				temppos+=2;		
				
				MOD_Set->TX_len[1] = temppos;
        MYDMA_Enable(DMA2_Stream7,MOD_Set->TX_len[1]);  
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
            encodenum=SIZE(struct serial);
            STMFLASH_Erase(ADDR_FLASH_SECTOR_7);//VCC=2.7~3.6V之间!!
					  STMFLASH_Write(Ponit_BASE+Flash_Pos,(u32*)&srl, encodenum);		
            //STMFLASH_Read(Ponit_BASE+Flash_Pos,(u32 *)&srl,  encodenum);           
					  Flash_Pos += encodenum*4;			
            ucNorcnt=0;     
            ucEventcnt=0;                            
            strcpy(MOD_Set->TX_BUF, "OK\r\n");	
            MYDMA_Enable(DMA2_Stream7,4);   
        }
        break;
    case '2'://用户信息
        {
            memset((u8*)&csr,0,199); 
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
            p1=p;
            p = (char*)strchr(p, '\r');	
            *p=0;
            strcpy(csr.password, p1);
					 // p = (char*)strtok(NULL, 0x0d);			
            encodenum=SIZE(struct cloudServer);            
					  STMFLASH_Write(Ponit_BASE+Flash_Pos,(u32*)&csr, encodenum);
					 // STMFLASH_Read(Ponit_BASE+Flash_Pos,(u32 *)&csr,  encodenum);         
            Flash_Pos += encodenum*4;			
					  strcpy(MOD_Set->TX_BUF, "OK\r\n");	
            MYDMA_Enable(DMA2_Stream7,4);  
        }
        break;
    case '3'://
        {
            //static u8 tempnum1=0;
					  if(point_Pro(p,code,&ucNorcnt,0,tempnum)) 
						{					
               // encodenum=sizeof(stPoint_Base);
                encodenum=SIZE(stPoint_Base);  
                encodenum*=tempnum[0];             
                STMFLASH_Write(Ponit_BASE+Flash_Pos,(u32*)stNorPoint,encodenum);
								Flash_Pos+=encodenum*4;			
                tempnum[0]=0;
						}	
            strcpy(MOD_Set->TX_BUF, "OK\r\n");	
            MYDMA_Enable(DMA2_Stream7,4);     
        }
        return 1;
     case '4'://
        {
					  //static u8 tempnum2=0;
//            u16 num;
					  if(point_Pro(p,code,&ucEventcnt,1,tempnum+1)) 
						{	
                //encodenum=sizeof(stPoint_Base);
                encodenum=SIZE(stPoint_Base);
                encodenum*=tempnum[1];                 
                STMFLASH_Write(Ponit_BASE+Flash_Pos,(u32*)stEventPoint,encodenum);
								tempnum[1]=0;
						}	
            strcpy(MOD_Set->TX_BUF, "OK\r\n");	
            MYDMA_Enable(DMA2_Stream7,4);  
        }	
        return 1;
     default: return 1;
    }
    return 1;
}

void LoadSetting(void)
{   
	  uint8_t i,j=0;
    uint32_t NumDataRead = 1;  
  
    STMFLASH_Read(Ponit_BASE,(u32 *)&ucNorcnt,1);
	  STMFLASH_Read(Ponit_BASE+4,(u32 *)&ucEventcnt,1);
    if(ucNorcnt!=0xffff && ucEventcnt!=0xffff)
    {  
        //NumDataRead = sizeof(struct serial);
        NumDataRead=SIZE(struct serial);
        STMFLASH_Read(Ponit_BASE+Flash_Pos,(u32 *)&srl, NumDataRead);
        Flash_Pos += NumDataRead*4;
        NumDataRead = SIZE(struct cloudServer);
        STMFLASH_Read(Ponit_BASE+Flash_Pos,(u32 *)&csr,  NumDataRead);
        Flash_Pos += NumDataRead*4;
        //WorkMode=srl.workmode;

        csr.heartbeat_cycle=csr.heartbeat_cycle?csr.heartbeat_cycle:60;
        stTimTaskCycle.heart=csr.heartbeat_cycle*1000;
        csr.indvalue_cycle=csr.indvalue_cycle?csr.indvalue_cycle:60;
        stTimTaskCycle.indvalue=csr.indvalue_cycle*1000;
        memcpy(ASCIICCID+4,csr.CCID,12);
        //strcat((char*)ASCIICCID, "/u");
        for(i=0;i<12;i+=2)
        {		 
            if(csr.CCID[i]>=0x61)
            {
                hexCCID[j]=csr.CCID[i]-0x57;
            }	
            else if(csr.CCID[i]>=0x41)
            {
                hexCCID[j]=csr.CCID[i]-0x37;
            }	
            else if(csr.CCID[i]>=0x30)
            {
                hexCCID[j]=csr.CCID[i]-0x30;
            }	
            if(csr.CCID[i+1]>=0x61)
            {  
                hexCCID[j]<<=4;
                hexCCID[j]+=csr.CCID[i+1]-0x57;
            }	
            else if(csr.CCID[i+1]>=0x41)
            {
                hexCCID[j]<<=4;
                hexCCID[j]+=csr.CCID[i+1]-0x37;
            }	
            else if(csr.CCID[i+1]>=0x30)
            {
                hexCCID[j]<<=4;
                hexCCID[j]+=csr.CCID[i+1]-0x30;
            }	
            j++;
        }	
        
        NumDataRead = SIZE(stPoint_Base)*ucNorcnt;
         //for(i = 0; i < ucNorcnt; i++)
        //{      
           // stNorPoint[i] = malloc(NumDataRead);		
        if(NumDataRead)    
        {          
            STMFLASH_Read(Ponit_BASE+Flash_Pos,(u32*)&stNorPoint,NumDataRead);		 
            Flash_Pos += NumDataRead*4;		
        }  
        //}
        
       // for(i = 0; i < ucEventcnt; i++)
        //{       
            //stEventPoint[i] = malloc(NumDataRead);		
            NumDataRead = SIZE(stPoint_Base)*ucEventcnt;  
        if(NumDataRead)    
        {         
            STMFLASH_Read(Ponit_BASE+Flash_Pos,(u32*)&stEventPoint,NumDataRead);		  
            Flash_Pos += NumDataRead;		
        }  
        //}	
    }   
		Flash_Pos=20;
}



u8 SettinRxPro(u8 res,stuUSARTx *uartx)
{  
    static u8 proC=0;
   // temprx[uartx->RX_len[0]]=res;
    uartx->RX_len[0]++;
    if(res=='>'&& uartx->RX_len[0]<0x17 )
    {
        uartx->RX_BUF[0]=res;
        uartx->bRecord = FALSE;
        uartx->RX_len[1]= 0;
        uartx->RX_len[0] = 0;
        rnCount = 0;
        proC=0;
        UartRunFlag = FALSE;
        return 0;
    } 
    else
    {     
        if(uartx->bRecord) uartx->RX_BUF[uartx->RX_len[1]++]=res;
        if(proC == '\r' && res == '\n' && uartx->RX_len[0]>1)
        {
            rnCount++;
            if(rnCount == uartx->mynbase[0] && !uartx->bRecord)
            {
                uartx->bRecord = TRUE;
            }
            else if(!uartx->mynbase[1])
            {  
                if(rnCount == uartx->mynbase[0] + 1)
                {
                    uartx->bRecord = FALSE;
                    uartx->RX_len[1]= 0;
                    uartx->RX_len[0] = 0;
                    rnCount = 0;
                    proC=0;
                    UartRunFlag=FALSE;
                    return 0;
                }
            }
            else 
            {
                if(rnCount == uartx->mynbase[1] + 1)
                {
                    uartx->bRecord = FALSE;
                    uartx->RX_len[1]= 0;
                    uartx->RX_len[0] = 0;
                    rnCount = 0;
                    proC=0;
                    UartRunFlag = FALSE;
                    return 0;
                }  
            }  
        }      
        proC = res;
     }
    return 1;
}


u8 SettinRxPro4G(u8 res,stuUSARTx4G *uartx)
{  
    static u8 proC=0;
    temprx[uartx->RX_len[0]++]=res;
   // uartx->RX_len[0]++;
    if(res=='>'&& uartx->RX_len[0]<0x17 )
    {
        uartx->RX_BUF[0]=res;
        uartx->bRecord = FALSE;
        uartx->RX_len[1]= 0;
        uartx->RX_len[0] = 0;
        rnCount = 0;
        proC=0;
        UartRunFlag = FALSE;
        return 0;
    } 
    else
    {     
        if(uartx->bRecord) uartx->RX_BUF[uartx->RX_len[1]++]=res;
        if(proC == '\r' && res == '\n' && uartx->RX_len[0]>1)
        {
            rnCount++;
            if(rnCount == uartx->mynbase[0] && !uartx->bRecord)
            {
                uartx->bRecord = TRUE;
            }
            else if(!uartx->mynbase[1])
            {  
                if(rnCount == uartx->mynbase[0] + 1)
                {
                    uartx->bRecord = FALSE;
                    uartx->RX_len[1]= 0;
                    uartx->RX_len[0] = 0;
                    rnCount = 0;
                    proC=0;
                    UartRunFlag=FALSE;
                    return 0;
                }
            }
            else 
            {
                if(rnCount == uartx->mynbase[1] + 1)
                {
                    uartx->bRecord = FALSE;
                    uartx->RX_len[1]= 0;
                    uartx->RX_len[0] = 0;
                    rnCount = 0;
                    proC=0;
                    UartRunFlag = FALSE;
                    return 0;
                }  
            }  
        }      
        proC = res;
     }
    return 1;
}

u8 ConFigWait(void)
{
    if(FALSE == UartRunFlag)
    {
        if(0 == strncmp((char*)MOD_Set->RX_BUF, "DTU:CONFIG", 10))
        {
					  strcpy(MOD_Set->TX_BUF, "OK\r\n");	
            MYDMA_Enable(DMA2_Stream7,4);    
            MOD_Set->bRecord=1;
            //MOD_Set->TX_STATION = 1;
            while(MOD_Set->TX_STATION);
            return 1;
        }		
    }
    return 0;
}

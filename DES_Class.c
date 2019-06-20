#include "stm32f10x.h"
#include <stdio.h>
#include <absacc.h>
#include "public_define.h"
#include "public.h"
//#include "usb_lib.h"
#include "usb_regs.h"
#include "system_defineDES.h"

extern uint8 sw[3];
extern uint8 Receive_Buffer[Max_Rec];
extern uint8 Transceive_Buffer[Max_Tra];

#define PCReciveData Receive_Buffer
#define Replay_buff Transceive_Buffer

#define Communication_ToPC RS232_SendBuffNoBuff
void e2prom_dowith(uint8 *buf,uint16 len)
{
	uint8 sbuf[1024];
	uint16 marlen[3];
	uint16 i;
	int16 t;
	uint16 add,leng,tmp;
    
    
	switch(buf[8])
	{
//device managment information
//Write product date = 2016-03-18
//aa aa aa 96 69 00 0D DC 00 32 30 31 36 5F 30 33 5F 31 38 DE
        case 0x00:
           add=0; //每100字节为一组信息
           leng=10;
           ReadFlashMyByte(&sbuf[0],leng,add);
           for(i=0;i<leng;)
           {
               if(buf[9+i]!=sbuf[i])break; 
               i+=1;
           }
           if(i>=leng) 
           {
           //不重复写
               sw[0]=0x00;
               sw[1]=0x00;
               sw[2]=0x90;	
               leng=0;
               //ReaderToPCNoData(leng,&sw[0]);
               Communication_ToPC(&sw[0],leng,&Replay_buff[0]);
               break;
           }
           if(WriteFlashMyByte(&buf[9],leng,add)==FLASH_COMPLETE)
           {
               sw[0]=0x00;
               sw[1]=0x00;
               sw[2]=0x90;	
               leng=0;
               //ReaderToPCNoData(leng,&sw[0]);
               Communication_ToPC(&sw[0],leng,&Replay_buff[0]);
           }
           else
           {
               sw[0]=0x00;
               sw[1]=0x00;
               sw[2]=0x91;
               Replay_buff[0]=0x33;
               //Transceive_Buffer[Redata_offset]=0x33;	
               leng=1;
              // ReaderToPCNoData(leng,&sw[0]);              
               Communication_ToPC(&sw[0],leng,&Replay_buff[0]);
           }
           
        
        break;
//read product date = 2016-03-18
//aa aa aa 96 69 00 03 DC 01 DE         
        case 0x01:
           add=0; //每100字节为一组信息
           leng=10;
           sw[0]=0x00;
           sw[1]=0x00;
           sw[2]=0x90;
          // ReadFlashMyByte(&Transceive_Buffer[Redata_offset],leng,add); 
          // ReaderToPCNoData(leng,&sw[0]);  
           ReadFlashMyByte(&Replay_buff[0],leng,add);
           Communication_ToPC(&sw[0],leng,&Replay_buff[0]);      
        break;
        //------------------
//write机身号00ABC00000000001
//aa aa aa 96 69 00 13 DC 02 30 30 41 42 43 30 30 30 30 30 30 30 30 30 30 31 BC      
        case 0x02:
           add=100; //每100字节为一组信息
           leng=16;
           ReadFlashMyByte(&sbuf[0],leng,add);
           for(i=0;i<leng;)
           {
               if(buf[9+i]!=sbuf[i])break; 
               i+=1;
           }
           if(i>=leng) 
           {
           //不重复写
               sw[0]=0x00;
               sw[1]=0x00;
               sw[2]=0x90;	
               leng=0;
               Communication_ToPC(&sw[0],leng,&Replay_buff[0]); 
               //ReaderToPCNoData(leng,&sw[0]);
               break;
           }
           if(WriteFlashMyByte(&buf[9],leng,add)==FLASH_COMPLETE)
           {
               sw[0]=0x00;
               sw[1]=0x00;
               sw[2]=0x90;	
               leng=0;
               Communication_ToPC(&sw[0],leng,&Replay_buff[0]); 
               //ReaderToPCNoData(leng,&sw[0]);
           }
           else
           {
               sw[0]=0x00;
               sw[1]=0x00;
               sw[2]=0x91;
               leng=1;	
              // Transceive_Buffer[Redata_offset]=0x33;	              
              // ReaderToPCNoData(leng,&sw[0]);
               Replay_buff[0]=0x33;
               Communication_ToPC(&sw[0],leng,&Replay_buff[0]); 
           }
           
        
        break;  
//read机身号
//aa aa aa 96 69 00 03 DC 03 DC        
        case 0x03:
           add=100; //每100字节为一组信息
           leng=16;
           sw[0]=0x00;
           sw[1]=0x00;
           sw[2]=0x90;
           //ReadFlashMyByte(&Transceive_Buffer[Redata_offset],leng,add); 
          // ReaderToPCNoData(leng,&sw[0]);
           ReadFlashMyByte(&Replay_buff[0],leng,add);
           Communication_ToPC(&sw[0],leng,&Replay_buff[0]);
        break;
        //------------------
//write设备型号版本00ABC00000000001
//aa aa aa 96 69 00 13 DC 04 30 30 41 42 43 30 30 30 30 30 30 30 30 30 30 31 BA   
        case 0x04:
            add=200; //每100字节为一组信息
           leng=16;
           ReadFlashMyByte(&sbuf[0],leng,add);
           for(i=0;i<leng;)
           {
               if(buf[9+i]!=sbuf[i])break; 
               i+=1;
           }
           if(i>=leng) 
           {
           //不重复写
               sw[0]=0x00;
               sw[1]=0x00;
               sw[2]=0x90;	
               leng=0;
              // ReaderToPCNoData(leng,&sw[0]);
               Communication_ToPC(&sw[0],leng,&Replay_buff[0]);
               break;
           }
           if(WriteFlashMyByte(&buf[9],leng,add)==FLASH_COMPLETE)
           {
               sw[0]=0x00;
               sw[1]=0x00;
               sw[2]=0x90;	
               leng=0;              
               //ReaderToPCNoData(leng,&sw[0]);
               Communication_ToPC(&sw[0],leng,&Replay_buff[0]);
           }
           else
           {
               sw[0]=0x00;
               sw[1]=0x00;
               sw[2]=0x91;
               leng=1;	
              // Transceive_Buffer[Redata_offset]=0x33;	              
              // ReaderToPCNoData(leng,&sw[0]);
               Replay_buff[0]=0x33;	
               Communication_ToPC(&sw[0],leng,&Replay_buff[0]);
           }
        break;
//read设备型号版本
//aa aa aa 96 69 00 03 DC 05 DA         
        case 0x05:
           add=200; //每100字节为一组信息
           leng=16;
           sw[0]=0x00;
           sw[1]=0x00;
           sw[2]=0x90;
          // ReadFlashMyByte(&Transceive_Buffer[Redata_offset],leng,add);
          // ReaderToPCNoData(leng,&sw[0]);
           ReadFlashMyByte(&Replay_buff[0],leng,add);
           Communication_ToPC(&sw[0],leng,&Replay_buff[0]);
        break;
        //---------------
//write出厂日期2016-03-30
//aa aa aa 96 69 00 0D DC 06 32 30 31 36 5F 30 33 5F 31 38 D8        
        case 0x06:
           add=300; //每100字节为一组信息
           leng=10;
           ReadFlashMyByte(&sbuf[0],leng,add);
           for(i=0;i<leng;)
           {
               if(buf[9+i]!=sbuf[i])break; 
               i+=1;
           }
           if(i>=leng) 
           {
           //不重复写
               sw[0]=0x00;
               sw[1]=0x00;
               sw[2]=0x90;	
               leng=0;
               //ReaderToPCNoData(leng,&sw[0]);
               Communication_ToPC(&sw[0],leng,&Replay_buff[0]);
               break;
           }
           if(WriteFlashMyByte(&buf[9],leng,add)==FLASH_COMPLETE)
           {
               sw[0]=0x00;
               sw[1]=0x00;
               sw[2]=0x90;	
               leng=0;
               //ReaderToPCNoData(leng,&sw[0]);
               Communication_ToPC(&sw[0],leng,&Replay_buff[0]);
           }
           else
           {
               sw[0]=0x00;
               sw[1]=0x00;
               sw[2]=0x91;	
               leng=1;
               //Transceive_Buffer[Redata_offset]=0x33;	               
              // ReaderToPCNoData(leng,&sw[0]);
               Replay_buff[0]=0x33;
               Communication_ToPC(&sw[0],leng,&Replay_buff[0]);
           }
        break;
//read出厂日期
//aa aa aa 96 69 00 03 DC 07 D8         
        case 0x07:
            add=300; //每100字节为一组信息
           leng=10;
           sw[0]=0x00;
           sw[1]=0x00;
           sw[2]=0x90;
           //ReadFlashMyByte(&Transceive_Buffer[Redata_offset],leng,add);
           //ReaderToPCNoData(leng,&sw[0]);
           ReadFlashMyByte(&Replay_buff[0],leng,add);
           Communication_ToPC(&sw[0],leng,&Replay_buff[0]);
        break;
//---预留
        case 0xfe://aa aa aa 96 69 00 07 DC fe 56 ab 67 bc 03
            tmp=buf[5];
            tmp=((tmp<<8)&0xff00)+buf[6];
            if((tmp==7)
					 &&(buf[9]==0x56)&&(buf[10]==0xab)
				   &&(buf[11]==0x67)&&(buf[12]==0xbc))
            {
                ClearFlashAppMark();
                sw[2]=0x90;
                leng=0;
            }
            else
            {    sw[2]=0x91;
            	   //Transceive_Buffer[Redata_offset]=0x33;	
            	   Replay_buff[0]=0x33;
                 leng=1;
            }
            sw[0]=0x00;
            sw[1]=0x00;
            //	
            
           // ReaderToPCNoData(leng,&sw[0]);
            Communication_ToPC(&sw[0],leng,&Replay_buff[0]);
        break;
        case 0xcc://磁条
        	//磁条卡关闭看门狗
        	  tmp=buf[9];
        	  if(tmp>65)tmp=65000;
        	  else
        	  	tmp=tmp*1000;
        	  //ê1?ü1·1·
            IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable);
            PA1100_Init();
						if(PA1100_WaitCard_SwipeLimitted(tmp)==0)
						{
							PA1100_ClearCardPresent();
							PA1100_WaitBufferReady();
							PA1100_GetAllDataMa(sbuf,marlen);
							//--------
							add=0;//Redata_offset;
							leng=0;
							if(marlen[0]==0)
							{
								Replay_buff[add++]=0x1f;
								Replay_buff[add++]=0x4e;
								Replay_buff[add++]=0xe1;
								leng+=3;
							}
							else
							{
								Replay_buff[add++]=0x1f;
								Replay_buff[add++]=0x59;
								leng+=2;
								for(i=0;i<marlen[0];)
								{
									Replay_buff[add]=sbuf[i];
								  add+=1;
									leng+=1;
									i+=1;	
								}
							}
							//------------
							if(marlen[1]==0)
							{
								Replay_buff[add++]=0x1f;
								Replay_buff[add++]=0x4e;
								Replay_buff[add++]=0xe1;
								leng+=3;
							}
							else
							{
								Replay_buff[add++]=0x1f;
								Replay_buff[add++]=0x59;
								leng+=2;
								for(i=0;i<marlen[1];)
								{
									Replay_buff[add]=sbuf[marlen[0]+i];
								  add+=1;
									leng+=1;
									i+=1;	
								}
							}
							//--------------
							//------------
							if(marlen[2]==0)
							{
								Replay_buff[add++]=0x1f;
								Replay_buff[add++]=0x4e;
								Replay_buff[add++]=0xe3;
								leng+=3;
							}
							else
							{
								Replay_buff[add++]=0x1f;
								Replay_buff[add++]=0x59;
								leng+=2;
								for(i=0;i<marlen[2];)
								{
									Replay_buff[add]=sbuf[marlen[0]+marlen[1]+i];
								  add+=1;
									leng+=1;
									i+=1;	
								}
							}
						}
						else
						{
							Replay_buff[0]=0x1f;
							Replay_buff[0+1]=0x4e;
							Replay_buff[0+2]=0xe1;
							
							Replay_buff[0+3]=0x1f;
							Replay_buff[0+4]=0x4e;
							Replay_buff[0+5]=0xe1;
							
							Replay_buff[0+6]=0x1f;
							Replay_buff[0+7]=0x4e;
							Replay_buff[0+8]=0xe3;
            		
              leng=9;         
						}
						Set_WatchDog();
						
						sw[0]=0x00;
            sw[1]=0x00;
            sw[2]=0x90;
           // ReaderToPCNoData(leng,&sw[0]);
            Communication_ToPC(&sw[0],leng,&Replay_buff[0]);
        break;
		default:
			sw[0]=0x00;
            sw[1]=0x00;
            sw[2]=0x4e;		
            leng=0;
            //ReaderToPCNoData(leng,&sw[0]);
            Communication_ToPC(&sw[0],leng,&Replay_buff[0]);
			break;
	}
}


#include "stm32f10x.h"
#include <stdio.h>
#include <absacc.h>
#include "public_define.h"
#include "public.h"
//#include "usb_lib.h"
#include "usb_regs.h"
#include "system_defineDES.h"

/********************************
串口只支持如下
	switch (baud) {
	case 1200:	temp = 0x01;	break;
	case 2400:	temp = 0x02;	break;
	case 4800:	temp = 0x03;	break;
	case 9600:	temp = 0x04;	break;
	case 19200:	temp = 0x05; break;
	case 28800:	temp = 0x06; break;
	case 38400: temp = 0x07; break;
	case 57600:	temp = 0x08; break;
	case 115200:temp = 0x09; break;
	default:    temp = 0x04; break;
	}
********************************/
uint8 sw[3];
uint8 Receive_Buffer[Max_Rec];
uint8 Transceive_Buffer[Max_Tra];

//#define Communication_ToPC RS232_Command_ReplyTogether

extern volatile uint32_t Systick_sum;//10ms系统滴答计时
extern volatile uint32_t 	systicnum;//1ms

uint8_t TypeB_Iso14443=1;//0不支持14443-4协议的，如二代证
uint8_t bHalBufferReader[256];
uint8_t bBufferReader[0x60];

void *pHal;
phbalReg_Type_t balReader;
phhalHw_Rc663_DataParams_t halReader;
phpalI14443p3a_Sw_DataParams_t I14443p3a;
phpalI14443p3b_Sw_DataParams_t I14443p3b;
phpalI14443p4_Sw_DataParams_t I14443p4;
phpalI14443p4a_Sw_DataParams_t     palI14443p4a;              /* PAL  I14443-4A component */
phpalMifare_Sw_DataParams_t palMifare;
phKeyStore_Sw_DataParams_t         SwkeyStore;
phalMfc_Sw_DataParams_t alMfc;
phpalSli15693_Sw_DataParams_t palSli15693;
phalICode_Sw_DataParams_t  palICode;

phalTop_Sw_DataParams_t tagtop;
//phalTop_T1T_t   t1tparam;
phalTop_T2T_t   t2tparam;
//phalTop_T3T_t   t3tparam;
//phalTop_T4T_t   t4tparam;
phalTop_T5T_t   t5tparam;

phalMful_Sw_DataParams_t phalMful;
phCryptoRng_Sw_DataParams_t phCryptoRng;
phCryptoSym_Sw_DataParams_t phCryptoSym;
	
#define NUMBER_OF_KEYENTRIES        1
#define NUMBER_OF_KEYVERSIONPAIRS   1
#define NUMBER_OF_KUCENTRIES        1	
phKeyStore_Sw_KeyEntry_t           pKeyEntries[NUMBER_OF_KEYENTRIES];
phKeyStore_Sw_KeyVersionPair_t     pKeyVersionPairs[NUMBER_OF_KEYVERSIONPAIRS * NUMBER_OF_KEYENTRIES];
phKeyStore_Sw_KUCEntry_t           pKUCEntries[NUMBER_OF_KUCENTRIES];
//校验
unsigned char  cr_bcc(u16 len, unsigned char *bcc_buffer)
{
	unsigned char temp=0,bcc1=0;
  u16 i;

	for(i=0;i<len;i++)
		temp=temp^bcc_buffer[i];
	return(temp);
}
/*#if (USE_BCCorCRC==1)
 //------------------汇卡双界面 BCC------------------------------                               

unsigned char  crre_bcc_old(u16 len, unsigned char *bcc_buffer,u8 st)
{
	//bcc	校验
	unsigned char temp=0;
  u16 i;
  u8 buf[4];
  buf[0]=MCU_HeardValue;
  buf[1]=st;
  buf[2]=(len>>8);
  buf[3]=(len>>0);
  for(i=0;i<4;i++)
		temp=temp^buf[i];

	for(i=0;i<len;i++)
		temp=temp^bcc_buffer[i];
	return temp;
}
unsigned char  crrecheck_bcc_old(u16 len, unsigned char *bcc_buffer)
{
	//bcc	校验
	unsigned char bcc=0;
  u16 i;
	for(i=0;i<len;i++)
		bcc=bcc^bcc_buffer[i];
	return bcc;
}    
       
#else 
//-------------汇卡双界面 CRC--------------------------------------------------------------
static unsigned char CrcTable1[16] = {0x9a, 0x39, 0xaa, 0x96, 0x18, 0x64, 0x37, 0x12,
	   0x2b, 0xc5, 0x64, 0xf3, 0x72, 0x69, 0x33, 0x11};
	   
	static unsigned char CrcTable2[16] = {0x39, 0x5a, 0xc7, 0xf1, 0x2b, 0x4d, 0x75, 0x88,
	   0x64, 0x38, 0xaa, 0xa5, 0x55, 0x37, 0xd2, 0x03};
//接收PC端的校验第一字节
unsigned char  cr_bcc(u16 len, unsigned char *bcc_buffer)
{
	unsigned char temp=0,bcc1=0;
  u16 i;

	for(i=0;i<len;i++)
		temp=temp^bcc_buffer[i];

	bcc1 = temp&0x0F;
	temp = CrcTable1[bcc1] +CrcTable2[bcc1] + temp;

	return(temp);
}

//回复
//ST为状态字节，LEN为数据包长度
unsigned char  crre_bcc(u16 len, unsigned char *bcc_buffer,u8 st)
{
	unsigned char temp=0,bcc1=0;
  u16 i;
  u8 buf[4];
  buf[0]=MCU_HeardValue;
  buf[1]=st;
  buf[2]=(len>>8);
  buf[3]=(len>>0);
  for(i=0;i<4;i++)
		temp=temp^buf[i];

	for(i=0;i<len;i++)
		temp=temp^bcc_buffer[i];

	bcc1 = temp&0x0F;
	temp = CrcTable1[bcc1] +CrcTable2[bcc1] + temp;

	return(temp);
}
//第二字节
unsigned char  cr_add(u16 len, unsigned char *bcc_buffer)
{
	unsigned char temp=0,bcc1=0;
  u16 i;

	for(i=0;i<len;i++)
		temp=temp+bcc_buffer[i];

	bcc1 = temp&0x0F;
	temp = CrcTable1[bcc1] +CrcTable2[bcc1] + temp;

	return(temp);
}
unsigned char  crre_add(u16 len, unsigned char *bcc_buffer,u8 st)
{
	unsigned char temp=0,bcc1=0;
  u16 i;
  u8 buf[5];
  buf[0]=MCU_HeardValue;
  buf[1]=st;
  buf[2]=(len>>8);
  buf[3]=(len>>0);
  buf[4]=crre_bcc(len, bcc_buffer,st);
  for(i=0;i<5;i++)
		temp=temp+buf[i];


	for(i=0;i<len;i++)
		temp=temp+bcc_buffer[i];

	bcc1 = temp&0x0F;
	temp = CrcTable1[bcc1] +CrcTable2[bcc1] + temp;

	return(temp);
}                                
#endif*/
#if (USE_USBorRS232==1)

//--------------RS232---------------------------------------------------------


#define PCReciveData Receive_Buffer
#define Replay_buff Transceive_Buffer

#define Communication_ToPC RS232_SendBuffNoBuff
//RS232_SendBuffNoBuff(uint8 *er,uint16 len,uint8 *cbuf)

//-------------------------------------


int16 RS232_SendBuffNoBuff(uint8 *er,uint16 len,uint8 *cbuf)
{
	uint8 crc;
	uint16 len_save,i;
	uint32 timesave;
	uint8 tbuf[5],ttbuf[2];
	
	tbuf[0]=(len+4>>8);	
	tbuf[1]=len+4;
  tbuf[2]=er[0];//sw[0];
  tbuf[3]=er[1];//sw[1];
  tbuf[4]=er[2];//sw[2];
	crc=cr_bcc(5, &tbuf[0]);
	ttbuf[0]=crc;
	//crc=cr_bcc(len, &Transceive_Buffer[10]);
	crc=cr_bcc(len, &cbuf[0]);
	ttbuf[1]=crc;
	crc=cr_bcc(2, &ttbuf[0]);
	
	RS232_SentChar(trasfor_head1);
	RS232_SentChar(trasfor_head2);
	RS232_SentChar(trasfor_head3);
	RS232_SentChar(trasfor_head4);
	RS232_SentChar(trasfor_head5);
	
	RS232_SentBuff(&tbuf[0],5);
	
	RS232_SentBuff(&cbuf[0],len);
	RS232_SentChar(crc);
	return 0;
}
int16 Get_CommandData(uint8 *rbuf)
{	
	int16 i,k;
	uint16 off=0;
	uint16 leng,t;
	uint32 timesave;
	uint8 buf[6],tbuf[5],crc;
	
	tbuf[0]=tbuf[1]=tbuf[2]=tbuf[3]=tbuf[4]=0;
	while(1)
	{
		IWDG_ReloadCounter();
		Delay_Ms(5);
		k=RS232_GetChar();
		if(k!=-1)
		{
			tbuf[0]=tbuf[1];
			tbuf[1]=tbuf[2];
            tbuf[2]=tbuf[3];
            tbuf[3]=tbuf[4];
			tbuf[4]=k;		
		}
		else continue;
		if((tbuf[0]==trasfor_head1)&&(tbuf[1]==trasfor_head2)&&(tbuf[2]==trasfor_head3)
            &&(tbuf[3]==trasfor_head4)&&(tbuf[4]==trasfor_head5)
            )
		break;	
	} 	
    	
	rbuf[0]=tbuf[0];rbuf[1]=tbuf[1];rbuf[2]=tbuf[2];rbuf[3]=tbuf[3];rbuf[4]=tbuf[4];
	//接收长度
	off=5;
	k=RS232_ReOnlyLenByteInTime(&rbuf[off],2);
	if(k==-1)
		return -1;
	leng=rbuf[off];
	off+=1;
	leng=((leng<<8)&0xff00)+rbuf[off];
	off+=1;
	//接收数据包
	k=RS232_ReLenByteInTime(&rbuf[off],leng);
	if(k==-1)
		return -1;
	off+=(leng);
	//判定CRC校验
	if(cr_bcc(leng+2, &rbuf[5])!=0)
		return -2;
	else
		return off;	
}

//------------------RS232 END---------------------------------------------------------------


#endif
/*
void Contactcpu_dowith(uint8 *buf,uint16 len)
{}
void e2prom_dowith(uint8 *buf,uint16 len){}
void ContactLessA_dowith(uint8 *buf,uint16 len){}
void ContactLessB_dowith(uint8 *buf,uint16 len){}
void Mif_dowith(uint8 *buf,uint16 len){}
void SCPSAM_GetSerialNumber(uint8 *buf,uint16 len){}
void SCPSAM_ActiveCard(uint8 *buf,uint16 len){}
void SCPSAM_GetCardData(uint8 *buf,uint16 len){}
*/	
/*
void test_rs232(void)
{
	int16 i;
	RS232_Open(Uart_Rate_115200);
	Delay_Ms(50);
	for(i=0;i<10;)
	{
		RS232_SentChar(i+0x90);
		i+=1;	
	}
	while(1)
	{
		i=RS232_GetChar();
		if(i!=-1)
			RS232_SentChar(i+1);	
		Delay_Ms(50);	
	}
}*/

void  DES_DoWith(void)
{
	
	phStatus_t status;
	//uint8_t i,j;
	//uint8_t rbuf[10];
	uint8_t  rate;
	
	int16 leng;
	
	//test_rs232();
	 /* Add your application code here
     */
   phhalHw_Rc663_Reset();
   
   phbalReg_Init_ST(&balReader, sizeof(phbalReg_Type_t));
   status = phhalHw_Rc663_Init(&halReader,sizeof(phhalHw_Rc663_DataParams_t),&balReader,0,bHalBufferReader,sizeof(bHalBufferReader),bHalBufferReader,sizeof(bHalBufferReader));
  halReader.bBalConnectionType = PHHAL_HW_BAL_CONNECTION_SPI;	
	if(status)
	{
		while(1)
		{
			Delay_Ms(50);
			Beep_run(0);
			LEDA_run(0);
			Delay_Ms(50);
			Beep_run(1);
			LEDA_run(1);
		}
	}
	

	RS232_Open(Uart_Rate_115200);
	PA1100_IOINIT();
//	RS232_Open(Uart_Rate_115200);
//-----------初始化LIB---------------------
// Set the generic pointer 
    pHal = &halReader;
   phhalHw_Rc663_ReadRegister(&halReader,PHHAL_HW_RC663_REG_VERSION, bBufferReader);
// Initialize the 14443-3A PAL (Protocol Abstraction Layer) component 
	phpalI14443p3a_Sw_Init(&I14443p3a,sizeof(phpalI14443p3a_Sw_DataParams_t), pHal);
	
	phpalI14443p3b_Sw_Init(&I14443p3b,sizeof(phpalI14443p3b_Sw_DataParams_t), pHal);
	/* Initialize the 14443-4 PAL component */
	phpalI14443p4_Sw_Init(&I14443p4,sizeof(phpalI14443p4_Sw_DataParams_t), pHal);
	
	phpalI14443p4a_Sw_Init(&palI14443p4a, sizeof(phpalI14443p4a_Sw_DataParams_t), pHal);
	phpalI14443p4_ResetProtocol  ( &I14443p4 )  ;
	  /* Initialize the Mifare PAL component */
	phpalMifare_Sw_Init(&palMifare,sizeof(phpalMifare_Sw_DataParams_t), pHal, &I14443p4);
	

	/* Initialize the keystore component */
    status = phKeyStore_Sw_Init(
            &SwkeyStore,
            sizeof(phKeyStore_Sw_DataParams_t),
            &pKeyEntries[0],
            NUMBER_OF_KEYENTRIES,
            &pKeyVersionPairs[0],
            NUMBER_OF_KEYVERSIONPAIRS,
            &pKUCEntries[0],
            NUMBER_OF_KUCENTRIES);
            
  phhalHw_FieldReset(pHal);
  //初始化密码
 // PH_CHECK_SUCCESS_FCT(status, phKeyStore_FormatKeyEntry(&SwkeyStore, 0, PH_KEYSTORE_KEY_TYPE_MIFARE));
 
	//初始化3组(1,2,3)密码都为FF
	//实际中应该从E2PROM里拷贝出密码
	//memset(phkey, 0xff, 32);
	//PH_CHECK_SUCCESS_FCT(status, phKeyStore_SetKey(&SwkeyStore, 0, 0, PH_KEYSTORE_KEY_TYPE_MIFARE, &phkey[0], 0));
				
	status = phalMfc_Sw_Init(&alMfc, sizeof(phalMfc_Sw_DataParams_t), &palMifare, &SwkeyStore);
	
	status=phCryptoSym_Sw_Init(
                               &phCryptoSym,
                               sizeof(phCryptoSym_Sw_DataParams_t),
                               &SwkeyStore
                               );   
	                       
status=phCryptoRng_Sw_Init(
                               &phCryptoRng,
                               sizeof(phCryptoRng_Sw_DataParams_t),
                               &phCryptoSym
                               );  
  

status=phalMful_Sw_Init(
                            &phalMful,
                            sizeof(phalMful_Sw_DataParams_t),
                            &palMifare,
                            &SwkeyStore,
                            &phCryptoSym,
                            &phCryptoRng
                            );
 /* init. 15693 pal */
  phpalSli15693_Sw_Init(&palSli15693,
                                    sizeof(phpalSli15693_Sw_DataParams_t), pHal);     
	 //palICode
  phalICode_Sw_Init(
        &palICode,                                /**< [In] Pointer to this layers parameter structure. */
        sizeof(phalICode_Sw_DataParams_t),                                             /**< [In] Specifies the size of the data parameter structure. */
       &palSli15693,
				&SwkeyStore,
         &phCryptoSym,
          &phCryptoRng                                              /**< [In] Pointer to the parameter structure of the underlying Keystore layer. */
);   

phalTop_Sw_Init(&tagtop, sizeof(phalTop_Sw_DataParams_t), NULL, &t2tparam, NULL, NULL,&t5tparam, NULL);
//(phalTop_T2T_t *)(tagtop->pTopTagsDataParams[0])	
tagtop.pTopTagsDataParams[1]=&phalMful;
tagtop.pTopTagsDataParams[4]=&palICode;

	phhalHw_Rc663_Cmd_SoftReset(pHal);
	/* Apply the type A protocol settings and activate the RF field. */
	phhalHw_ApplyProtocolSettings(pHal,PHHAL_HW_CARDTYPE_ISO14443A);
	/* Reset the Rf field */
	phhalHw_FieldReset(pHal);      
//-----------------------------------------	
    Set_WatchDog();
//test_ant();
//test_antb();
//test_card();
while(1)
{
	  if(systicnum>0x0fffffff)
	  	System_ClearTimer();
	
		IWDG_ReloadCounter();
        //--通讯解析
		LEDS_run(LED_TurnOff);
		
		leng=Get_CommandData(PCReciveData);
		if(leng<=0)
		{	     
       if(leng==-2)//校验错误
				{
           sw[0]=0x00;
           sw[1]=0x00;
           sw[2]=0x91;		
           Communication_ToPC(&sw[0],0,&Replay_buff[0]);
				}	
        continue;}
		//---通讯应答
		LEDS_run(LED_TurnOn);
		switch(PCReciveData[7])
		{
        case DES_Card_RFA_CPU://非接TYPEA卡
					ContactLessA_dowith(PCReciveData,leng);
					break;
        case DES_Card_RFB_CPU://非接TYPEB卡
					ContactLessB_dowith(PCReciveData,leng);
					break;
        case DES_Card_Contactcpu://接触CPU
					Contactcpu_dowith(PCReciveData,leng);
					break;
        case DES_Card_RF_M1://非接卡M1
					Mif_dowith(PCReciveData,leng);
					break;
				case DES_Card_COMUNAL://E2PROM//版本号//磁条卡
                
					e2prom_dowith(PCReciveData,leng);
					break;
         //二代证
         case DES_Card_SCIDPSAM://获取模块版本号
            SCPSAM_GetSerialNumber(PCReciveData,leng);
            break;
         case DES_Card_SCIDRequst://寻卡，选卡
            //[8]=01寻卡
            //[8]=02选卡
            SCPSAM_ActiveCard(PCReciveData,leng);
            break;
         case DES_Card_SCIDDATA://获取二代证信息
            //[8]是否带指纹
            SCPSAM_GetCardData(PCReciveData,leng); 
            break;
				default:
					sw[0]=0x00;
          sw[1]=0x00;
          sw[2]=0x92;		
          Communication_ToPC(&sw[0],0,&Replay_buff[0]);
					break;
    }
	}	
		
}
//e2prom存储6字节KEYA和6字节KEYB
//将LIB的KEY作为临时的，只留一扇区一组密码
//扇区
int16	Load_key_E2(uint16 Secnr,uint8	Mode)
{
	uint16	temp1;
	uint8 phkey[32];
	phStatus_t status;
	//格式化
	status=phKeyStore_FormatKeyEntry(&SwkeyStore, 0, PH_KEYSTORE_KEY_TYPE_MIFARE);
	if (status)
	{
		return -1;
	}
	temp1=Secnr;
	ReadFlashFMByte(phkey,12,(temp1*12));
	status=phKeyStore_SetKey(&SwkeyStore, 0, 0, PH_KEYSTORE_KEY_TYPE_MIFARE, &phkey[0],0);
	if (status)
	{
		return -1;
	}
	return 0;
}
//扇区
int16	DirLoad_key_E2(uint8	Mode,uint8 *ke)
{
	uint8 phkey[32];
	phStatus_t status;
	//格式化
	status=phKeyStore_FormatKeyEntry(&SwkeyStore, 0, PH_KEYSTORE_KEY_TYPE_MIFARE);
	if (status)
	{
		return -1;
	}
	memset(phkey, 0, 12);
	if(Mode==0)//A
	{
			memcpy(phkey,ke,6);
	}
	else//B
	{
			memcpy(&phkey[6],ke,6);
	}
	status=phKeyStore_SetKey(&SwkeyStore, 0, 0, PH_KEYSTORE_KEY_TYPE_MIFARE, &phkey[0],0);
	if (status)
	{
		return -1;
	}
	return 0;
}
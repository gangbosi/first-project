/* STM32F1、STM32F4 CAN双机通信  
	做此实验需将USB模块的短接片短接到CAN端，并且将J9端子处的F1和F4的CAN短接，在CAN通信
	实验中有介绍*/

#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "key.h"
#include "tftlcd.h"
#include "can.h"


int cal_checksum(u8 * data,int len)
{
	int checksum = 0;
	int i=0;
	checksum = *data;
	//for(i = 0;i<len;i++)
	for(i = 1;i<len;i++)
	{
		checksum ^= *(data + i );
	}
	return checksum;
	
}
void cal_alivecout(u8 *i)
{ 
	u8 bit_high ;
	bit_high = ((*i ) & 0xf0) >> 4; 
	if(bit_high <15)
	{
		//*i += 1;
		*i = (bit_high+1)<<4;
	}
	else
	{
		*i = 0;
	}
 /* if(*i<15)
	{
		*i+=1;
	}
	else{
		*i=0;
	}*/
}

void number2string(u8 *num, u8 * str)
{
	 u8 i = 0,m=0,n=0;
	 for(i = 0; i<8;i++)
	 { 
		 m = num[i]&0xf;
		 n = num[i] >> 4 ;
	   *str++= m>=10 ? (m-10+'A'):(m+'0');
		 *str++= n>=10 ? (n-10+'A'):(n+'0');
	
	 }
}

/*******************************************************************************
* 函 数 名         : main
* 函数功能		   : 主函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
int main()
		{	
	u8 i=0,j=0;
	u8 key;
	u8 mode=1;
	u8 res;
	u8 buf[8],char_buf[8];	
	u8 brk_cmd[8],dri_cmd[8],str_cmd[8],gear_cmd[8],park_cmd[8],light_cmd[8]; //define fxb chassis command canbus message
  u32 brk_cmd_id = 0x98c4d4d0;
	u32 dri_cmd_id = 0x98c4d3d0;
	u32 str_cmd_id = 0x98c4d2d0;
	u32 gear_cmd_id = 0x98c4d1d0;
	u32 park_cmd_id = 0x98c4d5d0;
	u32 light_cmd_id = 0x98c4d7d0;
	
	u8 rbuf[8];
	u8 brk_data_str[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	
	//brake
	brk_cmd[0] =1;
	brk_cmd[1] = 0;

	
	//drive
	dri_cmd[0] = 1;
	dri_cmd[1] = 0;
	
	//str
	str_cmd[0] =1;
	str_cmd[1] = 0;
	str_cmd[2] = 8;
	
	//gear
	gear_cmd[0] = 1;
	gear_cmd[1] = 4;
	
	park_cmd[0] = 1;
	park_cmd[1] = 0;
	
	light_cmd[0] = light_cmd[1] = light_cmd[2] =0;
	
	
	
	SysTick_Init(168);//时钟初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //中断优先级分组 分2组
	LED_Init();//LED初始化
	USART1_Init(9600);//串口初始化
	TFTLCD_Init();   //液晶屏初始化
	//KEY_Init();
	CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_7tq,6,CAN_Mode_Normal);//500Kbps波特率
	
	FRONT_COLOR=BLACK;
	LCD_ShowFontHZ(10, 10,"深圳风向标教育资源");
	LCD_ShowFontHZ(10, 50,"股份有限公司");

	LCD_ShowString(10,110,tftlcd_data.width,tftlcd_data.height,16,"steering and light control");

	FRONT_COLOR=RED;
	

	LCD_ShowString(10,170,tftlcd_data.width,tftlcd_data.height,16,"Send:");

	
	while(1)
	{
		i++;

    str_cmd[2] = 10;
		light_cmd[0]=1;
		light_cmd[1]=light_cmd[2] =0;
		
		cal_alivecout(&brk_cmd[6]);

		brk_cmd[7] = cal_checksum(brk_cmd,7);
			
    cal_alivecout(&dri_cmd[6]);
	  dri_cmd[7] = cal_checksum(dri_cmd,7);
			
    cal_alivecout(&str_cmd[6]);
		str_cmd[7] = cal_checksum(str_cmd,7);
			
    cal_alivecout(&gear_cmd[6]);
		gear_cmd[7] = cal_checksum(gear_cmd,7);
			
    cal_alivecout(&park_cmd[6]);
		park_cmd[7] = cal_checksum(park_cmd,7);
		
		cal_alivecout(&light_cmd[6]);
		light_cmd[7] = cal_checksum(light_cmd,7);
			
			
		number2string(brk_cmd,brk_data_str);

		res= FxbCAN1_Send_Msg(brk_cmd_id,brk_cmd,8);
		delay_ms(2);
		res= FxbCAN1_Send_Msg(dri_cmd_id,dri_cmd,8);
		delay_ms(2);
		res= FxbCAN1_Send_Msg(str_cmd_id,str_cmd,8);
		delay_ms(2);
		res= FxbCAN1_Send_Msg(gear_cmd_id,gear_cmd,8);
		delay_ms(2);
		res= FxbCAN1_Send_Msg(park_cmd_id,park_cmd,8);
		delay_ms(2);
		res= FxbCAN1_Send_Msg(light_cmd_id,light_cmd,8); 

			LCD_ShowString(10+5*8,170,tftlcd_data.width,tftlcd_data.height,16,brk_data_str);

		
		if((i%20)==0) //200ms
		{
			led1=!led1;
		}
		delay_ms(10);
	}
}

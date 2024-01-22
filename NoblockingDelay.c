/*
 *   文件名: FILE NAME
 *   作者:    -
 *   日期:    -
 *   版本:    -
 *   说明:    -
 *   修改记录: -
 */
#define 		U32_MAX_VALUE 		0xFFFFFFFF
uint32_t 		gSystickTimes = 0;

/* 按键按下判断逻辑 */

void SysTick_Handler(void)
{
	gSystickTimes ++;				/*系统心跳计数*/
}


uint32_t BSP_Get_Systick(void)
{
	return gSystickTimes; 			/*获取系统心跳*/
}


bool Ticker_out(uint32_t *pTicker, uint32_t uCompareValue)
{
	uint32_t uTicks = 0;
	uTicks = BSP_Get_Systick();
	
	if(0== uCompareValue)
	{
		*pTicker = uTicks;
		return true;
	}
	else
	{
		if(uTicks >= (*pTicker))
		{
			uTicks = uTicks - (*pTicker);
		}
		else
		{
			uTicks = (U32_MAX_VALUE - (*pTicker)) + uTicks;
		}
		if(uTicks >= uCompareValue)
		{
			return true;
		}
	}
	
	return false;
}


int main(void)
{
	bool eState = false;
	
	static uint32_t uSampleTicker = 0;
	
	while(1)
	{
		eState = Ticker_out(&uSampleTicker, 10);
		
		if(true == eState)
		{
			Ticker_out(&uSampleTicker, 0);
			LED_ON();
		}
	}
}



bool Delay_ms(uint32_t ms)
{    
	static uint32_t uSampleTicker   = 0;
    return Ticker_out(uSampleTicker, 10);
}

typedef struct 
{
    /* data */
    uint8_t     left_count;
    uint8_t     left_count_status;

    uint8_t     right_count;
    uint8_t     right_count_status;

	uint8_t	 	last_state;
	uint8_t     status_flag;



}nfc_button_cnt;

nfc_button_cnt ntc_button_status;


uint8_t     left_count       = 0;
uint8_t     last_state  = 0;


 void read_button_state( pin_index_en type)
{	
    uint8_t state;

    state = Botton_get_information(type);               /*读取当前按键状态*/

    if(state != ntc_button_status.last_state)                             /*如果当前按键状态和上一次状态不同*/
    {
        if(state)                                       /*当前状态是按下*/
        {
            ntc_button_status.left_count ++;            /*计数—+1*/
        }

        ntc_button_status.last_state = state;                             /*更新上一次按键状态*/
    }
}




uint8_t Botton_get_information( pin_index_en type)
{	 
    return hal_pin_read(type);
}









static bool LTurnLSW_OFF_Check(void)
{
	if((Botton_get_information(Power) == 0) && button_status.status_flag == 0))
	{
		return true;
	}
	else
	{
		return false;
	}
}


static bool  LTurnLSW_Delay_Check(void)
{
	if((Check_timer(&LTurnLSW_cnt_timer) == 0))
	{
		return true;
	}
	else
	{
		return false;
	}
}


static bool LTurnLSW_Delay_End_Check(void)
{
	if((Check_timer(&LTurnLSW_cnt_timer) == 1))
	{
		return true;
	}
	else
	{
		return false;
	}
}




void LTurnLSW_OFF_Perform(void)
{
	Delay_timer(&LTurnLSW_cnt_timer, 10000);
	button_status.status_flag = 1;
}


void  LTurnLSW_Delay_Perform(void)
{
	read_button_state(LTurnLSW);

	if(button_status.left_count == 3)
	{
		button_status.left_count = 0;
		Delay_timer(&RTurnLSW_cnt_timer, 3000);
	}
}

static void LTurnLSW_Delay_End_Perform(void)
{
	if(button_status.left_count == 3)
	{
		button_status.left_count = 0;
		Delay_timer(&RTurnLSW_cnt_timer, 10000);
	}
	else
	{
		button_status.status_flag = 0;
		button_status.left_count  = 0;
	}	
}





void  RTurnLSW_Delay_Perform(void)
{
	read_button_state(RTurnLSW);

	if(button_status.right_count == 3)
	{
		button_status.right_count = 0;
		Delay_timer(&LTurnLSW_cnt_timer, 3000);
	}

}

static void RTurnLSW_Delay_End_Perform(void)
{
	if(button_status.right_count == 3)
	{
		button_status.right_count = 0;
		Delay_timer(&RTurnLSW_cnt_timer, 10000);
	}
	else
	{
		button_status.status_flag = 0;
		button_status.left_count  = 0;
	}	
}


uint8_t  rand_buf[17]  = {0};

uint8_t  rn136_byte0[8] = {0};
uint8_t  rn136_byte1[8] = {0};
uint8_t  rn136_byte2[8] = {0};
uint8_t  rn136_byte3[8] = {0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};




/* 随机数生成17个字节 数据大小范围：0-255*/
void rands_num(uint8_t *buf)
{
	uint8_t i;

	for(i=0; i<17; i++)
	{
		srand(tick_count);
		buf[i] = rand()%256;
	}
}

/* RN136数据帧 0byte*/
void Rn136_data_byte0(uint8_t *rn136_byte0, uint8_t *buf)
{
	uint8_t i;

	rn136_byte0[0] = 0x11;

	for(i=1; i<8; i++)
	{
		rn136_byte0[i] = buf[i-1];
	}
}

/* RN136数据帧 1byte*/
void Rn136_data_byte1(uint8_t *rn136_byte1, uint8_t *buf)
{
	uint8_t i;

	rn136_byte1[0] = 0x12;

	for(i=8; i<14; i++)
	{
		rn136_byte1[i-7] = buf[i];
	}
}

/* RN136数据帧 2byte*/
void Rn136_data_byte2(uint8_t *rn136_byte2, uint8_t *buf)
{
	uint8_t i;

	rn136_byte2[2] = 0x13;

	for(i=14; i<21; i++)
	{
		if(i>16)
		{
			if(i==20)
			{
				rn136_byte2[i-13] = 0x00;
				break;
			}
			rn136_byte2[i-13] = 0x00;
		}
		else
		{
			rn136_byte2[i-13] = buf[i];
		}
		
	}

}



/*
* 函数名 :CRC8
* 描述 : 计算CRC8
* 输入 : puchMsg---数据地址, usDataLen---数据长度
* 输出 : 校验值
*/
uint8_t CRC_8(uint8_t *puchMsg, uint8_t usDataLen)
{
	uint8_t uCRC = 0x00;	//CRC寄存器
	
	for(uint8_t num=0; num<usDataLen; num++)
	{
		uCRC = (*puchMsg++)^uCRC;	//把数据与8位的CRC寄存器的8位相异或，结果存放于CRC寄存器。
		for(uint8_t x=0; x<8; x++)	//循环8次
		{	
			if(uCRC&0x80)			//判断最低位为：“1”
			{	
				uCRC = uCRC<<1;		//先左移
				uCRC = uCRC^0x07;	//再与多项式0x07异或
			}
			else
			{	//判断最低位为：“0”
				uCRC = uCRC<<1;	//右移
			}
		}
	}

	return uCRC;//返回CRC校验值
}

int main(void)
{
	uint8_t x[]={0x01 ,0x03 ,0x61 ,0x00 ,0x00 ,0x02};
	while(1){
		uint8_t wCRC_8 = CRC_8(x, sizeof(x)); //获取CRC8校验值
		//输出校验值wCRC_8 即可
	}
}
38 41 45 48 51 54 58 61 64 68 71 74 77 81 84 87 90
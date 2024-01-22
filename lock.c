


/*
 * Lock.c
 *
 *  Created on: Dec 21, 2023
 *      Author: Tlak
 */
#include "Lock.h"



extern uint8_t NFC_cnt;
extern uint8_t lock_cmd[3];		//上锁命令：	
extern uint8_t NFC_Shortbrush;
extern uint8_t NFC_buffer[8];
extern uint8_t Encode_buff[8];
extern uint8_t Key[8];

extern BCM_VehSt_status BCM_Lock_Status;



Delay_st_type Lock_Power_timer;
Delay_st_type Lock_timer;



uint32_t last_cnt;
static lock_work_status g_car_work = Lock_Invalid;

#define NFC_LONGBRUSH					0X7A
#define VERFILY_SUCCESS					0X01
#define VERFILY_UNSUCCESS				0X00


typedef struct vehicle_state
{
	uint8_t lock_status;			/* 锁状态*/
	uint8_t verify_status;
	uint8_t short_flag;
}vehicle_state;

vehicle_state status  ;

/* NFCflag 计数值 */
uint32_t nfc_data_cnt(uint8_t *buf)
{
    uint32_t num   = 0;

    /* 获取计数值*/
    num = ((buf[3]<<16) | (buf[2]<<8) | (buf[1])) ;

    return num;
}


/* 计数比较  */
uint8_t nfc_cnt_compare(uint8_t *buf)
{
	uint32_t current_cnt = 0;
	
	current_cnt = nfc_data_cnt(buf);
	
    if(last_cnt == CNT_FULL)
    {
        last_cnt = 0;
        
        if(last_cnt < current_cnt)
        {
             last_cnt = current_cnt;
             return 1;
        }
    }
    else if (last_cnt < current_cnt)
    {
        last_cnt = current_cnt;
        return 1;
    }
    else
    {
        return 0;
    }

}



/* 校验  */
void nfc_data_verify(void)
{
	/* 有效刷卡动作完成 */
	if(Get_SlotCard_Read() == 1 && vehicle_state.verify_status == 0)
	{
		/* NFC数据解码  */
		Des_Unifold(NFC_buffer, Key, Encode_buff);
		
		/* 数据校验 NFCflag */
		status.verify_status = nfc_cnt_compare(Encode_buff);
	}
	else
	{
		status.verify_status = 0;
	}
}





/*锁控无效状态检测*/
static bool Lock_Invalid_Check(void) 
{
	/* 不合法卡片 */
	if(status.verify_status == VERFILY_UNSUCCESS)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/* 上电状态 */
static bool Lock_Valid_Check(void)
{
	if(status.verify_status == VERFILY_SUCCESS)&&((Botton_get_information(Power) == 0x01)))
	{
		return true;
	}
	else
	{
		
		return false;
	}
}

static bool Lock_Power_Delay_Check(void)
{
	/*方向锁上电延时未到*/
	if((Check_timer(&Lock_Power_timer) == 0))
	{
		return true;
	}
	else
	{
		return false;
	}	
}

/*锁控动作检测*/
static bool Lock_Action_Check(void)
{
	/*方向锁上电延时已到*/
	if((Check_timer(&Lock_Power_timer) == 1))
	{
		/*锁当前状态为解锁状态，边撑放下*/
		return true;
	}
	else
	{
		return false;
	}
}

/*上锁延时检测*/
static bool Lock_Time_Delay_Check(void)
{
	if((Check_timer(&Lock_timer) == 0))
	{
		return true;
	}
	else
	{	
		return false;
	}
}

/*上锁延时完成检测*/
static bool Lock_Response_Check(void)
{	
	if((Check_timer(&Lock_timer) == 1))
	{
		return true;
	}
	else
	{
		return false;
	}	
}

lock_check_state lock_check_perform[] =
{
	//车锁-状态切换
	{Lock_Invalid, 		Lock_Invalid_Check,   	Lock_Invalid},
	{Lock_Invalid, 		Lock_Valid_Check,  		Lock_Valid},
	
	{Lock_Valid, 		Lock_Invalid_Check,  	Lock_Invalid},		
	{Lock_Valid, 		Lock_Power_Delay_Check, Lock_Power_Delay},	
	{Lock_Valid, 		Lock_Action_Check,  	Lock_Action},
	
	{Lock_Power_Delay, 	Lock_Invalid_Check,  	Lock_Invalid},
	{Lock_Power_Delay, 	Lock_Action_Check,  	Lock_Action},
	{Lock_Power_Delay, 	Lock_Power_Delay_Check, Lock_Power_Delay},
	
	{Lock_Action, 		Lock_Invalid_Check,  	Lock_Invalid},
	{Lock_Action, 		Lock_Response_Check,  	Lock_Response},
	{Lock_Action, 		Lock_Time_Delay_Check,  Lock_Time_Delay}, 	//上锁延时
	
	{Lock_Time_Delay, 	Lock_Invalid_Check,  	Lock_Invalid},
	{Lock_Time_Delay, 	Lock_Response_Check,  	Lock_Response},
	{Lock_Time_Delay, 	Lock_Time_Delay_Check,  Lock_Time_Delay},
	
	{Lock_Response, 	Lock_Invalid_Check,  	Lock_Invalid},
	{Lock_Response, 	Lock_Valid_Check,  		Lock_SlotCard},
};



void Lock_Invalid_Perform(void)
{
	/* 锁初始态 */
	Get_SlotCard_Write(0);
}

void Lock_Valid_Perform(void)
{
	/* IG下电， 锁供电*/
	Leds_Display(PowerRelay, 		PIN_LOW);
	
	hal_pwm_enabled(PWM0_CHANNEL11, HAL_ENABLE);
	Delay_timer(&Lock_Power_timer, 100);
}

void Lock_Power_Delay_Perform(void)
{
	/* 锁延时时间未到 */
}

void Lock_Action_Perform(void)
{
	/* 锁延时时间到 */
	
	uint8_t i=0;
	
	/* 执行上锁前，当前锁为解锁状态 */
	if((Botton_get_information(Lock) == 1)&&(Botton_get_information(Unlock) == 0))
	{
		/* 执行方向锁上锁Tx	*/
		for(i=0; i<3; i++)
		{
			hal_uart_tx(UART0_CHANNEL, lock_cmd[i]);
		}
	}

	/* 上锁成功，延时*/
	Delay_timer(&Lock_timer, 200);	
}

void Lock_Time_Delay_Perform(void)
{
	/* 上锁成功，延时未到*/

}

void Lock_Response_Perform(void)
{
	/* 上锁成功，延时到*/
	
	/* 上锁反馈成功 */
	if((Botton_get_information(Lock) == 0)&&(Botton_get_information(Unlock) == 1))
	{	
		hal_pwm_enabled(PWM0_CHANNEL11, HAL_DISABLE);          
		status.lock_status			= 1;
		status.verify_status		= 0;
		Get_SlotCard_Write(0);
	}
	//上锁反馈失败
	else
	{
		//NFC 上锁失败标志
		status.lock_status			= 0;
		status.verify_status		= 0;
		Get_SlotCard_Write(0);
	}
}


lock_work_state lock_work_perform[]=
{
	{Lock_Invalid, 		Lock_Invalid_Perform},
	
	{Lock_Valid, 		Lock_Valid_Perform},
	
	{Lock_Power_Delay, 	Lock_Power_Delay_Perform},
	
	{Lock_Action, 		Lock_Action_Perform},
	
	{Lock_Time_Delay, 	Lock_Time_Delay_Perform},
	
	{Lock_Response, 	Lock_Response_Perform},
};


//检索状态更新
static void lock_mode_check_perfrom(void)
{
	uint8_t i=0;
	
	for(i=0; i<sizeof(lock_check_perform)/sizeof(lock_check_perform[0]); i++)
	{
		if(g_car_work == lock_check_perform[i].mode)
		{
			if(lock_check_perform[i].ctrlcheck() == true) //Next Mode
			{
				g_car_work = lock_check_perform[i].next_work_mode;
				
				break;
			}
		}
	}
}


//执行当前状态对应的函数：默认状态Init
static void lock_work_all_perform(void)
{
	uint8_t i;

	for(i=0; i<sizeof(lock_work_perform)/sizeof(lock_work_perform[0]); i++)
	{
		if(g_car_work == lock_work_perform[i].work_mode)	
		{
//			if(g_car_work_old != g_car_work)
//			{
//				car_work_perform[i].ctrlinit(); 		
//				g_car_work_old = g_car_work;			
//			}
			
			lock_work_perform[i].ctrlLoop(); 

			break;
		}
	}	
}



void lock_status_perform(void)
{
	lock_mode_check_perfrom();
	lock_work_all_perform();
}
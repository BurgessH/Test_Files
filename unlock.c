
/*锁控无效状态检测*/
static bool Unlock_Invalid_Check(void) 
{
	/* NFCflag 校验非法 */
	if(status.verify_status == VERFILY_UNSUCCESS)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/* NFCflag校验合法卡片、IG=OFF状态 */
static bool Unlock_Valid_Check(void)
{
	if((status.verify_status == VERFILY_SUCCESS)&&((Botton_get_information(Power) == 0x00)))
	{
		return true;
	}
	else
	{
		
		return false;
	}
}

static bool Unlock_Power_Delay_Check(void)
{
	/*方向锁上电延时未到*/
	if((Check_timer(&Unlock_Power_timer) == 0))
	{
		return true;
	}
	else
	{
		return false;
	}	
}

/*锁控动作检测*/
static bool Unlock_Action_Check(void)
{
	/*方向锁上电延时已到*/
	if((Check_timer(&Unlock_Power_timer) == 1))
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
static bool Unlock_Time_Delay_Check(void)
{
	if((Check_timer(&Unlock_timer) == 0))
	{
		return true;
	}
	else
	{	
		return false;
	}
}

/*上锁延时完成检测*/
static bool Unlock_Response_Check(void)
{	
	if((Check_timer(&Unlock_timer) == 1))
	{
		return true;
	}
	else
	{
		return false;
	}	
}

unlock_check_state unlock_check_perform[] =
{
	//车锁-状态切换
	{Unlock_Invalid, 		Unlock_Invalid_Check,   	Unlock_Invalid},
	{Unlock_Invalid, 		Unlock_Valid_Check,  		Unlock_Valid},
	
	{Unlock_Valid, 		Unlock_Invalid_Check,  	Unlock_Invalid},		
	{Unlock_Valid, 		Unlock_Power_Delay_Check, Unlock_Power_Delay},	
	{Unlock_Valid, 		Unlock_Action_Check,  	Unlock_Action},
	
	{Unlock_Power_Delay, 	Unlock_Invalid_Check,  	Unlock_Invalid},
	{Unlock_Power_Delay, 	Unlock_Action_Check,  	Unlock_Action},
	{Unlock_Power_Delay, 	Unlock_Power_Delay_Check, Unlock_Power_Delay},
	
	{Unlock_Action, 		Unlock_Invalid_Check,  	Unlock_Invalid},
	{Unlock_Action, 		Unlock_Response_Check,  	Unlock_Response},
	{Unlock_Action, 		Unlock_Time_Delay_Check,  Unlock_Time_Delay}, 	//上锁延时
	
	{Unlock_Time_Delay, 	Unlock_Invalid_Check,  	Unlock_Invalid},
	{Unlock_Time_Delay, 	Unlock_Response_Check,  	Unlock_Response},
	{Unlock_Time_Delay, 	Unlock_Time_Delay_Check,  Unlock_Time_Delay},
	
	{Unlock_Response, 	Unlock_Invalid_Check,  	Unlock_Invalid},
	{Unlock_Response, 	Unlock_Valid_Check,  		Unlock_SlotCard},
};



void Unlock_Invalid_Perform(void)
{
	/* 锁初始态、重新刷卡 */
	Get_SlotCard_Write(0);
}

void Unlock_Valid_Perform(void)
{
	/* 方向锁供电、方向锁延时*/
	hal_pwm_enabled(PWM0_CHANNEL11, HAL_ENABLE);
	Delay_timer(&Unlock_Power_timer, 100);
}

void Unlock_Power_Delay_Perform(void)
{
	/* 锁延时时间未到 */
	
}

void Unlock_Action_Perform(void)
{
	/* 锁延时时间到 */
	
	uint8_t i=0;
	
	/* 执行解锁前，当前锁为闭锁状态 */
	if((Botton_get_information(Unlock) == 1)&&(Botton_get_information(Lock) == 0))
	{
		/* 锁状态正常、执行方向锁解锁Tx	*/
		for(i=0; i<3; i++)
		{
			hal_uart_tx(UART0_CHANNEL, Unlock_cmd[i]);
		}
	}

	/* 解锁成功，延时*/
	Delay_timer(&Unlock_timer, 200);	
}

void Unlock_Time_Delay_Perform(void)
{
	/* 解锁成功，延时未到*/

}

void Unlock_Response_Perform(void)
{
	/* 解锁成功，延时到*/
	
	/* 解锁反馈成功 */
	if((Botton_get_information(Unlock) == 0)&&(Botton_get_information(Lock) == 1))
	{	
		hal_pwm_enabled(PWM0_CHANNEL11, HAL_DISABLE);   
		
		status.Unlock_status		= 1;
		status.verify_status		= 0;
		
		Get_SlotCard_Write(0);
	}
	//解锁反馈失败
	else
	{
		//NFC 解锁失败标志
		status.Unlock_status		= 0;
		status.verify_status		= 0;
		Get_SlotCard_Write(0);
	}
}


unlock_work_state unlock_work_perform[]=
{
	{Unlock_Invalid, 		Unlock_Invalid_Perform},
	
	{Unlock_Valid, 			Unlock_Valid_Perform},
	
	{Unlock_Power_Delay, 	Unlock_Power_Delay_Perform},
	
	{Unlock_Action, 		Unlock_Action_Perform},
	
	{Unlock_Time_Delay, 	Unlock_Time_Delay_Perform},
	
	{Unlock_Response, 		Unlock_Response_Perform},
};


//检索状态更新
static void unlock_mode_check_perfrom(void)
{
	uint8_t i=0;
	
	for(i=0; i<sizeof(unlock_check_perform)/sizeof(unlock_check_perform[0]); i++)
	{
		if(g_car_work == unlock_check_perform[i].mode)
		{
			if(unlock_check_perform[i].ctrlcheck() == true) //Next Mode
			{
				g_car_work = unlock_check_perform[i].next_work_mode;
				
				break;
			}
		}
	}
}


//执行当前状态对应的函数：默认状态Init
static void unlock_work_all_perform(void)
{
	uint8_t i;

	for(i=0; i<sizeof(unlock_work_perform)/sizeof(unlock_work_perform[0]); i++)
	{
		if(g_car_work == unlock_work_perform[i].work_mode)	
		{
//			if(g_car_work_old != g_car_work)
//			{
//				car_work_perform[i].ctrlinit(); 		
//				g_car_work_old = g_car_work;			
//			}
			
			unlock_work_perform[i].ctrlLoop(); 

			break;
		}
	}	
}

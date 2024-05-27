typedef struct MultiTimer 
{
    uint8_t             state;                 /* 任务状态 */	
    uint32_t            tick;                  /* 滴答值 */
    uint16_t            count;                 /* TIMER_STATE_COUNT 状态计数 */
    uint32_t            interval;              /* 任务周期间隔 */
	
    pMultiTimerCallback callback;              /* 超时回调函数 */
    pCallback           callback2;             /* */
	
    struct MultiTimer*  next;
	
} xMultiTimer_t;

/* 定时器句柄列表头。 */
static xMultiTimer_t* timerList = NULL;

init_timerTask1(xMultiTimer_t *timerTaskx1)
{
	timerTaskx1->state 	  = TIMER_STATE_INIT;
	timerTaskx1->interval = 10u;
	timerTaskx1->callback = app_cantx_10ms_proc;
	timerTaskx1->next	  = NULL;
}

init_timerTask3(xMultiTimer_t *timerTaskx3)
{
	timerTaskx3->state 	  = TIMER_STATE_INIT;
	timerTaskx3->interval = 50u;
	timerTaskx3->callback = app_lamp_brink_proc;
}


void vMultiTimerStart(xMultiTimer_t* handle)
{
	xMultiTimer_t **nextTime = &timerList;
	
	for(; *nextTime; nextTime=&(*nextTime)->next)
	{
		if(*nextTime == handle)
		{
			return ;
		}
	}
	
	/*init tick*/
	if(privGetTick)
	{
		handle->tick  = privGetTick();
		handle->count = 0u;
	}
	else
	{
		return ;
	}

	for(nextTimer = &timerList;; nextTimer = &(*nextTimer)->next) 
	{
		if(NULL == (*nextTime))
		{
			(*nextTime)->next = handle;
		}
		else
		{
			
		}	
	}
}

void vMultiTimerHandler(void)
{
	xMultiTimer_t *handle = timerList;

	for(; handle; handle->next)
	{
		switch(hanle->state)
		{
			case TIMER_STATE_INIT:
									if(handle->callback)
									{
										handle->tick = privGetTick();
										handle->callback();
									}
									else
									{
										
									}
									handle->state = (uint8_t)TIMER_STATE_ACTIVE;
									break;
									
			case TIMER_STATE_ACTIVE:
									 if(privTaskTimeout(handle))
									 {
										 if(handle->callback)
										 {
											 handle->tick = privGetTick();
											 handle->callback;
										 }
										 else
										 {
											 
										 }
									 }
									 else
									 {
										 
									 }
									 break;
			case TIMER_STATE_COUNT:
									if(privTaskTimeout(handle))
									{
										if(handle->callback)
										{
											handle->tick = privGetTick();
											handle->callback();
										}
										if(0 == --handle->count)
										{
											 handle->state = TIMER_STATE_IDLE;
										}
										else
										{
											
										}
									}
									else
									{
										
									}
									break;
									
			case TIMER_STATE_IDLE:							
									break;
		}	
	}
}




volatile	 uint32_t 	g_iRunTime = 0; /* 1ms time value */
#define 	TICK_MAX 	0xFFFFFFFFL
void ISR_TICK(void)
{
	if(++g_iRunTime == TICK_MAX)
	{
		g_iRunTime = 0;
	}
	else
	{

	}

}

uint32_t GetMsTickVal(void)
{
	uint32_t Tick_Temp = 0;
	Tick_Temp = g_iRunTime;
	return Tick_Temp;
}


//是否超时
static uint8_t privTaskTimeout(xMultiTimer_t* handle)
{
	uint8_t ret 			= FALSE;
	uint32_t CurrentTickVal = 0;
	uint32_t TickInterval   = 0;

	CurrentTickVal = privGetTick();
	
	if(handle->tick <= CurrentTickVal)
	{
		TickInterval = CurrentTickVal-handle->tick;
	}
	else
	{
		TickInterval = (MT_TICK_MAX_VALUE-handle->tick)+CurrentTickVal;
	}
	
	if(handle->interval <= TickInterval)
	{
		ret = TRUE;
	}
	else
	{
		ret = FALSE;
	}
	
	return ret;
}




/****************************************CAN数据帧******************************************************/
typedef union 
{
    uint8_t data[8];
    
    struct 
    {
        uint8_t rsp_result;
        uint8_t rsp_data[7];
    }sig;
    
}app_canrx_nfc_560;

if (nfc_560_un.sig.rsp_result == 0x05 || nfc_560_un.sig.rsp_result == 0x06)
{
	nfc_rsp_rn136_result = nfc_560_un.sig.rsp_result;
	
	RTE_NFC_CTRL_SET_RN136_RESULT(nfc_rsp_rn136_result);
}

void app_canrx_recevie(RTE_CAN_PDU *can_pdu)
{
    switch(can_pdu->ID)
    {
		case APP_CANRX_ID_NFC_560:
									app_canrx_nfc_560_proc(can_pdu);
									break;
									
		case APP_CANRX_ID_NFC_561:
									app_canrx_nfc_561_proc(can_pdu);
									break;							
	
		case APP_CANRX_ID_ABS_202:
									app_canrx_abs_202_proc(can_pdu);
									break;
	
		case APP_CANRX_ID_ECU_101:
									app_canrx_ecu_101_proc(can_pdu);
									break;
	
		default:
									break;
    }
}
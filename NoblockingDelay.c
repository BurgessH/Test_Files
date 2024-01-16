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
		eState = Ticker_out(&uSampleTicker, 500);
		
		if(true == eState)
		{
			Ticker_out(&uSampleTicker, 0);
			LED_ON();
		}
	}
}
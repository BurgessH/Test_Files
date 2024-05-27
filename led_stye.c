
typedef enum
{
	LED_STATE_STOP = 0,								//LED停止
	LED_STATE_WORK = 1								//LED工作
}en_led_state;

typedef enum
{
	LED_MODE_CONTINUE 					= 0,						//常亮\灭
	LED_MODE_FREQUENCY_FLICKER 			= 1,						//频率闪烁
	LED_MODE_TIME_FLICKER				= 2,						//闪烁几次后熄灭
	LED_MODE_BREATHING 					= 3							//呼吸灯
}en_led_mode;


typedef enum
{
	BREATHING_MODE_UP,
	BREATHING_MODE_DOWN
}en_breathing_mode;



typedef struct
{
	en_led_state 	led_state;					//LED工作状态
	en_led_mode 	led_mode;					//闪烁的方式
	
	uint16_t 		time_cnt;					//
	uint16_t 		bright_cnt;					//点亮的时间
	uint16_t		extinct_cnt;				//熄灭的时间
	
	uint16_t 		frequency_cnt;				//周期次数
	uint16_t 		breathing_cnt;				//呼吸时间
	
	uint16_t 		breathing_cycle_cnt;
	uint16_t 		breathing_cycle;			//呼吸周期
	en_breathing_mode breathing_mode;			//呼吸模式，递增或递减
	
	void (*led_open)(void);						//打开LED
	void (*led_close)(void);					//关闭LED
}st_led;

enum
{
	LED_LTURN,								//左转灯
	LED_RTURN,								//右转灯
	
	LED_HBEAM,								//远光
	LED_LBEAM,								//近光
	
	LED_POSL,								//位置灯
	LED_LCONERL,							//左角灯
	LED_RCONERL,							//右角灯
	
	LED_COURL,								//迎宾灯
	LED_AMBL,								//氛围灯
	
	LED_TAILL,								//尾灯
	SPEAKER,								//喇叭
	LEDCONTROL_NUM
};
static st_led led[LEDCONTROL_NUM];


void ledcontrol_init(void)
{
	uint8_t i = 0;
	for(uint8_t i=0; i<LEDCONTROL_NUM; ++i)
	{
		led[i].led_state 		= LED_STATE_STOP;
		led[i].led_mode  		= LED_MODE_CONTINUE;
		led[i].time_cnt 		= 0;
		led[i].bright_cnt 		= 0;
		led[i].extinct_cnt 		= 0;
		led[i].frequency_cnt 	= 0;
	 }
	 
	 //	LED_LTURN,							//左转灯
	led[LED_LTURN].led_open 	= RTE_OPEN_LED_LTURN;
	led[LED_LTURN].led_close 	= RTE_CLOSE_LED_LTURN;
	
//	LED_RTURN,							//右转灯
	led[LED_RTURN].led_open 	= RTE_OPEN_LED_RTURN;
	led[LED_RTURN].led_close 	= RTE_CLOSE_LED_RTURN;
	
//	LED_HBEAM,							//远光
	led[LED_HBEAM].led_open 	= RTE_OPEN_LED_HBEAM;
	led[LED_HBEAM].led_close 	=RTE_CLOSE_LED_HBEAM;
	
//	LED_LBEAM,							//近光
	led[LED_LBEAM].led_open 	= RTE_OPEN_LED_LBEAM;
	led[LED_LBEAM].led_close 	= RTE_CLOSE_LED_LBEAM;
	
////	LED_POSL,								//位置灯
	led[LED_POSL].led_open 		= RTE_OPEN_LED_POSL;
	led[LED_POSL].led_close 	= RTE_CLOSE_LED_POSL;
	
//	LED_LCONERL,						//左角灯
	led[LED_LCONERL].led_open 	= RTE_OPEN_LED_LCONERL;
	led[LED_LCONERL].led_close 	= RTE_CLOSE_LED_LCONERL;
	
//	LED_RCONERL,						//右角灯
	led[LED_RCONERL].led_open 	= RTE_OPEN_LED_RCONERL;
	led[LED_RCONERL].led_close 	= RTE_CLOSE_LED_RCONERL;
	
//	LED_COURL,							//迎宾灯
	led[LED_COURL].led_open 	= RTE_OPEN_LED_COURL;
	led[LED_COURL].led_close 	= RTE_CLOSE_LED_COURL;
	
//	LED_AMBL,								//氛围灯
	led[LED_AMBL].led_open 		= RTE_OPEN_LED_AMBL;
	led[LED_AMBL].led_close 	= RTE_CLOSE_LED_AMBL;
	
//	LED_TAILL,							//尾灯
	led[LED_TAILL].led_open 	= RTE_OPEN_LED_TAILL;
	led[LED_TAILL].led_close 	= RTE_CLOSE_LED_TAILL;
	
//SPEAKER,									//喇叭
	led[SPEAKER].led_open  		= RTE_OPEN_SPEAKER;
	led[SPEAKER].led_close 		= RTE_CLOSE_SPEAKER;
	
}



void ledcontrol(st_led *pled)
{
	if(pled->led_state == LED_STATE_WORK)
	{
		switch(pled->led_mode)
		{
			case LED_MODE_CONTINUE:
									pled->led_open();
									break;
									
			case LED_MODE_FREQUENCY_FLICKER:
									ledcontrol_frequency(pled);
									break;
									
			case LED_MODE_TIME_FLICKER:
									ledcontrol_time(pled);
									break;
			default:
									break;							
		}
	}
	else
	{
		pled->led_close();
	}
}
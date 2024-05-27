
enum
{
	//GPIO
	FEEDBACK_LEFTBL,								//左边箱状态
	FEEDBACK_RIGHTBL,								//右边箱状态
	FEEDBACK_CUSBOX,								//坐垫箱状态
	
	FEEDBACK_WINDUP,								//电动挡风上止点
	FEEDBACK_WINDD,									//电动挡风下止点
	
	//ADC
	FEEDBACK_BAT,										//电源
	FEEDBACK_CELL,									//电池
	
	FEEDBACK_NUM
};

typedef enum
{
	FEEDBACK_MODE_GPIO=0,
	FEEDBACK_MODE_ADC
}en_feedback_mode;

typedef enum
{
	FEDDBACK_STATE_NORMAL  = 0,
	FEEDBACK_STATE_LFAULT  = 1,				//ADC状态（低压）
	FEEDBACK_STATE_HFAULT  = 2,				//ADC状态（高压）
	FEEDBACK_STATE_INVALID = 3,				//GPIO状态
	FEEDBACK_STATE_VALID   = 4
}en_feedback_state;


typedef struct
{
	uint16_t feedback_time;						//反馈状态变化持续时间
	uint16_t feedback_validtime;				//判断是否有效的时间
	hal_pin_level_et pin_activ_levle;			//有效状态时的电平
	hal_pin_level_et (*get_pin_value)(void);	//获取电平值函数
}st_feedback_gpio;


typedef struct
{
	uint16_t feedback_time;					//反馈状态变化持续时间
	uint16_t feedback_validtime;			//判断是否有效的时间
	uint16_t feedback_fault_value_H;		//故障参考值
	uint16_t feedback_fault_value_L;		//故障参考值
	uint16_t feedback_normal_value_H;		//正常参考值
	uint16_t feedback_normal_value_L;		//正常参考值
	uint16_t (*get_adc_value)(void);		//获得AD值函数
}st_feedback_adc;


typedef struct
{
	en_feedback_mode  feedback_mode_mode;	//反馈的类型:GPIO、ADC
	en_feedback_state feedback_state;		//反馈状态
	st_feedback_gpio  *st_gpio;				//反馈类型为高低电平时判断参数
	st_feedback_adc   *st_adc;				//反馈类型为adc时的判断参数//反馈类型为ADC时判断参数
}st_feedback;


static st_feedback 				feedback[FEEDBACK_NUM];
static st_feedback_gpio 		feedback_gpio[5];
static st_feedback_adc  		feedback_adc[2];

static void feedback_scan(void)
{
	uint16_t i = 0;
	
	for(i=0; i<FEEDBACK_NUM; ++i)
	{
		switch(feedback[i].feedback_mode)
		{
			case FEEDBACK_MODE_GPIO:
									feedback_gpio_scan(&feedback[i]);
									break;
									
			case FEEDBACK_MODE_ADC:
									feedback_adc_scan(&feedback[i]);
									break;
									
			default:
									break;
		}
	}
}
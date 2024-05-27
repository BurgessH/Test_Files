/*
 *   文件名:    FILE NAME
 *   作者:      -
 *   日期:      -
 *   版本:      -
 *   说明:       -
 *   修改记录:   -
 */

/* LED信息  */
typedef struct led_infor
{
	en_led_state        led_state;					    //LED工作状态
	en_led_mode         led_mode;					    //闪烁的方式
	
	uint16_t            time_cnt;						//计数值
	uint16_t            bright_cnt;						//点亮的时间
	uint16_t            extinct_cnt;					//熄灭的时间

	uint16_t            frequency_cnt;						//周期次数
	uint16_t            breathing_cnt;						//呼吸时间
	uint16_t            breathing_cycle_cnt;
	uint16_t            breathing_cycle;					//呼吸周期
	en_breathing_mode   breathing_mode;			//呼吸模式，递增或递减
	
	void (*led_open)(void);						//打开LED
	void (*led_close)(void);					//关闭LED
	
}led_infor_s;


typedef struct
{
		en_led_mode 		led_mode;								//设置工作方式
		en_led_state 		led_state;								//设置LED状态
		uint8_t 			led_index;								//LED序号
		uint16_t 			bright_cnt;								//点亮时间
		uint16_t 			extinct_cnt;							//熄灭时间
		uint16_t 			breathing_cycle;					//呼吸周期			
		uint16_t 			frequency_cnt;						//周期次数
}led_config_st;



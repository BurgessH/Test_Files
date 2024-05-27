#define	ADCFILTER_BUFFER_SIZE			10

typedef struct
{
	uint16_t (*get_adc_value)(void);				//获得AD值函数
	uint16_t data[ADCFILTER_BUFFER_SIZE];			//数据缓冲区
	uint8_t	 data_index;							//当前数据下标
	uint8_t  data_len;								//数据缓存数量
	uint16_t average_value;
}st_adcfilter;

enum
{
	ADC_SUN,				//光敏
	ADC_PRESS,				//压力
	ADC_BAT,
	ADC_FILTER_NUM
};

static void adc_filter(uint8_t index)
{
    uint8_t i 			= 0;
	
    /* 每10ms 获取一次ADC采样值*/
    if(adcfilter[index].data_len < ADCFILTER_BUFFER_SIZE)
    {
        adcfilter[index].data[adcfilter[index].data_index++] = adcfilter[index].get_adc_value();
        adcfilter[index].data_len++;
    }
    else
    {
        printf("adcfilter[%d].data_len 		= %d \n",  index,  adcfilter[index].data_len);
        printf("adcfilter[%d].data_index 	= %d \n",  index,  adcfilter[index].data_index);

        for(uint8_t j=0; j<ADC_FILTER_NUM; j++)
        {
            for(i=0; i<adcfilter[j].data_len; i++)
            {
                 printf("adcfilter[%d].data[%d] = %d \n", j, i, adcfilter[j].data[i]);
                 adcfilter[j].data_sum += adcfilter[j].data[i];
            }
            printf("adcfilter[%d].data_sum = %d \n", j, adcfilter[j].data_sum);

            adcfilter[j].average_value = (float)adcfilter[j].data_sum/adcfilter[j].data_len;
            printf("adcfilter[%d].average_value = %.2f \n", j, adcfilter[j].average_value);

            adcfilter[j].data_index = 0;
            adcfilter[j].data_len   = 0;

            printf("\n");
        }
    }
}


void adcfilter_func(void)
{
	uint8_t i=0;
	for(i=0; i<ADC_FILTER_NUM; ++i)
	{
		adc_filter(i);
	}
	
	
}

typedef struct
{
	en_scan_mode 		scan_mode;			//按键类型
	uint16_t 			press_time;			//按键按下的时间
	uint16_t 			press_faulttime;	//按键故障判断时间
	uint16_t 			press_validtime;	//按键有效时间
	en_key_state 		key_state;			//获得的按键状态
	hal_pin_level_et 	pin_active_level;	//按键按下时的有效电平
	hal_pin_level_et (*get_pin_value)(void);//读取引脚状态函数
}st_key;



void scan_key_self_recovery(st_key *pkey)
{
	if(((pkey->pin_active_level == pkey->get_pin_value()))
	{
		if(pkey->press_time < pkey->press_fault_time)
		{
			pkey->press_time ++;
		}
		else
		{
			pkey->key_state  = KEY_STATE_FAULT;
		}
	}
	else
	{
		if(KEY_STATE_FAULT == pkey->key_state)
		{
			pkey->key_state  = KEY_STATE_INVALID;
			pkey->press_time = 0;
		}
		else
		{
			if(pkey->press_time >= pkey->press_valid_time)
			{
				
				pkey->key_state  = KEY_STATE_VALID;
				pkey->press_time = 0;
			}
			else
			{
				pkey->key_state  = KEY_STATE_INVALID;
				pkey->press_time = 0;
			}
		}	
	}
	
}

}

void scan_key_self_locking(st_key *pkey)
{	
	if((KEY_STATE_INVALID == pkey->key_state)&&((pkey->pin_active_level == pkey->get_pin_value()))
	{
		if(pkey->press_time < pkey->press_valid_time)
		{
			pkey->press_time ++;
		}
		else
		{
			pkey->key_state  = KEY_STATE_VALID;
			pkey->press_time = 0;
		}
	}
	else if((KEY_STATE_VALID == pkey->key_state)&&((pkey->pin_active_level != pkey->get_pin_value()))
	{
		if(pkey->press_time < pkey->press_valid_time)
		{
			pkey->press_time ++;
		}
		else
		{
			pkey->key_state  = KEY_STATE_INVALID;
			pkey->press_time = 0;
		}
	}
	else
	{
	}	
}


void scan_key(void)
{
	for(i=0; i<KEY_NUM; ++i)
	{
		if(NULL != key[i].get_pin_value)
		{
			switch(key[i].scan_mode)
			{
				case SCAN_MODE_SELF_RECOVERY:
												scan_key_self_locking(&key[i]);
												break;
												
				case SCAN_MODE_SELF_LOCKING:
												scan_key_self_locking(&key[i]);
												break;
												
				default:
												break;
			}
		}
		else
		{
			key[i].key_state = KEY_STATE_FAULT;
		}
	}
	
}
/*
 *   文件名: FILE NAME
 *   作者:    -
 *   日期:    -
 *   版本:    -
 *   说明:    -
 *   修改记录: -
 */

/* 按键信息 左右边箱、远近灯、左右转向、应急、超车、位置、自动大灯、刹车、座垫、尾箱、边撑*/
typedef struct key_infor
{
    //按键类型： 自恢复、自锁
    
    //按键按下时间
    //按键故障判断时间

    //按键有效时间
    //按键状态

    //按键有效电平
    //读取引脚状态FUNC
}key_infor_s;



static void scan_key_self_recovery(st_key *pkey)
{
	
	if(pkey->pin_active_level == pkey->get_pin_value())
	{
		if(pkey->press_time < pkey->press_faulttime)//按键按下记时
		{
			pkey->press_time++;
		}
		else
		{
			pkey->key_state = KEY_STATE_FAULT;
		}
	}
	else	//故障弹起
	{
		if(KEY_STATE_FAULT == pkey->key_state)
		{
			pkey->key_state  = KEY_STATE_INVALID;
			pkey->press_time = 0;
		}
		else//正常弹起
		{
			if(pkey->press_time >= pkey->press_validtime)
			{
				pkey->key_state  = KEY_STATE_VALID;
				pkey->press_time = 0;
			}
			else
			{
				pkey->key_state = KEY_STATE_INVALID;
				pkey->press_time = 0;
			}
		}
	}
}

static void scan_key_self_locking(st_key *pkey)
{
	//按下记时：
	if((KEY_STATE_INVALID == pkey->key_state) && (pkey->pin_active_level == pkey->get_pin_value()))
	{
			if(pkey->press_time < pkey->press_validtime)
			{
				pkey->press_time++;
			}
			else
			{
				pkey->key_state  = KEY_STATE_VALID;
				pkey->press_time = 0;
			}
	}
	else if((KEY_STATE_VALID == pkey->key_state) && (pkey->pin_active_level != pkey->get_pin_value()))//弹起记时：
	{
		if(pkey->press_time < pkey->press_validtime)
		{
			pkey->press_time++;
		}
		else
		{
			pkey->key_state = KEY_STATE_INVALID;
			pkey->press_time = 0;
		}
	}
	else
	{
	
	}
}

static void scan_key(void)
{
	uint16_t i = 0;
	
	for(i=0; i<KEY_NUM; ++i)//遍历每个按键
	{
		if(NULL != key[i].get_pin_value)
		{
				switch(key[i].scan_mode)
				{
					case SCAN_MODE_SELF_RECOVERY:
													scan_key_self_recovery(&key[i]);
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
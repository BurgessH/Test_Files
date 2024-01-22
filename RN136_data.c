// CRC8多项式（x^8 + x^2 + x + 1）
#define CRC8_POLYNOMIAL 0x07

/* RN136 Rand Data 17 Byte*/
uint8_t  rand_buf[17]  = {0};


/* RN136 Data */
uint8_t  rn136_byte0[8] = {0};
uint8_t  rn136_byte1[8] = {0};
uint8_t  rn136_byte2[8] = {0};
uint8_t  rn136_byte3[8] = {0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};


/*  Test RN136 Rand Data 17 Byte*/
uint8_t cal_data[]={0x26, 0x29, 0x2D, 0x30, 0x33, 0x36, 0x3A, 0x3D, 0x40, 0x44, 0x47, 0x4A, 0x4D, 0x51, 0x54, 0x57, 0x5A};



/* 随机数生成17个字节 数据大小范围：0-255*/
void rands_num(uint8_t *buf)
{
    int i;
    for(i=0; i<17; i++)
    {
        srand(i);
        rand_buf[i] = rand()%256;

        //printf("rand_buf[%d] = %X \n", i, rand_buf[i]);
    }

}

// 计算CRC8校验值:
unsigned char crc8(const unsigned char *data, size_t size)
{
    unsigned char crc = 0;

    for (size_t i = 0; i < size; i++)
    {
        crc ^= data[i];

        for (int j = 0; j < 8; j++)
        {
            if (crc & 0x80)
            {
                crc = (crc << 1) ^ CRC8_POLYNOMIAL;
            }
            else
            {
                crc <<= 1;
            }
        }
    }

    return crc;
}

/* RN136数据帧 0byte */
void Rn136_data_byte0(uint8_t *rn136_byte0, uint8_t *buf)
{
    uint8_t i;

    rn136_byte0[0] = 0x11;

    for(i=1; i<8; i++)
    {
        rn136_byte0[i] = buf[i-1];

        //printf("rn136_byte0[%d] = %d \n", i, rn136_byte0[i]);
    }
}

/* RN136数据帧 1byte */
void Rn136_data_byte1(uint8_t *rn136_byte1, uint8_t *buf)
{
    uint8_t i;

    rn136_byte1[0] = 0x12;

    for(i=7; i<14; i++)
    {
        rn136_byte1[i-6] = buf[i];
    }
}

/* RN136数据帧 2byte */
void Rn136_data_byte2(uint8_t *rn136_byte2, uint8_t *buf)
{
    uint8_t i;

    rn136_byte2[0] = 0x13;

    for(i=14; i<21; i++)
    {
        if(i>16)
        {
            if(i==20)
            {
                rn136_byte2[i-13] = crc8(rand_buf, sizeof(rand_buf)/sizeof(rand_buf[0]));
                break;
            }
            rn136_byte2[i-13] = 0x00;
        }
        else
        {
            rn136_byte2[i-13] = buf[i];
        }

    }

}


typedef struct 
{
        uint8_t* buff[32];
        uint32_t ID;
        ID_type  id_type;
        uint8_t  DLC;
}pdu_str;


pdu_str 		pdu;

//BCM CAN初始化配置
void dbc_tx_data0_init(void)
{
	uint8_t cnt = 0;
	
	str  			hal_strtest;
	filter_struct 	can_filter_array[3];

    pdu.DLC 							= 32u;
    pdu.ID 								= 0x300;

    hal_strtest.arbitr 					= CANHAL_KBPS_500;
    hal_strtest.canenabled 				= CAN_ENABLE;
    
    can_filter_array[0].id_type 		= ID_TYPE_STAND;
    can_filter_array[0].filter_code 	= 0x561;
    can_filter_array[0].filter_mask 	= 0xffffffffU;
    
    
    can_filter_array[1].id_type 		= ID_TYPE_STAND;
    can_filter_array[1].filter_code 	= 0x200;
    can_filter_array[1].filter_mask 	= 0xffffffffU;
    
    
    can_filter_array[2].id_type 		= ID_TYPE_STAND;
    can_filter_array[2].filter_code 	= 0x500;
    can_filter_array[2].filter_mask 	= 0xffffffffU;
    
    
    for(cnt=0; cnt<8; cnt++)
    {
    	pdu.buff[cnt]       = &Brn136_byte0[cnt]; 
        pdu.buff[cnt+8]     = &Brn136_byte1[cnt]; 
        pdu.buff[cnt+16]    = &Brn136_byte2[cnt]; 
        pdu.buff[cnt+24]    = &Brn136_byte3[cnt];
    }
    
    hal_can_trcv_mode(CAN_CHANNEL0, 	HAL_CAN_TRCV_MODE_NORMAL);
    hal_can_init(CAN_CHANNEL0, 			&hal_strtest);
    hal_can_filter_config(CAN_CHANNEL0, can_filter_array, 3);
}


typedef struct 
{
        uint8_t* buff[8];
        uint32_t ID;
        ID_type  id_type;
        uint8_t  DLC;
}pdu_str;

pdu_str Brn136_byte0;
pdu_str Brn136_byte1;
pdu_str Brn136_byte2;
pdu_str Brn136_byte3;

init_Brn136_byte0(pdu_str *ptr)
{
    uint i;
    ptr->DLC = 8u;
    ptr->ID  = 0x561;
    for(i=0; i<8; i++)
    {
        ptr->buff[i] = &rn136_byte0[i];
    }
}

init_Brn136_byte1(pdu_str *ptr)
{
    uint i;
    ptr->DLC = 8u;
    ptr->ID  = 0x561;
    for(i=0; i<8; i++)
    {
        ptr->buff[i] = &rn136_byte1[i];
    }
}

init_Brn136_byte2(pdu_str *ptr)
{
    uint i;
    ptr->DLC = 8u;
    ptr->ID  = 0x561;
    for(i=0; i<8; i++)
    {
        ptr->buff[i] = &rn136_byte2[i];
    }
}

init_Brn136_byte3(pdu_str *ptr)
{
    uint i;
    ptr->DLC = 8u;
    ptr->ID  = 0x561;
    for(i=0; i<8; i++)
    {
        ptr->buff[i] = &rn136_byte3[i];
    }
}





void init_rn136_data(void)
{
    init_Brn136_byte0(&Brn136_byte0);
    init_Brn136_byte1(&Brn136_byte1);
    init_Brn136_byte2(&Brn136_byte2);
    init_Brn136_byte3(&Brn136_byte3);
}


[0]	    0x63	        RN0
[1]	    0x2a	        RN1
[2]	    0xf0	        RN2     
[3]	    0xb6	        RN3
[4]	    0x7d	        RN4
[5] 	0x43	        RN5
[6]	    0x09	        RN6

[7]	    0xcf	        RN7
[8]	    0x96	        RN8
[9]	    0x5c	        RN9
[10]	0x22	        RN10
[11]	0xe9	        RN11
[12]	0xaf	        RN12
[13]	0x75	        RN13

[14]	0x3c	        RN14
[15]	0x02	        RN15
[16]	0xc8	        RN16
[17]	0x00	        
[18]	0x00	        
[19]	0x00	        



void Can_rec_data(const pdu_str * nfc)
{
stict
}
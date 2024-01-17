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
                rn136_byte2[i-13] = crc8(rand_buf, sizeof(rand_buf)/sizeof (rand_buf[0]));
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


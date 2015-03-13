#include "preprocess.h"

/**
 * [BU_CAryToDFNum1 将4字节的二进制无符号字符型比特数组转化为十进制浮点型数字]
 * @param  hex [description]
 * @param  i   [description]
 * @return     [description]
 */
float BU_CAryToDFNum1(const uint8_t *hex, int i)
{
	uint8_t hex_data[4];
	float *result;
    memset(hex_data, 0, sizeof(hex_data));

 	/*big-endian need to change little-endian*/
    hex_data[0] = hex[i];
	hex_data[1] = hex[i-1];
	hex_data[2] = hex[i-2];
	hex_data[3] = hex[i-3];
	result = (float *)&hex_data;
	return *result;
}

/**
 * [BU_CAryToDFNum 将4字节的二进制无符号字符型比特数组转化为十进制浮点型数字]
 * @param  code [二进制无符号字符型比特数组]
 * @return      [十进制浮点型数字]
 */
float BU_CAryToDFNum(const uint8_t *code)
{
	uint8_t mid_re[4] = {'\0'};
	float *result;
	mid_re[0] = code[3];
	mid_re[1] = code[2];
	mid_re[2] = code[1];
	mid_re[3] = code[0];
	result = (float *)&mid_re;
	return *result;
}

/**
 * [BU_CAryToDINum description]
 * @param  code [description]
 * @param  num  [description]
 * @return      [description]
 */
int BU_CAryToDU_INum(const uint8_t *code, int num)
{
	if(1 == num)
	{
		int8_t *result;
		result = (int8_t *)code;
		return *result;
	}
	else if(2 == num)
	{
		int16_t *result;
		uint8_t mid_re[2] = {0};
		mid_re[0] = code[1];
		mid_re[1] = code[0];
		result = (int16_t *)mid_re;
		return *result;
	}
	else if(4 == num)
	{
		int32_t *result;
		uint8_t mid_re[4] = {0};
		mid_re[0] = code[3];
		mid_re[1] = code[2];
		mid_re[2] = code[1];
		mid_re[3] = code[0];
		result = (int32_t *)mid_re;
		return *result;
	}
	return 0;
}

/**
 * [HCStrToDFStr 将十六进制字符型字符串转换为十进制浮点型字符串,其中每个浮点数有4个有效数字. ("")]
 * @param  code   [十六进制字符型字符串]
 * @param  result [十进制浮点型字符串]
 * @param  num    [code长度]
 * @return        [0: 成功, 负数: 失败]
 */
int HCStrToDFStr(const uint8_t *code, uint8_t *result, int num)
{
	int i;
	int hex_dat_num;
	uint8_t hex_dat[num];
	uint8_t *hex_poi = NULL;
	uint8_t *res_poi = NULL;
	memset(hex_dat, '\0', num);
	if((hex_dat_num = HCStrToBU_CAry(code, hex_dat, num)) < 0)
	{
		perror("HCStrToDFStr error: function HCStrToBU_CAry error");
		return -1;
	}
	//printf("hex_dat_num: %d\n", hex_dat_num);
	hex_poi = hex_dat;
	res_poi = result;
	for(i=0; i<hex_dat_num; i+=4)
	{
		double float_re;
		float_re = (double)BU_CAryToDFNum(hex_poi);
		sprintf(res_poi, "%10.3f", float_re);
		//printf("res_poi: %s\n", res_poi);
		res_poi += strlen(res_poi);
		hex_poi += 4;
	}
	return 0;
}


/**
 * [HCStrToDINum 将十六进制字符型字符串转换为十进制整形数字. ("F4" -> 0d12)]
 * @param  code [十六进制字符型字符串]
 * @param  num  [code长度]
 * @return      [十进制整形数字]
 */
int HCStrToDINum(const uint8_t *code, int num)
{
	int i;
	int result = 0;
	int mid_re = 0;
	for(i=0; i<num; i++)
	{
		if(code[i]>='0' && code[i]<='9')
			mid_re = (int)(code[i]-'0');
		else if(code[i]>='A' && code[i]<='F')
			mid_re = (int)(code[i]-'A')+10;
		else if(code[i]>='a' && code[i]<='f')
			mid_re = (int)(code[i]-'a')+10;
		else
			perror("HCStrToDINum error: unexpected the  abnormal character");
		result *=16;
		result += mid_re;
	}
	return result;
}

/**
 * [HCStrToDU_INum description]
 * @param  code [description]
 * @param  num  [description]
 * @return      [description]
 */
int HCStrToDU_INum(const uint8_t *code, int num)
{
	if(num > 8)
	{
		perror("HCStrToDU_INum error: string length > 8");
		return -1;
	}
	uint8_t mid_re[10] = {0};
	int mid_len = 0;
	int result = 0;

	mid_len = HCStrToBU_CAry(code, mid_re, num);
	result = BU_CAryToDU_INum(mid_re, mid_len);

	return result;
}


/**
 * [DIStrToDINum 将十进制整形字符串转换为十进制整形数字. ("12" -> 0d12)]
 * @param  code [十进制整形字符串]
 * @param  num  [code长度]
 * @return      [十进制整形数字]
 */
int DIStrToDINum(const uint8_t *code, int num)
{
	int i;
	int result = 0;
	for(i=0; i<num; i++)
	{
		if(code[i]>='0' && code[i]<='9')
		{
			result *=10;
			result += (int)(code[i]-'0');
		}
		else
			perror("DIStrToDINum error: unexpected the character except '0'-'9'");
	}
	return result;
}


/**
 * [DINumToDIStr 将十进制整形数字转换为十进制整形字符串. (0d12 -> "12")]
 * @param arg1    [十进制整形数字]
 * @param result1 [十进制整形字符串]
 * @param num     [设置result1的长度，若result1长度大于arg1的位数，则result1前面添'0']
 */
void DINumToDIStr(int arg1, uint8_t *result1, int num)
{
	int i;

	sprintf(result1, "%d", arg1);
	int len = strlen(result1);

	if(num-len > 0)
	{
		for(i=len-1; i>=0; i--)
		{
			result1[i+num-len] = result1[i];
		}
		for(i=0; i<num-len; i++)
		{
			result1[i] = '0';
		}
	}
}


/**
 * [BU_CAryToDINum 将二进制无符号字符型比特数组转换为十进制整形数字. (0b11110100 -> 0d12)]
 * @param  code [二进制无符号字符型比特数组]
 * @param  num  [code长度]
 * @return      [十进制整形数字]
 */
int BU_CAryToDINum(const uint8_t *code, int num)
{
	int i;
	int result = 0;
	for(i=0; i<num; i++)
	{
		result *= 256;
		result += (int)code[i];
	}
	return result;
}


/**
 * [HU_CNumToHCStr 将十六进制无符号字符型数字转换为十六进制字符型字符串. (0hF4A3 -> "F4A3")]
 * @param hex [十六进制无符号字符型数字]
 * @param str [十六进制字符型字符串]
 */
void HU_CNumToHCStr(uint16_t hex, uint8_t *str)
{
    uint16_t temp = hex;
    int i;
    for(i = 3; i >= 0; i--)
    {
        temp = hex % 16;
        hex >>= 4;
        if(temp >= 10)
           str[i] = temp + 7 +'0';
        else
           str[i] = temp + '0';
    }
    str[4] = '\0';
}


/**
 * [BU_CAryToHCStr 将二进制无符号字符型比特数组转换为十六进制字符型字符串. (0b11110100 -> "F4")]
 * @param code [二进制无符号字符型比特数组]
 * @param str  [十六进制字符型字符串]
 * @param num  [code长度]
 */
void BU_CAryToHCStr(const uint8_t *code, uint8_t *str, int num)
{
	int i, j;
	uint8_t high_4bit, low_4bit;
	for(i=0,j=0; i<num; i++,j+=2)
	{
		high_4bit = '\0';
		low_4bit = '\0';
		high_4bit = (code[i]>>4) & 0x0f;
		low_4bit = (code[i] & 0x0f);

		if((int)high_4bit >= 10)
			str[j] = high_4bit + 7 + '0';
		else
			str[j] = high_4bit + '0';
		if((int)low_4bit >= 10)
			str[j+1] = low_4bit + 7 + '0';
		else
			str[j+1] = low_4bit + '0';
	}
	str[j] = '\0';
}


/**
 * [HCStrToBU_CAry 将十六进制字符型字符串转换为二进制无符号字符型比特数组,但是字符串长度必须是偶数. ("F4" -> 0b11110100)]
 * @param  code   [十六进制字符型字符串]
 * @param  result [二进制无符号字符型比特数组]
 * @param  num    [code长度]
 * @return        [result长度]
 */
int HCStrToBU_CAry(const uint8_t *code, uint8_t *result, int num)
{
	int i, j, k;
	uint8_t mid_re;
	if(num%2 == 1)
	{
		perror("HCStrToBU_CAry error: unexpected byte number");
		return -1;
	}
	for(i=0,k=0; i<num; i+=2,k++)
	{
		result[k] = 0;
		mid_re = 0;
		for(j=0; j<2; j++)
		{
			if(code[i+j]>='0' && code[i+j]<='9')
				mid_re = code[i+j]-'0';
			else if(code[i+j]>='A' && code[i+j]<='F')
				mid_re = code[i+j]-'A'+10;
			else if(code[i+j]>='a' && code[i+j]<='f')
				mid_re = code[i+j]-'a'+10;
			else
			{
				perror("HCStrToBU_CAry error: unexpected the abnormal character");
				return -1;
			}
			result[k] *=16;
			result[k] += mid_re;
		}
		//printf("result[k]: %X\n", result[k]);
	}
	//printf("num: %d\n", k);
	return k;
}

/**
 * [strrstr 检索子串在字符串中最后出现的位置]
 * @param  str    [字符串]
 * @param  substr [子串]
 * @return        [最后出现的位置]
 */
char *strrstr(char *str, char *substr)
{
	char *front = NULL;
	char *rear = NULL;
	char *curLoca = str;
	while(curLoca)
	{
		if(front = strstr(curLoca, substr))
		{
			rear = front;
			curLoca = front + strlen(substr);
		}
		else
		{
			return rear;
		}
	}
	return rear;
}


/**
 * [CRC16 生成16位的CRC校验码]
 * @param  p_uiData     [欲校验的比特串]
 * @param  uiCrcDataLen [比特串的长度]
 * @return              [生成16位的CRC校验码]
 */
uint16_t CRC16(const uint8_t  *p_uiData, int uiCrcDataLen)
{
	const uint8_t c_uiCrcHigh[256] =
	{
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,	0x80, 0x41,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,	0x00, 0xC1, 0x81, 0x40,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x01, 0xC0, 0x80, 0x41,	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
		0x01, 0xC0,	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
		0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
	};
	const uint8_t c_uiCrcLow[256] =
	{
		0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
		0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,	0x08, 0xC8,
		0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
		0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,	0x11, 0xD1, 0xD0, 0x10,
		0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
		0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
		0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
		0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
		0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
		0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
		0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
		0xB4, 0x74, 0x75, 0xB5,	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
		0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
		0x9C, 0x5C,	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
		0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
		0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40
	};
	uint16_t uiCrcHi;
	uint16_t uiCrcLo;
	uint16_t uiIndex;
	uiCrcHi = 0xFF;
	uiCrcLo = 0xFF;
	uiIndex = 0;
	while( uiCrcDataLen-- )
	{
		uiIndex  = uiCrcHi ^ *p_uiData++ ;
		uiIndex &= 0xFF;
		uiCrcHi  = uiCrcLo ^ c_uiCrcHigh[uiIndex];
		uiCrcHi &= 0xFF;
		uiCrcLo  = c_uiCrcLow[uiIndex] ;
		uiCrcLo &= 0xFF;
	}
	uiCrcHi <<= 8;
	uiCrcHi  += uiCrcLo;

	return ( uiCrcHi );
}


/**
 * [hj212CRC16 国标CRC校验，生成16位的CRC校验码]
 * @param  buf   [欲校验的比特串]
 * @param  lenth [比特串的长度]
 * @return       [生成16位的CRC校验码]
 */
uint16_t hj212CRC16(const char *buf, int lenth)
{
    uint8_t  x=0,j;
    uint16_t lwrdCrc = 0xFFFF;
    uint8_t  lwrdMoveOut;
    while (lenth --)
    {
        x = lwrdCrc >> 8;
        lwrdCrc = (uint16_t)(*buf ++ ^ x);
        j = 0;

        while (j < 8)
        {
            lwrdMoveOut = (uint8_t)(lwrdCrc & 0x0001);
            lwrdCrc = lwrdCrc >> 1;
            if (lwrdMoveOut)
            {
                lwrdCrc = (lwrdCrc ^ 0xa001);
            }
            j++;
        }
    }
    return lwrdCrc;
}

/**
 * [gettimestamp get the timestamp commanded :YmdHMS]
 * @param  time_now [the function's result]
 * @return          [0,success]
 */
int gettimestamp(char *time_now)
{
    time_t current;
    struct tm *currTime;
    time(&current);
    currTime = localtime(&current);

    strftime(time_now, 30, "%F %T", currTime);
    return 0;
}

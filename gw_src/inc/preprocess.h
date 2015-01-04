#ifndef _PREPROCESS_H_
#define _PREPROCESS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

/**
 * [hexToFloat description]
 * @param  hex [description]
 * @param  i   [description]
 * @return     [description]
 */
float BU_CAryToDFNum1(const uint8_t *hex, int i);

/**
 * [BU_CAryToDFNum 将4字节的二进制无符号字符型比特数组转化为十进制浮点型数字]
 * @param  code [二进制无符号字符型比特数组]
 * @return      [十进制浮点型数字]
 */
float BU_CAryToDFNum(const uint8_t *code);

/**
 * [BU_CAryToDU_INum description]
 * @param  code [description]
 * @param  num  [description]
 * @return      [description]
 */
int BU_CAryToDU_INum(const uint8_t *code, int num);

/**
 * [HCStrToDFStr 将十六进制字符型字符串转换为十进制浮点型字符串,其中每个浮点数有4个有效数字]
 * @param  code   [十六进制字符型字符串]
 * @param  result [十进制浮点型字符串]
 * @param  num    [code长度]
 * @return        [0: 成功, 负数: 失败]
 */
int HCStrToDFStr(const uint8_t *code, uint8_t *result, int num);

/**
 * [HCStrToDINum 将十六进制字符型字符串转换为十进制整形数字]
 * @param  code [十六进制字符型字符串]
 * @param  num  [code长度]
 * @return      [十进制整形数字]
 */
int HCStrToDINum(const uint8_t *code, int num);

/**
 * [HCStrToDU_INum description]
 * @param  code [description]
 * @param  num  [description]
 * @return      [description]
 */
int HCStrToDU_INum(const uint8_t *code, int num);

/**
 * [DIStrToDINum 将十进制整形字符串转换为十进制整形数字]
 * @param  code [十进制整形字符串]
 * @param  num  [code长度]
 * @return      [十进制整形数字]
 */
int DIStrToDINum(const uint8_t *code, int num);

/**
 * [BU_CAryToDINum 将二进制无符号字符型比特数组转换为十进制整形数字]
 * @param  code [二进制无符号字符型比特数组]
 * @param  num  [code长度]
 * @return      [十进制整形数字]
 */
int BU_CAryToDINum(const uint8_t *code, int num);

/**
 * [HU_CNumToHCStr 将十六进制无符号字符型数字转换为十六进制字符型字符串]
 * @param hex [十六进制无符号字符型数字]
 * @param str [十六进制字符型字符串]
 */
void HU_CNumToHCStr(uint16_t hex, uint8_t *str);

/**
 * [BU_CAryToHCStr 将二进制无符号字符型比特数组转换为十六进制字符型字符串]
 * @param code [二进制无符号字符型比特数组]
 * @param str  [十六进制字符型字符串]
 * @param num  [code长度]
 */
void BU_CAryToHCStr(const uint8_t *code, uint8_t *str, int num);

/**
 * [HCStrToBU_CAry 将十六进制字符型字符串转换为二进制无符号字符型比特数组,但是字符串长度必须是偶数]
 * @param  code   [十六进制字符型字符串]
 * @param  result [二进制无符号字符型比特数组]
 * @param  num    [code长度]
 * @return        [result长度]
 */
int HCStrToBU_CAry(const uint8_t *code, uint8_t *result, int num);

/**
 * [DINumToDIStr 将十进制整形数字转换为十进制整形字符串]
 * @param arg1    [十进制整形数字]
 * @param result1 [十进制整形字符串]
 * @param num     [设置result1的长度，若result1长度大于arg1的位数，则result1前面添'0']
 */
void DINumToDIStr(int arg1, uint8_t *result1, int num);

/**
 * [strrstr 检索子串在字符串中最后出现的位置]
 * @param  str    [字符串]
 * @param  substr [子串]
 * @return        [最后出现的位置]
 */
char *strrstr(char *str, char *substr);

/**
 * [CRC16 生成16位的CRC校验码]
 * @param  p_uiData     [欲校验的比特串]
 * @param  uiCrcDataLen [比特串的长度]
 * @return              [生成16位的CRC校验码]
 */
uint16_t CRC16(const uint8_t *p_uiData, int uiCrcDataLen);

/**
 * [hj212CRC16 国标CRC校验，生成16位的CRC校验码]
 * @param  buf   [欲校验的比特串]
 * @param  lenth [比特串的长度]
 * @return       [生成16位的CRC校验码]
 */
uint16_t hj212CRC16(const char *buf, int lenth);

/**
 * [gettimestamp description]
 * @param  time_now [description]
 * @return          [description]
 */
//int gettimestamp(char *time_now);

#endif

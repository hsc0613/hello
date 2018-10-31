
/**
 * @file  common_lib.c
 * @brief  the common functions
 * @auth  fangyuejian
 * @version ver1.0
 * @date  2018/07/12
*/

#include "common_lib.h"
#include "common_mem.h"
#include "common_convert.h"


#define FLT_MAX         3.402823466e+38F        /* max float32_t value */


/**
 * @brief 求平方根运算
 * @param value 输入浮点数
 * @return 计算结果
 * @auth wufuwei
 * @date 2018/07/27
 */
float32_t SquareRoot(float32_t value)
{
    uint8_t i;
    float32_t xn;

    if (value <= 0.0f)
        return 0.0f;

    xn = value;
    *((uint16_t*)&xn + 1) += 0x3F80;
    *((uint16_t*)&xn + 1) >>= 1;

    for (i = 4; i > 0; i--)             /* 迭代计算 */
    {
        xn = xn + value / xn;
        *((uint16_t*)&xn + 1) -= 0x80;
    }

    return xn;
}

/**
 * @brief 2字节整型高低字节交换
 * @param value 输入的2字节整型
 * @return 交换字节后的2字节整型
 * @auth wufuwei
 * @date 2018/07/27
 */
uint16_t SwapWord(uint16_t value)
{
    uint16_t temp;
    uint16_t temp1;
    uint16_t temp2;

    temp1 = value >> 8;
    temp1 &= 0x00ff;
    temp2 = value << 8;
    temp2 &= 0xff00;
    temp = temp1 | temp2;

    return temp;
}

/**
 * @brief 求立方根运算
 * @param value 输入浮点数
 * @return 计算结果
 * @auth wufuwei
 * @date 2018/07/27
 */
float32_t CubeRoot(float value)
{
    uint8_t i;
    uint16_t hbyte;
    uint16_t quotient;
    uint16_t remainder;
    float32_t xn;
    float32_t xn2;

    if (value == 0.0f)
        return 0.0f;

    xn = value;
    hbyte = SwapWord(*((uint16_t*)&xn + 1) << 1 & 0xFF00) + 254;
    quotient = hbyte / 3;
    remainder = hbyte - (quotient + (quotient << 1));
    hbyte = (SwapWord(quotient) >> 1) + (*((uint16_t*)&xn + 1) & 0x8000);

    if (remainder == 2)
    {
        *(uint16_t*)&xn = 0x9768;
        *((uint16_t*)&xn + 1) = hbyte + (0x3FE8 - 0x3F80);
    }
    else if (remainder == 1)
    {
        *(uint16_t*)&xn = 0x9BA2;
        *((uint16_t*)&xn + 1) = hbyte + (0x3FB8 - 0x3F80);
    }
    else
    {
        *(uint16_t*)&xn = 0x85FF;
        *((uint16_t*)&xn + 1) = hbyte + (0x3F92 - 0x3F80);
    }

    for (i = 4; i > 0; i--)             /* 迭代计算 */
    {
        xn2 = xn;
        *((uint16_t*)&xn2 + 1) += 0x80;
        xn = (1 / 3.0f) * (xn2 + value / (xn * xn));
    }

    return xn;
}

/**
 * @brief 计算浮点数的绝对值
 * @param f 输入的浮点数
 * @return 取绝对值后的浮点数
 * @auth wufuwei
 * @date 2018/07/27
 */
float32_t Fabs( float32_t f ) 
{
	int16_t tmp = * ( int16_t * ) &f;
	tmp &= 0x7FFFFFFF;

	return * ( float32_t * ) &tmp;
}

/**
 * @brief 判断是否IEEE 754浮点数
 * @param x 判断是否浮点数的值
 * @return 1:是    0:否
 * @auth wufuwei
 * @date 2018/07/27
 */
uint8_t Isnan(float32_t x)
{
    return (x == x);            /* 这里的比较操作看上去总是会得到 true */
                                /* 但有趣的是对于 IEEE 754 浮点数 NaN 来说总会得到 false! */
}

/**
 * @brief 判断是否在IEEE 754单精度浮点数范围内
 * @param x 是否在范围内浮点数的值
 * @return 1:是    0:否
 * @auth wufuwei
 * @date 2018/07/27
 */
uint8_t Finite(float32_t x)
{
    return ((x <= FLT_MAX) && (x >= -FLT_MAX));
}

/**
 * @brief 希尔排序 针对有序序列在插入时采用移动法
 * @param arr 待排序数组
 * @auth wufuwei
 * @date 2018/07/27
 */
void ShellSort(float32_t* arr, int16_t length)
{
    int16_t gap;
    int16_t i;
    int16_t j;
    float32_t temp;

    for (gap = length / 2; gap > 0; gap /= 2)   /* 增量gap, 并逐步缩小增量 */
    {
        for (i = gap; i < length; i++)          /* 从第gap个元素, 逐个对其所在组进行直接插入排序操作 */
        {
            j = i;
            temp = arr[j];
            if (arr[j] < arr[j-gap]) 
            {
                while (j-gap >= 0 && temp < arr[j-gap]) 
                {
                    arr[j] = arr[j-gap];       /* 移动法 */
                    j -= gap;
                }
            }
            arr[j] = temp;
        }
    }
}

/**
 * @brief 将数据限制在上下限范围内
 * @param data 待检查的数据
 * @param lower 数据下限
 * @param upper 数据上限
 * @auth wufuwei
 * @date 2018/07/28
 */
void CheckLimit(float32_t* pdata, float32_t lower, float32_t upper)
{
    if(*pdata < lower)
        *pdata = lower;
    else if(*pdata > upper)
        *pdata = upper;
}

/**
 * @brief 加权平均值计算方法
 * @param value 本次输入数据
 * @param average 平均值
 * @param count 平均值计算次数
 * @auth wufuwei
 * @date 2018/08/03
 */
void WeightedAverage(float32_t value, float32_t* average, uint32_t* count)
{
    uint32_t count_prev;

    count_prev = *count;
    *count += 1;

    *average = (*average * count_prev + value) / *count;
}

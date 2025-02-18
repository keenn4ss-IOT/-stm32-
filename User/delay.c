#include "delay.h"
#include "tim.h"
#include "main.h"

volatile uint32_t tick = 0;

void delay_ms(uint32_t ms)
{
    uint32_t startTick = tick;
    while((tick - startTick) < ms || tick < startTick); // 处理溢出情况
}



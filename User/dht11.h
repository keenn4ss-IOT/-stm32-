/*
 * dht11.h
 *
 *  Created on: 2022?6?17?
 *      Author: 28980
 */

#ifndef INC_DHT11_H_
#define INC_DHT11_H_

#include "main.h"

uint8_t dht11_init (void);
uint8_t dht11_read_data(uint8_t *temp,uint8_t *humi);
void num_check(uint8_t temp_value, uint8_t humi_value);

void threshold_value_set(void);

#endif /* INC_DHT11_H_ */


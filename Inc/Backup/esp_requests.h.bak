/*
 * esp_requests.h
 *
 *  Created on: 05.07.2019
 *      Author: steelph0enix
 */

#ifndef INC_ESP_REQUESTS_H_
#define INC_ESP_REQUESTS_H_

#include "rtc.h"
#include <stdbool.h>

typedef struct time_date_t {
	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;
	bool success;
} TimeDate;

TimeDate getTimeDate(uint32_t timeout);

#endif /* INC_ESP_REQUESTS_H_ */

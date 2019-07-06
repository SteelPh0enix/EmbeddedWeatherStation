/*
 * esp_requests.c
 *
 *  Created on: 05.07.2019
 *      Author: steelph0enix
 */

#include "esp_requests.h"
#include "usart.h"
#include "ssd1306.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define ESP_REQUESTS_STRBUFFER_LEN 32

char const* getEndOfDigit(char const* beginning) {
	while(isdigit(*beginning)) beginning++;
	return beginning;
}

bool parseTimeDate(char const* data, long* hours, long* minutes, long* seconds, long* years, long* months, long* days, long* weekDay) {
	// Format: HH:MM:SS/YYYY-MM-DD/WW
	char* nextDigit = NULL;
	*hours = strtol(data, &nextDigit, 10);
	if (errno == ERANGE) {
		return false;
	}

	*minutes = strtol(nextDigit + 1, &nextDigit, 10);
	if (errno == ERANGE) {
		return false;
	}

	*seconds = strtol(nextDigit + 1, &nextDigit, 10);
	if (errno == ERANGE) {
		return false;
	}

	*years = strtol(nextDigit + 1, &nextDigit, 10);
	if (errno == ERANGE) {
		return false;
	}

	*months = strtol(nextDigit + 1, &nextDigit, 10);
	if (errno == ERANGE) {
		return false;
	}

	*days = strtol(nextDigit + 1, &nextDigit, 10);
	if (errno == ERANGE) {
		return false;
	}

	*weekDay = strtol(nextDigit + 1, &nextDigit, 10);
	if (errno == ERANGE) {
		return false;
	}

	return true;
}

TimeDate getTimeDate(uint32_t timeout) {
	char uartReceiveStrBuffer[ESP_REQUESTS_STRBUFFER_LEN];
	char const* getTimeDateRequest = "getTimeDate()\n";
	size_t length = strlen(getTimeDateRequest);

	HAL_UART_Transmit(&huart1, (uint8_t*) getTimeDateRequest, length, timeout);
	HAL_UART_Receive(&huart1, (uint8_t*) uartReceiveStrBuffer,
	ESP_REQUESTS_STRBUFFER_LEN, timeout);

	TimeDate timeDate = { 0 };
	long hour = 0, min = 0, sec = 0, year = 0, month = 0, day = 0, weekday = 0;
	char* beginningOfData = uartReceiveStrBuffer;

	while (*beginningOfData != '>'
			&& beginningOfData
					< uartReceiveStrBuffer + ESP_REQUESTS_STRBUFFER_LEN) {
		beginningOfData++;
	}
	if (beginningOfData == uartReceiveStrBuffer + ESP_REQUESTS_STRBUFFER_LEN) {
		timeDate.success = false;
		return timeDate;
	}

	// 2-char offset because the date data starts there
	timeDate.success = parseTimeDate(beginningOfData + 2, &hour, &min, &sec, &year, &month, &day, &weekday);
	timeDate.date.Date = (uint8_t) (day);
	timeDate.date.Month = (uint8_t) (month);
	timeDate.date.Year = (uint8_t) (year - 2000);
	timeDate.date.WeekDay = (uint8_t) (weekday);
	timeDate.time.Hours = (uint8_t) (hour);
	timeDate.time.Minutes = (uint8_t) (min);
	timeDate.time.Seconds = (uint8_t) (sec);
	return timeDate;
}

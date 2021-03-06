/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "esp_requests.h"
#include "BMP280.h"
#include "i2c.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "rtc.h"
#include "printf.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef struct bme280_reading_t {
	float temperature;
	float humidity;
	float pressure;
} BME280Reading;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

extern BMP280_HandleTypedef bme280;
BME280Reading environmentData;
char strBuffer[64];

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId oledTaskHandle;
osSemaphoreId dataMutexHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

void scanI2C();
void updateEnvironmentData(BME280Reading* data);
void printEnvironmentData(BME280Reading* data);
void displayOverlay();
void displayEnvironmentData(BME280Reading* data);
void displayDateTime();
char const* getWeekDayName(uint8_t weekDay);
bool updateTimeDate();

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void startOledTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of dataMutex */
  osSemaphoreDef(dataMutex);
  dataMutexHandle = osSemaphoreCreate(osSemaphore(dataMutex), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of oledTask */
  osThreadDef(oledTask, startOledTask, osPriorityHigh, 0, 256);
  oledTaskHandle = osThreadCreate(osThread(oledTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used 
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{

  /* USER CODE BEGIN StartDefaultTask */

//	printf("Hello, world!\r\n");
	/* Infinite loop */
	for (;;) {
		while (bmp280_is_measuring(&bme280)) {
			osDelay(100);
		}
		osSemaphoreWait(dataMutexHandle, osWaitForever);
		updateEnvironmentData(&environmentData);
		osSemaphoreRelease(dataMutexHandle);
		osDelay(500);
	}
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_startOledTask */
/**
 * @brief Function implementing the oledTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_startOledTask */
void startOledTask(void const * argument)
{
  /* USER CODE BEGIN startOledTask */
	bool dateUpdated = false;
	displayOverlay();
	ssd1306_UpdateScreen();
	/* Infinite loop */
	for (;;) {
		osSemaphoreWait(dataMutexHandle, osWaitForever);
		displayEnvironmentData(&environmentData);
		osSemaphoreRelease(dataMutexHandle);

		if (!dateUpdated) {
			dateUpdated = updateTimeDate();
		}

		displayDateTime();
		ssd1306_UpdateScreen();
		osDelay(1000);
	}
  /* USER CODE END startOledTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void scanI2C() {
	printf("Scanning I2C devices...\r\n");
	HAL_StatusTypeDef status;
	for (uint16_t i = 0; i < 127; i++) {
		if (i % 16 == 0) {
			printf("\r\n");
		}
		status = HAL_I2C_IsDeviceReady(&hi2c1, i << 1, 1, 1000);
		if (status == HAL_OK) {
			printf(" 0x%d ", i);
		} else {
			printf(".");
		}
	}
}

void updateEnvironmentData(BME280Reading* data) {
	bmp280_read_float(&bme280, &(data->temperature), &(data->pressure),
			&(data->humidity));
}

void printEnvironmentData(BME280Reading* data) {
	printf("Temp: %.2f*C, Hum: %.2f%, Pres: %.2fhPa\r\n", data->temperature,
			data->humidity, data->pressure / 100.);
}

void displayOverlay() {
//	ssd1306_SetCursor(0, 0);
//	ssd1306_WriteString("welcome, retard", Font_7x10, White);
}

void displayEnvironmentData(BME280Reading* data) {
	sprintf(strBuffer, "Temp.: %.2f*C", data->temperature);
	ssd1306_SetCursor(0, 16);
	ssd1306_WriteString(strBuffer, Font_7x10, White);

	sprintf(strBuffer, "Hum.: %.2f%%", data->humidity);
	ssd1306_SetCursor(0, 26);
	ssd1306_WriteString(strBuffer, Font_7x10, White);

	sprintf(strBuffer, "Press.: %.2fhPa", data->pressure / 100.);
	ssd1306_SetCursor(0, 36);
	ssd1306_WriteString(strBuffer, Font_7x10, White);
}

void displayDateTime() {
	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;

	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

	sprintf(strBuffer, "%s %02d-%02d %02d:%02d:%02d",
			getWeekDayName(date.WeekDay), date.Date, date.Month, time.Hours,
			time.Minutes, time.Seconds);
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString(strBuffer, Font_7x10, White);
}

char const* getWeekDayName(uint8_t weekDay) {
	switch (weekDay) {
	case 1:
		return "Pon";
	case 2:
		return "Wto";
	case 3:
		return "Sr";
	case 4:
		return "Czw";
	case 5:
		return "Pia";
	case 6:
		return "Sob";
	case 7:
		return "Nie";
	}

	return "";
}

bool updateTimeDate() {
	TimeDate timeDate = getTimeDate(500);
	if (timeDate.success) {
		HAL_RTC_SetTime(&hrtc, &(timeDate.time), RTC_FORMAT_BIN);
		HAL_RTC_SetDate(&hrtc, &(timeDate.date), RTC_FORMAT_BIN);
	}

	return timeDate.success;
}

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

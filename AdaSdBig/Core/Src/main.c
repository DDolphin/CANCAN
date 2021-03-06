/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
//#include "usbd_cdc_if.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan1;

CRC_HandleTypeDef hcrc;

RTC_HandleTypeDef hrtc;

SD_HandleTypeDef hsd;
DMA_HandleTypeDef hdma_sdio_rx;
DMA_HandleTypeDef hdma_sdio_tx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_RTC_Init(void);
static void MX_SDIO_SD_Init(void);
static void MX_CRC_Init(void);
static void MX_CAN1_Init(void);
/* USER CODE BEGIN PFP */
void GetRtcSecond(void);
FRESULT InitBSP(void);
FRESULT InitToFile(char* path, size_t path_len, char* msg, size_t msg_len);
FRESULT AppendToFile(char* path, size_t path_len, char* msg, size_t msg_len);
FRESULT set_timestamp (char* obj, int year, int month, int mday, int hour, int min, int sec);

void BlinkLED(uint32_t blink_delay, uint8_t num_blinks);

float microseconds;
RTC_TimeTypeDef timeVar;
RTC_DateTypeDef dateVar;
int BSPInited = 0;
uint8_t timeSetup = 0;

//FS_NORTC
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	  FRESULT fres;
	  //uint32_t raw_sec;
	  //float temp_c;
	  char log_path[20];
	  char buf[20];
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_RTC_Init();
  MX_SDIO_SD_Init();
  MX_FATFS_Init();
  MX_CRC_Init();
  MX_CAN1_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(1000);
  GetRtcSecond();

  InitBSP();

  sprintf((char*)log_path,"/20%02d%02d%02d_%02d%02d%02d.asc",(int)dateVar.Year,(int)dateVar.Month,(int)dateVar.Date,(int)timeVar.Hours,(int)timeVar.Minutes,(int)timeVar.Seconds );
  sprintf((char*)buf,"INI TFILE");
  InitToFile(log_path, strlen(log_path), buf, strlen(buf));
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_GPIO_WritePin(GPIOE, LED0_Pin|LED1_Pin, GPIO_PIN_SET);
	  //raw_sec = GetRtcSecond();
	  //raw_sec = 10000;
	  GetRtcSecond();
	    //if ( raw_sec == 0 ) {
	   //   BlinkLED(100, 5);
	   // } else {

	  microseconds = (float)timeVar.Seconds+ (float)(32768-timeVar.SubSeconds)/(32768);
	      // Convert temperature to decimal format (without float conversion)

	      sprintf((char*)buf,
	    		  "%02d:%02d:%.4f\r\n",
				  (int)timeVar.Hours,(int)timeVar.Minutes,(float)microseconds);

	      // Print temperature to console
	      //CDC_Transmit_FS((uint8_t*)buf, strlen(buf));

	      // Turn LED on while writing to file

	      fres = AppendToFile(log_path, strlen(log_path), buf, strlen(buf));
	      HAL_GPIO_WritePin(GPIOE, LED0_Pin|LED1_Pin, GPIO_PIN_RESET);

	      // If error writing to card, blink 3 times
	      if ( fres != FR_OK) {
		      sprintf((char*)buf,
		    		  "%d\n",
					  fres);
		      //AppendToFile(log_path, strlen(log_path), buf, strlen(buf));
		      //CDC_Transmit_FS((uint8_t*)buf, strlen(buf));
	        BlinkLED(100, 3);
	      }
	   // }
//HAL_SD_MspInit
	  //BlinkLED(100, 5);
	    // Wait before sampling again

	  //HAL_GPIO_WritePin(GPIOA, 6, GPIO_PIN_SET);
	  //HAL_GPIO_WritePin(GPIOA, LED2_Pin|LED3_Pin, GPIO_PIN_SET);
	  //HAL_Delay(1000);
	  //HAL_GPIO_WritePin(GPIOA, LED2_Pin|LED3_Pin, GPIO_PIN_RESET);
	  //HAL_GPIO_WritePin(GPIOA, 6, GPIO_PIN_RESET);
	  HAL_Delay(1);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 16;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_1TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan1.Init.TimeTriggeredMode = ENABLE;
  hcan1.Init.AutoBusOff = ENABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 0;
  hrtc.Init.SynchPrediv = 32767;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */
  /** Initialize RTC and set the Time and Date
  */
//  sTime.Hours = 0x20;
//  sTime.Minutes = 0x02;
//  sTime.Seconds = 0x0;
//  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
//  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
//  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
//  {
//    Error_Handler();
//  }
//  sDate.WeekDay = RTC_WEEKDAY_TUESDAY;
//  sDate.Month = RTC_MONTH_MARCH;
//  sDate.Date = 0x2;
//  sDate.Year = 0x21;
//
//  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
//  {
//    Error_Handler();
//  }
  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SDIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDIO_SD_Init(void)
{

  /* USER CODE BEGIN SDIO_Init 0 */

  /* USER CODE END SDIO_Init 0 */

  /* USER CODE BEGIN SDIO_Init 1 */

  /* USER CODE END SDIO_Init 1 */
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 0;
  /* USER CODE BEGIN SDIO_Init 2 */

  /* USER CODE END SDIO_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
  /* DMA2_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, LED0_Pin|LED1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED0_Pin LED1_Pin */
  GPIO_InitStruct.Pin = LED0_Pin|LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PC4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : LED2_Pin */
  GPIO_InitStruct.Pin = LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(LED2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void GetRtcSecond(void)
{


    //  HAL_GPIO_TogglePin(LEDR_GPIO_Port, LEDR_Pin);
//  if(HAL_RTC_WaitForSynchro(&hrtc) == HAL_OK){


        if (HAL_RTC_GetTime(&hrtc,&timeVar,RTC_FORMAT_BIN) != HAL_OK)
        {
          Error_Handler();
        }
        if (HAL_RTC_GetDate(&hrtc,&dateVar,RTC_FORMAT_BIN) != HAL_OK)
        {
          Error_Handler();
        }

        //return timeVar.SubSeconds;
}


FRESULT InitBSP(void){


	  // Re-initialize SD
	  if(!BSPInited){
		  if ( BSP_SD_Init() != MSD_OK ) {
			  BSPInited = 1;
		    return FR_NOT_READY;
		  }
		  // Re-initialize FATFS
		  if ( FATFS_UnLinkDriver(SDPath) != 0 ) {
		    return FR_NOT_READY;
		  }
		  if ( FATFS_LinkDriver(&SD_Driver, SDPath) != 0 ) {
		    return FR_NOT_READY;
		  }
	  }


}


// Init string to file given at path
FRESULT InitToFile(char* path, size_t path_len, char* msg, size_t msg_len) {

  FATFS fs;
  FIL myFILE;
  UINT testByte;
  FRESULT stat;

  FILINFO myINFO;
  DWORD temp;


  // Mount filesystem
  stat = f_mount(&fs, SDPath, 0);
  if (stat != FR_OK) {
    f_mount(0, SDPath, 0);
    return stat;
  }

  // Open file for appending
  stat = f_open(&myFILE, path, FA_WRITE | FA_OPEN_APPEND);
  if (stat != FR_OK) {
    f_mount(0, SDPath, 0);
    return stat;
  }


  // Write message to end of file
  stat = f_write(&myFILE, msg, msg_len, &testByte);
  if (stat != FR_OK) {
    f_mount(0, SDPath, 0);
    return stat;
  }

  //Write file modify time
//  GetRtcSecond();
//  myINFO.fdate = (WORD)(((dateVar.Year+2000 - 1980) * 512U) | dateVar.Month * 32U | dateVar.Date);
//  myINFO.ftime = (WORD)(timeVar.Hours * 2048U | timeVar.Minutes * 32U | timeVar.Seconds / 2U);
//  stat = f_utime(path, &myINFO);
//  if (stat != FR_OK) {
//	f_mount(0, SDPath, 0);
//	return stat;
//  }


  // Sync, close file, unmount
  stat = f_close(&myFILE);
  f_mount(0, SDPath, 0);

  return stat;
}


// Append string to file given at path
FRESULT AppendToFile(char* path, size_t path_len, char* msg, size_t msg_len) {

  FATFS fs;
  FIL myFILE;
  UINT testByte;
  FRESULT stat;

  FILINFO myINFO;
  DWORD temp;


  // Mount filesystem
  stat = f_mount(&fs, SDPath, 0);
  if (stat != FR_OK) {
    f_mount(0, SDPath, 0);
    return stat;
  }

  // Open file for appending
  stat = f_open(&myFILE, path, FA_WRITE | FA_OPEN_APPEND);
  if (stat != FR_OK) {
    f_mount(0, SDPath, 0);
    return stat;
  }


  // Write message to end of file
  stat = f_write(&myFILE, msg, msg_len, &testByte);
  if (stat != FR_OK) {
    f_mount(0, SDPath, 0);
    return stat;
  }

//  //Write file modify time
//  GetRtcSecond();
//  myINFO.fdate = (WORD)(((dateVar.Year+2000 - 1980) * 512U) | dateVar.Month * 32U | dateVar.Date);
//  myINFO.ftime = (WORD)(timeVar.Hours * 2048U | timeVar.Minutes * 32U | timeVar.Seconds / 2U);
//  stat = f_utime(path, &myINFO);
//  if (stat != FR_OK) {
//	f_mount(0, SDPath, 0);
//	return stat;
//  }

  //temp = get_fattime();
  // Sync, close file, unmount
  stat = f_close(&myFILE);
  f_mount(0, SDPath, 0);

  return stat;
}
//_FS_NORTC

FRESULT set_timestamp (char *obj, int year, int month, int mday, int hour, int min,  int sec)
{
    FILINFO fno;

    fno.fdate = (WORD)(((year+2000 - 1980) * 512U) | month * 32U | mday);
    fno.ftime = (WORD)(hour * 2048U | min * 32U | sec / 2U);
    //fno.fdate = get_fattime() >>16;
    //fno.ftime = get_fattime() & 0xFFFF;

    return f_utime(obj, &fno);
}

// Blink onboard LED
void BlinkLED(uint32_t blink_delay, uint8_t num_blinks) {
  for ( int i = 0; i < num_blinks; i++ ) {
    //HAL_GPIO_WritePin(GPIOA, 7, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOE, LED0_Pin|LED1_Pin, GPIO_PIN_SET);
    HAL_Delay(blink_delay);
    //HAL_GPIO_WritePin(GPIOA, 7, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOE, LED0_Pin|LED1_Pin, GPIO_PIN_RESET);
    HAL_Delay(blink_delay);
  }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

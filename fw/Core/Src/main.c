/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fdcan.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "usb_drd_fs.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <buttons.h>
#include <ssd1306.h>
#include <ssd1306_fonts.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define STANDARD_DATA 0x9A

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t tx_byte_cnt = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
unsigned int rand(unsigned int* seed);
HAL_StatusTypeDef transmit(uint8_t data);
HAL_StatusTypeDef receive(uint8_t* data);
int countSetBits(unsigned int num);
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
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_USART4_UART_Init();
  MX_FDCAN2_Init();
  MX_USB_DRD_FS_PCD_Init();
  MX_TIM6_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  unsigned int transmitSeed = 0;
  unsigned int receiveSeed = 0;

  unsigned int receiveErrors = 0;


  ssd1306_Init();

  ssd1306_Fill(Black);
  ssd1306_SetCursor(0, 0);
  ssd1306_WriteString("Interface: UART", Font_7x10, White);
  ssd1306_SetCursor(0, 11);
  ssd1306_WriteString("Mode: TX", Font_7x10, White);
  ssd1306_SetCursor(0, 22);
  ssd1306_WriteString("Byte Count: ", Font_7x10, White);
  ssd1306_UpdateScreen();


  char buffer[6];


  while (1)
  {
	  if (resetRNG) {
		  transmitSeed = 0;
		  receiveSeed = 0;
		  resetRNG = 0;
	  }

	  if ((role == TRANSMIT || role == RECEIVE_TRANSMIT) && running) {
		  uint8_t data = STANDARD_DATA;
		  if (sendMode == CONTINUOUS_SEND_MODE)
			  data = rand(&transmitSeed) % 256;
		  transmit(data);
		  tx_byte_cnt++;


		  //put to function
		  ssd1306_SetCursor(100, 22); // 20 ok
		  snprintf(buffer, sizeof(buffer), "%d", tx_byte_cnt);
		  ssd1306_WriteString(buffer, Font_7x10, White);
		  ssd1306_UpdateScreen();
	  }

	  if (role == RECEIVE || role == RECEIVE_TRANSMIT) {
		  uint8_t data = 0;
		  if (receive(&data) == HAL_OK) {
			  uint8_t expected = STANDARD_DATA;
			  if (sendMode == CONTINUOUS_SEND_MODE)
				  expected = rand(&receiveSeed) % 256;
			  receiveErrors += countSetBits(expected ^ data);
		  }
	  }
  }


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

unsigned int rand(unsigned int* seed) {
	*seed = *seed * 1103515243 + 12345;
    return (unsigned int)(*seed / 65536) % 32768;
}

HAL_StatusTypeDef transmit(uint8_t data) {
	switch (currInterface) {
	case UART_RS232_INTERFACE: return HAL_UART_Transmit(&huart4, &data, 1, 10); // UART4
	case UART_RS485_INTERFACE: return HAL_UART_Transmit(&huart3, &data, 1, 10);  // UART3
	case UART_3V3_INTERFACE: HAL_UART_Transmit(&huart1, &data, 1, 10); // UART1
	case CAN_INTERFACE: return ERROR;
	}
	return ERROR;
}

HAL_StatusTypeDef receive(uint8_t* data) {
	switch (currInterface) {
	case UART_RS232_INTERFACE: return HAL_UART_Receive(&huart4, data, 1, 10); // UART4
	case UART_RS485_INTERFACE: return HAL_UART_Receive(&huart3, data, 1, 10);  // UART3
	case UART_3V3_INTERFACE: return HAL_UART_Receive(&huart1, data, 1, 10); // UART1
	case CAN_INTERFACE: return ERROR;
	}
	return ERROR;
}

int countSetBits(unsigned int num)
{
    int count = 0;
    while (num) {
        num &= (num - 1); // Turn off the rightmost set bit
        count++;
    }
    return count;
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
#ifdef USE_FULL_ASSERT
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

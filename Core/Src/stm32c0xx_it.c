/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32c0xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32c0xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "brisk.h"
#include "IRQ_Self_Settle.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */
  INT_Enter(SYSTICK_INT);
  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */
  Brisk_1ms_ISR();
  INT_Leave(SYSTICK_INT);
  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32C0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32c0xx.s).                    */
/******************************************************************************/

/* USER CODE BEGIN 1 */
void USART1_IRQHandler(void)
{
  uint8_t byte;
  /* USER CODE BEGIN USART1_IRQn 0 */
  INT_Enter(USART1_INT);
  if (LL_USART_IsEnabledIT_RXNE(USART1)&&(LL_USART_IsActiveFlag_RXNE(USART1))) { // RNXEbyte received by USART
    AddTou8fifo(&u8fifo_to_SPIP[BT_STL_SERIAL], LL_USART_ReceiveData8(USART1));
  }

  if (LL_USART_IsEnabledIT_TXE(USART1)&&(LL_USART_IsActiveFlag_TXE(USART1))) { // TXE byte ready to send by USART
    byte = Clipu8fifo_Down(&u8fifo_from_SPIP[BT_STL_SERIAL]);
    LL_USART_TransmitData8(USART1, byte);
  }
  /* USER CODE END USART1_IRQn 0 */
  /* USER CODE BEGIN USART1_IRQn 1 */
  INT_Leave(USART1_INT);
  /* USER CODE END USART1_IRQn 1 */
}

/**
* @brief This function handles USART1 global interrupt.
*/
void USART2_IRQHandler(void)
{
  uint8_t byte;
  /* USER CODE BEGIN USART2_IRQn 0 */
  INT_Enter(USART2_INT);
  if (LL_USART_IsEnabledIT_RXNE(USART2)&&(LL_USART_IsActiveFlag_RXNE(USART2))) { // RNXEbyte received by USART
    AddTou8fifo(&u8fifo_to_SPIP[ARD_STM_SERIAL], LL_USART_ReceiveData8(USART2));
  }

  if (LL_USART_IsEnabledIT_TXE(USART2)&&(LL_USART_IsActiveFlag_TXE(USART2))) { // TXE byte ready to send by USART
    byte = Clipu8fifo_Down(&u8fifo_from_SPIP[ARD_STM_SERIAL]);
    LL_USART_TransmitData8(USART2, byte);
  }
  /* USER CODE END USART2_IRQn 0 */
  /* USER CODE BEGIN USART2_IRQn 1 */
INT_Leave(USART2_INT);
  /* USER CODE END USART2_IRQn 1 */
}

/* USER CODE END 1 */

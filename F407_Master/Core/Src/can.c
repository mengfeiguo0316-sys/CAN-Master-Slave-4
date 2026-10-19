/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
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
#include "can.h"
#include "stm32f4xx_hal_can.h"

/* USER CODE BEGIN 0 */
#include"can_protocol.h"
#include <stdint.h>
static void CAN_Filter_Config(void);
/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;

/* CAN1 init function */
void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 6;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_2TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_10TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_3TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = ENABLE;
  hcan1.Init.AutoWakeUp = ENABLE;
  hcan1.Init.AutoRetransmission = ENABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */
  CAN_Filter_Config();
  if (HAL_CAN_Start(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)//数据到达中断标志位使能
  {
    Error_Handler();
  }

  /* USER CODE END CAN1_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOI_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PI9     ------> CAN1_RX
    PB9     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_TX_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN1 GPIO Configuration
    PI9     ------> CAN1_RX
    PB9     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOI, GPIO_PIN_9);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_9);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
static void CAN_Filter_Config(void)
{
  CAN_FilterTypeDef CAN_FilterInitStructure={0};

  CAN_FilterInitStructure.FilterBank = 1;                          
  CAN_FilterInitStructure.FilterMode = CAN_FILTERMODE_IDMASK;      
  CAN_FilterInitStructure.FilterScale = CAN_FILTERSCALE_16BIT;     
  CAN_FilterInitStructure.FilterFIFOAssignment = CAN_FILTER_FIFO0; 
  CAN_FilterInitStructure.FilterActivation = CAN_FILTER_ENABLE;    
  CAN_FilterInitStructure.SlaveStartFilterBank = 14; // 双CAN系列配置,3号开始归CAN2,简单说：就是划地盘的线

  CAN_FilterInitStructure.FilterIdHigh     = FILTER16_STD_DATA(CAN_ID_FB_TEMP_STATUS);
  CAN_FilterInitStructure.FilterIdLow      = FILTER16_MASK_ALL;

  CAN_FilterInitStructure.FilterMaskIdHigh = FILTER16_STD_REMOTE(CAN_ID_FB_TEMP_STATUS);
  CAN_FilterInitStructure.FilterMaskIdLow  = FILTER16_MASK_ALL;

  HAL_CAN_ConfigFilter(&hcan1, &CAN_FilterInitStructure);
}

void CAN_Send(CanMsg_TempStatus_t *p)
{
  CAN_TxHeaderTypeDef Can_Tx_Typedef={0};
  uint32_t TxMailbox;

  Can_Tx_Typedef.StdId=CAN_ID_FB_TEMP_STATUS;
  Can_Tx_Typedef.DLC=6;
  Can_Tx_Typedef.IDE=CAN_ID_STD;
  Can_Tx_Typedef.RTR=CAN_RTR_DATA;

  HAL_CAN_AddTxMessage(&hcan1,&Can_Tx_Typedef,(uint8_t*)p,&TxMailbox);
}

void CAN_Send_cmd(CanMsg_MotorCmd_t *p)
{
  CAN_TxHeaderTypeDef Can_Tx_Typedef={0};
  uint32_t TxMailbox;

  Can_Tx_Typedef.StdId=CAN_ID_CMD_MOTOR_CD;
  Can_Tx_Typedef.DLC=8;
  Can_Tx_Typedef.IDE=CAN_ID_STD;
  Can_Tx_Typedef.RTR=CAN_RTR_DATA;

  HAL_CAN_AddTxMessage(&hcan1,&Can_Tx_Typedef,(uint8_t*)p,&TxMailbox);
}
/* USER CODE END 1 */


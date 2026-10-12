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

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

CAN_HandleTypeDef hcan;

/* CAN init function */
void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 4;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_15TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = ENABLE;
  hcan.Init.AutoWakeUp = ENABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */

  /* USER CODE END CAN_Init 2 */

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

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**CAN GPIO Configuration
    PB8     ------> CAN_RX
    PB9     ------> CAN_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    __HAL_AFIO_REMAP_CAN1_2();

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

    /**CAN GPIO Configuration
    PB8     ------> CAN_RX
    PB9     ------> CAN_TX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8|GPIO_PIN_9);

  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
//CAN_FilterTypeDef
//过滤器设置结构体
static void CAN_Filter_Config(void)
{
	CAN_FilterTypeDef CAN_FilterInitStructure;
	
	CAN_FilterInitStructure.FilterBank = 0;//选择过滤器0
	CAN_FilterInitStructure.FilterMode = CAN_FILTERMODE_IDMASK;//选择过滤器的屏蔽模式
	CAN_FilterInitStructure.FilterScale = CAN_FILTERSCALE_32BIT;//选择过滤器的位宽
	
	CAN_FilterInitStructure.FilterIdHigh = ((((uint32_t)0X1314 << 3)|
		CAN_ID_EXT|CAN_RTR_DATA)&0XFFFF0000) >> 16;		//要过滤的高十六位
	
	CAN_FilterInitStructure.FilterIdLow = (((uint32_t)0X1314 << 3)|
		CAN_ID_EXT|CAN_RTR_DATA)&0XFFFF;		//要过滤的低十六位
	
	CAN_FilterInitStructure.FilterMaskIdHigh = 0XFFFF;
	CAN_FilterInitStructure.FilterMaskIdLow = 0XFFFF;//设置过滤器的每一位都必须匹配
	
	CAN_FilterInitStructure.FilterFIFOAssignment = CAN_FilterFIFO0;//匹配FIFO0
	CAN_FilterInitStructure.FilterActivation = ENABLE;//使能
	
	HAL_CAN_ConfigFilter(&hcan,&CAN_FilterInitStructure);//调用HAL库接口将配置写入CAN
	
}


//初始化
void CAN_ConfigInit(void)
{
	MX_CAN_Init();
	CAN_Filter_Config();
	
}

//CAN发送
CAN_TxHeaderTypeDef CAN_HandleTxMsg;
void CAN_Send(void)
{
	uint32_t TxMailbox;
	uint8_t temp[8] = {1,2,3,4,5,6,7,8};
	
	CAN_HandleTxMsg.StdId = 0X00;
	CAN_HandleTxMsg.ExtId = 0X1314;			//使用扩展ID
	CAN_HandleTxMsg.IDE = CAN_ID_EXT;		//使用扩展模式
	CAN_HandleTxMsg.RTR = CAN_RTR_DATA;		//发送的是数据帧
	CAN_HandleTxMsg.DLC = 8;				//数据长度
	
	HAL_CAN_AddTxMessage(&hcan,&CAN_HandleTxMsg,temp,&TxMailbox);//调用HAL库接口将配置写入CAN
}


/* USER CODE END 1 */








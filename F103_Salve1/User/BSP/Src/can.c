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

/* USER CODE BEGIN 0 */

/* ── 位置目标语义开关 ──────────────────────────────────────────────
 * 1 = 增量模式：每次收到 0x105 的 target_pos，都在当前目标上累加
 *     → 每次发 414(180°) 就再转 180°，主节点不用记当前位置
 * 0 = 绝对模式：target_pos 就是"上电后第 N 个脉冲"的绝对位置
 *     → 同一帧发第二次不会动（已经在目标点）
 *
 * 注意：增量模式下，主节点如果想"回到起点"，要发足够大的反向增量，
 *       而不能发 0（发 0 表示"不移动"）。
 * ───────────────────────────────────────────────────────────────── */
#define POS_TARGET_RELATIVE   1


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
  hcan.Init.AutoRetransmission = ENABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */
  HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);//开启FIFO0收报中断
  CAN_ConfigInit();//初始化

  /* USER CODE END CAN_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */
    __HAL_RCC_AFIO_CLK_ENABLE(); //开启重映射时钟
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

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
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

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
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
	
	// ID 和 掩码全部设为 0，代表不筛选任何位，接收所有发过来的标准帧/扩展帧
  CAN_FilterInitStructure.FilterIdHigh = 0x0000;
  CAN_FilterInitStructure.FilterIdLow  = 0x0000;
  CAN_FilterInitStructure.FilterMaskIdHigh = 0x0000;
  CAN_FilterInitStructure.FilterMaskIdLow  = 0x0000;
	
	CAN_FilterInitStructure.FilterFIFOAssignment = CAN_FilterFIFO0;//匹配FIFO0
	CAN_FilterInitStructure.FilterActivation = ENABLE;//使能
	
	HAL_CAN_ConfigFilter(&hcan,&CAN_FilterInitStructure);//调用HAL库接口将配置写入CAN
	
}


//初始化
void CAN_ConfigInit(void)
{
	CAN_Filter_Config();
	
}

//CAN发送
CAN_TxHeaderTypeDef CAN_HandleTxMsg;

//1.发送数据函数
void CAN_Send_Data(uint32_t id, uint8_t *data, uint8_t len)
{
	uint32_t TxMailbox;
	
	CAN_HandleTxMsg.StdId = id;         //标准帧ID
	CAN_HandleTxMsg.ExtId = 0X00;			  //标准帧不使用 ExtId
	CAN_HandleTxMsg.IDE = CAN_ID_STD;		//使用标准模式
	CAN_HandleTxMsg.RTR = CAN_RTR_DATA;	//发送的是数据帧
	CAN_HandleTxMsg.DLC = len;				    //数据长度
	CAN_HandleTxMsg.TransmitGlobalTime = DISABLE;//关闭“发送全局时间戳”功能

  uint32_t timeout = 0xFF; // 超时计数
  while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan) == 0)
  {
    if (timeout-- == 0) return; // 如果等太久还没空位（说明总线断开了），直接退出不卡死程序
  }

	HAL_CAN_AddTxMessage(&hcan,&CAN_HandleTxMsg,data,&TxMailbox);//调用HAL库接口将配置写入CAN
}

// 2.发远程帧函数
void CAN_Send_Remote(uint32_t id, uint8_t len)
{
  uint32_t TxMailbox;
  uint8_t dummy_data[8] = {0}; // 占位数组，硬件发远程帧时会自动忽略它

  CAN_HandleTxMsg.StdId = id;
  CAN_HandleTxMsg.ExtId = 0x00;
  CAN_HandleTxMsg.IDE   = CAN_ID_STD;
  CAN_HandleTxMsg.RTR   = CAN_RTR_REMOTE; // 固定为远程帧
  CAN_HandleTxMsg.DLC   = len;            // 请求对方返回的数据长度
  CAN_HandleTxMsg.TransmitGlobalTime = DISABLE;

  uint32_t timeout = 0xFF; // 超时计数
  while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan) == 0)
  {
    if (timeout-- == 0) return; // 如果等太久还没空位（说明总线断开了），直接退出不卡死程序
  }
  HAL_CAN_AddTxMessage(&hcan, &CAN_HandleTxMsg, dummy_data, &TxMailbox);
}


//CAN接收
volatile uint8_t      CAN_RxFlag = 0;         // 收到新报文的标志
uint8_t               CAN_RxData[8] = {0};    // 接收数据缓冲区
CAN_RxHeaderTypeDef   CAN_RxHeader;           // 报文头（ID、RTR、DLC等）               
// —— CAN 接收中断回调 ——
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &CAN_RxHeader, CAN_RxData) == HAL_OK)
  {
    CAN_RxFlag = 1; // 置标志位，通知主循环去处理
  }
}
// 3.接收处理函数（在主循环中调用）
void CAN_Rx_Process(void)
{
  if (CAN_RxFlag == 0) return; // 没有新报文就直接返回
  CAN_RxFlag = 0;              // 清除标志位，准备接收下一帧

  // 只处理标准帧 (Standard ID)
  if(CAN_RxHeader.IDE != CAN_ID_STD) return;
    
  // 处理远程帧
  if(CAN_RxHeader.RTR == CAN_RTR_REMOTE)
  {
    if(CAN_RxHeader.StdId == CAN_ID_FB_MOTOR_STATE) 
    {
      uint8_t state_buf[8] = {0};

      int16_t cur_speed = (int16_t)PID[0].speed_now; // 当前转速 
      int32_t cur_pos   = (int32_t)(PID[0].pos_now * 360.0f / Circlemax); // 当前角度 

      state_buf[0] = (uint8_t)g_motor_mode;           // 字节 1: 当前控制模式
      memcpy(&state_buf[1], &cur_speed, 2);           // 字节 2~3: 转速
      memcpy(&state_buf[3], &cur_pos,   4);           // 字节 4~7: 位置(角度)
      //state_buf[7] = (uint8_t)g_fault_code;         // 字节 8: 故障码
      
      // 立刻把电机状态作为【数据帧】发回给控制板
      CAN_Send_Data(CAN_ID_FB_MOTOR_STATE, state_buf, 7);
    }
  } 
  //处理数据帧
  else if(CAN_RxHeader.RTR ==  CAN_RTR_DATA)
  {
    switch (CAN_RxHeader.StdId)
    {
      //————————————————电机控制指令————————————————//

      //急停指令(0x001)
      case CAN_ID_EMERG_STOP:
        g_motor_mode = MOTOR_MODE_STOP;      // 切换为 0x00 停机刹车模式
        PID_Clear(&PID[0]);
        break;

      //电机换向指令(0x101)  
      case CAN_ID_CMD_MOTOR_CD:
        g_target_speed = -g_target_speed; // 目标速度直接取反实现换向
        g_target_pos = -g_target_pos;     // 目标位置取反
        break;

      //————————————————PID参数调节————————————————//
      //远程动态微调 PID 参数(0x102~0x104)
      case CAN_ID_CMD_PID_PSET:
        if (CAN_RxHeader.DLC < sizeof(PID[0].Kp)) break; // 确保数据长度足够
        memcpy(&PID[0].Kp, &CAN_RxData[0], sizeof(float));
        break;
      case CAN_ID_CMD_PID_ISET:
        if (CAN_RxHeader.DLC < sizeof(PID[0].Ki)) break; // 确保数据长度足够
        memcpy(&PID[0].Ki, &CAN_RxData[0], sizeof(float));
        break;
      case CAN_ID_CMD_PID_DSET:
        if (CAN_RxHeader.DLC < sizeof(PID[0].Kd)) break; // 确保数据长度足够
        memcpy(&PID[0].Kd, &CAN_RxData[0], sizeof(float));
        break;

      //————————————————电机控制设置(0x105)————————————————//
      // 协议 CanMsg_MotorCmd_t (8 字节):
      //   [0]    ctrl_mode     控制模式 
      //   [1..2] target_speed  目标转速 
      //   [3..6] target_pos    目标位置 
      //   [7]    cmd_seq       命令序号
      case CAN_ID_CMD_MOTOR_CONTROL:
      {
        if (CAN_RxHeader.DLC < 8) break;          // 整个结构体必须完整，防止读到残留

        uint8_t mode = CAN_RxData[0];
        if (mode > MOTOR_MODE_TEMP) break;        // 非法模式：整帧忽略

        //切换控制模式
        switch (mode)
        {
          //停机(急停)模式
          case MOTOR_MODE_STOP:
            g_motor_mode = MOTOR_MODE_STOP;
            PID_Clear(&PID[0]);
            break;
          //速度环控制
          case MOTOR_MODE_SPEED:
            if (g_motor_mode != MOTOR_MODE_SPEED) PID_Clear(&PID[0]);
            g_motor_mode = MOTOR_MODE_SPEED;
            break;
          //位置环控制
          case MOTOR_MODE_POSITION:
            if (g_motor_mode != MOTOR_MODE_POSITION) PID_Clear(&PID[0]);
            g_motor_mode = MOTOR_MODE_POSITION;
            break;
          //位置-速度双环控制
          case MOTOR_MODE_POS_SPEED:
            if (g_motor_mode != MOTOR_MODE_POS_SPEED) PID_Clear(&PID[0]);
            g_motor_mode = MOTOR_MODE_POS_SPEED;
            break;
          //温度控制模式
          case MOTOR_MODE_TEMP:
            if (g_motor_mode != MOTOR_MODE_TEMP) PID_Clear(&PID[0]);
            g_motor_mode = MOTOR_MODE_TEMP;
            break;

          default:
            break;
        }

        //停机模式只刹车，不再更新目标值
        if (g_motor_mode == MOTOR_MODE_STOP) break;

        //解析目标值
        {
          int16_t spd;
          int32_t pos;

          memcpy(&spd, &CAN_RxData[1], sizeof(spd));  // 速度: [1..2]
          memcpy(&pos, &CAN_RxData[3], sizeof(pos));  // 位置: [3..6]

          g_target_speed = (float)spd;

          /* 位置目标语义可切换：
           *   POS_TARGET_RELATIVE = 1  →  增量：每次发同一帧都再前进这么多
           *                                例如每次发 414(180°) 就再转 180°
           *   POS_TARGET_RELATIVE = 0  →  绝对：转到"上电后第 N 个脉冲"的位置
           *                                同一帧发第二次不会动（已经在目标点）
           */
          if (POS_TARGET_RELATIVE)
          {
            g_target_pos += (float)pos * 360.0f / Circlemax;
          }
          else
          {
            g_target_pos  = (float)pos * 360.0f / Circlemax;
          }
        }
        break;
      }

      //————————————————温度数据解析————————————————//
      //温度数据反馈(0x301)
      case CAN_ID_FB_TEMP_STATUS:
      {
        if(g_motor_mode != MOTOR_MODE_TEMP) break; // 只有在温度控制模式下才处理温度数据
        if (CAN_RxHeader.DLC < 6) break;
        uint8_t temp = CAN_RxData[4];
        if (temp == 10)
        {
          g_target_speed = 50.0f;
        }
        else if (temp == 100)
        {
          g_target_speed = 500.0f;
        }
        break;
      }
      default:
        break;
    }


  }
}

/* USER CODE END 1 */


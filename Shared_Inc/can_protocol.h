#ifndef __CAN_PROTOCOL_H
#define __CAN_PROTOCOL_H

#include <stdint.h>

/* ==========================================================================
 * 1. CAN 报文 ID 定义 (11-bit，ID 越小优先级越高)
 * 命名规范：CAN_ID_<方向/模块>_<具体内容>
 * ========================================================================== */
// 最高优先级：控制器 -> 电机板 (高优先级: 0x001 ~ 0x100)
#define CAN_ID_EMERG_STOP       0x001u   // 急停

// 控制指令类：控制板 -> 电机板 (高优先级: 0x100 ~ 0x1FF)
#define CAN_ID_CMD_MOTOR_CD        0x101u   // 电机换向（兼容老命名）
#define CAN_ID_CMD_MOTOR_CHANGE    0x101u   // 电机换向（兼容新命名）
#define CAN_ID_CMD_MOTOR_CONTROL   0x105u   // 电机控制指令

// 远程动态微调 PID 参数
#define CAN_ID_CMD_PID_PSET     0x102   
#define CAN_ID_CMD_PID_ISET     0x103   
#define CAN_ID_CMD_PID_DSET     0x104   

// 状态反馈类：电机板 -> 控制板 (中优先级: 0x200 ~ 0x2FF)
#define CAN_ID_FB_MOTOR_STATE   0x201u   // 电机双环实时状态反馈（转速、位置、状态是否正常等）

// 温度与故障类：传感器/节点 -> 控制板 (高优先级: 0x300 ~ 0x3FF)
#define CAN_ID_FB_TEMP_STATUS   0x301u   // 温度状态

/* ==========================================================================
 * 2. 枚举类型：定义控制模式和系统状态
 * ========================================================================== */
// 电机控制模式
typedef enum {
    MOTOR_MODE_STOP       = 0x00u, // 停止运行（急停/自动停止/刹车）
    MOTOR_MODE_SPEED      = 0x01u, // 速度闭环控制
    MOTOR_MODE_POSITION   = 0x02u, // 位置闭环控制
    MOTOR_MODE_POS_SPEED  = 0x03u, // 位置-速度双环控制
} MotorCtrlMode_e;

// 系统告警与故障状态码（兼容两侧定义）
typedef enum {
    SYS_ERR_NONE          = 0x00u, // 正常
    SYS_ERR_OVER_TEMP     = 0x01u, // 温度过高报警
    SYS_ERR_MOTOR_STALL   = 0x02u, // 电机堵转
    SYS_ERR_CAN_TIMEOUT   = 0x04u  // 通信超时失联
} SystemError_e;

/* ==========================================================================
 * 3. 报文 Payload 数据结构定义 (严格 1 字节对齐，总大小不可超过 8 Bytes)
 * ========================================================================== */
#pragma pack(push, 1)

/**
 * @brief 电机控制指令帧（对应 CAN_ID_CMD_MOTOR_CD / CAN_ID_CMD_MOTOR_CHANGE: 0x101）
 * @note  总长: 1 + 2 + 4 + 1 = 8 Bytes
 */
typedef struct {
    uint8_t  ctrl_mode;      // 控制模式 (可选 MotorCtrlMode_e)
    int16_t  target_speed;   // 目标转速 (单位: RPM，带符号表示正反向)
    int32_t  target_pos;     // 目标位置 (单位: 编码器脉冲计数)
    uint8_t  cmd_seq;        // 命令序号/校验计数
} CanMsg_MotorCmd_t;

/**
 * @brief 电机状态反馈帧（对应 CAN_ID_FB_MOTOR_STATE: 0x201）
 * @note  总长: 1 + 2 + 4 + 1 = 8 Bytes
 */
typedef struct {
    uint8_t  current_mode;   // 当前实际运行模式
    int16_t  actual_speed;   // 当前实际转速 (RPM)
    int32_t  actual_pos;     // 当前实际累计位置 
    uint8_t  error_code;     // 电机端故障码 (参考 SystemError_e)
} CanMsg_MotorFb_t;

/**
 * @brief 温度与温控状态帧（对应 CAN_ID_FB_TEMP_STATUS: 0x301）
 */
typedef struct {
    int16_t  current_temp;    // 当前温度 (放大10倍传输，例如 255 代表 25.5℃)
    int16_t  threshold_temp;  // 温度阈值 (放大10倍传输)
    uint8_t  fan_pwm_duty;    // 风扇占空比 (0~100%)
    uint8_t  is_over_temp;    // 温度标志 (0: 正常, 1: 温度过高)
} CanMsg_TempStatus_t;

#pragma pack(pop)

/* ==========================================================================
 * 4 滤波器常用宏定义
 * ========================================================================== */

#define FILTER16_STD_ID(id)     ((uint16_t)(((id) & 0x7FFu) << 5))
#define FILTER16_IDE_STD        (0u << 4)
#define FILTER16_IDE_EXT        (1u << 4)
#define FILTER16_RTR_DATA       (0u << 3)
#define FILTER16_RTR_REMOTE     (1u << 3)
#define FILTER16_STD_DATA(id)   (FILTER16_STD_ID(id) | FILTER16_IDE_STD | FILTER16_RTR_DATA)
#define FILTER16_STD_REMOTE(id) (FILTER16_STD_ID(id) | FILTER16_IDE_STD | FILTER16_RTR_REMOTE)
#define FILTER16_MASK_ALL       0xFFFFu
#define FILTER16_MASK_STD       0xFFF8u

#define FILTER32_STD_ID(id)     ((uint32_t)(((id) & 0x7FFu)       << 21))
#define FILTER32_EXT_ID(id)     ((uint32_t)(((id) & 0x1FFFFFFFu)  <<  3))
#define FILTER32_IDE_STD        (0u << 2)
#define FILTER32_IDE_EXT        (1u << 2)
#define FILTER32_RTR_DATA       (0u << 1)
#define FILTER32_RTR_REMOTE     (1u << 1)
#define FILTER32_STD_DATA(id)   (FILTER32_STD_ID(id) | FILTER32_IDE_STD | FILTER32_RTR_DATA)
#define FILTER32_STD_REMOTE(id) (FILTER32_STD_ID(id) | FILTER32_IDE_STD | FILTER32_RTR_REMOTE)
#define FILTER32_EXT_DATA(id)   (FILTER32_EXT_ID(id) | FILTER32_IDE_EXT | FILTER32_RTR_DATA)
#define FILTER32_EXT_REMOTE(id) (FILTER32_EXT_ID(id) | FILTER32_IDE_EXT | FILTER32_RTR_REMOTE)
#define FILTER32_HIGH(reg32)    ((uint16_t)(((uint32_t)(reg32) >> 16) & 0xFFFFu))
#define FILTER32_LOW(reg32)     ((uint16_t)((uint32_t)(reg32) & 0xFFFFu))
#define FILTER32_MASK_ALL       0xFFFFFFFFu
#define FILTER32_MASK_STD       0xFFFFFFFdu

#endif /* __CAN_PROTOCOL_H */

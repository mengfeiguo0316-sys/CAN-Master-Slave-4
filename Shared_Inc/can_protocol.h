#ifndef __CAN_PROTOCOL_H
#define __CAN_PROTOCOL_H

#include <stdint.h>

/* ==========================================================================
 * 1. CAN 报文 ID 定义 (标准帧 11-bit，值越小优先级越高)
 * 命名规范：CAN_ID_<方向/属性>_<具体内容>
 * ========================================================================== */
// 急停与报警类 (最高优先级: 0x001 ~ 0x100)
#define CAN_ID_EMERG_STOP       0x001   // 急停

// 控制指令类：控制板 -> 电机板 (高优先级: 0x100 ~ 0x1FF)
#define CAN_ID_CMD_MOTOR_CD   0x101   // 电机换向

// 远程动态微调 PID 参数, 通过 CAN 指令进行增减 幅度为0.01
#define CAN_ID_CMD_PID_PSET     0x102   
#define CAN_ID_CMD_PID_ISET     0x103   
#define CAN_ID_CMD_PID_DSET     0x104   

#define CAN_ID_CMD_TARGET_SPEED     0x108   
#define CAN_ID_CMD_TARGET_POSITION  0x109

#define CAN_ID_CMD_SPEEDLOOP     0x10A  
#define CAN_ID_CMD_POSITIONLOOP  0x10B
#define CAN_ID_CMD_DOUBLELOOP    0x10C

// 状态反馈类：电机板 -> 控制板 (中优先级: 0x200 ~ 0x2FF)
#define CAN_ID_FB_MOTOR_STATE   0x201   //电机双环实时状态反馈 (转速,位置,状态是否正常等)

// 环境传感类：温控节点 -> 其他节点 (低优先级: 0x300 ~ 0x3FF)
#define CAN_ID_FB_TEMP_STATUS   0x301   // 实时温度


/* ==========================================================================
 * 2. 枚举类型：模式、按键动作与故障码
 * ========================================================================== */
// 电机控制模式
typedef enum {
    MOTOR_MODE_STOP       = 0x00, // 停机释放 (失能/自由停止/刹车)
    MOTOR_MODE_SPEED      = 0x01, // 单速度环控制
    MOTOR_MODE_POSITION   = 0x02, // 单位置环控制
    MOTOR_MODE_POS_SPEED  = 0x03, // 位置-速度双环控制
} MotorCtrlMode_e;

// 系统警告与故障状态码 (可按位或组合，也可作枚举)
typedef enum {
    SYS_ERR_NONE          = 0x00, // 正常
    SYS_ERR_OVER_TEMP     = 0x01, // 温度过高报警
    SYS_ERR_MOTOR_STALL   = 0x02, // 电机堵转
    SYS_ERR_CAN_TIMEOUT   = 0x04  // 通信超时失联
} SystemError_e;


/* ==========================================================================
 * 3. 报文 Payload 数据结构定义 (严格 1 字节对齐，总大小不可超过 8 Bytes)
 * ========================================================================== */
#pragma pack(push, 1) // 开启 1 字节对齐，防止编译器自动填充空隙导致错位

/**
 * @brief 电机控制指令帧 (对应 CAN_ID_CMD_MOTOR_CTRL: 0x101)
 * @note  总长: 1 + 2 + 4 + 1 = 8 Bytes
 */
typedef struct {
    uint8_t  ctrl_mode;      // 控制模式 (参考 MotorCtrlMode_e)
    int16_t  target_speed;   // 目标转速 (单位: RPM，有符号代表正反转)
    int32_t  target_pos;     // 目标位置 (单位: 编码器脉冲数 或 0.1度)
    uint8_t  cmd_seq;        // 帧序列号/心跳计数 (每次按键更新自增，用于检测丢包)
} CanMsg_MotorCmd_t;

/**
 * @brief 电机状态反馈帧 (对应 CAN_ID_FB_MOTOR_STATE: 0x201)
 * @note  总长: 1 + 2 + 4 + 1 = 8 Bytes
 */
typedef struct {
    uint8_t  current_mode;   // 当前实际运行模式
    int16_t  actual_speed;   // 当前实际转速 (RPM)
    int32_t  actual_pos;     // 当前实际累计位置 (编码器脉冲数)
    uint8_t  error_code;     // 电机端故障码 (参考 SystemError_e)
} CanMsg_MotorFb_t;

/**
 * @brief 温度与温控状态帧 (对应 CAN_ID_FB_TEMP_STATUS: 0x301)
 * @note  总长: 2 + 2 + 1 + 1 = 6 Bytes (不超过8字节即可)
 */
typedef struct {
    int16_t  current_temp;   // 当前温度 (放大10倍传输，例如 255 代表 25.5℃，避免传float)
    int16_t  threshold_temp; // 报警阈值温度 (放大10倍，如 600 代表 60.0℃)
    uint8_t  fan_pwm_duty;   // 散热风扇占空比 / 温控输出状态 (0~100%)
    uint8_t  is_over_temp;   // 过温标志位 (0: 正常, 1: 过温警告蜂鸣器)
} CanMsg_TempStatus_t;

#pragma pack(pop) // 恢复默认对齐方式

#endif /* __CAN_PROTOCOL_H */
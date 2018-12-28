#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"

#ifndef ENGINE_H_
#define ENGINE_H_

void InitMotorCommunication(UART_HandleTypeDef huart);
void InitServoPositions(void);
void FillServoPositionString(void);
void TransmitServoString();
void RotatePositions(void);
void EmptyMovementString(void);

#endif /* ENGINE_H_ */

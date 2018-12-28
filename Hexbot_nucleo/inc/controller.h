
#ifndef CONTROLLER_H_
#define CONTROLLER_H_

void InitRobot(UART_HandleTypeDef uartSSC32U);

void EngineTransfer();
void WalkForward();
void WalkReverse();
void Start();

#endif

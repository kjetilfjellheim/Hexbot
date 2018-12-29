#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"
#include <string.h>

#include "motor.h"

void InitRobot(UART_HandleTypeDef uartSSC32U) {
	InitMotorCommunication(uartSSC32U);
	InitServoPositions();
}

void Start() {
	SetRun();
	SetRotateRight();
	/**
	 * Start walking.
	 */
	while (1) {
		EngineTransfer();
	}
}

void EngineTransfer() {
	/**
	 * Sets the inital servo positions.
	 */
	FillServoPositionString();
	/**
	 * Transmit positions.
	 */
	TransmitServoString();
	/**
	 * Change servos to next position.
	 */
	RotatePositions();
}


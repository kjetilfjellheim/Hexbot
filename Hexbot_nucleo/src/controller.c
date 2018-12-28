#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"
#include <string.h>

#include "motor.h"

void InitRobot(UART_HandleTypeDef uartSSC32U) {
	InitMotorCommunication(uartSSC32U);
	InitServoPositions();
}

void Start() {
	WalkForward();
	SetWalk();
	SetRotateLeft();
	/**
	 * Start walking.
	 */
	while (1) {
		EngineTransfer();
	}
}

void WalkForward() {
	/**
	 * Set forward for motor control.
	 */
	SetForward();
}

void WalkReverse() {
	/**
	 * Set reverse for motor control.
	 */
	SetReverse();
}

void RotateLeft() {
	/**
	 * Set rotate left for motor control.
	 */
	SetRotateLeft();
}

void RotateRight() {
	/**
	 * Set rotate right for motor control.
	 */
	SetRotateRight();
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


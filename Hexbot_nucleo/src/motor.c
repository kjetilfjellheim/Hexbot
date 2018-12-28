#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"
#include <string.h>

UART_HandleTypeDef engineUart;

/**
 * Max length of the movement string.
 */
#define MAX_MOVEMENT_STRING_LENGTH 256
/**
 *Positions durinmg movement.
 */
#define POSITIONS 6

/**
 * Each leg is a servoset.
 */
#define SERVOSETS 6

typedef struct {
	int verticalMovement[POSITIONS];
	int horizontalMovement[POSITIONS];
} Position;

int rotation = 0; // 1 = Rotate right, 0 = Do not rotate, -1 = Rotate left
int speed = 500;   //500 = Walk, 200 = Run
int direction = 1; // 1 = Forward, 0 = Stop, -1 = Reverse

const int FORWARD = 1;
const int STOP = 0;
const int REVERSE = -1;
const int FAST = 200;
const int SLOW = 500;
const int LEFT = -1;
const int NO_ROTATE = 0;
const int RIGHT = 1;

typedef struct {
	unsigned int currentServoPosition;
	char horizontalServo[3];
	char upperVerticalServo[3];
	char lowerVerticalServo[3];
	int horizontalMediumPosition;
	int verticalUpperMediumPosition;
	int verticalLowerMediumPosition;
	int multiplierHorizontal;
	int multiplierVertical;
	int side;
} Servoset;

//const Position positions = { { -100, 200, -100 }, { -200, 0, 200 } };
const Position positions = { { -100, -100, -100, -100, -100, 200 }, { 200, 100, 0, -100, -200, 0 } };

/**
 * Current servoinformation for the servosets.
 */
Servoset servosets[] = { { 0, "12", "1", "2", 1700, 2200, 1800, 1, 1, -1 },
						 { 2, "4", "5", "6", 1350, 1600, 1700, 1, 1, -1 },
						 { 4, "8", "9", "10", 1000, 1600, 1800, 1, 1, -1 },
						 { 5, "16", "17", "18", 1600, 1200, 1500, -1, -1, 1 },
						 { 3, "20", "21", "22", 2100, 1300, 1000, -1, -1, 1 },
						 { 1, "24", "25", "26", 1900, 1400, 1000, -1, -1, 1 } };

char movementString[MAX_MOVEMENT_STRING_LENGTH] = "";

void InitMotorCommunication(UART_HandleTypeDef huart);
void InitServoPositions();
void FillServoPositionString(void);
void TransmitServoString();
void RotatePositions(void);
void EmptyMovementString(void);

/**
 * Initalizes HC-06 to Xbee communication.
 */
void InitMotorCommunication(UART_HandleTypeDef huart) {
	engineUart = huart;
}

void InitServoPositions(void) {
	char tmp[5];
	unsigned int i = 0;
	for (i = 0; i < SERVOSETS; i++) {
		EmptyMovementString();
		strcat(movementString, "#");
		strcat(movementString, servosets[i].horizontalServo);
		strcat(movementString, "P");
		sprintf(tmp, "%d", servosets[i].horizontalMediumPosition);
		strcat(movementString, tmp);
		strcat(movementString, "\r");
		HAL_UART_Transmit(&engineUart, (uint8_t*) movementString, strlen(movementString), 0x012c);
		EmptyMovementString();
		strcat(movementString, "#");
		strcat(movementString, servosets[i].upperVerticalServo);
		strcat(movementString, "P");
		sprintf(tmp, "%d", servosets[i].verticalUpperMediumPosition);
		strcat(movementString, tmp);
		strcat(movementString, "\r");
		HAL_UART_Transmit(&engineUart, (uint8_t*) movementString, strlen(movementString), 0x012c);
		EmptyMovementString();
		strcat(movementString, "#");
		strcat(movementString, servosets[i].lowerVerticalServo);
		strcat(movementString, "P");
		sprintf(tmp, "%d", servosets[i].verticalLowerMediumPosition);
		strcat(movementString, tmp);
		strcat(movementString, "\r");
		HAL_UART_Transmit(&engineUart, (uint8_t*) movementString, strlen(movementString), 0x012c);
		HAL_Delay(1000);
	}

}


void RotatePositions() {
	if (direction == FORWARD) {
		for (unsigned int i = 0; i < SERVOSETS; i++) {
			if (servosets[i].currentServoPosition < (POSITIONS - 1)) {
				servosets[i].currentServoPosition += 1;
			} else {
				servosets[i].currentServoPosition = 0;
			}
		}
	} else if (direction == REVERSE) {
		for (unsigned int i = 0; i < SERVOSETS; i++) {
			if (servosets[i].currentServoPosition > 1) {
				servosets[i].currentServoPosition -= 1;
			} else {
				servosets[i].currentServoPosition = POSITIONS - 1;
			}
		}
	}
}

void EmptyMovementString(void) {
    memset(movementString, '\0', MAX_MOVEMENT_STRING_LENGTH);
}

void FillServoPositionString(void) {
    EmptyMovementString();
    char tmp[5];
    unsigned int i = 0;
    for(i = 0; i < SERVOSETS; i++){
        strcat(movementString, "#");
        strcat(movementString, servosets[i].horizontalServo);
        strcat(movementString, "P");
        int deltaHorizontal = positions.horizontalMovement[servosets[i].currentServoPosition] * servosets[i].multiplierHorizontal;
        int newHorizontalPosition = servosets[i].horizontalMediumPosition + (deltaHorizontal * getRotation(servosets[i].side));
        sprintf(tmp, "%d", newHorizontalPosition);
        strcat(movementString, tmp);
        strcat(movementString, "#");
        strcat(movementString, servosets[i].upperVerticalServo);
        strcat(movementString, "P");
        int deltaVertical = positions.verticalMovement[servosets[i].currentServoPosition] * servosets[i].multiplierVertical;
        int newVerticalPosition = servosets[i].verticalUpperMediumPosition + deltaVertical;
        sprintf(tmp, "%d", newVerticalPosition);
        strcat(movementString, tmp);
        strcat(movementString, "#");
        strcat(movementString, servosets[i].lowerVerticalServo);
        strcat(movementString, "P");
        sprintf(tmp, "%d", servosets[i].verticalLowerMediumPosition);
        strcat(movementString, tmp);
    }
    strcat(movementString, "T");
    sprintf(tmp, "%d", speed);
    strcat(movementString, tmp);
    strcat(movementString, "\r");
}

void TransmitServoString() {
	HAL_UART_Transmit(&engineUart, (uint8_t*) movementString, strlen(movementString), 0x012c);
	HAL_Delay(speed - 100);
}

void SetForward() {
	direction = FORWARD;
}

void SetStop() {
	direction = STOP;
}

void SetReverse() {
	direction = REVERSE;
}

void SetRotateLeft() {
	rotation = LEFT;
}

void SetStopRotate() {
	rotation = NO_ROTATE;
}

void SetRotateRight() {
	rotation = RIGHT;
}

void SetRun() {
	speed = FAST;
}

void SetWalk() {
	speed = SLOW;
}


int getRotation(int side) {
	if (rotation == NO_ROTATE) {
		return 1;
	} else if (rotation == LEFT) {
		return LEFT * side;
	} else {
		return RIGHT * side;
	}
}

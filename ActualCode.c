#pragma config(Sensor, S1,     WallSensor,     sensorEV3_Ultrasonic)
#pragma config(Sensor, S2,     ColorSens2,     sensorEV3_Color, modeEV3Color_Color)
#pragma config(Sensor, S3,     ColorSensor,    sensorLightActive)
#pragma config(Sensor, S4,     BallSensor,     sensorEV3_Ultrasonic)
#pragma config(Motor,  motorA,          LeftMotor,     tmotorNXT, PIDControl, driveLeft, encoder)
#pragma config(Motor,  motorB,          ClawGrab,      tmotorNXT, PIDControl)
#pragma config(Motor,  motorC,          ClawVertical,  tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorD,          RightMotor,    tmotorNXT, PIDControl, driveRight, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/* Mnemonics for motion control */ //copied from emailed example
enum Movement {
	HALT,
	FWD,
	BWD,
	LEFT_ARC,
	RIGHT_ARC,
	LEFT_TURN,
	HARD_RIGHT,
	HARD_LEFT,
	RIGHT_TURN,
	NORMAL_SEARCH,
	HARD_LEFT_SEARCH,
	HARD_RIGHT_SEARCH,
	FORWARD_MOVE_SEARCH,
	DONT_CHANGE
};

enum Direction {
	NORTH,
	EAST,
	SOUTH,
	WEST
};

/* Mnemonics for motor speed control */
enum MotorValues {
	FORD = 80,
	SYNCH = 0,
	HALT_VALUE = 0,
	HARD_LEFT_VALUE = 45,
	ARCING = 10,
	FORWARD_TIME_UNIT = 1000,
	FORWARD_TIME_UNIT_FASTER = 10000,
	ARCRATIO = 70,
	SEARCH_TURN = 60,
	TURNCLICKCOUNT = 175,
	HARD_TURN = 99,
	STOPMOVEMENT = 0
};

enum TuringValues {
	HARD_RIGHT_VALUE = 75,
	FORWARD_VALUE = 0
};

enum ClawStates {
	CLAWCLOSE,
	CLAWOPEN,
	CLAWHOLD,
	CLAWTOHOLD,
	CLAWTODROP,
	CLAWTOGROUND
};

enum ClawControlValues {
	CLAWDOWNM = -10,
	CLAWUPM = 55,
	CLAWCLOSEM = 30,
	CLAWOPENM = -30,
	CLAWHOLDM = 3
};

enum TimingMotors {
	timeTillClawMoves = 200
};

enum EncoderValues {
	CLAWHOLDE = 80,
	CLAWDROPE = 110,
	MAX_ROTATION = 1900
};

enum SensorTolerances {
	tolleranceBall = 0,
	tolleranceClawFloor = 15,
	deltaDistanceSensors = 10,

};

enum SensorThreshold {
	pickupThreshold = 30,
	SENSOR_DIST_TOP_MAX = 70,
	SENSOR_DIST_TOP_WALL = 120,
	SENSOR_DIST_DEPOSIT = 25,
	WALL_THRESHOLD_2ND = 20,
	WALL_PROTECTOR_VALUE = 20,
	SENSOR_TAPE_MIN = 1,
	SENSOR_TAPE_MAX = 9
};

int robot_drive_command;

int search_motor_command;

int grabbing_motor_command;

int depositing_motor_command;

int geting_motor_command;

int wall_protector_command;

//Booleans to see what state the robot is in

bool searching = false;
bool foundBall = false;
bool depositingBall = false;

// Location Tracking for later

//int currentX = 0;
//int currentY = 0;

//int exploredX = -1;
//int exploredY = -1;



//Debug global values

int motorEncoder;
int wallAmmount = 0;






//This function will take in an int representing what operation it should do
//and translates this into motor controls.  It is also from the emailed example.
task moveCommand () {
	while(true) {
		switch (robot_drive_command) {
		case FWD:
			setMotorSync (LeftMotor, RightMotor, SYNCH, FORD);
			break;
		case BWD:
			setMotorSync (LeftMotor, RightMotor, SYNCH, -FORD);
			break;
		case HALT:
			setMotorSync (LeftMotor, RightMotor, 0, HALT_VALUE);
			break;
		case HARD_RIGHT:
			setMotorSync (LeftMotor, RightMotor, 100, FORD);
			break;
		case HARD_LEFT:
			setMotorSync (LeftMotor, RightMotor, -100, FORD);
			break;
		case HARD_LEFT_SEARCH:
			setMotorSync (LeftMotor, RightMotor, HARD_LEFT, FORD);
			sleep(FORWARD_TIME_UNIT);
			setMotorSync (LeftMotor, RightMotor, SYNCH, FORD);
			sleep(FORWARD_TIME_UNIT_FASTER);
			break;
		case HARD_RIGHT_SEARCH:
			setMotorSync (LeftMotor, RightMotor, HARD_RIGHT, FORD);
			sleep(FORWARD_TIME_UNIT);
			setMotorSync (LeftMotor, RightMotor, SYNCH, FORD);
			sleep(FORWARD_TIME_UNIT_FASTER);
			break;
		case NORMAL_SEARCH:
			setMotorSync (LeftMotor, RightMotor, HARD_TURN, SEARCH_TURN);
			break;
		case FORWARD_MOVE_SEARCH:
			setMotorSync (LeftMotor, RightMotor, FORWARD_VALUE, FORD);
			 // TODO : CHANGE FOR THE CORRECT DISTANCE OR SLEEP TILL PROPER ENCODER VALUE
			// Also probs possible to use pid drive which would allow for perfectly straight driving
			break;
		}
		releaseCPU();
	}
}


int changeDirection(int direction, bool left) {
	if (direction == 3 && !left) {
		return 0;
		} else if (direction == 0 && left) {
		return 3;
		} else if (left) {
		return direction -1;
	}
	return direction+1;
}

void changeX(int direction) {
	if (direction == 2) {
		//currentX--;
		} else if (direction == 0) {
		//currentX++;
	}
}

void changeY(int direction) {
	if (direction == 3) {
		//currentY--;
		} else if (direction == 1) {
		//currentY++;
	}
}

int gotHere = false;

task getBall() {
	int didntActuallyFind = 0;
	while(true) {
		geting_motor_command = FWD;
		gotHere = true;

		if (sensorValue[ColorSensor] > 2) {
			depositingBall = true;
			searching = false;
			stopTask(getBall);
		}
		if(didntActuallyFind > 10000) {
			depositingBall = false;
			searching = true;
			startTask(searchForBallTask);
			stopTask(getBall);
		}
		didntActuallyFind ++;
		sleep(5);
		releaseCPU();
	}


}


// This is the thread that controls the top downward facing reflection sensor. As a cockroach, the robot behavior is to stop
// when appropriate darkeness is sensed.

int topDistanceSensor;
int bottomDistanceSensor;

int topDistanceSensorFound;
int bottomDistanceSensorFound;



bool moveToBall() {
	topDistanceSensor = SensorValue[WallSensor];
	bottomDistanceSensor = SensorValue[BallSensor];
	search_motor_command = HALT;
	sleep(400);
	search_motor_command = HARD_LEFT;
	sleep(100);
	topDistanceSensor = topDistanceSensor;
	bottomDistanceSensor = bottomDistanceSensor;

	if ((bottomDistanceSensor) < (topDistanceSensor)
		&& (abs(bottomDistanceSensor - topDistanceSensor) > 10)) {
		topDistanceSensorFound = topDistanceSensor;
		bottomDistanceSensorFound = bottomDistanceSensor;
		search_motor_command = HARD_RIGHT;
		sleep(200);
		search_motor_command = FWD;
		sleep(20);
		foundBall = true;
		return true;

	}
	return false;
}




bool wall1;
bool wall2;
bool wall3;
bool wall4;

int direction = 0;

int startEncoderValue;
int currentRotation;

task searchForBallTask() {


	bool passedThreshold;
	foundBall = false;



	int localWallMax = -1;
	bool walls[4] = {false, false, false, false};
	bool turnLeft = false;



	while (true) {

		wall1 = walls[0];
		wall2 = walls[1];
		wall3 = walls[2];
		wall4 = walls[3];
		//TODO: Fix the sensor ports/ THere is a better command for finding the value anyways
		topDistanceSensor = SensorValue[WallSensor];
		bottomDistanceSensor = SensorValue[BallSensor];
		currentRotation = getMotorEncoder(LeftMotor);

		if (searching) {

			// Start spinning
			// Record the xy that we are currently at

			if (currentRotation > (startEncoderValue + MAX_ROTATION)) {
				// Initalizing the next search


					if (!walls[0]) {
						setLEDColor(ledRedPulse)
						search_motor_command = FORWARD_MOVE_SEARCH;
						sleep(3000);
					}

					if (walls[0]) {
						//Turn left or right based on local var
						// Wait some seconds
						if (turnLeft) {
							search_motor_command = HARD_LEFT_SEARCH;
							direction = changeDirection(direction, true);
							} else {
							search_motor_command = HARD_RIGHT_SEARCH;
							direction = changeDirection(direction, false);
						}
						sleep(FORWARD_TIME_UNIT_FASTER + FORWARD_TIME_UNIT);


					}
					if (walls[0] && walls[1]) {
						// Turn left uninterupted
						// Wait some seconds
						search_motor_command = HARD_LEFT_SEARCH;
						direction = changeDirection(direction, true);
						sleep(FORWARD_TIME_UNIT_FASTER + FORWARD_TIME_UNIT);
					}

					if (walls[0] && walls[3]) {
						// Turn right
						// Wait some seconds
						search_motor_command = HARD_RIGHT_SEARCH;
						direction = changeDirection(direction, false);
						sleep(FORWARD_TIME_UNIT_FASTER + FORWARD_TIME_UNIT);

					}
					changeX(direction);
					changeY(direction);

					walls[0] = false;
				walls[1] = false;
				walls[2] = false;
				walls[3] = false;
				localWallMax = -1;
				startEncoderValue = getMotorEncoder(LeftMotor);
				currentRotation = 0;

			} else {


				// Move Forward state

				search_motor_command = NORMAL_SEARCH;


			}
			// Cases for turning based on the the wall positions

			//Case for reconizing a wall - recording local max, when goes down past threshold record corner
			// Record relativeish direction of the wall

			if (topDistanceSensor < SENSOR_DIST_TOP_WALL) {
				if (localWallMax < topDistanceSensor) {
					//TODO : This math is wrong, something something shifted by 45 deg
					walls[(int) ((currentRotation + (MAX_ROTATION/8))/(MAX_ROTATION*4))%MAX_ROTATION] = true;
					localWallMax = topDistanceSensor;
					wallAmmount++;
				}
			}

			if ((((bottomDistanceSensor) < (topDistanceSensor)
				&& (abs(bottomDistanceSensor - topDistanceSensor) > 10)) && (topDistanceSensor < SENSOR_DIST_TOP_MAX)) || foundBall) {
				setLEDColor(ledOrange);
				if (moveToBall()) {
						startTask(getBall);
						stopTask(searchForBallTask);
					} else {
						setLEDColor(ledGreen);

				}


				//TODO : make these vars turningMax turningDistance
			}

		}
		releaseCPU();
	}
}




bool findWall = true;

bool findDirection = false;

bool findCorner = false;

bool faceTwordsMiddle = false;
int colorSensorAHHHH;

task depositingBallTask () {



	// Assuming that we can get the max length of the field, we can then get the closest corner, else, return to starting point
	while(true) {

		topDistanceSensor = SensorValue[WallSensor];
		bottomDistanceSensor = SensorValue[BallSensor];
		colorSensorAHHHH = SensorValue[ColorSens2];

		if (findWall) {
			if (topDistanceSensor < SENSOR_DIST_DEPOSIT) {
				findWall = false;
				findDirection = true;
			}
			depositing_motor_command = FWD;
		}

		if (findDirection) {
			if (topDistanceSensor < SENSOR_DIST_TOP_MAX) {
				depositing_motor_command = HARD_RIGHT;
				setLEDColor(ledGreenFlash);
				} else {
				if (colorSensorAHHHH > SENSOR_TAPE_MIN && colorSensorAHHHH < SENSOR_TAPE_MAX) {

					findDirection = false;
					findCorner = true;
				}
			}
		}


		if (findCorner) {
			if (!(topDistanceSensor < SENSOR_DIST_DEPOSIT)) {
				depositing_motor_command = FWD;
				} else {
				depositing_motor_command = BWD;
				sleep(1000);
				findCorner = false;
				faceTwordsMiddle = true;
			}

			if (faceTwordsMiddle) {
				depositing_motor_command = HARD_LEFT;
				sleep(500);
				foundBall = false;
				startTask(searchForBallTask);
				stopTask(depositingBallTask);
			}

		}
	}

}

task wallProtector() {
	while(true) {

		topDistanceSensor = SensorValue[WallSensor];

		if (topDistanceSensor < WALL_PROTECTOR_VALUE) {
			wall_protector_command = HARD_RIGHT;

			} else {

			wall_protector_command = DONT_CHANGE;
		}
	}

}

//task EYES() {
//	while(true) {
//		 LCD.BmpFile(1,0,0, "Images/Tired left")
//		 sleep(1000);
//		 LCD.BmpFile(1,0,0, "Images/Tired right")
//		 sleep(1000);
//	}
//}


//This will begin all appropriate threads, one for arbitrater, the bumper, the right hand sonar sensor
// and one for the upward facing light sensor
task main() {
	int motorCommands;
	int clawCommands;
	int leverControl;
	// Need to redo the structure of these so that they make way the fuck more sense
	startTask(moveCommand);
	startTask(searchForBallTask);
	startTask(depositingBallTask);
	startTask(wallProtector);
	//startTask(EYES);
	searching = true;
	setLEDColor(ledGreen);


	while(true) {
		motorEncoder = getMotorEncoder(ClawVertical);
		if (searching) {
			motorCommands = search_motor_command;
		}

		if (foundBall) {
			motorCommands = geting_motor_command;
		}

		if (depositingBall) {
			motorCommands = depositing_motor_command;
		}

		if (wall_protector_command != DONT_CHANGE) {
			motorCommands = wall_protector_command;
			setLEDColor(ledOrangePulse);

		}

		robot_drive_command = motorCommands;
		sleep(5);
		releaseCPU();
	}


}

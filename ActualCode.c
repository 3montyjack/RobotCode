#pragma config(Sensor, S1,     WallSensor,     sensorEV3_Ultrasonic)
#pragma config(Sensor, S2,     ColorSensor,    sensorEV3_Color, modeEV3Color_RGB_Raw)
#pragma config(Sensor, S4,     BallSensor,     sensorEV3_Ultrasonic)
#pragma config(Motor,  motorA,          LeftMotor,     tmotorNXT, PIDControl, driveLeft, encoder)
#pragma config(Motor,  motorB,          ClawGrab,     tmotorNXT, PIDControl)
#pragma config(Motor,  motorC,          ClawVertical,   tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorD,          RightMotor,    tmotorNXT, PIDControl, driveRight, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//


/* Mnemonics for motion control */ //copied from emailed example
enum Movement {
	HALT = 0,
	FORWARD = 1,
	BACKWARD = 2,
	LEFT_ARC = 3,
	RIGHT_ARC = 4,
	LEFT_TURN = 5,
	RIGHT_TURN = 6,
	HARD_LEFT = 7
}

/* Mnemonics for motor speed control */
enum MotorValues {
	FWD = -35,
	SYNCH = 0,
	ARCING = 10,
	FORWARDTIMEUNIT = 600,
	ARCRATIO = 70,
	TURNCLICKCOUNT = 175
}

enum ClawStates {
	CLAWCLOSE,
	CLAWOPEN,
	CLAWHOLD,
	CLAWTOHOLD,
	CLAWTODROP,
	CLAWTOGROUND
}

enum ClawControlValues {
	CLAWDOWNM = 15,
	CLAWUPM = 75,
	CLAWCLOSEM = 30,
	CLAWOPENM = -30
}

enum TimingMotors {
	timeTillClawMoves = 20
}

enum EncoderValues {
	CLAWHOLDE = 80,
	CLAWDROPE = 110
}

enum SensorTolerances {
	tolleranceBall = 0,
	tolleranceClawFloor = 10
}

enum SensorThreshold {
	pickupThreshold = 20
}

//Booleans to see what state the robot is in

bool searching = false;
bool foundBall = false;
bool depositingBall = false;

//Initialize behavior flags

bool top_reflect_active = false;
bool down_reflect_active = false;
bool front_bumper_active = false;
bool right_distance_active = false;

//Initialize behavior commands
int down_reflect_command = HALT;
int top_reflect_command = HALT;
int front_bumper_command = HALT;
int right_distance_command = HALT;

// Magic number for downward facing relection sensors
const int LIGHTTHRESHOLD = 25;
const int DISTANCETHRESHOLD = 15;
const int LOSTWALLDISTANCE = 200;
const int DELTA = 5;
const int DARKNESSTHESHOLD = 25;


//This function will take in an int representing what operation it should do
//and translates this into motor controls.  It is also from the emailed example.
void Move (int operation) {

	switch (operation) {
	case HALT:
		stopAllMotors();
		break;

	case FORWARD:
		setMotorSyncTime (leftMotor, rightMotor, SYNCH, FORWARDTIMEUNIT, FWD);
		break;

	case BACKWARD:
		setMotorSyncTime (leftMotor, rightMotor, SYNCH, FORWARDTIMEUNIT, -FWD);
		break;

	case LEFT_TURN:
		setMotorSyncEncoder (leftMotor, rightMotor, 100, TURNCLICKCOUNT, FWD);
		break;

	case RIGHT_TURN:
		setMotorSyncEncoder (leftMotor, rightMotor, -100, TURNCLICKCOUNT, FWD);
		break;

	case LEFT_ARC:
		setMotorSyncTime (leftMotor, rightMotor, ARCING, FORWARDTIMEUNIT, FWD);
		break;

	case RIGHT_ARC:
		setMotorSyncTime (leftMotor, rightMotor, -ARCING, FORWARDTIMEUNIT, FWD);
		break;

	case HARD_LEFT:
		setMotorSyncTime (leftMotor, rightMotor, SYNCH, FORWARDTIMEUNIT, -FWD);
		sleep(800);
		setMotorSyncEncoder (leftMotor, rightMotor, 100, 90, FWD);
		break;

	default:
		stopAllMotors();
	}
	sleep (1000);
}


void controlClaw(int operation) {

	switch(operation) {
		CLAWCLOSE,
		CLAWOPEN,
		CLAWHOLD,
		CLAWTOHOLD,
		CLAWTODROP,
		CLAWTOGROUND
		case GRABBALL:
			setMotor(ClawGrab , CLAWCLOSEM);
			break;




	}

}


// This is the thread that controls the right side facing sonar sensor for a right hand wall walk. Arc toward the wall
// when too far away. Arc away when too close, and stay straight otherwise.
task wall_follow() {
	int rightSonarSensorValue;
	while (true) {
		rightSonarSensorValue = getUSDistance(rightSonarSensor);
		right_distance_active = true;
		right_distance_command = FORWARD;
		if (rightSonarSensorValue > (DISTANCETHRESHOLD + DELTA)) {
			right_distance_command = RIGHT_ARC;
		}
		if (rightSonarSensorValue > LOSTWALLDISTANCE) {
			right_distance_command = RIGHT_TURN;
		}
		if (rightSonarSensorValue < (DISTANCETHRESHOLD - DELTA)) {
			right_distance_command = LEFT_ARC;
		}
		releaseCPU();
	}
}


// This is the thread that controls the top downward facing reflection sensor. As a cockroach, the robot behavior is to stop
// when appropriate darkeness is sensed.
task avoid_light() {
	int topReflectStatus;
	while (true) {
		topReflectStatus = SensorValue[leftDownSensor];
		top_reflect_active = false;
		if (topReflectStatus > LIGHTTHRESHOLD) {
			top_reflect_active = true;
			top_reflect_command = STOP;
		}
		releaseCPU();
	}
}


// This is the thread that controls the top downward facing reflection sensor. As a cockroach, the robot behavior is to stop
// when appropriate darkeness is sensed.
task searchForBallTask() {
	int topDistanceSensor;
	int bottomDistanceSensor;
	while (true) {
		//TODO: Fix the sensor ports/ THere is a better command for finding the value anyways
		topDistanceSensor = SensorValue[leftDownSensor];
		bottomDistanceSensor = SensorValue[leftDownSensor];

		if ((bottomDistanceSensor + deltaDistanceSensors) < (topDistanceSensor - tolleranceBall)) {
			foundBall = true;
			//TODO : make these vars turningMax turningDistance
		} else if (turningDistance < turningMax) {
			//Find wall potential
			//Find corner maybe?

		}


		releaseCPU();
	}
}

task grabbingBallTask() {
	int topDistanceSensor;
	int bottomDistanceSensor;
	bool pickingUp = false;
	while (true) {
		//TODO: Fix the sensor ports/ THere is a better command for finding the value anyways
		topDistanceSensor = SensorValue[leftDownSensor];
		bottomDistanceSensor = SensorValue[leftDownSensor];

		if (bottomDistanceSensor <= pickupThreshold) {
			grabbing_claw_command = CLAWTOGROUND;
		}
		if (getMotorEncoder(ClawVertical) < tolleranceClawFloor) {
			grabbing_claw_command = CLAWCLOSE;
			pickingUp = true;
			sleep(timeTillClawMoves);
		}

		if (pickingUp) {

		}

		releaseCPU();
	}
}



//This will begin all appropriate threads, one for arbitrater, the bumper, the right hand sonar sensor
// and one for the upward facing light sensor
task main() {
	int motorCommands;
	int clawCommands;

	//startTask(obstacle_Bumper_Hit);
	//startTask(wall_follow);

	startTask(searchForBallTask);
	startTask(grabbingBallTask);
	startTask(depositingBallTask);

	while(true) {
		motorCommands = FORWARD;

		if (searching) {
			motorCommands = search_motor_command;
			clawCommands = search_claw_command;
		}

		if (foundBall) {
				motorCommands = grabbing_motor_command;
				clawCommands = grabbing_claw_command;
		}

		if (depositingBall) {
			motorCommands = depositing_motor_command;
			clawCommands = depositing_claw_command;
		}



		Move(motorCommands);
		Move(clawCommands);
		releaseCPU();
	}

}

#pragma config(Sensor, S1,     SideSensor,     sensorEV3_Ultrasonic)
#pragma config(Sensor, S2,     TouchSensor,    sensorEV3_Touch)
#pragma config(Sensor, S3,     RightLight,     sensorEV3_Color, modeEV3Color_Ambient)
#pragma config(Motor,  motorA,          LeftMotor,     tmotorNXT, PIDControl, driveLeft, encoder)
#pragma config(Motor,  motorD,          RightMotor,    tmotorNXT, PIDControl, driveRight, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//This is the set threshold I think we got somewhere from 40 to 50 will work

int lightThreshold = 15;
int minDist = 5;
int maxDist = 15;
int speedMotor = 20;
int direction = 1;

enum Directions
{
	forwardDir = 0,
	leftSoft = 25,
	rightSoft = -25,
	leftHard = 75,
	rightHard = -75

};

enum Speed
{
	normal = -40,
	softTurn = -30,
	hardTurn = -20,
	reverseNormal = 40,
	stopMovement = 0
}


bool enableDistSensor = false;
int stateDistSensor = 0;

bool enableLightSensor = false;
bool stateLightSensor = false;

bool enableTouchSensor = false;
bool stateTouchSensor = false;


enum SensorValues
{
	good,
	tooClose,
	tooFar
};

task moveSenseTouch() {

	while(true) {
		if (enableTouchSensor) {
			//If equal to the last then go straight
			// if last was both black then veer right
			// if color is white and last was black then reverse and turn?
			// if color is white and one was black turn to the one that was black and reverse a bit
			// have a wait at the end

			if (0 == SensorValue(TouchSensor)) {
				stateTouchSensor = true;
			} else {
				stateTouchSensor = false;
			}


			wait1Msec(3);
		}
		else {
			wait1Msec(100);
		}
	}
	releaseCPU();

}

task moveSenseLight() {

	while(true) {
		if (enableLightSensor) {
			//If equal to the last then go straight
			// if last was both black then veer right
			// if color is white and last was black then reverse and turn?
			// if color is white and one was black turn to the one that was black and reverse a bit
			// have a wait at the end
			stateLightSensor = SensorValue(RightLight);

			stateLightSensor = (stateLightSensor < lightThreshold);

			wait1Msec(3);
		}
		else {
			wait1Msec(100);
		}
	}
	releaseCPU();

}


task moveSenseSide() {
	short sensorVal = 0;

	while(true) {
		if (enableDistSensor) {
			//If equal to the last then go straight
			// if last was both black then veer right
			// if color is white and last was black then reverse and turn?
			// if color is white and one was black turn to the one that was black and reverse a bit
			// have a wait at the end
			sensorVal = SensorValue(SideSensor);



			if (sensorVal > minDist && sensorVal < maxDist){
				// If the color is white on each then
				stateDistSensor = good;
			}

			if (sensorVal < minDist) {
				// If the color is black on each then
				stateDistSensor = tooClose;
			}

			if (sensorVal > maxDist) {
				// If the color is white only on the left
				stateDistSensor = tooFar;
			}

			wait1Msec(3);
		}
		else {
			wait1Msec(100);
		}
	}
	releaseCPU();

}

task main()
{
	//begins subthread
	startTask(moveSenseSide);
	startTask(moveSenseLight);
	startTask(moveSenseTouch);

	enableDistSensor = true;
	enableLightSensor = true;
	enableTouchSensor = true;

	int waitReverseLeft = 0;


	while(true) {
		//if no line is found

		if (stateDistSensor == tooFar) {
				direction = rightSoft;
			speedMotor = normal;
		}
	if (stateDistSensor == tooClose) {
				direction = leftSoft;
			speedMotor = normal;
		}
		if (stateDistSensor == good) {
				direction = forwardDir;
			speedMotor = normal;
		}

		if (stateTouchSensor == true && !stateLightSensor) {
			direction = rightHard;
			speedMotor = reverseNormal;
			waitReverseLeft = 500;
		}

		if (waitReverseLeft > 0 && !stateLightSensor) {
			direction = rightHard;
			speedMotor = reverseNormal;
				waitReverseLeft = waitReverseLeft-1;
		}

		if (stateLightSensor) {
				speedMotor = stopMovement;
		}

		setMotorSync(LeftMotor, RightMotor, direction, speedMotor);
		wait1Msec(1);
	}
	releaseCPU();


}

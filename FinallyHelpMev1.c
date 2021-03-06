#pragma config(Sensor, S2,     ColorSensor,    sensorEV3_Color, modeEV3Color_RGB_Raw)
#pragma config(Sensor, S3,     DistSensor,     sensorEV3_Ultrasonic)
#pragma config(Motor,  motorA,          LeftMotor,     tmotorNXT, PIDControl, driveLeft, encoder)
#pragma config(Motor,  motorB,           ,             tmotorNXT, openLoop)
#pragma config(Motor,  motorC,          ConveyMotor,   tmotorNXT, PIDControl, encoder)
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

enum Colors { // TODO: fix the values for this
	yellow = 0,
	red = 1
};

enum Speed
{
	normal = -40,
	softTurn = -30,
	hardTurn = -20,
	reverseNormal = 40,
	stopMovement = 0
};


bool enableDistSensor = false;
int stateDistSensor = 0;

bool enableLightSensor = false;
bool stateLightSensor = false;

bool enableTouchSensor = false;
bool stateTouchSensor = false;


enum SensorValues
{
	good,
	tooClose
};

task conveySenseLight() {

	while(true) {
		if (enableLightSensor) {
			// If equal to the last then go straight
			// if last was both black then veer right
			// if color is white and last was black then reverse and turn?
			// if color is white and one was black turn to the one that was black and reverse a bit
			// have a wait at the end

			//stateLightSensor = (stateLightSensor =< lightThreshold);

			wait1Msec(3);
		}
		else {
			wait1Msec(100);
		}
	}
	releaseCPU();

}


task moveDistanceSenseForward() {
	short sensorVal = 0;

	while(true) {
		if (enableDistSensor) {
			// If equal to the last then go straight
			// if last was both black then veer right
			// if color is white and last was black then reverse and turn?
			// if color is white and one was black turn to the one that was black and reverse a bit
			// have a wait at the end
			sensorVal = SensorValue(DistSensor);

			if (sensorVal < minDist) {
				// If the color is black on each then
				stateDistSensor = tooClose;
			} else {
				stateDistSensor = good;
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
	startTask(moveDistanceSenseForward);
	startTask(conveySenseLight);

	enableDistSensor = true;
	enableLightSensor = true;
	enableTouchSensor = true;

	int waitReverseLeft = 0;


	while(true) {
		//if no line is found

 	  if (stateDistSensor == tooClose) {
 	  	//Need to change for reverse
				direction = leftSoft;
			  speedMotor = normal;
		}

		if (stateDistSensor == good) {
			// Something for controlling algorythim for searching
				direction = leftSoft;
			  speedMotor = normal;
		}

		if(stateLightSensor == yellow) {
			  //Turn on conveyor belt
				direction = rightHard;
				speedMotor = reverseNormal;
				waitReverseLeft = 500;
		}


		if (stateTouchSensor == true && !stateLightSensor) {
			// Code for if having ball and then its also against wall,
			// Probibly turn and then start conveyor after going forward
			direction = rightHard;
			speedMotor = reverseNormal;
			waitReverseLeft = 500;
		}

		if (waitReverseLeft > 0 && !stateLightSensor) {
			// Code for rejecting ball
			direction = rightHard;
			speedMotor = reverseNormal;
				waitReverseLeft = waitReverseLeft-1;
		}

		setMotorSync(LeftMotor, RightMotor, direction, speedMotor);
		wait1Msec(1);
	}
	releaseCPU();


}

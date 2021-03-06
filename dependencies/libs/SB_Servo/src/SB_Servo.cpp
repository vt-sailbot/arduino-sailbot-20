/**
 * Source file for SB_Servo.hpp, contains function definitions 
 * to implement the SailBot 2021 Servo wrapper 
 *
 * AHJ
 */

#include "SB_Servo.hpp"

// Here the maestro is initialized to Serial1 on the Teensy, this is just one of 8 ports 
MiniMaestro SB_Servo::maestro(Serial1);
int SB_Servo::servoCount{0};


/** 
 * Uses point slope form to calculate a us value for the maestro for a given degree
 */ 
int SB_Servo::degToUS(float degree) {
	return ((float) (maxUS - minUS) / maxDegreeRange) * (degree) + minUS;
}

/**
 * Converts microseconds to degrees
 */
float SB_Servo::usToDegrees(int us) { 
	return (maxDegreeRange / ((float) maxUS - minUS)) * (us - minUS) + minDegreeRange;
}

SB_Servo::SB_Servo(int channel) : SB_Servo(DEFAULT_MIN_US, DEFAULT_MAX_US, channel) {}

SB_Servo::SB_Servo(int minUS, int maxUS, int channel) : 
	SB_Servo(
		minUS, maxUS, 
		(float) DEFAULT_MIN_ANGLE, (float) DEFAULT_MAX_ANGLE, (float) DEFAULT_MIN_ANGLE, (float) DEFAULT_MAX_ANGLE, channel) {}


SB_Servo::SB_Servo(int minimumUS, int maximumUS, float minimumRange, float maximumRange, 
	float minimumAngle, float maximumAngle, int channel) : 
		minUS(4 * minimumUS), // Remember, the maestro uses 4x us from manufacturer specs
		maxUS(4 * maximumUS), 
		minDegreeRange(minimumRange),
		maxDegreeRange(maximumRange),
		minAngle(minimumAngle),
		maxAngle(maximumAngle),
		channelNum(channel), 
		servoNumber(servoCount++) { 
	
	checkMinUS();	
	checkMaxUS();	
	checkMinDegreeRange();
	checkMaxDegreeRange();
	checkMinAngle();
	checkMaxAngle();
	checkChannel();

}

// In the the future these methods can be shortened and all put together
// For example minUS and maxUS can be the same method
// but for now I'm just going to leave them be there are other things to do 


/** 
 *  READ THE setMaximumUS() comment in the .cpp and .hpp
 */
void SB_Servo::checkMinUS() { 
	if (minUS < 0 || minUS > maxUS) { 
		errorCode |= US_ERROR_BIT;
		printDebug("checkMinUS() error");
	}
}

/**
 * This method uses the manufacturer's maximum
 * So, for example the HS422 has a rating of 500us - 2500us, thus we pass 2500 here
 * The maestro will use 2000 - 10k us. Don't ask me why the Maestro uses 4x larger values, it just does.
 * See the .hpp for more info
 */
void SB_Servo::checkMaxUS() { 
	if (maxUS < 0 || maxUS <= minUS) { 
		errorCode |= US_ERROR_BIT;
		printDebug("checkMaxMS() error");
	}
}

void SB_Servo::checkMinDegreeRange() { 
	if (minDegreeRange < 0 || minDegreeRange > maxDegreeRange) { 
		errorCode |= RANGE_ERROR_BIT; 
		printDebug("checkMinDegreeRange() error");
	}
}

void SB_Servo::checkMaxDegreeRange() { 
	if (maxDegreeRange > 360 || maxDegreeRange < minDegreeRange ) { 
		errorCode |= RANGE_ERROR_BIT; 
		printDebug("checkMaxDegreeRange() error");
	}
}


void SB_Servo::checkMinAngle() { 
	if (minAngle < 0 || minAngle > maxAngle || 
			minAngle < minDegreeRange) { 
		errorCode |= ANGLE_ERROR_BIT;
		printDebug("checkMinAngle() error");
	}
}

void  SB_Servo::checkMaxAngle() { 
	if (maxAngle < 0 || maxAngle < minAngle || 
			maxAngle > maxDegreeRange) { 
		errorCode |= ANGLE_ERROR_BIT;
		printDebug("checkMaxAngle() error");
	}
}

void SB_Servo::checkChannel() { 
	if (channelNum < 0 || channelNum >= NUM_MAESTRO_CHANNELS) { 
		errorCode |= CHANNEL_ERROR_BIT;
		printDebug("checkChannel() error");
	}
}

float SB_Servo::getCurrentDegrees() { 
	if (errorCode & CHANNEL_ERROR_BIT) { // Mask wit the channel error bit to see if we fucked up the channel num
		printDebug("Bad channel num, aborting getCurrentDegrees()"); 
		return -1; // Servo not connected properly 
	} else { 
		int currentUS = maestro.getPosition(channelNum); 
		return usToDegrees(currentUS); 
	}
}


void SB_Servo::rotateToDegrees(float degree) { 
	if (degree > maxAngle) { 
		degree = maxAngle;
		errorCode |= ROTATE_TO_OVER_ERROR_BIT;
		printDebug("Requested RotateTo() angle exceeds rating warning");
	} else if (degree < minAngle) { 
		degree = minAngle;
		errorCode |= ROTATE_TO_UNDER_ERROR_BIT;
		printDebug("Requested RotateTo() angle under rating warning");
	}
	int usToWrite = degToUS(degree);
	checkChannel();
	if (errorCode & CHANNEL_ERROR_BIT) { // Mask w/ error bit to see if channelNumber isn't set properly
		printDebug("Bad channel num, aborting rotateTo()"); 
		return; // Servo not connected yet 
	} 
	maestro.setTarget(channelNum, usToWrite); 
	return;
}


void SB_Servo::rotateBy(float degreesBy) { 
	currentDeg = (int) (getCurrentDegrees() + .5); // Round down to make life easier and stop errors
	float desiredAngle = currentDeg + degreesBy;	
	rotateToDegrees(desiredAngle);
}

void SB_Servo::printDebug(String printMe) { 
#ifdef DEBUG
	Serial.print("Servo #");
	Serial.print(servoNumber);
	Serial.print(": ");
	Serial.println(String(printMe));
#endif
}

int SB_Servo::getErrorCode() { 
	return errorCode;
}

void SB_Servo::clearErrorCode() { 
	errorCode = 0;
}

void SB_Servo::setMultipleTargets(std::vector<SB_Servo> servos, std::vector<float> degrees) { 
	// Need to make sure the channels are contiguous
	for (int i = 0; i < servos.size() - 1; i++) { 
		if (servos[i].channelNum + 1 != servos[i + 1].channelNum ) { 
			// Make an issue!!!!
			servos[i].printDebug("Continuity is wrong in setMultipleTargets!!!"); 
		}
	}

	uint16_t targets[NUM_MAESTRO_CHANNELS];
	for (int i = 0; i < degrees.size(); i++) { 
		targets[i] = servos[i].degToUS(degrees[i]); 
	}
	int numberOfServosToMove = servos.size();
	int firstChannel = servos[0].channelNum; 
	maestro.setMultiTarget(numberOfServosToMove, firstChannel, &targets[0]);
}



	

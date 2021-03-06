/** 
 * A class representing a servo for SailBot 2021 @ Virginia Tech.
 *
 * The class utilizes the Servo controller, the Pololu Mini Maestro 
 * and is written for the Teensy 4.0, 3.2 platforms, which can be compiled and 
 * uploaded using the Arduino IDE. It will also work with most Arduino &
 * Arduino compatible platforms.
 *
 *
 * By convention this program uses: 
 * 		`floats` for floating point numbers as the double are just not needed 
 * 		@sets in function prototype comments indicating the error code that is set
 * from a particular method
 *
 * AHJ
 */

#ifndef SB_servo
#define SB_servo


// Defining debug allows runtime debug statements to be printed to the Serial monitor
// this most likely will need to be turned off in production as no one will be watching
// the Serial monitor 
#define DEBUG 
#include <PololuMaestro.h>
#include <vector> // Needed for set multiple targets

/** 
 * These are the error codes for our system, 
 * See the documentation that I haven't written yet for 
 * a full list detailing the error codes
 */

#define US_ERROR_BIT 0x01
#define RANGE_ERROR_BIT 0x02
#define ANGLE_ERROR_BIT 0x04
#define CHANNEL_ERROR_BIT 0x08
#define ROTATE_TO_UNDER_ERROR_BIT 0x10
#define ROTATE_TO_OVER_ERROR_BIT 0x20

/**
 * These are default values for instantiated servos
 * the min/max us are the general guidelines for minimum and maximum
 * microsecond pulse times of servos. 
 * The default min angle and max angle are used as the default range of degrees of a servo
 * most servos go from 0-180 believe it or not
 */
#define DEFAULT_MIN_US 500
#define DEFAULT_MAX_US 2500 

#define DEFAULT_MIN_ANGLE 0  
#define DEFAULT_MAX_ANGLE 180 

/**
 * The number of servos that the maestro can handle
 * This could change in the future if a different maestro were to be used
 */
#define NUM_MAESTRO_CHANNELS 8

class SB_Servo { 
	private: 
		// We make the maestro static so that it's shared across all instances 
		// of Servos
		static MiniMaestro maestro;
		// This is the number of servos we're using, the count increments for 
		// each servo added. The servo count is used in debugging print values 
		// as it provides a unique identifier for each servo 
		static int servoCount; 

		/** 
		 * Error codes are logical OR'd with this member, it serves as a way
		 * of keeping track of what errrors have happened
		 */
		int errorCode = 0;

		/** 
		 * These values are found from servo specific data sheets					 
		 * The default values are referenced from the HS-422 0-180* digital servo 
		 * Which has specific values of 500-2500 us. The maestro's specifications use 
		 * 4x pulse widths, just how it works, thus we use 2k and 10k as
		 * the default minimum and maximum pulses, these are modified in the parameterized constructor
		 */
		const int minUS; 		// default 500 
		const int maxUS; 		//default 2500 

		// The RANGES are manufacturer define ranges that the servo can move to 
		// For example, most servos move between 0 and 180. However, some go from 0 to 200
		const float minDegreeRange; // default: 0 
		const float maxDegreeRange; // default: 180  

		// The ANGLES are maximum and minimum angles that a servo can go to, found experimentally
		// For example, one of the HS475's goes from 3* to 200* 
		// You can find out the min and max angles of a servo by using the 
		// simpleSerialRead.ino program and finding where the servo start jittering.
		// Some servos work fine for their full operational range and thus these min and max 
		// Angles won't need to be modified in the constructor 
		const float minAngle; 	//default 0 
		const float maxAngle; 	// default 180

		const int channelNum; 	// no default value
		const int servoNumber;  // The identifier for this servo taken from servoCount 
		

		/** 
		 * Convert a us value into a degrees value
		 *
		 * @param ms -- the us to convert
		 * @return the degrees in float form from 0 to 180
		 */
		float usToDegrees(int us);

		/**
		 * Convert traditional 0-180 degrees into us for the maestro to use
		 * the maestro uses ms for calculating servo angles, thus this method
		 * acts as an interface from human readable degrees to us, which the machine uses
		 *
		 * @param degrees -- the degrees in 0-180
		 * @return the us representation of the degrees
		 */
		int degToUS(float degrees);  

		/**
		 * Checks that the channel was set to a correct value 
		 * @sets CHANNEL_ERROR_BIT
		 */
		void checkChannel();

		/**
		 * Check that the minimum and maximum values provided by the constructor 
		 * make sense. Else throw error codes. 
		 * @sets US_ERROR_BIT
		 */
		void checkMinUS();
		void checkMaxUS();

		/**
		 * The range are the manufacturer specific degrees. 
		 * These methods just make sure the user set ranges that make sense 
		 *
		 * Simply checks that the min and max range specified makes sense
		 * @sets RANGE_ERROR_BIT
		 */ 
		void checkMinDegreeRange();
		void checkMaxDegreeRange();

		/** 
		 * Checks the max/min angle
		 * For now, you can't set a max angle over 360 or under 0 
		 * It also cannot be set less than or equal to the minimum
		 * @sets ANGLE_ERROR_BIT
		 */
		void checkMinAngle();
		void checkMaxAngle();

		/**
		 * Simply used to print debug lines if debug mode has been toggled on,
		 * which can easily be done from line #20
		 *
		 * @param String the string to print
		 */
		void printDebug(String);

   	public: 
		/**
		 * Sets the channelNumber of a particular servo 
		 * Calls the next few constructors using hte default values
		 * as specified in the preprocessor directives 
		 *
		 * @param channel the channel number to use, which is the pin on
		 * the maestro that the servo is connected to 
		 */
		SB_Servo(int channel);

		/**
		 * Sets the channelNumber of a particular servo 
		 * Also sets the minimum and maximum degrees 
		 *
		 * @param channel -- the channel number to use, which is the pin on the maestro to use
		 * @param minDegr -- the minimum degree the servo can rotate to 
		 * @param maxDegr -- the maximum degree the servo can rotate to 
		 *
		 */
		SB_Servo(int minDegr, int maxDegr, int channel);


		/**
		 * Big fat constructor and the final constructor to be called
		 * All SB_Servo objects have to eventually call this method upon construction
		 * It also performs a lot of configuration checks to set the error codes if 
		 * a parameter in this is not set properly 
		 *
		 * @param minUS -- the bottom of the range of the us of the servo 
		 * @param maxUS -- the top of the range of the us of the servo 
		 * @param  minRange -- the bottom degree of the servo
		 * @param  maxRange -- the top degree of the servo
		 * @param minAngle -- the experimentally found minimum angle the servo can move to 
		 * @param maxAngle -- the experimentally found maximum angle the servo can move to 
		 * @param channelNum -- the channel number this servo uses on the maestro
		 */ 
		SB_Servo(int minUS, int maxUS, float minRange, float maxRange, 
				float minAngle, float maxAngle, int channel);


		
		/** 
		 * Gets the current degrees of this servo 
		 * by sending asking the maestro for the current degrees of the servo
		 *
		 * @return the current degrees.
		 * -1 if there is a communication failure / channel number not set properly
		 */
		float getCurrentDegrees();

		/** 
		 * Rotates this servo to a specific degrees, 0-180, or within the minimum
		 * and maximum values as dictated by the member values
		 *
		 * @param degrees -- the degrees to rotate to 
		 * @sets ROTATE_TO_UNDER_ERROR_BIT
		 * @sets ROTATE_TO_OVER_ERROR_BIT
		 */
		void rotateToDegrees(float degrees);


		/** 
		 * Rotates the servo by a set amount   
		 * Checks to make sure that the requested amount is within the servos
		 * tolerated limits
		 * This method also directly modifies the Servos, and thus 
		 * needs to be able to raise some kind of error if communication with the Maestro
		 * fails 
		 *
		 * @param degreesBy -- the amount of degrees to turn by 
		 * @return whether or not the requested operation was doable
		 * @sets ROTATE_BY_UNDER_ERROR_BIT
		 * @sets ROTATE_BY_OVER_ERROR_BIT
		 */
		void rotateBy(float degreesBy);

		/**
		 * gets the current error code for this servo object
		 * @return the current errorcode
		 */
		int getErrorCode();
		/**
		 * sets the current errorcode to 0
		 * thereby effectively 'clearing' the code
		 * to a clean slate
		 */
		void clearErrorCode();

		/**
		 * Moves servos at the exact same time  
		 * THIS METHOD IS UN TESTED AS IT'S NOT ANTICIPATED TO BE USED 4/6/2021... 
		 * it probably works though there's just no error codes
		 *
		 * @param servos -- the servos to move, these servos also need contiguous channel numbers, 
		 * for example if you want to move 3 servos you're going to need to use channels 0, 1, 2. 
		 * (or 4, 5, 6, etc)
		 *
		 * @param degrees -- the degrees to send to the servos. They match up respectively. For example 
		 * degrees[0] is where to turn servos[0]  
		 *
		 */
		static void setMultipleTargets(std::vector<SB_Servo> servos, std::vector<float> degrees);
};

#endif

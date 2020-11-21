

/** 
 * A class representing a Servo for SailBot 2021 @ Virginia Tech.
 * A. Heller-Jones 
 * 11.20.2020
 *
 * The class utilizes the Servo controller, the Pololu Mini Maestro 
 * and is written for the Teensy 4.0, 3.2 platforms, which can be compiled and 
 * uploaded using the Arduino IDE 
 * directory referencing:
/home/andy/Documents/Personal/SailBOT/ArduinoMotorControl2020/arduino-sailbot-20/main/Motors/src

/home/andy/Documents/Personal/SailBOT/ArduinoMotorControl2020/arduino-sailbot-20/dependencies/libs
 */

#include <PololuMaestro.h>
// #include <stdexcept> arduino compiler doesnt like to throw errors

#define MAESTRO_SERVOANGLE_MS_MAX 8000 // The max servo angle in milliseconds
#define MAESTRO_SERVOANGLE_MS_MIN 4000 // The min servo angle in milliseconds

class SB_Servo { 
	private: 
		// We make the maestro static so that it's shared across all instances 
		// of Servos
		static MiniMaestro maestro;
		int channelNum = -1; // default value for the channel, 
						     // indicates the channel has not been assigned
		int maxDegree = 180; // the maximum degree the servo can rotate to 
		int minDegree = 0; // the minimum degree the servo can rotate to  

   	public: 
		/** 
		 * Default constructor for the servo, I don't think 
		 * this guy really needs to do anything
		 * 
		 * Begins the Serial monitor on Serial1. This could be changed in the future
		 * to be able to dictated by .xml or .yml file
		 */
   		SB_Servo();

		/**
		 * Sets the channelNumber of a particular servo 
		 * @param channel the channel number to use, which is the pin on
		 * the maestro that the servo is connected to 
		 * Begins the Serial monitor on Serial1. This could be changed in the future
		 * to be able to dictated by .xml or .yml file
		 */
		SB_Servo(int);

		/**
		 * Sets the channelNumber of a particular servo 
		 * Also sets the minimum and maximum degrees 
		 * @param channel -- the channel number to use, which is the pin on the maestro to use
		 * @param minDegr -- the minimum degree the servo can rotate to 
		 * @param maxDegr -- the maximum degree the servo can rotate to 
		 *
		 * Begins the Serial monitor on Serial1. This could be changed in the future
		 * to be able to dictated by .xml or .yml file
		 */
		SB_Servo(int minDegr, int maxDegr, int channel);


		/** 
		 * Sets the channel number for a particular system. Pretty straight forward
		 *
		 * Requires that the channel setting is within the  bounds for the maestro, 
		 * 0 and 127.  
		 * TODO: change 127 to the number of channels available in the MicroMaestro, 
		 * obviously we don't have 127 channels, but there is a max num
		 * @param channel -- the channel to assign this servo to 
		 * @return whether or not the channel was set properly, the method only returns flase
		 * if your provide a bad channel number
		 */
		bool setChannel(int);

		/** 
		 * Sets the max angle
		 * @param maximum -- the value to set to maxDegrees
		 * 						which dictates the max value the servo can rotate to 
		 * For now, you can't set a max angle over 180 or under 0 
		 * It also cannot be set less than or equal to the minimum
		 * @return whether or not the passed value was within the acceptable range
		 */
		bool setMaximumAngle(int);

		/** 
		 * Sets the min angle
		 * @param minimum --  the value to set to minDegrees
		 * 						which dictates the min value the servo can rotate to 
		 * For now, you can't set a max angle over 180 or under 0 
		 * It also cannot be set greater or equal to than the maximum 
		 * @return whether or not the passed value was within the acceptable range
		 */
		bool setMinimumAngle(int);
		
		/** Rotates this servo to a specific degrees, 0-180, or within the minimum
		 * and maximum values as dictated by the member values
		 *
		 * @param degrees -- the degrees to rotate to 
		 * @return whether or not the Servo is properly connected to the Maestro, for example
		 * if a channel number was never asserted then the Servo will do nothing and return false; 
		 */
		bool rotateToDegrees(float degrees);


};

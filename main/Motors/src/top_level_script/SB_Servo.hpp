

/** 
 * A class representing a Servo for SailBot 2021 @ Virginia Tech.
 * A. Heller-Jones 
 * 11.20.2020
 *
 * The class utilizes the Servo controller, the Pololu Mini Maestro 
 * and is written for the Teensy 4.0, 3.2 platforms, which can be compiled and 
 * uploaded using the Arduino IDE 
 * directory referencing:
 *
 *
 * By convention this program uses: 
 * `floats` for floating point numbers as the double is just not needed 
 */

#include <PololuMaestro.h>
// #include <stdexcept> arduino compiler doesnt like to throw errors


// The maestro returns milliseconds as a representation of the current position
// of a servo and has the return value of uint8_t, here we type alias it to 
// make things a little more readable
using ms_t = uint8_t; 
#define MAX_DEGREE 180 
#define MIN_DEGREE 0 

class SB_Servo { 
	private: 
		// We make the maestro static so that it's shared across all instances 
		// of Servos
		static MiniMaestro maestro;
		int channelNum = -1; // default value for the channel, 
						     // indicates the channel has not been assigned
		/** 
		 * These values are found from servo specific data sheets					 
		 * The default values are referenced from the HS-422 0-180* digital servo 
		 * Which has specific values of 500-2500 us. The maestro's specifications use 
		 * 4x pulse widths, just how it works, thus we use 2k and 10k as
		 * the minimum and maximum pulses.
		 */
		int maxMS = 10000; 
		int minMS = 2000; 

		float msToDegrees(ms_t ms);
		ms_t degToMS(float degrees);  

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
		
		/** 
		 * Gets the current degrees of this servo 
		 * by sending asking the maestro for the current degrees of the servo
		 *
		 * @return the current degrees.
		 * @return -1 if there is a communication failure
		 */
		float getCurrentDegrees();

		/** Rotates this servo to a specific degrees, 0-180, or within the minimum
		 * and maximum values as dictated by the member values
		 *
		 * @param degrees -- the degrees to rotate to 
		 * @return whether or not the Servo is properly connected to the Maestro, for example
		 * if a channel number was never asserted then the Servo will do nothing and return false; 
		 */
		bool rotateToDegrees(float degrees);


		/** 
		 * Rotates the servo by a set amount   
		 * Checks to make sure that the requested amount is within the servos
		 * tolerated limits
		 * This method also directly modifies the Servos, and thus 
		 * needs to be able to raise some kind of error if communication with the Maestro
		 * fails 
		 * @param degreesBy -- the amount of degrees to turn by 
		 * @return whether or not the requested operation was doable
		 */
		bool rotateBy(float degreesBy);


};

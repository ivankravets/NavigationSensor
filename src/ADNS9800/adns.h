/*
  adns.h - Library for communicating with ADNS-9800 laser mouse sensor.
  Created by Mark Bucklin, May 21, 2014.
*/

#ifndef ADNS_h
#define ADNS_h

#if ARDUINO >= 100
#include <Arduino.h>
#include <Print.h>
#else
#include <WProgram.h>
#endif
#include <SPI.h>
#include <stdint.h>

// Sensor Register Include - ADNS-9800
#include "adns_config.h"
#include "adns_types.h"

// Common Include
#include "NavSensorLib\navsensor.h"

// =============================================================================
// ADNS Class Declaration
// =============================================================================
class ADNS
{
  public:
	// Constructor
	ADNS(const int chipSelectPin = 4) : _chipSelectPin(chipSelectPin){};

	// Setup
	bool begin(const uint16_t resolutionCPI = ADNS_DEFAULT_SENSOR_RESOLUTION,
			   const uint16_t minSampleRateHz = ADNS_DEFAULT_SENSOR_MINSAMPLERATE);

	// Trigger Start, Capture, & Readout
	void triggerAcquisitionStart();
	void triggerSampleCapture();
	void triggerPositionUpdate();
	void triggerAcquisitionStop();

	// Convert & Return Captured Data
	displacement_t readDisplacement();
	position_t readPosition();
	velocity_t readVelocity();
	void printLast();

	// Units
	void setDisplacementUnits(Unit::Distance, Unit::Time = Unit::Time::MILLISECOND);
	void setPositionUnits(Unit::Distance, Unit::Time = Unit::Time::MILLISECOND);
	void setVelocityUnits(Unit::Distance, Unit::Time = Unit::Time::MILLISECOND);
	// // void setDistanceUnit(Unit::Distance);
	// // void setTimeUnit(Unit::Time);

	// Sensor Communication (SPI)
	void select();
	void deselect();
	uint8_t readRegister(const RegisterAddress address);
	void writeRegister(const RegisterAddress address, const uint8_t data);

	// Sensor Settings
	void setResolution(const uint16_t cpi);
	uint16_t getResolution();
	void setMaxSamplePeriod(const uint16_t us);
	uint16_t getMaxSamplePeriod();
	void setMinSamplePeriod(const uint16_t us);
	uint16_t getMinSamplePeriod();
	void setMinSampleRate(const uint16_t hz);
	uint16_t getMinSampleRate();
	uint16_t getSamplePeriod();
	uint16_t getSampleRate();

	// Mode Settings
	void setMotionSensePinInterruptMode(const int pin);
	// todo mode: accumulate or buffer, rising? falling?

  protected:
	// Configuration
	void initialize();
	void powerUpSensor();
	void shutDownSensor();
	void resetSensor();
	void uploadFirmware();
	void enableLaser();
	void delaySleepTimeout();
	void disableSleepTimeout();
	void setMaxLiftDetectionThreshold();

	// Status Flags
	bool _configuredFlag = false;
	bool _initializedFlag = false;
	volatile bool _runningFlag = false;
	volatile bool _selectedFlag = false;

	// Hardware Configuration (Pins) //todo should be made all const
	const int _chipSelectPin;
	const size_t _readoutSize = adns_readout_max_size;
	int _motionSensePin;

	// Current Sensor Settings
	String _firmwareRevision;
	uint16_t _resolutionCountsPerInch;				// Counts-Per-Inch
	uint16_t _minSampleRate;						// Hz
	uint16_t _maxSamplePeriod;						// Microseconds
	uint16_t _minSamplePeriod;						// Microseconds
	adns_time_t _acquisitionStartMicrosCountOffset; // Microseconds

	// Unit Conversions
	typedef struct
	{
		Unit::Distance distance = Unit::Distance::MILLIMETER;
		Unit::Time time = Unit::Time::MILLISECOND;
	} dist_time_unit_t;
	struct
	{
		dist_time_unit_t position;
		dist_time_unit_t displacement;
		dist_time_unit_t velocity;
	} _unit;
	float _resolutionInchPerCount;

	// Readout Data
	adns_capture_t _currentCapture;
	adns_displacement_t _currentDisplacement;
	adns_position_t _currentPosition;
	adns_sample_t _lastSample; //todo use CircularBuffer, push, implement available()

// Static Settings
#ifdef ADNS_READMODE_BURST
	static const uint8_t _motionLatchRegAddr = (uint8_t)RegisterAddress::Motion_Burst;
	static const bool _burstModeFlag = ADNS_READMODE_BURST;
#else
	static const uint8_t _motionLatchRegAddr = (uint8_t)RegisterAddress::Motion;
	static const bool _burstModeFlag = false;
#endif
#ifdef ADNS_POSITIONUPDATE_AUTOMATIC
	static const bool _autoUpdateFlag = ADNS_POSITIONUPDATE_AUTOMATIC;
#else
	static const bool _autoUpdateFlag = false;
#endif
};

// =============================================================================
// Inline Convenience Functions
// =============================================================================
inline int16_t reg2Int16(uint8_t, uint8_t);
inline int16_t convertTwosComplement(int16_t b);
inline uint32_t getMicrosElapse(uint32_t t1, uint32_t t2);

// ADAFRUIT_SENSOR UNIFIED
// bool getEvent(sensors_event_t *event)
// void getSensor(sensor_t *sensor)
// enable
// disable
// void displaySensorDetails(void)
// void configureSensor(void)

#endif

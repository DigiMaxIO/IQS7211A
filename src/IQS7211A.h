#ifndef IQS7211A_h
#define IQS7211A_h

// Include Files
#include "Arduino.h"
#include "Wire.h"
#include "iqs7211a_addresses.h"

// Public Global Definitions
/* For use with Wire.h library. True argument with some functions closes the
I2C communication window. */
#define STOP    true
/* For use with Wire.h library. False argument with some functions keeps the
I2C communication window open. */
#define RESTART false

// Device Info
#define IQS7211A_PRODUCT_NUM                    0x02FB

// Info Flags Byte Bits.
#define IQS7211A_CHARGING_MODE_BIT_0            0
#define IQS7211A_CHARGING_MODE_BIT_1            1
#define IQS7211A_CHARGING_MODE_BIT_2            2
#define IQS7211A_ACTIVE_BITS     	        0b000
#define IQS7211A_IDLE_TOUCH_BITS	        0b001
#define IQS7211A_IDLE_BITS		        0b010
#define IQS7211A_LP1_BITS		        0b011
#define IQS7211A_LP2_BITS		        0b100
#define IQS7211A_ATI_ERROR_BIT	                3
#define IQS7211A_RE_ATI_OCCURRED_BIT	        4
#define IQS7211A_ALP_ATI_ERROR_BIT	        5
#define IQS7211A_ALP_RE_ATI_OCCURRED_BIT	4
#define IQS7211A_SHOW_RESET_BIT	                7
#define IQS7211A_NUM_FINGERS_BIT_0              0       // 8
#define IQS7211A_NUM_FINGERS_BIT_1              1       // 9
#define IQS7211A_NO_FINGERS_BITS     	        0b00
#define IQS7211A_1_FINGER_ACTIVE_BITS	        0b01
#define IQS7211A_2_FINGER_ACTIVE_BITS		0b10
#define IQS7211A_TP_MOVEMENT_BIT	        2       // 10
#define IQS7211A_TOO_MANY_FINGERS_BIT	        4       // 12
#define IQS7211A_ALP_OUTPUT_BIT	                6       // 14

// System Control Bits
#define IQS7211A_MODE_SELECT_BIT_0	        0
#define IQS7211A_MODE_SELECT_BIT_1              1
#define IQS7211A_MODE_SELECT_BIT_2              2
#define IQS7211A_TP_RESEED_BIT		        3
#define IQS7211A_ALP_RESEED_BIT		        4
#define IQS7211A_TP_RE_ATI_BIT		        5
#define IQS7211A_ALP_RE_ATI_BIT		        6
#define IQS7211A_ACK_RESET_BIT		        7
#define IQS7211A_SW_RESET_BIT		        1       // 9
#define IQS7211A_TX_TEST_BIT                    7       // 15

// Config Settings Bits
#define IQS7211A_COMMS_REQ_BIT	                4
#define IQS7211A_EVENT_MODE_BIT	                0       // 8
#define IQS7211A_GESTURE_EVENT_BIT	        1       // 9
#define IQS7211A_TP_EVENT_BIT	                2       // 10

// Gesture Bits
#define IQS7211A_GESTURE_SINGLE_TAP_BIT         0
#define IQS7211A_GESTURE_PRESS_HOLD_BIT         1
#define IQS7211A_GESTURE_SWIPE_X_NEGATIVE_BIT   2
#define IQS7211A_GESTURE_SWIPE_X_POSITIVE_BIT   3
#define IQS7211A_GESTURE_SWIPE_Y_POSITIVE_BIT   4
#define IQS7211A_GESTURE_SWIPE_Y_NEGATIVE_BIT   5

#define FINGER_1                                1
#define FINGER_2                                2

/* Defines and structs for IQS7211A states */
/**
 * @brief  iqs7211a Init Enumeration.
 */
typedef enum {
        IQS7211A_INIT_NONE = (uint8_t) 0x00,
        IQS7211A_INIT_VERIFY_PRODUCT,
        IQS7211A_INIT_READ_RESET,
	IQS7211A_INIT_CHIP_RESET,
	IQS7211A_INIT_UPDATE_SETTINGS,
	IQS7211A_INIT_CHECK_RESET,
	IQS7211A_INIT_ACK_RESET,
	IQS7211A_INIT_ATI,
        IQS7211A_INIT_WAIT_FOR_ATI,
        IQS7211A_INIT_READ_DATA,
	IQS7211A_INIT_ACTIVATE_EVENT_MODE,
        IQS7211A_INIT_ACTIVATE_STREAM_MODE,
	IQS7211A_INIT_DONE
} iqs7211a_init_e;

typedef enum {
        IQS7211A_STATE_NONE = (uint8_t) 0x00,
        IQS7211A_STATE_START,
        IQS7211A_STATE_INIT,
        IQS7211A_STATE_SW_RESET,
        IQS7211A_STATE_CHECK_RESET,
	IQS7211A_STATE_RUN,
} iqs7211a_state_e;

typedef enum {
        IQS7211A_CH_NONE = (uint8_t) 0x00,
        IQS7211A_CH_PROX,
        IQS7211A_CH_TOUCH,
        IQS7211A_CH_UNKNOWN,
} iqs7211a_ch_states;

typedef enum {
        IQS7211A_ACTIVE = (uint8_t) 0x00,
        IQS7211A_IDLE_TOUCH,
        IQS7211A_IDLE,
        IQS7211A_LP1,
        IQS7211A_LP2,
        IQS7211A_POWER_UNKNOWN
} iqs7211a_power_modes;

typedef enum {
        IQS7211A_GESTURE_SINGLE_TAP = (uint8_t) 0x00,
        IQS7211A_GESTURE_PRESS_HOLD,
        IQS7211A_GESTURE_SWIPE_X_NEGATIVE,
        IQS7211A_GESTURE_SWIPE_X_POSITIVE,
        IQS7211A_GESTURE_SWIPE_Y_POSITIVE,
        IQS7211A_GESTURE_SWIPE_Y_NEGATIVE,
        IQS7211A_GESTURE_NONE,
} iqs7211a_gestures_e;

/* IQS7211A Memory map data variables, only save the data that might be used
   during program runtime */
#pragma pack(1)
typedef struct
{
	/* READ ONLY */			//  I2C Addresses:
	uint8_t VERSION_DETAILS[20]; 	// 	0x00 -> 0x09
        uint8_t INFO_FLAGS[2];          // 	0x10
        uint8_t GESTURES[2]; 		// 	0x11
        int16_t REL_X[2];               //      0x12
        int16_t REL_Y[2];               //      0x13
        uint8_t FINGER_1_X[2];          // 	0x14
        uint8_t FINGER_1_Y[2];          // 	0x15
        uint8_t FINGER_2_X[2];          // 	0x18
        uint8_t FINGER_2_Y[2];          // 	0x19

	/* READ WRITE */		//  I2C Addresses:
	uint8_t SYSTEM_CONTROL[2]; 	// 	0x50
} IQS7211A_MEMORY_MAP;
#pragma pack(4)

#pragma pack(1)
typedef struct {
        iqs7211a_state_e        state;
        iqs7211a_init_e         init_state;
}iqs7211a_s;
#pragma pack(4)

// Class Prototype
class IQS7211A
{
   public:
        /* Public Constructors */
        IQS7211A();

        /* Public Device States */
        iqs7211a_s iqs7211a_state;

        /* Public Variables */
        IQS7211A_MEMORY_MAP IQSMemoryMap;
        bool new_data_available;

        /* Public Methods */
        void begin(uint8_t deviceAddressIn, uint8_t readyPinIn, uint8_t rstPinIn);
        bool init(void);
        void run(void);
        void queueValueUpdates(void);
        bool readATIactive(void);
        uint16_t getProductNum(bool stopOrRestart);
        uint8_t getmajorVersion(bool stopOrRestart);
        uint8_t getminorVersion(bool stopOrRestart);
        void acknowledgeReset(bool stopOrRestart);
        void ReATI(bool stopOrRestart);
        void SW_Reset(bool stopOrRestart);
        void writeMM(bool stopOrRestart);
        void clearRDY(void);
        bool getRDYStatus(void);

        void setStreamMode(bool stopOrRestart);
        void setEventMode(bool stopOrRestart);

        void updateInfoFlags(bool stopOrRestart);
        iqs7211a_power_modes getPowerMode(void);
        bool checkReset(void);
        void updateAbsCoordinates(bool stopOrRestart, uint8_t fingerNum);
        uint16_t getAbsYCoordinate(uint8_t fingerNum);
        uint16_t getAbsXCoordinate(uint8_t fingerNum);
        void Print_signed(int16_t i16Num);
        int16_t getRelYCoordinate();
        int16_t getRelXCoordinate();
        bool touchpad_event_occurred(void);
        iqs7211a_gestures_e get_gesture_event(void);
        uint8_t getNumFingers(void);

        void force_I2C_communication(void);

        void enableGestureEvent(bool stopOrRestart);
        void disableGestureEvent(bool stopOrRestart);
        void enableTPEvent(bool stopOrRestart);
        void disableTPEvent(bool stopOrRestart);
        void enableCommsReqEn(bool stopOrRestart);
        void disableCommsReqEn(bool stopOrRestart);

private:
        /* Private Variables */
        uint8_t _deviceAddress;
        uint8_t _reset_pin;

        /* Private Methods */
        
        void readRandomBytes(uint8_t memoryAddress, uint8_t numBytes, uint8_t bytesArray[], bool stopOrRestart);
        void writeRandomBytes(uint8_t memoryAddress, uint8_t numBytes, uint8_t bytesArray[], bool stopOrRestart);
        void writeRandomBytes16(uint16_t memoryAddress, uint8_t numBytes, uint8_t bytesArray[], bool stopOrRestart);
        bool getBit(uint8_t data, uint8_t bit_number);
        uint8_t setBit(uint8_t data, uint8_t bit_number);
        uint8_t clearBit(uint8_t data, uint8_t bit_number);
};
#endif // IQS7211A_h

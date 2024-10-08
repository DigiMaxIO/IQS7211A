/* Include Files */
#include "IQS7211A.h"
#include "IQS7211A_INIT_TOUCHPAD4.h"

//Private Functions
void iqs7211a_ready_interrupt(void);
/* Private Global Variables */
bool iqs7211a_deviceRDY = false;
uint8_t _ready_pin;


/*****************************************************************************/
/*                             CONSTRUCTORS                                  */
/*****************************************************************************/
IQS7211A::IQS7211A(){
}

/*****************************************************************************/
/*                            PUBLIC METHODS                                 */
/*****************************************************************************/

/**
  * @name   begin
  * @brief  A method to initialize the IQS7211A device with the device address 
  *         and ready pin specified by the user.
  * @param  deviceAddress -> The address of the IQS7211A device.
  * @param  readyPin      -> The Arduino pin which is connected to the ready 
  *                          pin of the IQS7211A device.
  * @retval None.
  * @note   - Receiving a true return value does not mean that initialization was 
  *           successful.
  *         - Receiving a true return value only means that the IQS device 
  *           responded to the request for communication.
  *         - Receiving a false return value means that initialization did not 
  *           take place at all.
  *         - If communication is successfully established then it is unlikely 
  *           that initialization will fail.
  */
void IQS7211A::begin(uint8_t deviceAddressIn, uint8_t readyPinIn, uint8_t rstPinIn)
{
  /* Initialize I2C communication here, since this library can't function 
     without it. */
  Wire.begin();
  Wire.setClock(400000);

  _deviceAddress = deviceAddressIn;
  _ready_pin = readyPinIn;
  _reset_pin = rstPinIn;

  pinMode(this->_reset_pin, OUTPUT);
  delay(200);
  digitalWrite(this->_reset_pin, LOW);
  delay(200);
  digitalWrite(this->_reset_pin, HIGH);
  pinMode(_ready_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(_ready_pin), iqs7211a_ready_interrupt, CHANGE);

  /* Initialize "running" and "init" state machine variables. */
  iqs7211a_state.state      = IQS7211A_STATE_START;
  iqs7211a_state.init_state = IQS7211A_INIT_VERIFY_PRODUCT;
}

/**
 * @name   init
 * @brief  A method that runs through a normal start-up routine to set up the 
 *         IQS7211A with the desired settings from the IQS7211A_init.h file.
 * @retval Returns true if the full start-up routine has been completed, 
 *         returns false if not.
 * @note   -  No false return will be given, the program will thus be stuck when 
 *            one of the cases is not able to finish.
 *         -  See serial communication to find the ERROR case
 *         -  Note that the //Serial.println() commands can be commented out to   
 *            save on dynamic memory
 */
bool IQS7211A::init(void)
{
  uint16_t prod_num;
  uint8_t ver_maj, ver_min;

  switch (iqs7211a_state.init_state)
  {
    /* Verifies product number to determine if the correct device is connected 
    for this example */
    case IQS7211A_INIT_VERIFY_PRODUCT:
      if(iqs7211a_deviceRDY)
      {
        Serial.println("\tIQS7211A_INIT_VERIFY_PRODUCT");
        prod_num  = getProductNum(RESTART);
        ver_maj   = getmajorVersion(RESTART);
        ver_min   = getminorVersion(STOP);
        Serial.print("\t\tProduct number is: ");
        Serial.print(prod_num);
        Serial.print(" v");
        Serial.print(ver_maj);
        Serial.print(".");
        Serial.println(ver_min);
        //if(prod_num == IQS7211A_PRODUCT_NUM)
        //{
          //Serial.println("\t\tIQS7211A Release UI Confirmed!");
          iqs7211a_state.init_state = IQS7211A_INIT_READ_RESET;
        //}
        //else
        //{
        //  //Serial.println("\t\tDevice is not a IQS7211A!");
        //  iqs7211a_state.init_state = IQS7211A_INIT_NONE;
        //}
      }
    break;

    /* Verify if a reset has occurred */
    case IQS7211A_INIT_READ_RESET:
      if(iqs7211a_deviceRDY)
      {
        //Serial.println("\tIQS7211A_INIT_READ_RESET");
        updateInfoFlags(RESTART);
        if (checkReset())
        {
          //Serial.println("\t\tReset event occurred.");
          iqs7211a_state.init_state = IQS7211A_INIT_UPDATE_SETTINGS;
        }
        else
        {
          //Serial.println("\t\t No Reset Event Detected - Request SW Reset");
          iqs7211a_state.init_state = IQS7211A_INIT_CHIP_RESET;
        }
      }
    break;

    /* Perform SW Reset */
    case IQS7211A_INIT_CHIP_RESET:
      if(iqs7211a_deviceRDY)
      {
        //Serial.println("\tIQS7211A_INIT_CHIP_RESET");

        //Perform SW Reset
        SW_Reset(STOP);
        //Serial.println("\t\tSoftware Reset Bit Set.");
        delay(100);
        iqs7211a_state.init_state = IQS7211A_INIT_READ_RESET;
      }
    break;

    /* Write all settings to IQS7211A from .h file */
    case IQS7211A_INIT_UPDATE_SETTINGS:
      if(iqs7211a_deviceRDY)
      {
        //Serial.println("\tIQS7211A_INIT_UPDATE_SETTINGS");
        writeMM(RESTART);
        iqs7211a_state.init_state = IQS7211A_INIT_ACK_RESET;
      }
    break;

    /* Acknowledge that the device went through a reset */
    case IQS7211A_INIT_ACK_RESET:
      if(iqs7211a_deviceRDY)
      {
        //Serial.println("\tIQS7211A_INIT_ACK_RESET");
        acknowledgeReset(STOP);
        iqs7211a_state.init_state = IQS7211A_INIT_ATI;
      }
      break;

    /* Run the ATI algorithm to recalibrate the device with newly added settings */
    case IQS7211A_INIT_ATI:
      if(iqs7211a_deviceRDY)
      {
        //Serial.println("\tIQS7211A_INIT_ATI");
        ReATI(STOP);
        iqs7211a_state.init_state = IQS7211A_INIT_WAIT_FOR_ATI;
        //Serial.println("\tIQS7211A_INIT_WAIT_FOR_ATI");
      }
    break;

    /* Read the ATI Active bit to see if the rest of the program can continue */
    case IQS7211A_INIT_WAIT_FOR_ATI:
      if(iqs7211a_deviceRDY)
      {
        if(!readATIactive())
        {
          //Serial.println("\t\tDONE");
          iqs7211a_state.init_state = IQS7211A_INIT_READ_DATA;
        }
      }
    break;

    /* Read the latest data from the iqs7211a */
    case IQS7211A_INIT_READ_DATA:
      if(iqs7211a_deviceRDY)
      {
        //Serial.println("\tIQS7211A_INIT_READ_DATA");
        queueValueUpdates();
        iqs7211a_state.init_state = IQS7211A_INIT_ACTIVATE_EVENT_MODE;
      }
    break;

    /* Turn on I2C Event mode */
    case IQS7211A_INIT_ACTIVATE_EVENT_MODE:
      if(iqs7211a_deviceRDY)
      {
        //Serial.println("\tIQS7211A_INIT_ACTIVATE_EVENT_MODE");
        setEventMode(STOP);
        iqs7211a_state.init_state = IQS7211A_INIT_DONE;
      }
    break;

    /* Turn on I2C Stream mode */
    case IQS7211A_INIT_ACTIVATE_STREAM_MODE:
      if(iqs7211a_deviceRDY)
      {
        //Serial.println("\tIQS7211A_INIT_ACTIVATE_STREAM_MODE");
        setStreamMode(STOP);
        iqs7211a_state.init_state = IQS7211A_INIT_DONE;
      }
    break;

    /* If all operations have been completed correctly, the RDY pin can be set
     * up as an interrupt to indicate when new data is available */
    case IQS7211A_INIT_DONE:
      //Serial.println("\tIQS7211A_INIT_DONE");
      new_data_available = true;
      return true;
    break;

    default:
      break;
  }
  return false;
}

/**
 * @name   run
 * @brief  Run the device ready check and read the necessary bytes when the 
 *         IQS7211A has pulled the RDY line low.
 *         The new_data_available flag will be set when a ready low is received 
 *         from the IQS7211A.
 * @param  None
 * @retval None.
 * @note   queueValueUpdates can be edited by the user if other data should be 
 *         read every time a RDY window is received.
 */
void IQS7211A::run(void)
{
  switch(iqs7211a_state.state)
  {
    case IQS7211A_STATE_NONE:
    break;
    /* After a hardware reset, this is the starting position of the running state 
       machine */
    case IQS7211A_STATE_START:
      //Serial.println("IQS7211A Initialization:");
      iqs7211a_state.state = IQS7211A_STATE_INIT;
    break;

    /* Perform the initialization routine on the IQS7211A */
    case IQS7211A_STATE_INIT:
      if(init())
      {
        //Serial.println("IQS7211A Initialization complete!\n");
        iqs7211a_state.state = IQS7211A_STATE_RUN;
      }
    break;

    /* Send an I2C software reset in the next RDY window */
    case IQS7211A_STATE_SW_RESET:
      if(iqs7211a_deviceRDY)
      {
        SW_Reset(STOP);
        iqs7211a_state.state = IQS7211A_STATE_RUN;
      }
    break;

    /* Continuous reset monitoring state, ensure no reset event has occurred 
      for data to be valid */
    case IQS7211A_STATE_CHECK_RESET:
      if(checkReset())
      {
        //Serial.println("Reset occurred!\n");
        new_data_available = false;
        iqs7211a_state.state = IQS7211A_STATE_START;
        iqs7211a_state.init_state = IQS7211A_INIT_VERIFY_PRODUCT;
      }
      /* A reset did not occur, move to the run state and wait for a new RDY window */
      else
      {
        new_data_available = true; /* No reset, thus data is valid */
        iqs7211a_state.state = IQS7211A_STATE_RUN;
      }
    break;

    /* If a RDY Window is open, read the latest values from the IQS7211A */
    case IQS7211A_STATE_RUN:
      if(iqs7211a_deviceRDY)
      {
        queueValueUpdates();
        iqs7211a_deviceRDY = false;
        new_data_available = false;
        iqs7211a_state.state = IQS7211A_STATE_CHECK_RESET;
      }
    break;
  }
}

/**
 * @name    iqs7211a_ready_interrupt
 * @brief   A method used as an interrupt function. Activated when a High -> Low 
 *          (falling edge) and a Low -> High (Rising edge) interrupt is seen on 
 *          the correct Arduino interrupt pin.
 * @param   None.
 * @retval  None.
 * @note    Keep this function as simple as possible to prevent stuck states and 
 *          slow operations.
 */
void iqs7211a_ready_interrupt(void)
{
  if(digitalRead(_ready_pin))
  {
    iqs7211a_deviceRDY = false;
  }
  else
  {
    iqs7211a_deviceRDY = true;
  }
}

/**
  * @name   clearRDY
  * @brief  A method used to clear the ready interrupt bit.
  * @param  None.
  * @retval None.
  */
void IQS7211A::clearRDY(void)
{
  iqs7211a_deviceRDY = false;
}

/**
  * @name   getRDYStatus
  * @brief  A method used to retrieve the device RDY status.
  * @param  None.
  * @retval Returns the boolean IQS7211A RDY state.
  *         - True when RDY line is LOW
  *         - False when RDY line is HIGH
  */
bool IQS7211A::getRDYStatus(void)
{
  return iqs7211a_deviceRDY;
}

/**
 * @name    queueValueUpdates
 * @brief   All I2C read operations in the queueValueUpdates method will be 
 *          performed each time the IQS7211A opens a RDY window.
 * @param   None.
 * @retval  None.
 * @note    Any address in the memory map can be read from here. This is where  
 *          data read from the chip gets updated.
 */
void IQS7211A::queueValueUpdates(void)
{
  uint8_t transferBytes[10]; // The array which will hold the bytes to be transferred.

  /* Read the gesture and info flags. */
  readRandomBytes(IQS7211A_MM_INFOFLAGS, 4, transferBytes, RESTART);

  /* Assign the info flags to the info flags */
  IQSMemoryMap.INFO_FLAGS[0]  = transferBytes[0];
  IQSMemoryMap.INFO_FLAGS[1]  = transferBytes[1];

  /* Assign the gesture flags to the gesture flags */
  IQSMemoryMap.GESTURES[0]    = transferBytes[2];
  IQSMemoryMap.GESTURES[1]    = transferBytes[3];

  /* Read Finger 1 x and y coordinate. */
  readRandomBytes(IQS7211A_MM_FINGER_1_X, 4, transferBytes, RESTART);

  /* Read Finger 1 x and y coordinate. */
  IQSMemoryMap.FINGER_1_X[0]  = transferBytes[0];
  IQSMemoryMap.FINGER_1_X[1]  = transferBytes[1];
  IQSMemoryMap.FINGER_1_Y[0]  = transferBytes[2];
  IQSMemoryMap.FINGER_1_Y[1]  = transferBytes[3];

  /* Read Finger 2 x and y coordinate. */
  readRandomBytes(IQS7211A_MM_FINGER_2_X, 4, transferBytes, RESTART);

  IQSMemoryMap.FINGER_2_X[0]  = transferBytes[0];
  IQSMemoryMap.FINGER_2_X[1]  = transferBytes[1];
  IQSMemoryMap.FINGER_2_Y[0]  = transferBytes[2];
  IQSMemoryMap.FINGER_2_Y[1]  = transferBytes[3];

  /* Read Relative x and y coordinate. */
  readRandomBytes(IQS7211A_MM_RELATIVE_X, 4, transferBytes, STOP);

  /* Read Relative x and y coordinate. */
  IQSMemoryMap.REL_X[0]  = transferBytes[0];
  IQSMemoryMap.REL_X[1]  = transferBytes[1];
  IQSMemoryMap.REL_Y[0]  = transferBytes[2];
  IQSMemoryMap.REL_Y[1]  = transferBytes[3];
}

/**
  * @name	  readATIactive
  * @brief  A method that checks if the ATI routine is still active
  * @param  None.
  * @retval Returns true if the ATI_ACTIVE_BIT is cleared, false if the 
  *         ATI_ACTIVE_BIT is set.
  * @note   If the ATI routine is active the channel states (NONE, PROX, TOUCH) 
  *         might exhibit unwanted behaviour. Thus it is advised to wait for 
  *         the routine to complete before continuing.
  */
bool IQS7211A::readATIactive(void)
{
  /* Read the Info flags from the IQS7211A*/
  updateInfoFlags(STOP);

  if (getBit(IQSMemoryMap.INFO_FLAGS[0], IQS7211A_RE_ATI_OCCURRED_BIT))
  {
    return false; 
  }

  return true;
}

/**
 * @name	  checkReset
 * @brief   A method that checks if the device has reset and returns the reset 
 *          status.
 * @param   None.
 * @retval  Returns true if a reset has occurred, false if no reset has occurred.
 * @note    - If a reset has occurred the device settings should be reloaded using 
 *            the begin function.
 * 		      - After new device settings have been reloaded the acknowledgeReset() 
 *            function can be used to clear the reset flag.
 */
bool IQS7211A::checkReset(void)
{
	/* Perform a bitwise AND operation inside the getBit() function with the 
  SHOW_RESET_BIT to return the reset status */
  return getBit(IQSMemoryMap.INFO_FLAGS[0], IQS7211A_SHOW_RESET_BIT);
}

/**
  * @name	  getProductNum
  * @brief  A method that reads the device product number and returns a uint16_t 
  *         result.
  * @param  stopOrRestart -> Specifies whether the communications window must 
  *                          be kept open or must be closed after this action.
  *                          Use the STOP and RESTART definitions.
  * @retval Returns the product number as a 16-bit unsigned integer value.
  * @note   If the product is not correctly identified an appropriate messages 
  *         should be displayed.
  */
uint16_t IQS7211A::getProductNum(bool stopOrRestart)
{
	uint8_t transferBytes[2];	    // A temporary array to hold the byte to be transferred.
  uint8_t prodNumLow = 0;       // Temporary storage for the low byte.
  uint8_t prodNumHigh = 0;      // Temporary storage for the high byte.
  uint16_t prodNumReturn = 0;   // The 16-bit return value.

	// Read the Device info from the IQS7211A.
	readRandomBytes(IQS7211A_MM_PROD_NUM, 2, transferBytes, stopOrRestart);

  // Construct the 16-bit return value.
  prodNumLow = transferBytes[0];
  prodNumHigh = transferBytes[1];
  prodNumReturn = (uint16_t)(prodNumLow);
  prodNumReturn |= (uint16_t)(prodNumHigh<<8);
  // Return the product number value.
  return prodNumReturn;
}

/**
 * @name	  getmajorVersion
 * @brief   A method that gets the device firmware major version number
 * @param   stopOrRestart -> Specifies whether the communications window must  
 *                          be kept open or must be closed after this action.
 *                          Use the STOP and RESTART definitions.
 * @retval  Returns the major version number as an 8-bit unsigned integer value.
 */
uint8_t IQS7211A::getmajorVersion(bool stopOrRestart)
{
  /* A temporary array to hold the byte to be transferred. */
  uint8_t transferBytes[2]; 
  /* Temporary storage for the firmware version major number. */
  uint8_t ver_maj = 0;      

  /* Read the info from the IQS7211A. */
  readRandomBytes(IQS7211A_MM_MAJOR_VERSION_NUM, 2, transferBytes, stopOrRestart);
  /* Extract major value from correct byte */
  ver_maj = transferBytes[0];
  /* Return the major firmware version number value. */
  return ver_maj;
}

/**
 * @name	 getminorVersion
 * @brief  A method that checks the device firmware minor version number
 * @param  stopOrRestart -> Specifies whether the communications window must be 
 *                          kept open or must be closed after this action.
 *                          Use the STOP and RESTART definitions.
 * @retval Returns the minor version number as an 8-bit unsigned integer value.
 */
uint8_t IQS7211A::getminorVersion(bool stopOrRestart)
{
  /* A temporary array to hold the byte to be transferred. */
  uint8_t transferBytes[2]; 
  /* Temporary storage for the firmware version minor number. */
  uint8_t ver_min = 0;      

  /* Read the info from the IQS7211A. */
  readRandomBytes(IQS7211A_MM_MINOR_VERSION_NUM, 2, transferBytes, stopOrRestart);
  /* Extract minor value from correct byte */
  ver_min = transferBytes[0];
  /* Return the minor firmware version number value. */
  return ver_min;
}

/**
 * @name	acknowledgeReset
 * @brief  A method that clears the Show Reset bit by setting the Ack Reset bit
 * @param  stopOrRestart -> Specifies whether the communications window must be 
 *                          kept open or must be closed after this action.
 *                          Use the STOP and RESTART definitions.
 * @retval None.
 * @note   -  If a reset has occurred the device settings should be reloaded  
 *            using the begin function.
 * 		     -  After new device settings have been reloaded this method should  
 *            be used to clear the reset bit.
 */
void IQS7211A::acknowledgeReset(bool stopOrRestart)
{
	uint8_t transferByte[2];	// A temporary array to hold the bytes to be transferred.
	/* Read the System Flags from the IQS7211A, these must be read first in order 
    not to change any settings. */
	readRandomBytes(IQS7211A_MM_SYSTEM_CONTROL, 2, transferByte, RESTART);
	/* Write the Ack Reset bit to bit 1 to clear the Reset Event Flag. */
	transferByte[0] = setBit(transferByte[0], IQS7211A_ACK_RESET_BIT);
	/* Write the new byte to the System Flags address. */
	writeRandomBytes(IQS7211A_MM_SYSTEM_CONTROL, 2, transferByte, stopOrRestart);
}

/**
 * @name   ReATI
 * @brief  A method that sets the ReATI bit to force the IQS7211A device
 *         to run the Automatic Tuning Implementation (ATI) routine on 
 *         the TP channels.
 * @param  stopOrRestart -> Specifies whether the communications window must 
 *                          be kept open or must be closed after this action.
 *                          Use the STOP and RESTART definitions.
 * @retval None.
 * @note   To force ATI, the IQS7211A_TP_RE_ATI_BIT in the System Control 
 *         register is set.
 */
void IQS7211A::ReATI(bool stopOrRestart)
{
  uint8_t transferByte[2]; // Array to store the bytes transferred.

  readRandomBytes(IQS7211A_MM_SYSTEM_CONTROL, 2, transferByte, RESTART);
  /* Set the RE_ATI_BIT in the SYSTEM_CONTROL register */
  transferByte[0] = setBit(transferByte[0], IQS7211A_TP_RE_ATI_BIT); 
  /* Write the new byte to the required device. */
  writeRandomBytes(IQS7211A_MM_SYSTEM_CONTROL, 2, transferByte, stopOrRestart);
}

/**
 * @name   SW_Reset
 * @brief  A method that sets the SW RESET bit to force the IQS7211A device
 *         to do a SW reset.
 * @param  stopOrRestart -> Specifies whether the communications window must be 
 *                          kept open or must be closed after this action.
 *                          Use the STOP and RESTART definitions.
 * @retval None.
 * @note   To perform SW Reset, IQS7211A_SW_RESET_BIT in SYSTEM_CONTROL is set.
 */
void IQS7211A::SW_Reset(bool stopOrRestart)
{
  uint8_t transferByte[2]; // Array to store the bytes transferred.

  readRandomBytes(IQS7211A_MM_SYSTEM_CONTROL, 2, transferByte, RESTART);
  /* Set the SW_RESET_BIT in the SYSTEM_CONTROL register */
  transferByte[1] = setBit(transferByte[1], IQS7211A_SW_RESET_BIT); 
  /* Write the new byte to the required device. */
  writeRandomBytes(IQS7211A_MM_SYSTEM_CONTROL, 2, transferByte, stopOrRestart);
}

/**
  * @name   setStreamMode
  * @brief  A method to set the IQS7211A device into streaming mode.
  * @param  stopOrRestart -> Specifies whether the communications window must be 
  *                          kept open or must be closed after this action.
  *                          Use the STOP and RESTART definitions.
  * @retval None.
  * @note   All other bits at this register address are preserved.
  */
void IQS7211A::setStreamMode(bool stopOrRestart)
{
  uint8_t transferBytes[2]; // The array which will hold the bytes which are transferred.

  /* First read the bytes at the memory address so that they can be preserved.*/
  readRandomBytes(IQS7211A_MM_SYSTEM_CONTROL, 2, transferBytes, RESTART);
  /* Set/CLear the INTERFACE_SELECTION_BITS in CONTROL_SETTINGS */
  transferBytes[1] = clearBit(transferBytes[1], IQS7211A_EVENT_MODE_BIT);
  /* Write the bytes back to the device */
  writeRandomBytes(IQS7211A_MM_SYSTEM_CONTROL, 2, transferBytes, stopOrRestart);
}

/**
  * @name   setEventMode
  * @brief  A method to set the IQS7211A device into event mode.
  * @param  stopOrRestart -> Specifies whether the communications window must be 
  *                          kept open or must be closed after this action.
  *                          Use the STOP and RESTART definitions.
  * @retval None.
  * @note   All other bits at this register address are preserved.
  */
void IQS7211A::setEventMode(bool stopOrRestart)
{
  uint8_t transferByte[2]; // The array which will hold the bytes which are transferred.

  /* First read the bytes at the memory address so that they can be preserved */
  readRandomBytes(IQS7211A_MM_CONFIG_SETTINGS, 2, transferByte, RESTART);
  /* Set/CLear the INTERFACE_SELECTION_BITS in CONTROL_SETTINGS */
  transferByte[1] = setBit(transferByte[1], IQS7211A_EVENT_MODE_BIT);
  /* Write the bytes back to the device */
  writeRandomBytes(IQS7211A_MM_CONFIG_SETTINGS, 2, transferByte, stopOrRestart);
}

/**
 * @name   updateInfoFlags
 * @brief  A method which reads the IQS7211A info flags and assigns them to the 
 *         local memory map.
 * @param  stopOrRestart -> Specifies whether the communications window must be 
 *                          kept open or must be closed after this action.
 *              			      Use the STOP and RESTART definitions.
 * @retval None.
 */
void IQS7211A::updateInfoFlags(bool stopOrRestart)
{
  /* The array which will hold the bytes to be transferred. */
  uint8_t transferBytes[2]; 

  /* Read the info flags. */
  readRandomBytes(IQS7211A_MM_INFOFLAGS, 2, transferBytes, stopOrRestart);
  /* Assign the info flags to the info flags union. */
  IQSMemoryMap.INFO_FLAGS[0] = transferBytes[0];
  IQSMemoryMap.INFO_FLAGS[1] = transferBytes[1];
}

/**
  * @name   getPowerMode
  * @brief  A method which reads the INFO_FLAGS from the memory map and returns 
  *         the current power mode.
  * @param  void
  * @retval Returns the current iqs7211a_power_modes state the device is in.
  * @note   See Datasheet on power mode options and timeouts. 
  *         Normal Power, Low Power and Ultra Low Power (ULP).
  */
iqs7211a_power_modes IQS7211A::getPowerMode(void)
{
  uint8_t buffer = getBit(IQSMemoryMap.INFO_FLAGS[0], IQS7211A_CHARGING_MODE_BIT_0);
  buffer += getBit(IQSMemoryMap.INFO_FLAGS[0], IQS7211A_CHARGING_MODE_BIT_1) << 1;
  buffer += getBit(IQSMemoryMap.INFO_FLAGS[0], IQS7211A_CHARGING_MODE_BIT_2) << 2;

  if(buffer == IQS7211A_ACTIVE_BITS)
  {
    return IQS7211A_ACTIVE;
  }
  else if(buffer == IQS7211A_IDLE_TOUCH_BITS)
  {
    return IQS7211A_IDLE_TOUCH;
  }
  else if(buffer == IQS7211A_IDLE_BITS)
  {
    return IQS7211A_IDLE;
  }
    else if(buffer == IQS7211A_LP1_BITS)
  {
    return IQS7211A_LP1;
  }
    else if(buffer == IQS7211A_LP2_BITS)
  {
    return IQS7211A_LP2;
  }
  else
  {
    return IQS7211A_POWER_UNKNOWN;
  }
}

/**
 * @name   updateAbsCoordinates
 * @brief  A method which reads the IQS7211A x and y coordinates and assigns 
 *         them to FINGER_1_X and FINGER_1_X in the local memory map.
 * @param  stopOrRestart -> Specifies whether the communications window must be 
 *                          kept open or closed after retrieving the information.
 *                          Use the STOP and RESTART definitions.
 * @param  fingerNum     -> Specifies the finger number. Finger 1 is the first 
 *                          finger to touch the trackpad, finger 2 is the second
 *                          to touch the trackpad.
 * @retval None.
 * @note   The FINGER_1_X and FINGER_1_Y local memory map variables are altered 
 *         with the new values from the IQS7211A device. The user can use the 
 *         getAbsXCoordinate and getAbsYCoordinate methods to return the value.
 */
void IQS7211A::updateAbsCoordinates(bool stopOrRestart, uint8_t fingerNum)
{
  /* The temporary address which will hold the bytes from the 
  IQS7211A_MM_FINGER_1_X register address. */
  uint8_t transferBytes[4]; 
  if (fingerNum == FINGER_1)
  {
    /* Read the bytes using the readRandomBytes method to read bytes at the 
       Finger 1 address. */
    readRandomBytes(IQS7211A_MM_FINGER_1_X, 4, transferBytes, stopOrRestart);
    /*  Assign the bytes to the union. */
    IQSMemoryMap.FINGER_1_X[0] = transferBytes[0];
    IQSMemoryMap.FINGER_1_X[1] = transferBytes[1];
    IQSMemoryMap.FINGER_1_Y[0] = transferBytes[2];
    IQSMemoryMap.FINGER_1_Y[1] = transferBytes[3];
  }
  else if (fingerNum == FINGER_2)
  {
    /* Read the bytes using the readRandomBytes method to read bytes at the 
      Finger 2 address. */
    readRandomBytes(IQS7211A_MM_FINGER_2_X, 4, transferBytes, stopOrRestart);
    /*  Assign the bytes to the union. */
    IQSMemoryMap.FINGER_2_X[0] = transferBytes[0];
    IQSMemoryMap.FINGER_2_X[1] = transferBytes[1];
    IQSMemoryMap.FINGER_2_Y[0] = transferBytes[2];
    IQSMemoryMap.FINGER_2_Y[1] = transferBytes[3];
  }
}

/**
 * @name	  getAbsYCoordinate
 * @brief   A method that returns the constructed 16-bit coordinate value
 * @param   fingerNum     ->  Specifies the finger number. Finger 1 is the  
 *                            first finger to touch the trackpad, finger 2 is 
 *                            the second to touch the trackpad.
 * @retval  Returns 16-bit coordinate value.
 */
uint16_t IQS7211A::getAbsYCoordinate(uint8_t fingerNum)
{
  /* The 16-bit return value. */
  uint16_t absYCoordReturn = 0; 

  /* Construct the 16-bit return value. */
  if (fingerNum == FINGER_1)
  {
    absYCoordReturn = (uint16_t)(IQSMemoryMap.FINGER_1_Y[0]);
    absYCoordReturn |= (uint16_t)(IQSMemoryMap.FINGER_1_Y[1] << 8);
  }
  else if (fingerNum == FINGER_2)
  {
    absYCoordReturn = (uint16_t)(IQSMemoryMap.FINGER_2_Y[0]);
    absYCoordReturn |= (uint16_t)(IQSMemoryMap.FINGER_2_Y[1] << 8);
  }
  /* Return the coordinate value. Note that a value of 65535 (0xFFFF) means 
    there is no touch. */
  return absYCoordReturn;
}

/**
 * @name	  getAbsXCoordinate
 * @brief   A method that returns the constructed 16bit coordinate value
 * @param   fingerNum     ->  Specifies the finger number. Finger 1 is the  
 *                            first finger to touch the trackpad, finger 2 is 
 *                            the second to touch the trackpad.
 * @retval  Returns 16-bit coordinate value.
 */
uint16_t IQS7211A::getAbsXCoordinate(uint8_t fingerNum)
{
  /* The 16-bit return value. */
  uint16_t absXCoordReturn = 0; 

  /* Construct the 16-bit return value. */
  if (fingerNum == FINGER_1)
  {
    absXCoordReturn = (uint16_t)(IQSMemoryMap.FINGER_1_X[0]);
    absXCoordReturn |= (uint16_t)(IQSMemoryMap.FINGER_1_X[1] << 8);
  }
  else if (fingerNum == FINGER_2)
  {
    absXCoordReturn = (uint16_t)(IQSMemoryMap.FINGER_2_X[0]);
    absXCoordReturn |= (uint16_t)(IQSMemoryMap.FINGER_2_X[1] << 8);
  }
  /*- Return the coordinate value.
    - Note that a value of 65535 (0xFFFF) means there is no touch. */
  return absXCoordReturn;
}


int16_t IQS7211A::getRelYCoordinate()
{
  /* The 16-bit return value. */
  int16_t relYCoordReturn = 0; 

  relYCoordReturn = (int16_t)(IQSMemoryMap.REL_Y[0]);
  relYCoordReturn |= (int16_t)(IQSMemoryMap.REL_Y[1] << 8);

  return relYCoordReturn;
}

int16_t IQS7211A::getRelXCoordinate()
{
  /* The 16-bit return value. */
  int16_t relXCoordReturn = 0; 

  relXCoordReturn = (int16_t)(IQSMemoryMap.REL_X[0]);
  relXCoordReturn |= (int16_t)(IQSMemoryMap.REL_X[1] << 8);

  return relXCoordReturn;
}

void IQS7211A::Print_signed(int16_t i16Num)
{
	if(i16Num < (-99))
	{
		Serial.print(" ");
	}
	else if(i16Num < (-9))
	{
		Serial.print("  ");
	}
	else if(i16Num < 0)
	{
		Serial.print("   ");
	}
	else if(i16Num < 10)
	{
		Serial.print("    ");
	}
	else if(i16Num < 100)
	{
		Serial.print("   ");
	}
	else if(i16Num < 1000)
	{
		Serial.print("  ");
	}
	Serial.print(i16Num);
}

/**
 * @name	  touchpad_event_occurred
 * @brief   A method that returns the value of the TP_Movement bit in the 
 *          IQSMemoryMap.INFO_FLAGS local variable.
 * @param   None.
 * @retval  Returns a boolean of whether a touchpad event occurred.
 */
bool IQS7211A::touchpad_event_occurred(void)
{
  return getBit(IQSMemoryMap.INFO_FLAGS[1], IQS7211A_TP_MOVEMENT_BIT);
}

/**
 * @name	  get_gesture_event
 * @brief   A method that returns the specific gesture that occurred from 
 *          IQSMemoryMap.GESTURES 
 * @param   None.
 * @retval  Returns an iqs7211a_gestures_e value of which specific gesture event 
 *          occurred.
 */
iqs7211a_gestures_e IQS7211A::get_gesture_event(void)
{
  /* Find the gesture event that occurred */
  if(getBit(IQSMemoryMap.GESTURES[0], IQS7211A_GESTURE_SINGLE_TAP_BIT))
  {
    return IQS7211A_GESTURE_SINGLE_TAP;
  }
  else if(getBit(IQSMemoryMap.GESTURES[0], IQS7211A_GESTURE_PRESS_HOLD_BIT))
  {
    return IQS7211A_GESTURE_PRESS_HOLD;
  }
  else if(getBit(IQSMemoryMap.GESTURES[0], IQS7211A_GESTURE_SWIPE_X_POSITIVE_BIT))
  {
    return IQS7211A_GESTURE_SWIPE_X_POSITIVE;
  }
  else if(getBit(IQSMemoryMap.GESTURES[0], IQS7211A_GESTURE_SWIPE_X_NEGATIVE_BIT))
  {
    return IQS7211A_GESTURE_SWIPE_X_NEGATIVE;
  }
  else if(getBit(IQSMemoryMap.GESTURES[0], IQS7211A_GESTURE_SWIPE_Y_POSITIVE_BIT))
  {
    return IQS7211A_GESTURE_SWIPE_Y_POSITIVE;
  }
  else if(getBit(IQSMemoryMap.GESTURES[0], IQS7211A_GESTURE_SWIPE_Y_NEGATIVE_BIT))
  {
    return IQS7211A_GESTURE_SWIPE_Y_NEGATIVE;
  }
  else
  {
    return IQS7211A_GESTURE_NONE;
  }
}

/**
  * @name   getNumFingers
  * @brief  A method that returns the number of fingers active on the trackpad. 
  * @param  None.
  * @retval Returns an 8-bit unsigned integer value of the number of fingers 
  *         on the trackpad. 
  */
uint8_t IQS7211A::getNumFingers(void)
{
  uint8_t buffer = getBit(IQSMemoryMap.INFO_FLAGS[1], IQS7211A_NUM_FINGERS_BIT_0);
  buffer += getBit(IQSMemoryMap.INFO_FLAGS[1], IQS7211A_NUM_FINGERS_BIT_1);

  return buffer;
}

/**
  * @name   enableGestureEvent
  * @brief  A method to enable Gesture Events.
  * @param  stopOrRestart -> Specifies whether the communications window must be 
  *                          kept open or must be closed after this action.
  *                          Use the STOP and RESTART definitions.
  * @retval None. 
  * @note   All other bits at the IQS7211A_MM_CONFIG_SETTINGS register address 
  *         are preserved.
  */
void IQS7211A::enableGestureEvent(bool stopOrRestart)
{
  uint8_t transferBytes[2]; // The array which will hold the bytes which are transferred.

  // First read the bytes at the memory address so that they can be preserved.
  readRandomBytes(IQS7211A_MM_CONFIG_SETTINGS, 2, transferBytes, RESTART);
  // Set the IQS7211A_GESTURE_EVENT_BIT in CONFIG_SETTINGS
  transferBytes[1] = setBit(transferBytes[1], IQS7211A_GESTURE_EVENT_BIT);
  // Write the bytes back to the device
  writeRandomBytes(IQS7211A_MM_CONFIG_SETTINGS, 2, transferBytes, stopOrRestart);
}

/**
  * @name   disableGestureEvent
  * @brief  A method to disable Gesture Events. 
  * @param  stopOrRestart -> Specifies whether the communications window must be 
  *                          kept open or must be closed after this action.
  *                          Use the STOP and RESTART definitions.
  * @retval None. 
  * @note   All other bits at the IQS7211A_MM_CONFIG_SETTINGS register address 
  *         are preserved.
  */
void IQS7211A::disableGestureEvent(bool stopOrRestart)
{
  uint8_t transferBytes[2]; // The array which will hold the bytes which are transferred.

  // First read the bytes at the memory address so that they can be preserved.
  readRandomBytes(IQS7211A_MM_CONFIG_SETTINGS, 2, transferBytes, RESTART);
  // Clear the GESTURE_EVENT_BIT in CONFIG_SETTINGS
  transferBytes[1] = clearBit(transferBytes[1], IQS7211A_GESTURE_EVENT_BIT);
  // Write the bytes back to the device
  writeRandomBytes(IQS7211A_MM_CONFIG_SETTINGS, 2, transferBytes, stopOrRestart);
}

/**
  * @name   enableTPEvent
  * @brief  A method to enable TP Events. Trackpad finger movement or finger 
  *         up/down will trigger an event.
  * @param  stopOrRestart -> Specifies whether the communications window must 
  *                          be kept open or must be closed after this action.
  *                          Use the STOP and RESTART definitions.
  * @retval None. 
  * @note   All other bits at the IQS7211A_MM_CONFIG_SETTINGS register address 
  *         are preserved.
  */
void IQS7211A::enableTPEvent(bool stopOrRestart)
{
  uint8_t transferBytes[2]; // The array which will hold the bytes which are transferred.

  // First read the bytes at the memory address so that they can be preserved.
  readRandomBytes(IQS7211A_MM_CONFIG_SETTINGS, 2, transferBytes, RESTART);
  // Set the TP_EVENT_BIT in CONFIG_SETTINGS
  transferBytes[1] = setBit(transferBytes[1], IQS7211A_TP_EVENT_BIT);
  // Write the bytes back to the device
  writeRandomBytes(IQS7211A_MM_CONFIG_SETTINGS, 2, transferBytes, stopOrRestart);
}

/**
  * @name   disableTPEvent
  * @brief  A method to disable TP Events. Trackpad finger movement or finger 
  *         up/down will not trigger an event.
  * @param  stopOrRestart -> Specifies whether the communications window must 
  *                          be kept open or must be closed after this action.
  *                          Use the STOP and RESTART definitions.
  * @retval None. 
  * @note   All other bits at the IQS7211A_MM_CONFIG_SETTINGS register address 
  *         are preserved.
  */
void IQS7211A::disableTPEvent(bool stopOrRestart)
{
  uint8_t transferBytes[2]; // The array which will hold the bytes which are transferred.

  // First read the bytes at the memory address so that they can be preserved.
  readRandomBytes(IQS7211A_MM_CONFIG_SETTINGS, 2, transferBytes, RESTART);
  // Clear the TP_EVENT_BIT in CONFIG_SETTINGS
  transferBytes[1] = clearBit(transferBytes[1], IQS7211A_TP_EVENT_BIT);
  // Write the bytes back to the device
  writeRandomBytes(IQS7211A_MM_CONFIG_SETTINGS, 2, transferBytes, stopOrRestart);
}

/**
  * @name   enableCommsReqEn
  * @brief  A method to enable Comms request Enable. 
  * @param  stopOrRestart -> Specifies whether the communications window must be 
  *                          kept open or must be closed after this action.
  *                          Use the STOP and RESTART definitions.
  * @retval None. 
  * @note   All other bits at the IQS7211A_MM_CONFIG_SETTINGS register address 
  *         are preserved.
  *         Call this method once if you want to use the forceComms method.
  */
void IQS7211A::enableCommsReqEn(bool stopOrRestart)
{
  uint8_t transferBytes[2]; // The array which will hold the bytes which are transferred.

  // First read the bytes at the memory address so that they can be preserved.
  readRandomBytes(IQS7211A_MM_CONFIG_SETTINGS, 2, transferBytes, RESTART);
  // Set the COMMS_REQ_EN_BIT in CONFIG_SETTINGS
  transferBytes[0] = setBit(transferBytes[0], IQS7211A_COMMS_REQ_BIT);
  // Write the bytes back to the device
  writeRandomBytes(IQS7211A_MM_CONFIG_SETTINGS, 2, transferBytes, stopOrRestart);
}

/**
  * @name   disableCommsReqEn
  * @brief  A method to disable Comms request Enable. 
  * @param  stopOrRestart ->  Specifies whether the communications window must  
  *                           be kept open or must be closed after this action.
  *                           Use the STOP and RESTART definitions.
  * @retval None. 
  * @note   All other bits at the IQS7211A_MM_CONFIG_SETTINGS register address 
  *         are preserved. You can call this method once to be able to poll the 
  *         IQS device, otherwise you will need to enable Comms Req and use 
  *         forceComms. See the datasheet for more detail on this.
  */
void IQS7211A::disableCommsReqEn(bool stopOrRestart)
{
  uint8_t transferBytes[2]; // The array which will hold the bytes which are transferred.

  // First read the bytes at the memory address so that they can be preserved.
  readRandomBytes(IQS7211A_MM_CONFIG_SETTINGS, 2, transferBytes, RESTART);
  // Clear the TP_EVENT_BIT in CONFIG_SETTINGS
  transferBytes[0] = clearBit(transferBytes[0], IQS7211A_TP_EVENT_BIT);
  // Write the bytes back to the device
  writeRandomBytes(IQS7211A_MM_CONFIG_SETTINGS, 2, transferBytes, stopOrRestart);
}

/*****************************************************************************/
/*									     		ADVANCED PUBLIC METHODS							     		   	 */
/*****************************************************************************/

/**
  * @name   writeMM
  * @brief  Function to write the whole memory map to the device (writable) 
  *         registers 
  * @param  stopOrRestart ->  Specifies whether the communications window must  
  *                           be kept open or must be closed after this action.
  *                           Use the STOP and RESTART definitions.
  * @retval None.
  * @note   - Values are obtained from the init.h file exported from the GUI.
  */
void IQS7211A::writeMM(bool stopOrRestart)
{
	uint8_t transferBytes[30];	// Temporary array which holds the bytes to be transferred.
    
  /* Change the ATI Settings */
  /* Memory Map Position 0x30 - 0x3D */
  transferBytes[0] = TP_ATI_MULTIPLIERS_DIVIDERS_0;             
  transferBytes[1] = TP_ATI_MULTIPLIERS_DIVIDERS_1;             
  transferBytes[2] = TP_COMPENSATION_DIV_0;                     
  transferBytes[3] = TP_COMPENSATION_DIV_1;                     
  transferBytes[4] = TP_ATI_TARGET_0;                           
  transferBytes[5] = TP_ATI_TARGET_1;                           
  transferBytes[6] = TP_REF_DRIFT_LIMIT_0;                      
  transferBytes[7] = TP_REF_DRIFT_LIMIT_1;                      
  transferBytes[8] = TP_MIN_COUNT_REATI_0;                      
  transferBytes[9] = TP_MIN_COUNT_REATI_1;                      
  transferBytes[10] = REATI_RETRY_TIME_0;                       
  transferBytes[11] = REATI_RETRY_TIME_1;                       
  transferBytes[12] = ALP_ATI_MULTIPLIERS_DIVIDERS_0;           
  transferBytes[13] = ALP_ATI_MULTIPLIERS_DIVIDERS_1;           
  transferBytes[14] = ALP_COMPENSATION_DIV_0;                   
  transferBytes[15] = ALP_COMPENSATION_DIV_1;                   
  transferBytes[16] = ALP_ATI_TARGET_0;                         
  transferBytes[17] = ALP_ATI_TARGET_1;                         
  transferBytes[18] = ALP_LTA_DRIFT_LIMIT_0;                    
  transferBytes[19] = ALP_LTA_DRIFT_LIMIT_1;     
  //Serial.println("\t\t1. Write ATI Settings");
                 

  /* Change the ALP ATI Compensation */
  /* Memory Map Position 0x3A - 0x3D */
  transferBytes[20] = ALP_COMPENSATION_A_0;                     
  transferBytes[21] = ALP_COMPENSATION_A_1;                     
  transferBytes[22] = ALP_COMPENSATION_B_0;                     
  transferBytes[23] = ALP_COMPENSATION_B_1;                     
  writeRandomBytes(IQS7211A_MM_TP_ATI_MIR, 24, transferBytes, RESTART);
  //Serial.println("\t\t2. Write ALP Compensation Settings");

  /* Change the Report Rates and Timing */
  /* Memory Map Position 0x40 - 0x4A */
  transferBytes[0] = ACTIVE_MODE_REPORT_RATE_0;                
  transferBytes[1] = ACTIVE_MODE_REPORT_RATE_1;                
  transferBytes[2] = IDLE_TOUCH_MODE_REPORT_RATE_0;            
  transferBytes[3] = IDLE_TOUCH_MODE_REPORT_RATE_1;            
  transferBytes[4] = IDLE_MODE_REPORT_RATE_0;                  
  transferBytes[5] = IDLE_MODE_REPORT_RATE_1;                  
  transferBytes[6] = LP1_MODE_REPORT_RATE_0;                   
  transferBytes[7] = LP1_MODE_REPORT_RATE_1;                   
  transferBytes[8] = LP2_MODE_REPORT_RATE_0;                   
  transferBytes[9] = LP2_MODE_REPORT_RATE_1;                   
  transferBytes[10] = ACTIVE_MODE_TIMEOUT_0;                    
  transferBytes[11] = ACTIVE_MODE_TIMEOUT_1;                    
  transferBytes[12] = IDLE_TOUCH_MODE_TIMEOUT_0;                
  transferBytes[13] = IDLE_TOUCH_MODE_TIMEOUT_1;                
  transferBytes[14] = IDLE_MODE_TIMEOUT_0;                      
  transferBytes[15] = IDLE_MODE_TIMEOUT_1;                      
  transferBytes[16] = LP1_MODE_TIMEOUT_0;                       
  transferBytes[17] = LP1_MODE_TIMEOUT_1;                      
  transferBytes[18] = REF_UPDATE_TIME_0;                        
  transferBytes[19] = REF_UPDATE_TIME_1;                        
  transferBytes[20] = I2C_TIMEOUT_0;                            
  transferBytes[21] = I2C_TIMEOUT_1;                            
  writeRandomBytes(IQS7211A_MM_ACTIVE_MODE_RR, 22, transferBytes, RESTART);
    //Serial.println("\t\t3. Write Report rates and timings");

  /* Change the System Settings */
  /* Memory Map Position 0x50 - 0x5B */
  transferBytes[0] = SYSTEM_CONTROL_0;                         
  transferBytes[1] = SYSTEM_CONTROL_1;                         
  transferBytes[2] = CONFIG_SETTINGS0;                        
  transferBytes[3] = CONFIG_SETTINGS1;                         
  transferBytes[4] = OTHER_SETTINGS_0;                         
  transferBytes[5] = OTHER_SETTINGS_1;                         
  transferBytes[6] = TRACKPAD_TOUCH_SET_THRESHOLD;             
  transferBytes[7] = TRACKPAD_TOUCH_CLEAR_THRESHOLD;           
  transferBytes[8] = ALP_THRESHOLD_0;                          
  transferBytes[9] = ALP_THRESHOLD_1;                          
  transferBytes[10] = OPEN_0_0;                                 
  transferBytes[11] = OPEN_0_1;                                 
  transferBytes[12] = ALP_SET_DEBOUNCE;                         
  transferBytes[13] = ALP_CLEAR_DEBOUNCE;                       
  transferBytes[14] = OPEN_1_0;                                 
  transferBytes[15] = OPEN_1_1;                                 
  transferBytes[16] = TP_CONVERSION_FREQUENCY_UP_PASS_LENGTH;   
  transferBytes[17] = TP_CONVERSION_FREQUENCY_FRACTION_VALUE;   
  transferBytes[18] = ALP_CONVERSION_FREQUENCY_UP_PASS_LENGTH;  
  transferBytes[19] = ALP_CONVERSION_FREQUENCY_FRACTION_VALUE;  
  transferBytes[20] = TRACKPAD_HARDWARE_SETTINGS_0;             
  transferBytes[21] = TRACKPAD_HARDWARE_SETTINGS_1;             
  transferBytes[22] = ALP_HARDWARE_SETTINGS_0;                  
  transferBytes[23] = ALP_HARDWARE_SETTINGS_1;   
  writeRandomBytes(IQS7211A_MM_SYSTEM_CONTROL, 24, transferBytes, RESTART);    
    //Serial.println("\t\t4. Write System control settings");           

  /* Change the Trackpad Settings */
  /* Memory Map Position 0x60 - 0x69 */
  transferBytes[0] = TRACKPAD_SETTINGS_0_0;                    
  transferBytes[1] = TRACKPAD_SETTINGS_0_1;                    
  transferBytes[2] = TRACKPAD_SETTINGS_1_0;                    
  transferBytes[3] = TRACKPAD_SETTINGS_1_1;                    
  transferBytes[4] = X_RESOLUTION_0;                           
  transferBytes[5] = X_RESOLUTION_1;                           
  transferBytes[6] = Y_RESOLUTION_0;                           
  transferBytes[7] = Y_RESOLUTION_1;                           
  transferBytes[8] = XY_DYNAMIC_FILTER_BOTTOM_SPEED_0;         
  transferBytes[9] = XY_DYNAMIC_FILTER_BOTTOM_SPEED_1;         
  transferBytes[10] = XY_DYNAMIC_FILTER_TOP_SPEED_0;            
  transferBytes[11] = XY_DYNAMIC_FILTER_TOP_SPEED_1;            
  transferBytes[12] = XY_DYNAMIC_FILTER_BOTTOM_BETA;            
  transferBytes[13] = XY_DYNAMIC_FILTER_STATIC_FILTER_BETA;     
  transferBytes[14] = STATIONARY_TOUCH_MOV_THRESHOLD;           
  transferBytes[15] = FINGER_SPLIT_FACTOR;                      
  transferBytes[16] = X_TRIM_VALUE_0;                           
  transferBytes[17] = X_TRIM_VALUE_1;                           
  transferBytes[18] = Y_TRIM_VALUE_0;                           
  transferBytes[19] = Y_TRIM_VALUE_1;  
  writeRandomBytes(IQS7211A_MM_TP_SETTINGS_0, 20, transferBytes, RESTART); 
  //Serial.println("\t\t5. Write Hardware settings");                        

  /* Change the ALP Settings */
  /* Memory Map Position 0x70 - 0x74 */
  transferBytes[0] = ALP_COUNT_FILTER_BETA_0;                  
  transferBytes[1] = OPEN_0;                                   
  transferBytes[2] = ALP_LTA_BETA_LP1;                         
  transferBytes[3] = ALP_LTA_BETA_LP2;                         
  transferBytes[4] = ALP_SETUP_0;                              
  transferBytes[5] = ALP_SETUP_1;                              
  transferBytes[6] = ALP_TX_ENABLE_0;                          
  transferBytes[7] = ALP_TX_ENABLE_1;                          

  /* Change the Settings Version Numbers */
  /* Memory Map Position 0x74 - 0x75 */
  transferBytes[8] = MINOR_VERSION;                            
  transferBytes[9] = MAJOR_VERSION;
  writeRandomBytes(IQS7211A_MM_ALP_COUNT_FILTER_BETA, 10, transferBytes, RESTART);   
    //Serial.println("\t\t5. Write Filter Betas");                             

  /* Change the Gesture Settings */
  /* Memory Map Position 0x80 - 0x8F */
  transferBytes[0] = GESTURE_ENABLE_0;                         
  transferBytes[1] = GESTURE_ENABLE_1;                         
  transferBytes[2] = TAP_TIME_0;                               
  transferBytes[3] = TAP_TIME_1;                               
  transferBytes[4] = TAP_DISTANCE_0;                           
  transferBytes[5] = TAP_DISTANCE_1;                           
  transferBytes[6] = HOLD_TIME_0;                              
  transferBytes[7] = HOLD_TIME_1;                              
  transferBytes[8] = SWIPE_TIME_0;                             
  transferBytes[9] = SWIPE_TIME_1;                             
  transferBytes[10] = SWIPE_X_DISTANCE_0;                       
  transferBytes[11] = SWIPE_X_DISTANCE_1;                       
  transferBytes[12] = SWIPE_Y_DISTANCE_0;                       
  transferBytes[13] = SWIPE_Y_DISTANCE_1;                       
  transferBytes[14] = SWIPE_ANGLE_0;                            
  transferBytes[15] = GESTURE_OPEN_0;
  writeRandomBytes(IQS7211A_MM_GESTURE_ENABLE, 16, transferBytes, RESTART);   
    //Serial.println("\t\t6. Write Gesture Settings");                          

  /* Change the RxTx Mapping */
  /* Memory Map Position 0x90 - 0x9C */
  transferBytes[0] = RX_TX_MAP_0;                              
  transferBytes[1] = RX_TX_MAP_1;                              
  transferBytes[2] = RX_TX_MAP_2;                              
  transferBytes[3] = RX_TX_MAP_3;                              
  transferBytes[4] = RX_TX_MAP_4;                              
  transferBytes[5] = RX_TX_MAP_5;                              
  transferBytes[6] = RX_TX_MAP_6;                              
  transferBytes[7] = RX_TX_MAP_7;                              
  transferBytes[8] = RX_TX_MAP_8;                              
  transferBytes[9] = RX_TX_MAP_9;                              
  transferBytes[10] = RX_TX_MAP_10;                             
  transferBytes[11] = RX_TX_MAP_11;                             
  transferBytes[12] = RX_TX_MAP_12;
  writeRandomBytes(IQS7211A_MM_RXTX_MAPPING_1_0, 13, transferBytes, RESTART);   
  //Serial.println("\t\t7. Write TP Settings");                        

  /* Change the Allocation of channels into cycles 0-9 */
  /* Memory Map Position 0xA0 - 0xBD */
  transferBytes[0] = PLACEHOLDER_0;                            
  transferBytes[1] = CH_1_CYCLE_0;                             
  transferBytes[2] = CH_2_CYCLE_0;                             
  transferBytes[3] = PLACEHOLDER_1;                            
  transferBytes[4] = CH_1_CYCLE_1;                             
  transferBytes[5] = CH_2_CYCLE_1;                             
  transferBytes[6] = PLACEHOLDER_2;                            
  transferBytes[7] = CH_1_CYCLE_2;                             
  transferBytes[8] = CH_2_CYCLE_2;                             
  transferBytes[9] = PLACEHOLDER_3;                            
  transferBytes[10] = CH_1_CYCLE_3;                             
  transferBytes[11] = CH_2_CYCLE_3;                             
  transferBytes[12] = PLACEHOLDER_4;                            
  transferBytes[13] = CH_1_CYCLE_4;                             
  transferBytes[14] = CH_2_CYCLE_4;                             
  transferBytes[15] = PLACEHOLDER_5;                            
  transferBytes[16] = CH_1_CYCLE_5;                             
  transferBytes[17] = CH_2_CYCLE_5;                             
  transferBytes[18] = PLACEHOLDER_6;                            
  transferBytes[19] = CH_1_CYCLE_6;                             
  transferBytes[20] = CH_2_CYCLE_6;                             
  transferBytes[21] = PLACEHOLDER_7;                            
  transferBytes[22] = CH_1_CYCLE_7;                             
  transferBytes[23] = CH_2_CYCLE_7;                             
  transferBytes[24] = PLACEHOLDER_8;                            
  transferBytes[25] = CH_1_CYCLE_8;                             
  transferBytes[26] = CH_2_CYCLE_8;                             
  transferBytes[27] = PLACEHOLDER_9;                            
  transferBytes[28] = CH_1_CYCLE_9;                             
  transferBytes[29] = CH_2_CYCLE_9;
  writeRandomBytes(IQS7211A_MM_CYCLE_SETUP_0_9, 30, transferBytes, RESTART);  
    //Serial.println("\t\t8. Write Cycle 0 - 9 Settings");         

  /* Change the Allocation of channels into cycles 10-17 */
  /* Memory Map Position 0xB0 - 0xCA */
  transferBytes[0] = PLACEHOLDER_10;                           
  transferBytes[1] = CH_1_CYCLE_10;                            
  transferBytes[2] = CH_2_CYCLE_10;                            
  transferBytes[3] = PLACEHOLDER_11;                           
  transferBytes[4] = CH_1_CYCLE_11;                            
  transferBytes[5] = CH_2_CYCLE_11;                            
  transferBytes[6] = PLACEHOLDER_12;                           
  transferBytes[7] = CH_1_CYCLE_12;                            
  transferBytes[8] = CH_2_CYCLE_12;                            
  transferBytes[9] = PLACEHOLDER_13;                           
  transferBytes[10] = CH_1_CYCLE_13;                            
  transferBytes[11] = CH_2_CYCLE_13;                            
  transferBytes[12] = PLACEHOLDER_14;                           
  transferBytes[13] = CH_1_CYCLE_14;                            
  transferBytes[14] = CH_2_CYCLE_14;                            
  transferBytes[15] = PLACEHOLDER_15;                           
  transferBytes[16] = CH_1_CYCLE_15;                            
  transferBytes[17] = CH_2_CYCLE_15;                            
  transferBytes[18] = PLACEHOLDER_16;                           
  transferBytes[19] = CH_1_CYCLE_16;                            
  transferBytes[20] = CH_2_CYCLE_16;
  transferBytes[21] = PLACEHOLDER_17;
  transferBytes[22] = CH_1_CYCLE_17;
  transferBytes[23] = CH_2_CYCLE_17;
  writeRandomBytes(IQS7211A_MM_CYCLE_SETUP_10_17, 24, transferBytes, stopOrRestart);
  //Serial.println("\t\t9. Write Cycle 10 - 17 Settings");
}

/*****************************************************************************/
/*                            PRIVATE METHODS                                */
/*****************************************************************************/

/**
 * @name    readRandomBytes
 * @brief   A method that reads a specified number of bytes from a specified 
 *          address and saves it into a user-supplied array. This method is 
 *          used by all other methods in this class which read data from the 
 *          IQS7211A device.
 * @param   memoryAddress -> The memory address at which to start reading bytes 
 *                           from.  See the "iqs7211a_addresses.h" file.
 * @param   numBytes      -> The number of bytes that must be read.
 * @param   bytesArray    -> The array which will store the bytes to be read, 
 *                           this array will be overwritten.
 * @param   stopOrRestart -> A boolean that specifies whether the communication 
 *                           window should remain open or be closed after transfer.
 *                           False keeps it open, true closes it. Use the STOP 
 *                           and RESTART definitions. 
 * @retval  No value is returned, however, the user-supplied array is overwritten.
 * @note    Uses standard Arduino "Wire" library which is for I2C communication.
 *          Take note that C++ cannot return an array, therefore, the array which 
 *          is passed as an argument is overwritten with the required values.
 *          Pass an array to the method by using only its name, e.g. "bytesArray",
 *          without the brackets, this passes a pointer to the array.
 */
void IQS7211A::readRandomBytes(uint8_t memoryAddress, uint8_t numBytes, uint8_t bytesArray[], bool stopOrRestart)
{
  // A simple counter to assist with loading bytes into the user-supplied array.
  uint8_t i = 0;  
  
  // Select the device with the address of "_deviceAddress" and start communication.
  Wire.beginTransmission(_deviceAddress);
  // Send a bit asking for the "memoryAddress" register.
  Wire.write(memoryAddress);
  // Complete the selection and communication initialization.
  Wire.endTransmission(RESTART);  // Restart transmission for reading that follows.
  /* The required device has now been selected and it has been told which register 
  to send information from. */

  // Request "numBytes" bytes from the device which has the address "_deviceAddress"
  do
  {
    Wire.requestFrom((int)_deviceAddress, (int)numBytes, (int)stopOrRestart);
  }while(Wire.available() == 0); // Wait for response, this sometimes takes a few attempts

  // Load the received bytes into the array until there are no more 
  while(Wire.available())
  {
    // Load the received bytes into the user-supplied array
    bytesArray[i] = Wire.read();
    i++;
  }

  /* Always manually close the RDY window after a STOP is sent to prevent 
    writing while the RDY window closes */
  if(stopOrRestart == STOP)
  {
    iqs7211a_deviceRDY = false;
  }
}

/**
  * @name   writeRandomBytes
  * @brief  A method that writes a specified number of bytes to a specified 
  *         address, the bytes to write are supplied using an array pointer.
  *         This method is used by all other methods of this class which
  *         write I2C data to the IQS7211A device.
  * @param  memoryAddress -> The memory address at which to start writing the 
  *                          bytes to. See the "iqs7211a_addresses.h" file.
  * @param  numBytes      -> The number of bytes that must be written.
  * @param  bytesArray    -> The array which stores the bytes which will be 
  *                          written to the memory location.
  * @param  stopOrRestart -> A boolean that specifies whether the communication 
  *                          window should remain open or be closed of transfer.
  *                          False keeps it open, true closes it. Use the STOP 
  *                          and RESTART definitions.
  * @retval No value is returned, only the IQS device registers are altered.
  * @note   Uses standard Arduino "Wire" library which is for I2C communication.
  *         Take note that a full array cannot be passed to a function in C++.
  *         Pass an array to the function by using only its name, e.g. "bytesArray",
  *         without the square brackets, this passes a pointer to the 
  *         array. The values to be written must be loaded into the array prior 
  *         to passing it to the function.
  */
void IQS7211A::writeRandomBytes(uint8_t memoryAddress, uint8_t numBytes, uint8_t bytesArray[], bool stopOrRestart)
{
  /* Select the device with the address: "_deviceAddress" and start communication. */
  Wire.beginTransmission(_deviceAddress);
  /* Specify the memory address where the IQS7211A must start saving the data, 
    as designated by the "memoryAddress" variable. */
  Wire.write(memoryAddress);
  /* Write the bytes as specified in the array to which "arrayAddress" pointer points. */
  for (int i = 0; i < numBytes; i++)
  {
    Wire.write(bytesArray[i]);
  }
  /* End the transmission, user decides to STOP or RESTART. */
  Wire.endTransmission(stopOrRestart);

  /* Always manually close the RDY window after a STOP is sent to prevent writing 
     while ready closes */
  if(stopOrRestart == STOP)
  {
    iqs7211a_deviceRDY = false;
  }
}

/**
 * @name   writeRandomBytes16
 * @brief  A method that writes a specified number of bytes to a specified 
 *         address, the bytes to write are supplied using an array pointer.
 *         This method is used by all other methods of this class which 
 *         write data to the IQS7211A device.
 * @param  memoryAddress -> The memory address at which to start writing the 
 *                          bytes to. See the "iqs7211a_addresses.h" file.
 * @param  numBytes      -> The number of bytes that must be written.
 * @param  bytesArray    -> The array which stores the bytes which will be 
 *                          written to the memory location.
 * @param  stopOrRestart -> A boolean that specifies whether the communication 
 *                          window should remain open or be closed of transfer.
 *                          False keeps it open, true closes it. Use the STOP 
 *                          and RESTART definitions.
 * @retval No value is returned, only the IQS device registers are altered.
 * @note   Uses standard Arduino "Wire" library which is for I2C communication.
 *         Take note that a full array cannot be passed to a function in C++.
 *         Pass an array to the function by using only its name, e.g. "bytesArray", 
 *         without the square brackets, this passes a pointer to the 
 *         array. The values to be written must be loaded into the array prior 
 *         to passing it to the function.
 */
void IQS7211A::writeRandomBytes16(uint16_t memoryAddress, uint8_t numBytes, uint8_t bytesArray[], bool stopOrRestart)
{
  /* Select the device with the address: "_deviceAddress" and start communication. */
  Wire.beginTransmission(_deviceAddress);
  /* Specify the memory address where the IQS7211A must start saving the data, 
  as designated by the "memoryAddress" variable. */
  uint8_t addr_h, addr_l;
  addr_h = memoryAddress >> 8;
  addr_l = memoryAddress;
  Wire.write(addr_h);
  Wire.write(addr_l);
  /* Write the bytes as specified in the array which "arrayAddress" pointer 
  points to. */
  for (int i = 0; i < numBytes; i++)
  {
    Wire.write(bytesArray[i]);
  }
  /* End the transmission, and the user decides to STOP or RESTART. */
  Wire.endTransmission(stopOrRestart);

  /* Always manually close the RDY window after a STOP is sent to prevent 
  writing while the RDY window closes */
  if(stopOrRestart == STOP)
  {
    iqs7211a_deviceRDY = false;
  }
}

/**
  * @name   getBit
  * @brief  A method that returns the chosen bit value of the provided byte.
  * @param  data       -> byte of which a given bit value needs to be calculated.
  * @param  bit_number -> a number between 0 and 7 representing the bit in question.
  * @retval The boolean value of the specific bit requested. 
  */
bool IQS7211A::getBit(uint8_t data, uint8_t bit_number)
{
  return (data & ( 1 << bit_number )) >> bit_number;
}

/**
  * @name   setBit
  * @brief  A method that returns the chosen bit value of the provided byte.
  * @param  data       -> byte of which a given bit value needs to be calculated.
  * @param  bit_number -> a number between 0 and 7 representing the bit in question.
  * @retval Returns an 8-bit unsigned integer value of the given data byte with 
  *         the requested bit set.
  */
uint8_t IQS7211A::setBit(uint8_t data, uint8_t bit_number)
{
	return (data |= 1UL << bit_number);
}

/**
  * @name   clearBit
  * @brief  A method that returns the chosen bit value of the provided byte.
  * @param  data       -> byte of which a given bit value needs to be calculated.
  * @param  bit_number -> a number between 0 and 7 representing the bit in question.
  * @retval Returns an 8-bit unsigned integer value of the given data byte with 
  *         the requested bit cleared.
  */
uint8_t IQS7211A::clearBit(uint8_t data, uint8_t bit_number)
{
	return (data &= ~(1UL << bit_number));
}

/**
  * @name   force_I2C_communication
  * @brief  A method which writes data 0x00 to memory address 0xFF to open a 
  *         communication window on the IQS7211A.
  * @param  None.
  * @retval None.
  * @note   Uses standard Arduino "Wire" library which is for I2C communication.
  */
void IQS7211A::force_I2C_communication(void)
{
  /*Ensure RDY is HIGH at the moment*/
  if (!iqs7211a_deviceRDY)
  {
    /* Select the device with the address: "DEMO_IQS7211A_ADDR" and start 
    communication. */
    Wire.beginTransmission(_deviceAddress);

    /* Write to memory address 0xFF that will prompt the IQS7211A to open a 
    communication window.*/
    Wire.write(0xFF);
    Wire.write(0x00);

    /* End the transmission, user decides to STOP or RESTART. */
    Wire.endTransmission(STOP);
  }
}

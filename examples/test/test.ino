#include <Arduino.h>
#include <IQS7211A.h>
#include <Mouse.h>
#include <MouseTo.h>

//#define MOUSE_ENABLE


/*** Defines ***/
#define DEMO_IQS7211A_ADDR        0x56
#define DEMO_IQS7211A_POWER_PIN   2//49
#define DEMO_IQS7211A_RDY_PIN     A3//10

/*** Instances ***/
IQS7211A trackpad;

/*** Global Variables ***/
bool show_data = false;
iqs7211a_power_modes running_power_mode = IQS7211A_IDLE;
uint16_t running_x_output = 65535;
uint16_t running_y_output = 65535;
iqs7211a_gestures_e running_gestures = IQS7211A_GESTURE_NONE;

bool printData(void);
void printCoordinates(void);
void mouseCoordinates(void);
void init_iqs7211a(void);
void init_serial_comms(void);

void setup()
{
  #ifdef MOUSE_ENABLE
    Mouse.begin();
  #else
    init_serial_comms();
  #endif

  init_iqs7211a();
}

void loop()
{
  /* Read new data from IQS7211A if available  (RDY Line Low) */
  trackpad.run();

  /* If data was updated, display data read from IQS7211A */
  if(trackpad.new_data_available)
  {
    /* Print the following if the new data did not come from a force comms command */
    if (!show_data)
    {
      //if(printData())
      //{
        #ifdef MOUSE_ENABLE
          mouseCoordinates();
        #else
          printCoordinates();
        #endif
      //}
    }
    /* Set this flag to false to indicate that the new data was already displayed/used */
    trackpad.new_data_available = false;
  }
}

/* Function to determine if it is necessary to print all the relevant data 
in the serial terminal */
bool printData(void)
{
/* See if it is necessary to display the button state, power mode or gestures */
  //if((trackpad.getAbsXCoordinate(FINGER_1) != running_x_output)||(trackpad.getAbsYCoordinate(FINGER_1) != running_y_output))
  if(trackpad.touchpad_event_occurred())
  {
    return true; // Let the main loop know to display the latest values
  }
  else
  {
    return false; // Let the main loop know to not display the latest values
  }

  return false;
}

/* Function to return X and Y coordinates of finger 1 */
void printCoordinates(void)
{
  //uint16_t ui16TempX = trackpad.getAbsXCoordinate(FINGER_1);
  //uint16_t ui16TempY = trackpad.getAbsYCoordinate(FINGER_1);
  uint16_t ui16TempX = trackpad.getRelXCoordinate();
  uint16_t ui16TempY = trackpad.getRelYCoordinate();
  Serial.println("\nFinger 1 X:\tFinger 1 Y:");
  Serial.print(ui16TempX); // Print X coordinates
  Serial.print("\t\t");    // Spacing to match headings in Print
  Serial.print(ui16TempY); // Print Y coordinates
  Serial.print("\t\t");    // Spacing to match headings in Print
    //Serial.println("");
  
  running_x_output = ui16TempX;
  running_y_output = ui16TempY;

}

/* Control the Mouse with Trackpad */
void mouseCoordinates(void)
{
  uint16_t ui16TempX = trackpad.getAbsXCoordinate(FINGER_1);
  uint16_t ui16TempY = trackpad.getAbsYCoordinate(FINGER_1);
  if(ui16TempX != 65535 || ui16TempY != 65535)
  {
    int newTempX = map(ui16TempX, 0, 768, -500, 500);
    int newTempY = map(ui16TempY, 0, 1024, -500, 500);
    newTempX = constrain(newTempX, -500, 500);
    newTempY = constrain(newTempY, -500, 500);
    Mouse.move(newTempX, newTempY, 0);

    running_x_output = ui16TempX;
    running_y_output = ui16TempY;
  }
}

void init_iqs7211a(void)
{
    /* Power On IQS7211A */
  pinMode(DEMO_IQS7211A_POWER_PIN, OUTPUT);
  delay(200);
  digitalWrite(DEMO_IQS7211A_POWER_PIN, LOW);
  delay(200);
  digitalWrite(DEMO_IQS7211A_POWER_PIN, HIGH);

  #ifndef MOUSE_ENABLE
    Serial.println("IQS7211A MikroE Touchpad4 Selected!");
  #endif

  /* Initialize the IQS7211A */
  trackpad.begin(DEMO_IQS7211A_ADDR, DEMO_IQS7211A_RDY_PIN);

  #ifndef MOUSE_ENABLE
    Serial.println("IQS7211A Ready");
  #endif

  delay(1);
}

void init_serial_comms(void)
{
  /* Start Serial Communication */
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Start!");
}

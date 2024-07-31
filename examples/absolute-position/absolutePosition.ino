#include <Arduino.h>
#include <IQS7211A.h>


/*** Defines ***/
#define DEMO_IQS7211A_ADDR        0x56
#define DEMO_IQS7211A_POWER_PIN   2
#define DEMO_IQS7211A_RDY_PIN     A3

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
void init_iqs7211a(void);
void init_serial_comms(void);

void setup()
{
  init_serial_comms();
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
    if(!show_data)
    {
      if(printData())
      {
        printCoordinates();
      }
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
  uint16_t ui16TempX = trackpad.getAbsXCoordinate(FINGER_1);
  uint16_t ui16TempY = trackpad.getAbsYCoordinate(FINGER_1);
  Serial.println("\nFinger 1 X:\tFinger 1 Y:");
  Serial.print(ui16TempX); // Print X coordinates
  Serial.print("\t\t");    // Spacing to match headings in Print
  Serial.print(ui16TempY); // Print Y coordinates
  Serial.print("\t\t");    // Spacing to match headings in Print
  running_x_output = ui16TempX;
  running_y_output = ui16TempY;
}

void init_iqs7211a(void)
{
  pinMode(DEMO_IQS7211A_POWER_PIN, OUTPUT);
  delay(200);
  digitalWrite(DEMO_IQS7211A_POWER_PIN, LOW);
  delay(200);
  digitalWrite(DEMO_IQS7211A_POWER_PIN, HIGH);
  Serial.println("IQS7211A MikroE Touchpad4 Selected!");
  trackpad.begin(DEMO_IQS7211A_ADDR, DEMO_IQS7211A_RDY_PIN);
  Serial.println("IQS7211A Ready");
  delay(1);
}

void init_serial_comms(void)
{
  /* Start Serial Communication */
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Start!");
}

#include <Arduino.h>
#include <IQS7211A.h>

/*** Defines ***/
#define DEMO_IQS7211A_ADDR        0x56
#define DEMO_IQS7211A_POWER_PIN    2
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
  trackpad.run();

  if(trackpad.new_data_available)
  {
    if (!show_data)
    {
        if(printData())
        {
          printCoordinates();
        }
    }
    trackpad.new_data_available = false;
  }
}

/* Function to determine if it is necessary to print all the relevant data 
in the serial terminal */
bool printData(void)
{
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

/* Function to return relative X and Y coordinates of finger 1 */
void printCoordinates(void)
{
  uint16_t ui16TempX = trackpad.getRelXCoordinate();
  uint16_t ui16TempY = trackpad.getRelYCoordinate();
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
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Start!");
}

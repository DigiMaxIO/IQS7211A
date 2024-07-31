#include <Arduino.h>
#include <IQS7211A.h>

/*** Defines ***/
#define IQS7211A_ADDR     0x56
#define IQS_RST_PIN          2
#define IQS_RDY_PIN         A3

/*** Instances ***/
IQS7211A trackpad;

/*** Global Variables ***/
bool show_data = false;
iqs7211a_power_modes running_power_mode = IQS7211A_IDLE;
iqs7211a_gestures_e running_gestures = IQS7211A_GESTURE_NONE;

bool printData(void);
void printRelCoordinates(void);
void init_serial_comms(void);

void setup()
{
  init_serial_comms();
  trackpad.begin(IQS7211A_ADDR, IQS_RDY_PIN, IQS_RST_PIN);
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
          printRelCoordinates();
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
void printRelCoordinates(void)
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

void init_serial_comms(void)
{
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Start!");
}

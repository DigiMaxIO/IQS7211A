#include <Arduino.h>
#include <IQS7211A.h>
#include <Mouse.h>

//Serial monitor & mouse control cannot both run at the same time (if on the same port).
//To see serial data INSTEAD of controlling the mouse, comment out this line:
#define MOUSE_ENABLE

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

void printRelCoordinates(void);
void mouseControl(void);
void init_serial_comms(void);

void setup()
{
  #ifdef MOUSE_ENABLE
    Mouse.begin();
  #else
    init_serial_comms();
  #endif

  trackpad.begin(IQS7211A_ADDR, IQS_RDY_PIN, IQS_RST_PIN);
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
        #ifdef MOUSE_ENABLE
          mouseControl();
        #else
          printRelCoordinates();
        #endif
    }
    /* Set this flag to false to indicate that the new data was already displayed/used */
    trackpad.new_data_available = false;
  }
}


/* Function to return X and Y coordinates of finger 1 */
void printRelCoordinates(void)
{
  int16_t ui16TempX = trackpad.getRelXCoordinate();
  int16_t ui16TempY = trackpad.getRelYCoordinate();
  Serial.println("\nFinger 1 X:\tFinger 1 Y:");
  trackpad.Print_signed(ui16TempX);
  Serial.print("\t\t"); 
  trackpad.Print_signed(ui16TempY);
  Serial.print("\t\t");
  Serial.println();
}

/* Control the Mouse with Trackpad */
void mouseControl(void)
{
  int16_t ui16TempX = trackpad.getRelXCoordinate();
  int16_t ui16TempY = trackpad.getRelYCoordinate();
  Mouse.move(ui16TempX, ui16TempY, 0);
}

void init_serial_comms(void)
{
  /* Start Serial Communication */
  Serial.begin(115200);
  while(!Serial);
  Serial.println("Start!");
}

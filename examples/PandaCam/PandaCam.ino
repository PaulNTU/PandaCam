#include "PandaCam.h"

PandaCam camera;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.println("Initialising Panda Camera");
  if(camera.Init())
  {
    Serial.print("Camera Initialised Ok.\r\n Firmware Version: ");
    Serial.println(camera.GetCameraFirmwareVersion());

    
    camera.PanCamera(PAN_LEFT);
  }
  else
  {
    Serial.println("Camera Failed to initialise. Check wires and power");
  }
}

void loop() {

  // Used for serial input and allowing control from the serial window.
  CheckForIncommingSerial();

  // Update the camera regularly to ensure it checks its position and updates (e.g starts/stops motors)
  if(camera.Update())
  {
    // If it returns true then we are connected and we have the latest status of the camera.
    // camera.GetPanPosition(): Returns the cameras current pan angle
    // camera.GetTiltPosition(): Returns the cameras current tilt angle
    // camera.GetCameraPositionStatus(): returns whether the camera is idle (Unknown position), Moving, or has arrived at you desired location (including home)
    // Show some debug data. You can write code to react to these difference states if you need too.

    // Show status update
    Serial.print("Current Camera Status: ");
    switch(camera.GetCameraPositionStatus())
    {
      case PANDACAM_STATUS_IDLE:
        Serial.print("Idle \t");
        break;

      case PANDACAM_STATUS_MOVING:
        Serial.print("Moving \t");
        break;

      case PANDACAM_STATUS_ARRIVED:
        Serial.print("Arrived At Destination \t");
        break;
    }

    Serial.print ("Pan: ");
    Serial.print(camera.GetPanPosition());
    Serial.print("-->");
    Serial.print(camera.GetPanDestinationAngle());
    Serial.print ("\t Tilt: ");
    Serial.print(camera.GetTiltPosition());
    Serial.print("-->");
    Serial.println(camera.GetTiltDestinationAngle());
  }

  delay(100);
}

/* This function allows for controlling the camera through the Serial monitor. (Students: You won't need this as you will be using remotes)
// a: turn left
// d: turn right
// w: tilt upwards
// s: tilt dowmwards
// spacebar: stop
// p (angle): Pan to this angle position
// t (angle): Tilt to this angle position
*/
void CheckForIncommingSerial()
{
  if (Serial.available())
  {
      uint8_t val = Serial.read();

      int count = Serial.available();
      int angle = 0;

      if(count > 0)
      {
        uint8_t vals[count];


        for(int x = 0; x < count; x++)
        {
          vals[x] = Serial.read();
        }

        angle = ExtractNumber(vals);
      }

      switch (val)
      {
        case 'a':
            camera.PanCamera(PAN_LEFT);
          break;

        case 'd':
          camera.PanCamera(PAN_RIGHT);
          break;

        case 'w':
          camera.TiltCamera(TILT_UP);
          break;

        case 's':
          camera.TiltCamera(TILT_DOWN);
          break;

        case 32: // Spacebar
          camera.ForceStop();
          break;

        case 'h':
          camera.GoHome();
          break;

        case 'q':
          camera.PanCamera(PAN_STOP);
          break;

        case 'p':
          if(count > 0)
          {
            camera.PanToAngle(angle);
          }
          break;
        case 't':
          if(count > 0)
          {
            camera.TiltToAngle(angle);
          }
          break;
      }
  }
}

/* Helper function for extracting a number from the serial imput  */
long ExtractNumber(const uint8_t* input) {
  while (*input == ' ') input++;  // Skip leading spaces

  char buffer[5];  // 10 digits + null terminator
  int i = 0;

  while (isdigit(*input) && i < 4) {
    buffer[i++] = *input++;
  }
  buffer[i] = '\0';

  return atol(buffer);  // Convert to long
}
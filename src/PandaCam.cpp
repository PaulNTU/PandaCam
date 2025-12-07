#include "PandaCam.h"
#include <Wire.h>

void ReceivedI2CEvent(int howMany)
{
  // ToDo
}

bool PandaCam::Init()
{
  // Initialise and get the firmware to check we are connected
  Wire.begin();
  Wire.onReceive(ReceivedI2CEvent);

  Serial.println("Sending Data");
  // Request firmware version to verify connection
  uint8_t command[2] = {STATUS_COMMAND, STATUS_FIRMWARE};

  // Send the comand
  SendCommand(command, sizeof(command));

  // Wait
  delayMicroseconds(100);

  // Request the response
  uint8_t response[5];
  int size = this->GetResponse(response, 5);

  Serial.print("Received Data");
  Serial.println(size);

  // Is it the correct response?
  if(size > 4)
  {
    if(response[0] == STATUS_COMMAND && response[1] == STATUS_FIRMWARE)
    {
      // What version
      this->connectedCameraVersion[0] = response[2];
      this->connectedCameraVersion[1] = response[3];
      this->connectedCameraVersion[2] = response[4];

      return true;
    }
  }

  // Not a valid response from the camera.
  return false;
}

bool PandaCam::Update()
{
  // Get the latest status from the Camera
  uint8_t request[2] = { STATUS_COMMAND, STATUS_POSITION};
  this->SendCommand(request, 2);
  delayMicroseconds(100);

  uint8_t response[12];

  int size = this->GetResponse(response, 12);

  if(size >11)
  {
    // Make sure we are in sync
    if(response[0] == STATUS_COMMAND && response[1] == STATUS_POSITION)
    {
      // Check the status of each motor and create a simpler version
      this->panMotorStatus = response[2];
      this->tiltMotorStatus = response[3];

      // Get the positions
      this->panPosition = (response[4] << 8) | response[5];
      this->tiltPosition = (response[6] << 8) | response[7];

      // Get the destinations of each axis 
      this->panMotorDestinationAngle = (response[8] << 8) | response[9];
      this->tiltMotorDestinationAngle = (response[10] << 8) | response[11];

      return true;
    }
  }

  // Failed to get status
  return false;
}


int PandaCam::GetPanPosition()
{
  return this->panPosition;
}
  
int PandaCam::GetTiltPosition()
{
  return this->tiltPosition;
}


int PandaCam::GetPanDestinationAngle()
{
  return this->panMotorDestinationAngle;
}
  
int PandaCam::GetTiltDestinationAngle()
{
  return this->tiltMotorDestinationAngle;
}

int PandaCam::GetCameraPositionStatus()
{
   // Either moving?
  if(panMotorStatus == CAMERA_STATUS_MOVING || tiltMotorStatus == CAMERA_STATUS_MOVING)
  {
    return PANDACAM_STATUS_MOVING;
  }

  // Both at home or in arrived state?
  if ((panMotorStatus == CAMERA_STATUS_HOME || panMotorStatus == CAMERA_STATUS_ARRIVED) && (tiltMotorStatus == CAMERA_STATUS_HOME || tiltMotorStatus == CAMERA_STATUS_ARRIVED))
  {
    // Arrived at destination.
    return PANDACAM_STATUS_ARRIVED;
  }

  // Not moving but also not at any specified destination.
  return PANDACAM_STATUS_IDLE;
}

String PandaCam::GetCameraFirmwareVersion()
{
  return String(this->connectedCameraVersion[0]) + String(":") + String(this->connectedCameraVersion[1]) + String(":") + String(this->connectedCameraVersion[2]);
}

void PandaCam::PanCamera(int direction)
{
  // Check we have valid inputs
  if(direction == PAN_LEFT || direction == PAN_RIGHT || direction == PAN_STOP)
  {
    // Send I2C Command
    uint8_t command[2] = {PAN_COMMAND, (uint8_t)direction};
    SendCommand(command, sizeof(command));
  }
}

void PandaCam::TiltCamera(int direction)
{
  // Check we have valid inputs
  if(direction == TILT_UP || direction == TILT_DOWN || direction == PAN_STOP)
  {
    // Send I2C Command
    uint8_t command[2] = {TILT_COMMAND, (uint8_t)direction};
    SendCommand(command, sizeof(command));
  }
}

void PandaCam::PanToAngle(int angle)
{
  // Send I2C Command
  uint8_t command[4] = {PAN_COMMAND, PAN_TO, 0, 0};
  
  // Convert int to 2 bytes for transmission
  command[2] = (angle >> 8) & 0xFF;
  command[3] = angle & 0xFF;
  
  SendCommand(command, sizeof(command));
}

void PandaCam::TiltToAngle(int angle)
{
  // Send I2C Command
  uint8_t command[4] = {TILT_COMMAND, TILT_TO, 0, 0};
  
  // Convert int to 2 bytes for transmission
  command[2] = (angle >> 8) & 0xFF;
  command[3] = angle & 0xFF;
  
  SendCommand(command, sizeof(command));
}

void PandaCam::GoHome()
{
  // Send I2C Command
  uint8_t command[1] = {HOME_COMMAND};
  
  SendCommand(command, sizeof(command));
}

void PandaCam::ForceStop()
{
  uint8_t command[1] = {STOP_COMMAND};
  SendCommand(command, sizeof(command));
}

void PandaCam::SendCommand(uint8_t *command, int length)
{
  Wire.beginTransmission(PANDACAM_I2C_SLAVE_ADDRESS);
  Wire.write(command, length);
  Wire.endTransmission();
}

int PandaCam::GetResponse(uint8_t *data, int len)
{
  // Request the response
  Wire.requestFrom(PANDACAM_I2C_SLAVE_ADDRESS, len);
  int readBytes = Wire.available();
  
  if(readBytes > 0)
  {
    for(int x = 0; x < readBytes; x++)
    {
      data[x] = Wire.read();
    }
  }

  return readBytes;
}
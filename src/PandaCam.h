#include <Arduino.h>

#ifndef __PANDACAM_H__
#define __PANDACAM_H__

#define PANDACAM_I2C_SLAVE_ADDRESS 0x18

#define STATUS_COMMAND 0x00
#define STOP_COMMAND 0x01
#define PAN_COMMAND 0x02
#define TILT_COMMAND 0x03
#define HOME_COMMAND 0x04

#define PAN_LEFT 0x00
#define PAN_RIGHT 0x01
#define PAN_TO 0x02
#define PAN_STOP 0x03

#define TILT_UP 0x00
#define TILT_DOWN 0x01
#define TILT_TO 0x02
#define TILT_STOP 0x03

#define STATUS_FIRMWARE 0x00
#define STATUS_POSITION 0x01 

#define PANDACAM_STATUS_IDLE 0x00
#define PANDACAM_STATUS_MOVING 0x01
#define PANDACAM_STATUS_ARRIVED 0x02

#define CAMERA_STATUS_IDLE 0x00
#define CAMERA_STATUS_ARRIVED 0x01
#define CAMERA_STATUS_HOME 0x02
#define CAMERA_STATUS_MOVING 0x03
#define CAMERA_STATUS_HEADINGHOME 0x04
#define CAMERA_STATUS_ABORTED 0x05

class PandaCam
{
  private:
    uint8_t connectedCameraVersion[3] { 0, 0, 0};
    void SendCommand(uint8_t *command, int length);
    int GetResponse(uint8_t *data, int len);

    // Storage for received status from the camera
    int panPosition = -1;
    int tiltPosition = -1;
    int panMotorStatus = CAMERA_STATUS_IDLE;
    int tiltMotorStatus = CAMERA_STATUS_IDLE;
    
    int panMotorDestinationAngle = -1;
    int tiltMotorDestinationAngle = -1;
    
  public:
    bool Init();

    bool Update();

    void PanCamera(int direction);
    void TiltCamera(int direction);
    void PanToAngle(int angle);
    void TiltToAngle(int angle);
    void GoHome();
    void ForceStop();
    
    String GetCameraFirmwareVersion();

    int GetPanPosition();
    int GetTiltPosition();
    int GetPanDestinationAngle();
    int GetTiltDestinationAngle();

    int GetCameraPositionStatus();
};

#endif
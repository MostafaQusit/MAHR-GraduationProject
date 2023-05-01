#ifndef MAHR_MOTORS_H_
#define MAHR_MOTORS_H_

#include <MAHR.h>
#include <ESP32Encoder.h>
#include "esp_task_wdt.h"
#include <CytronMotorDriver.h>

#define ENCODER_PPR   360.0   // Encoder Resolution in pulse/rev.
#define LINEAR    0
#define ROTATINAL 1
#define DIAGONAL  2

static IRAM_ATTR void RightEncoder_cb(void* arg) {
  ESP32Encoder* enc = (ESP32Encoder*) arg;
}
static IRAM_ATTR void LeftEncoder_cb(void* arg) {
  ESP32Encoder* enc = (ESP32Encoder*) arg;
}

extern bool loopTaskWDTEnabled;
extern TaskHandle_t loopTaskHandle;
ESP32Encoder RightEncoder(true, RightEncoder_cb);             // ESP32Encoder object for Right Encoder.
ESP32Encoder LeftEncoder (true, LeftEncoder_cb );             // ESP32Encoder object for Left Encoder.
int64_t RightEncoder_CurrDistance, LeftEncoder_CurrDistance;      // Average Previous travelled distance of Encoders in deg.
double_t RightEncoder_OffsetDistance, LeftEncoder_OffsetDistance;  // Offest distance of Encoders in deg.
int8_t PrevState, CurrState = LINEAR;
static const char *LOG_TAG = "main";

/* Filter:
template <int order> // order is 1 or 2
class LowPass {
  private:
    float a[order];
    float b[order+1];
    float omega0;
    float dt;
    bool adapt;
    float tn1 = 0;
    float x[order+1]; // Raw values
    float y[order+1]; // Filtered values

  public:  
    LowPass(float f0, float fs, bool adaptive){
      // f0: cutoff frequency (Hz)
      // fs: sample frequency (Hz)
      // adaptive: boolean flag, if set to 1, the code will automatically set
      // the sample frequency based on the time history.
      
      omega0 = 6.28318530718*f0;
      dt = 1.0/fs;
      adapt = adaptive;
      tn1 = -dt;
      for(int k = 0; k < order+1; k++){
        x[k] = 0;
        y[k] = 0;        
      }
      setCoef();
    }

    void setCoef(){
      if(adapt){
        float t = micros()/1.0e6;
        dt = t - tn1;
        tn1 = t;
      }
      
      float alpha = omega0*dt;
      if(order==1){
        a[0] = -(alpha - 2.0)/(alpha+2.0);
        b[0] = alpha/(alpha+2.0);
        b[1] = alpha/(alpha+2.0);        
      }
      if(order==2){
        float alphaSq = alpha*alpha;
        float beta[] = {1, sqrt(2), 1};
        float D = alphaSq*beta[0] + 2*alpha*beta[1] + 4*beta[2];
        b[0] = alphaSq/D;
        b[1] = 2*b[0];
        b[2] = b[0];
        a[0] = -(2*alphaSq*beta[0] - 8*beta[2])/D;
        a[1] = -(beta[0]*alphaSq - 2*beta[1]*alpha + 4*beta[2])/D;      
      }
    }

    float filt(float xn){
      // Provide me with the current raw value: x
      // I will give you the current filtered value: y
      if(adapt){
        setCoef(); // Update coefficients if necessary      
      }
      y[0] = 0;
      x[0] = xn;
      // Compute the filtered values
      for(int k = 0; k < order; k++){
        y[0] += a[k]*y[k+1] + b[k]*x[k];
      }
      y[0] += b[order]*x[order];

      // Save the historical values
      for(int k = order; k > 0; k--){
        y[k] = y[k-1];
        x[k] = x[k-1];
      }
  
      // Return the filtered value    
      return y[0];
    }
};
LowPass<2> lowpass_l(6e3, 100e3, true);
LowPass<2> lowpass_r(6e3, 100e3, true);
*/

CytronMD motorR(PWM_DIR, MOTOR_RIGHT_PWM, MOTOR_RIGHT_DIR);   // Driver object for Right Motor
CytronMD motorL(PWM_DIR, MOTOR_LEFT_PWM , MOTOR_LEFT_DIR );   // Driver object for Left Motor
float_t LeftMotor_mmss, RightMotor_mmss;
float_t LeftMotor_mms, RightMotor_mms;

// Motors Initialization
void Motors_Setup() {
  // Motors: ...

  // Encoders:
  loopTaskWDTEnabled = true;

  RightEncoder.attachSingleEdge(ENCODER_RIGHT_A, ENCODER_RIGHT_B);
  LeftEncoder.attachSingleEdge(ENCODER_LEFT_A, ENCODER_LEFT_B);

  RightEncoder.clearCount();
  LeftEncoder.clearCount();

  RightEncoder.setFilter(1023);
  LeftEncoder.setFilter(1023);

  esp_log_level_set("*", ESP_LOG_DEBUG);
  esp_log_level_set("main", ESP_LOG_DEBUG);
  esp_log_level_set("ESP32Encoder", ESP_LOG_DEBUG);
  esp_task_wdt_add(loopTaskHandle);
}
// Speed Control Diff. Robot by Differential PID Controller
void Motors_RunSpeed() {
  // Check z-Axis State:
  if(1) {  //digitalRead(LOWER_LS)==HIGH
    // Acceleration & Speed Profile (like exp.):
    LeftMotor_mmss  = (Target_LeftMotor_mms -LeftMotor_mms )/50.0;
    RightMotor_mmss = (Target_RightMotor_mms-RightMotor_mms)/50.0;
    LeftMotor_mms  += LeftMotor_mmss;
    RightMotor_mms += RightMotor_mmss;
  }
  else {
    LeftMotor_mms  = 0;
    RightMotor_mms = 0;
  }

  // Position Calcu. :
  noInterrupts();
  RightEncoder_Distance = -RightEncoder.getCount();
  LeftEncoder_Distance  = -LeftEncoder.getCount();
  interrupts();

  // Define State:
  int8_t sign;
  if      (RightMotor_mms ==    LeftMotor_mms) { CurrState=LINEAR;    sign= 1; }
  else if (RightMotor_mms == -1*LeftMotor_mms) { CurrState=ROTATINAL; sign=-1; }
  else                                         { CurrState=DIAGONAL;  sign= 1; }

  // Encoder Resetting:
  if( CurrState != PrevState) {
    RightEncoder_OffsetDistance = RightEncoder_Distance;
    LeftEncoder_OffsetDistance  = LeftEncoder_Distance;
  }
  RightEncoder_CurrDistance = RightEncoder_Distance - RightEncoder_OffsetDistance;
  LeftEncoder_CurrDistance  = LeftEncoder_Distance  - LeftEncoder_OffsetDistance;
  
  // In 4 crossing-direction case: [in progressing]
  int16_t side_offset = LeftMotor_mms - RightMotor_mms;

  // calcu. error difference:
  int64_t diff;
  if(CurrState == ROTATINAL) { diff = 5.0*(RightEncoder_CurrDistance + LeftEncoder_CurrDistance); }
  else                       { diff = 5.0*(RightEncoder_CurrDistance - LeftEncoder_CurrDistance + side_offset); }

  // convert motor-speed from mm/s to RPM:
  float_t RightMotor_RPM = (RightMotor_mms - 0.5*diff     ) * 60.0/(WHEEL_RADIUS_MM* 2*PI);
  float_t LeftMotor_RPM  = (LeftMotor_mms  + 0.5*diff*sign) * 60.0/(WHEEL_RADIUS_MM* 2*PI);

  // convert motor-speed from RPM to Volt:
  float_t RightMotor_Volt = RightMotor_RPM * 255.0/69.0;
  float_t LeftMotor_Volt  = LeftMotor_RPM  * 255.0/69.0;

  motorR.setSpeed((int16_t) round(RightMotor_Volt));
  motorL.setSpeed((int16_t) round(LeftMotor_Volt));

  PrevState = CurrState;
}
// Print the Encoder Position and Speed
void Motors_PrintData() {
  Serial.printf("Motors: Speed(%4.0f,%4.0f)\tEncoders: Position(%8lld,%8lld)deg\n",
                LeftMotor_mms,
                RightMotor_mms,
                LeftEncoder_CurrDistance,
                RightEncoder_CurrDistance);
}

#endif
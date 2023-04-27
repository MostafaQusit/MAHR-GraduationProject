#ifndef MAHR_MOTORS_H_
#define MAHR_MOTORS_H_

#include <ESP32Encoder.h>
#include "esp_task_wdt.h"
#include <MAHR.h>
#include <CytronMotorDriver.h>
//#include <PID_v1.h>

#define ENCODER_PPR         360.0   // Encoder Resolution in pulse/rev.
#define MOTOR_MAXSPEED_RPM   60.0   // Motor max. speed in rpm

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
int64_t RightEncoder_PrevDistance, LeftEncoder_PrevDistance;  // Previous travelled distance of Encoders in deg.
float_t RightEncoder_degs, LeftEncoder_degs;                  // Enocoders Speed in deg/s.
float_t RightEncoder_mms,  LeftEncoder_mms;                   // Encoders::Speed in  mm/s
uint32_t Enc_CurrMicros, Enc_PrevMicros;
static const char *LOG_TAG = "main";

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
LowPass<2> lowpass_l(60e3, 600e3, true);
LowPass<2> lowpass_r(60e3, 600e3, true);

CytronMD motorR(PWM_DIR, MOTOR_RIGHT_PWM, MOTOR_RIGHT_DIR);   // Driver object for Right Motor
CytronMD motorL(PWM_DIR, MOTOR_LEFT_PWM , MOTOR_LEFT_DIR );   // Driver object for Left Motor

float_t PrevError_l, error_l, P_l, I_l, D_l, PID_l;
float_t PrevError_r, error_r, P_r, I_r, D_r, PID_r;
uint32_t current_time, previous_time;
float_t dt;

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
// Speed Control Diff. Robot by PID Controller
void Motors_RunSpeed(int16_t LeftMotor_mms, int16_t RightMotor_mms) {
  //if ( LeftMotor_mms  > 0) { LeftMotor_mms  /= 1.25; }
  // Position Calcu. :
  noInterrupts();
  RightEncoder_Distance = -RightEncoder.getCount();
  LeftEncoder_Distance  = -LeftEncoder.getCount();
  interrupts();

  current_time = micros();
  // Speed Calcu. :
  dt = ((float_t)current_time-previous_time)/1.0e6;
  RightEncoder_degs = ((float_t) RightEncoder_Distance - RightEncoder_PrevDistance)/dt;
  LeftEncoder_degs  = ((float_t) LeftEncoder_Distance  -  LeftEncoder_PrevDistance)/dt;
  // Update the variables for 
  RightEncoder_PrevDistance = RightEncoder_Distance;
  LeftEncoder_PrevDistance = LeftEncoder_Distance;
  previous_time = current_time;

  // convert speed from deg/s to mm/s
  RightEncoder_mms = RightEncoder_degs * (PI/180.0)*WHEEL_RADIUS_MM;
  LeftEncoder_mms  = LeftEncoder_degs  * (PI/180.0)*WHEEL_RADIUS_MM;

  // filtering:
  //RightEncoder_mms = lowpass_r.filt(RightEncoder_mms);
  //LeftEncoder_mms = lowpass_r.filt(LeftEncoder_mms);

  // PID Stage:
  error_r = RightMotor_mms - RightEncoder_mms;
  P_r  = error_r;
  I_r += (error_r+PrevError_r)/2.0 * dt;
  D_r  = (error_r-PrevError_r)/dt;
  PID_r = 1.00*P_r + 0.00*I_r + 0.00*D_r;

  error_l = LeftMotor_mms - LeftEncoder_mms;
  P_l  = error_l;
  I_l += (error_l+PrevError_l)/2.0 * dt;
  D_l  = (error_l-PrevError_l)/dt;
  PID_l = 1.00*P_l + 0.00*I_l + 0.00*D_l;

  // convert motor-speed from mm/s to RPM:
  float_t RightMotor_RPM = (RightMotor_mms + error_r) * 60.0/(WHEEL_RADIUS_MM* 2*PI);
  float_t LeftMotor_RPM  = (LeftMotor_mms  + error_l) * 60.0/(WHEEL_RADIUS_MM* 2*PI);

  // convert motor-speed from RPM to Volt:
  float_t RightMotor_Volt = RightMotor_RPM * 255.0/60.0;
  float_t LeftMotor_Volt  = LeftMotor_RPM  * 255.0/60.0;

  motorR.setSpeed((int16_t) fabs(RightMotor_Volt));
  motorL.setSpeed((int16_t) fabs(LeftMotor_Volt));
}
// Print the Encoder Position and Speed
void Encoders_PrintData() {
  Serial.printf("Encoders: Position(%6lld,%6lld)deg\tSpeed(%6ld,%6ld)mm/s\n",
                LeftEncoder_Distance,
                RightEncoder_Distance, 
                LeftEncoder_mms, 
                RightEncoder_mms );
  
}

#endif
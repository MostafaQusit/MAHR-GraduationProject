#ifndef MAHR_IMU_H_
#define MAHR_IMU_H_

#include <MAHR.h>
#include "MPU9250.h"

MPU9250 IMU; // an MPU9250 object with the MPU-9250 sensor

// IMU Initialization
void IMU_Setup() {
  Serial.print(F("IMU Initialization..."));
  Wire.begin();
  delay(2000);

  MPU9250Setting setting;
  setting.accel_fs_sel = ACCEL_FS_SEL::A16G;
  setting.gyro_fs_sel = GYRO_FS_SEL::G2000DPS;
  setting.mag_output_bits = MAG_OUTPUT_BITS::M16BITS;
  setting.fifo_sample_rate = FIFO_SAMPLE_RATE::SMPL_200HZ;
  setting.gyro_fchoice = 0x03;
  setting.gyro_dlpf_cfg = GYRO_DLPF_CFG::DLPF_41HZ;
  setting.accel_fchoice = 0x01;
  setting.accel_dlpf_cfg = ACCEL_DLPF_CFG::DLPF_45HZ;

  if (!IMU.setup(0x68)) {  // change to your own address
      while (1) {
        Serial.println(F("IMU connection failed. Please check your connection with `connection_check` example."));
        delay(5000);
      }
  }
  IMU.selectFilter(QuatFilterSel::MAHONY);
  Serial.println(F("Done."));
}
// Update the IMU Reading
void IMU_DataUpdate() {
  IMU.update();

  accelerometer[0] = IMU.getAccX();
  accelerometer[1] = IMU.getAccY();
  accelerometer[2] = IMU.getAccZ();

  gyroscope[0] = IMU.getGyroX();
  gyroscope[1] = IMU.getGyroY();
  gyroscope[2] = IMU.getGyroZ();

  magnometer[0] = IMU.getMagX();
  magnometer[1] = IMU.getMagY();
  magnometer[2] = IMU.getMagZ();

  quaternion[0] = IMU.getQuaternionX();
  quaternion[1] = IMU.getQuaternionY();
  quaternion[2] = IMU.getQuaternionZ();
  quaternion[3] = IMU.getQuaternionW();

  Yaw_angle   = IMU.getYaw();
  Pitch_angle = IMU.getPitch();
  Roll_angle  = IMU.getRoll();
}
// Print the Calibration Values
void IMU_PrintCalibration() {
  Serial.printf("Calibration parameters:\n\
                 \taccel bias: (%0.3f,%0.3f,%0.3f)\n\
                 \tgyro bias:  (%0.3f,%0.3f,%0.3f)\n\
                 \tmag bias:   (%0.3f,%0.3f,%0.3f)\n\
                 \tmag scale:  (%0.3f,%0.3f,%0.3f)\n",
                IMU.getAccBiasX(),
                IMU.getAccBiasY(),
                IMU.getAccBiasY(),

                IMU.getGyroBiasX(),
                IMU.getGyroBiasY(),
                IMU.getGyroBiasZ(),

                IMU.getMagBiasX(),
                IMU.getMagBiasY(),
                IMU.getMagBiasZ(),

                IMU.getMagScaleX(),
                IMU.getMagScaleY(),
                IMU.getMagScaleZ() );
}
// Claibrate the IMU Reading
void IMU_Calibrate() {
  Serial.println(F("Accel Gyro calibration will start in 5sec."));
  Serial.println(F("Please leave the device still on the flat plane."));
  IMU.verbose(true);
  delay(5000);
  IMU.calibrateAccelGyro();

  Serial.println(F("Mag calibration will start in 5sec."));
  Serial.println(F("Please Wave device in a figure eight until done."));
  delay(5000);
  IMU.calibrateMag();

  IMU_PrintCalibration();
  IMU.verbose(false);
}
// Print the IMU Readings every certain time
void IMU_PrintData() {
  Serial.printf("accel(%0.3f,%0.3f,%0.3f)\tgyro(%0.3f,%0.3f,%0.3f)\tmag(%0.3f,%0.3f,%0.3f)\tQuat(%0.3f,%0.3f,%0.3f,%0.3f)\tangles(%0.1f,%0.1f,%0.1f)\n",
                 accelerometer[0],
                 accelerometer[1], 
                 accelerometer[2], 

                 gyroscope[0],
                 gyroscope[1], 
                 gyroscope[2], 

                 magnometer[0],
                 magnometer[1], 
                 magnometer[2],
                 
                 Yaw_angle, 
                 Pitch_angle, 
                 Roll_angle);
}

#endif
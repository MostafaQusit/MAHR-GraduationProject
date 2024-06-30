## MAHR-GraduationProject
# Multi-function Autonomous Household Robot - Mechatronics Department

Everyday household chores can be painful, boring, and time-consuming actions, which may be unavoidable sometimes. This led to the idea of making an indoor autonomous robot that can perform most of the daily repetitive tasks for us, so we can focus on something else. The first step of developing this robot was deciding what it will be capable of. It needed a robotic arm to lift objects, a wheeled base to navigate the house, a method of communicating with the robot, a strong chassis to overcome small obstacles, and a brain to tackle large ones. Many sensors, microcontrollers, and actuators were used to achieve these required tasks. Multiple programs and software were used to design and evaluate the performance of the robot. Eventually, all these efforts led to the making of a Multi-function Autonomous Household Robot (M.A.H.R).

This Coding Project covering the Coding Part of the 3 microcontrollers (ESP32s) of the Robot as known as the LOW LEVEL Code for multiple tasks:
- Controlling the 2 DC Motors of Wheeling mechanics.
- Collecting 2 Encoders Data a for localization.
- Collecting IMU Data for locatization.
- Controlling the Stepper motor of the Spine mechanic.
- Controlling the Robotic Arm driven by 4 Stepper motors and servo motor.
- Supporting Communication abiltity by using ESPNOW feature exclusively for ESPs boards.
- Supporting Cellular Communication by ability of make calls and send SMS using GSM Module.
- Adding Voice feedback for simple procedures by using MP3 Module.
- Devoloping 2 controlling option: autonomously by communicating with the HIGH LEVEL BOARD (RaspberryPi Board) using rosserial, or manually by any controller.
- Appling 2 controller tools: PS4 Controller (via Bluetooth), or Mobile App using MIT Inverter App (via WiFi).

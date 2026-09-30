# ESP32-C3 PWM DC Motor Speed Control

A hardware implementation of PWM-based DC motor speed control using an **ESP32-C3 SuperMini**, **L298N motor driver**, a **12 V DC geared motor**, an OLED display, and an auxiliary Arduino Nano LED bar.

> **Implementation:** This project was built and tested directly on a breadboard. No Wokwi simulation was used.

## Project Overview

The system controls the speed of a 12 V DC geared motor by changing its PWM duty cycle. A potentiometer is used as the speed command input. The ESP32-C3 reads the potentiometer, generates PWM signals for the L298N motor driver, and displays the current duty cycle on a 0.96-inch I2C OLED.

An Arduino Nano is used as an auxiliary display controller for a 10-LED duty-cycle indicator because the ESP32-C3 was not able to provide the required OLED power reliably in the implemented setup.

## Main Features

- PWM-based DC motor speed control
- ESP32-C3 SuperMini as the main controller
- L298N dual H-bridge motor driver
- 12 V DC geared motor
- Potentiometer-based speed command
- 0.96-inch I2C OLED display
- Arduino Nano auxiliary LED duty-cycle bar
- Forward/reverse command through the Arduino Serial Monitor
- Brake/coast operating-mode selection
- Direct breadboard hardware implementation

## Hardware Used

| Component | Purpose |
|---|---|
| ESP32-C3 SuperMini | Main controller and PWM generation |
| L298N | DC motor driver |
| 12 V DC geared motor | Motor/load |
| Arduino Nano | Auxiliary LED-bar controller |
| 0.96-inch I2C OLED | Duty-cycle/status display |
| Potentiometer | Speed command input |
| 12 V power supply | Motor power |
| Breadboard and jumper wires | Prototyping and connections |
| LEDs + resistors | Visual duty-cycle indicator |

## System Architecture

```text
                +----------------------+
                |   Potentiometer      |
                +----------+-----------+
                           |
                           v
                 +---------------------+
                 |    ESP32-C3          |
                 |  Main Controller     |
                 +----+------------+----+
                      |            |
                 PWM / DIR         | I2C
                      |            v
                      |      +-------------+
                      |      |  0.96" OLED |
                      |      +-------------+
                      |
                      v
                +-----------+
                |   L298N   |
                | Motor     |
                | Driver    |
                +-----+-----+
                      |
                      v
                +-----------+
                | 12 V DC   |
                | Geared    |
                | Motor     |
                +-----------+

ESP32-C3 -- serial duty value --> Arduino Nano --> 10-LED indicator
```

## PWM Control Principle

The ESP32-C3 converts the potentiometer reading into a percentage from 0 to 100%.

The PWM duty cycle determines the average voltage applied to the motor through the L298N driver.

For an 8-bit PWM signal:

```text
PWM value = Duty (%) × 255 / 100
```

For example:

- 25% duty cycle → approximately 64/255
- 50% duty cycle → approximately 128/255
- 75% duty cycle → approximately 191/255
- 100% duty cycle → 255/255

The actual motor speed also depends on motor characteristics, load, supply voltage, friction, and driver losses.

## ESP32-C3 Program

The main program is available at:

`src/ESP32_C3_PWM_Motor_Control.ino`

The program:

1. Reads the potentiometer.
2. Filters the analog reading.
3. Converts it to a 0–100% duty-cycle command.
4. Generates PWM signals for the L298N.
5. Updates the OLED display.
6. Supports forward/reverse operation.
7. Supports brake/coast mode.
8. Sends the duty-cycle percentage to the Arduino Nano.

## Arduino Nano Program

The auxiliary program is available at:

`src/Arduino_Nano_LED_Bar.ino`

The Nano receives the duty-cycle value over serial communication and represents the duty cycle using ten LEDs.

- LEDs 1–9 represent approximately 10–90%.
- The tenth LED acts as a high-duty warning indicator above 95%.
- If communication from the ESP32-C3 stops for more than 2 seconds, the LED bar is cleared.

## Hardware Implementation

The project was physically assembled and tested on a breadboard rather than simulated in Wokwi.

### Hardware Photos

The `diagrams/` directory contains photographs and the annotated hardware setup.

### Demonstration Video

The project demonstration video is stored in the `videos/` directory.

## Arduino IDE

Both ESP32-C3 and Arduino Nano programs were developed using the Arduino IDE.

### ESP32-C3 libraries

The ESP32 program uses:

- Wire
- Adafruit GFX Library
- Adafruit SSD1306

### Arduino Nano

The Nano program uses:

- SoftwareSerial

## Important Implementation Note

The completed project is **PWM/open-loop speed control**. Although the motor used in the hardware setup has an encoder, the present implementation does not close the control loop using encoder feedback.

Therefore, this project should not be described as a PID speed controller.

A natural next step is to use the encoder feedback to measure actual motor speed and implement a closed-loop PID controller.

## Future Improvements

- Encoder feedback integration
- Closed-loop speed measurement
- PID speed control
- Automatic speed regulation under changing load
- RPM measurement and logging
- Serial/USB data visualization
- Improved motor-driver efficiency using a more suitable MOSFET-based driver
- PCB-based implementation

## Project Status

**Completed — hardware prototype tested**

## Author

**Himanshu Kumar**

B.Tech — Electronics and Communication Engineering

---

### Repository Structure

```text
ESP32-C3-PWM-DC-Motor-Control/
├── README.md
├── src/
│   ├── ESP32_C3_PWM_Motor_Control.ino
│   └── Arduino_Nano_LED_Bar.ino
├── diagrams/
│   ├── annotated_setup.png
│   ├── hardware_setup_1.jpg
│   └── hardware_setup_2.jpg
└── videos/
    └── motor_control_demo.mp4
```

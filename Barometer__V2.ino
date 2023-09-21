#include <Wire.h>                                                                                           // Include Librery for I2C communication
const byte MS5611_address = 0x77, RESET = 0x1E;                                                             // I2C address of MS5611 sensor.
uint8_t loop_counter = 0, start, pressure_counter = 0, temperature_counter = 0 ;
uint16_t C[7], no_of_pressure_value = 20, no_of_temperature_value = 5;                                      // calibration values which are preinstalled in the sensor, different for evry sensor.
int64_t OFF, OFF_C2, SENS, SENS_C1;                                                                         // Take help of the datasheet to understand these variables, are used in calculations of the pressure and temperature values.
int32_t p, dT, actual_pressure, reference_pressure = 0;
uint32_t D1, D2, pressure_array[20] = {0}, temperature_array[5] = {0} , sum_pressure = 0, sum_temperature = 0;
float height = 0.000, pressure_coefficient = 0.117, x =0.000;

void setup()  {
  Wire.begin();
  Serial.begin(115200);

  Wire.beginTransmission(MS5611_address);                       // Send reset command to clear any residual data, and start the sensor smoothly.
  Wire.write(RESET);                                            // Send reset command
  Wire.endTransmission();

  delay(10);                                                    // Wait for reset to complete

  for (start = 1; start <= 6; start++) {
    Wire.beginTransmission(MS5611_address);                     // Start communication with the MS5611
    Wire.write(0xA0 + start * 2);                               // Send the address that we want to read.
    Wire.endTransmission();                                     // End the transmission.

    Wire.requestFrom(MS5611_address, 2);                        // Request 2 bytes from the MS5611.
    C[start] = Wire.read() << 8 | Wire.read();                  // Add the low and high byte to the C[x] calibration variable.
  }

  OFF_C2 = C[2] * pow(2, 16);                                   // This value is pre-calculated to offload the main program loop.
  SENS_C1 = C[1] * pow(2, 15);                                  // This value is pre-calculated to offload the main program loop.

  int i = 0;
  while (i<100){
    readBarometer();
    i++;
    delay (10);
  }

  reference_pressure = actual_pressure;

//  Serial.println("Setup complete:  ");
//  delay(1000);
}

void loop() {
  int start_time = millis();
  readBarometer();
//  Serial.println(millis() - start_time);
//Serial.println(height);
delay (10);
}

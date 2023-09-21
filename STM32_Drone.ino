#include <Wire.h>
// BNO variables.
const int BNO055_ADDR = 0x28;  // BNO055's I2C address
const int BNO055_ID = 0xA0;    // BNO055's ID value
int16_t headingData, rollData, pitchData;
float heading, roll, pitch;

//GPS variables.
char read_serial_byte, incoming_message[100];
bool new_line_found = false, gps_available = false;
uint16_t message_counter, counter = 0;
int32_t lat_actual, lon_actual, lat_previous, lon_previous, lat_addition, lon_addition, lat, lon;

//Barometer variables
const byte MS5611_address = 0x77, RESET = 0x1E;                                                             // I2C address of MS5611 sensor.
uint8_t loop_counter = 0, start, pressure_counter = 0, temperature_counter = 0 ;
uint16_t C[7], no_of_pressure_value = 20, no_of_temperature_value = 5;                                      // calibration values which are preinstalled in the sensor, different for evry sensor.
int64_t OFF, OFF_C2, SENS, SENS_C1;                                                                         // Take help of the datasheet to understand these variables, are used in calculations of the pressure and temperature values.
int32_t p, dT, actual_pressure, reference_pressure = 0;
uint32_t d1, d2, pressure_array[20] = {0}, temperature_array[5] = {0} , sum_pressure = 0, sum_temperature = 0;
float height = 0.000, pressure_coefficient = 0.117, x = 0.000;

long loop_timer;

void setup() {
  Wire.begin();
  Wire.setClock(400000);
  Serial.begin(115200);
  Serial3.begin(38400);

  setupGPS(); // updates value of lat and lon
  setupBNO(); // refreshes value of heading, roll and pitch
  setupBarometer();
}

void loop() {
  
  readBarometer(); // updates value of height
  readBNO();       // refreshes value of heading, roll and pitch
  readGPS();       // updates value of lat and lon
  
  while (micros() - loop_timer < 5000);                                            //We wait until 5000us are passed.
  loop_timer = micros();                                                           //Set the timer for the next loop.

}

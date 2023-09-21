void setupBarometer() {

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
  while (i < 1000) {
    readBarometer();
    i++;
    delay (5);
  }

  reference_pressure = actual_pressure;
}

void readBarometer() {

  if ( loop_counter == 22) {
    loop_counter = 0;
  }
  if ( loop_counter % 2 == 0);
  else {
    if ( loop_counter == 21) {
      //read temperature
      Wire.beginTransmission(MS5611_address);                     // Start reading the digital temperature value
      Wire.write(0x00);                                           // Start reading from register 0.
      Wire.endTransmission();

      Wire.requestFrom(MS5611_address, 3);                        // request 3 bytes of data
      if (Wire.available() >= 3) {
        temperature_array [temperature_counter] = ((uint32_t)Wire.read() << 16) | ((uint32_t)Wire.read() << 8) | Wire.read();             //3 bytes are read like this, in my guess, Wire.read is delault 16bits, so we have to convert that to 32 bits
        temperature_counter ++;
        temperature_counter %= 5;                                                                                                        // created a rotating memory data of 5.
        for ( int i = 0; i < no_of_temperature_value; i++) {
          sum_temperature += temperature_array[i];
        }
        d2 = sum_temperature / no_of_temperature_value;
        sum_temperature = 0;

      }
    }
    else {
      //read pressure
      Wire.beginTransmission(MS5611_address);                     // Start reading the pressure value
      Wire.write(0x00);                                           // Start reading from register 0.
      Wire.endTransmission();

      Wire.requestFrom(MS5611_address, 3);                        // 3 bytes of data is requested
      if (Wire.available() >= 3) {
        d1 = ((uint32_t)Wire.read() << 16) | ((uint32_t)Wire.read() << 8) | Wire.read();            // 3 bytes are read like this, in my guess, Wire.read is delault 16bits, so we have to convert that to 32 bits
        //      Serial.print("pressure: ");  Serial.println(d1);
        // convert digital pressure to actual pressure
        dT = d2 - (C[5] * pow(2, 8));                                                               // difference in temperature and ref temperature (digitally)
        OFF = OFF_C2 + ((int64_t)C[4] * (int64_t)dT) / pow(2, 7);                                   // intermediate methods to calculate Pressure
        SENS = SENS_C1 + ((int64_t)dT * (int64_t)C[3]) / pow(2, 8);                                 // intermediate methods to calculate Pressure
        p = ((d1 * SENS) / pow(2, 21) - OFF) / pow(2, 15);                                          // actual pressure is calculated
        //Serial.print(p); Serial.print("  ");
        pressure_array[pressure_counter] = p;
        pressure_counter ++;
        pressure_counter %= no_of_pressure_value;

        for (int i = 0; i < no_of_pressure_value; i++) {
          sum_pressure += pressure_array[i];
        }

        actual_pressure = sum_pressure / no_of_pressure_value;
        //Serial.println(sum_pressure);
        sum_pressure = 0;
        //Serial.println(actual_pressure);
        height = float(reference_pressure - actual_pressure) / pressure_coefficient;
        Serial.println(height);
      }
    }


    if (loop_counter == 19) {
      //send temperature command
      Wire.beginTransmission(MS5611_address);
      Wire.write(0x58);                                           // Teamperture conversion command
      Wire.endTransmission();
    }
    else {
      //send pressure command
      Wire.beginTransmission(MS5611_address);
      Wire.write(0x48);                                           // Pressure conversion command
      Wire.endTransmission();
    }

  }
  loop_counter ++;
}

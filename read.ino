void readBarometer() {
  if (loop_counter == 0) {
    //update digital temperature value only.
    Wire.beginTransmission(MS5611_address);
    Wire.write(0x58);                                           // Teamperture conversion command
    Wire.endTransmission();

    delay(9);                                                  // Wait for conversion to complete. critical line , not to be edited. value has to be above 8 ms for the value to be correct

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
      D2 = sum_temperature / no_of_temperature_value;
      sum_temperature = 0;
      //      Serial.print("temp: "); Serial.println(D2);
      //convert digital temperature to actual temperature.
    }

  }

  else {
    //update pressure value only.
    Wire.beginTransmission(MS5611_address);
    Wire.write(0x48);                                           // Pressure conversion command
    Wire.endTransmission();

    delay(9);                                                  // Wait for conversion to complete. critical line , not to be edited. value has to be above 8 ms for the value to be correct

    Wire.beginTransmission(MS5611_address);                     // Start reading the pressure value
    Wire.write(0x00);                                           // Start reading from register 0.
    Wire.endTransmission();

    Wire.requestFrom(MS5611_address, 3);                        // 3 bytes of data is requested
    if (Wire.available() >= 3) {
      D1 = ((uint32_t)Wire.read() << 16) | ((uint32_t)Wire.read() << 8) | Wire.read();            // 3 bytes are read like this, in my guess, Wire.read is delault 16bits, so we have to convert that to 32 bits
      //      Serial.print("pressure: ");  Serial.println(D1);
      // convert digital pressure to actual pressure
      dT = D2 - (C[5] * pow(2, 8));                                                               // difference in temperature and ref temperature (digitally)
      OFF = OFF_C2 + ((int64_t)C[4] * (int64_t)dT) / pow(2, 7);                                   // intermediate methods to calculate Pressure
      SENS = SENS_C1 + ((int64_t)dT * (int64_t)C[3]) / pow(2, 8);                                 // intermediate methods to calculate Pressure
      p = ((D1 * SENS) / pow(2, 21) - OFF) / pow(2, 15);                                          // actual pressure is calculated
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
      //return height;
    }
  }

  loop_counter ++;                                          // Counter to track which digital value to get in the running loop cycly
  loop_counter %= 6;                                       // Getting digital value of temperature every 10th cycle.
}

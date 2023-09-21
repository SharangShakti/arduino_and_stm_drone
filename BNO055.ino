void setupBNO() {
  
  // Check the chip ID to ensure communication
  Wire.beginTransmission(BNO055_ADDR);
  Wire.write(0x00);  // Chip ID register
  Wire.endTransmission();
  Wire.requestFrom(BNO055_ADDR, 1);
  if (Wire.read() != BNO055_ID) {
    Serial.println("Device not found!");
    while (1);
  }

  // Set to IMU mode to get relative orientation data
  Wire.beginTransmission(BNO055_ADDR);
  Wire.write(0x3D);  // OPR_MODE register
  Wire.write(0x08);  // IMU mode
  Wire.endTransmission();
  delay(50);  // Give some time for the mode to change
  
}

void readBNO() {
    // Request Euler data (heading, roll, pitch)
  Wire.beginTransmission(BNO055_ADDR);
  Wire.write(0x1A);  // Start from HEADING_DATA_LSB
  Wire.endTransmission();

  Wire.requestFrom(BNO055_ADDR, 6);

  headingData = Wire.read() | (Wire.read() << 8);
  rollData = Wire.read() | (Wire.read() << 8);
  pitchData = Wire.read() | (Wire.read() << 8);

//   Convert to degrees. The raw values are in units of 1/16 degrees.
     heading = headingData / 16.0;
     roll = rollData / 16.0;
     pitch = pitchData / 16.0;


    Serial.print("Heading: ");
    Serial.print(heading, 2);
    Serial.print(" Roll: ");
    Serial.print(roll, 2);
    Serial.print(" Pitch: ");
    Serial.println(pitch, 2);
}

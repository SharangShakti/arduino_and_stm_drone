void setupGPS() {
  Serial3.begin(38400); // Initialize the GPS module communication
  // UBX command for 10Hz update rate
  byte set_to_10Hz[] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0x64, 0x00, 0x01, 0x00, 0x01, 0x00, 0x7A, 0x12};

  // Send the UBX command
  for (int i = 0; i < sizeof(set_to_10Hz); i++) {
    Serial3.write(set_to_10Hz[i]);
  }
  delay(5);
}
void readGPS() {
  while (Serial3.available() && new_line_found == false) {
    //    Serial.println(Serial3.available());
    read_serial_byte = Serial3.read();
    if (read_serial_byte == '$') {
      for (message_counter = 0; message_counter < 100; message_counter++) {
        incoming_message[message_counter] = '-';
      }
      message_counter = 0;
    }
    incoming_message[message_counter] = read_serial_byte;
    if ( read_serial_byte == '*') {
      new_line_found = true;
    }

    message_counter++;

  }
  if (new_line_found == true && incoming_message[3] == 'G' && incoming_message[4] == 'G') {
    //at this point the incoming_message is holding a complete NMEA line, perfor you desired task here.
    if ( incoming_message[28] == 'N' && incoming_message[42] == 'E') {
      gps_available = true;

      lat_actual = ((int)incoming_message[19] - 48) *  (long)10000000;                              //Filter the minutes for the GGA line multiplied by 10.
      lat_actual += ((int)incoming_message[20] - 48) * (long)1000000;                               //Filter the minutes for the GGA line multiplied by 10.
      lat_actual += ((int)incoming_message[22] - 48) * (long)100000;                                //Filter the minutes for the GGA line multiplied by 10.
      lat_actual += ((int)incoming_message[23] - 48) * (long)10000;                                 //Filter the minutes for the GGA line multiplied by 10.
      lat_actual += ((int)incoming_message[24] - 48) * (long)1000;                                  //Filter the minutes for the GGA line multiplied by 10.
      lat_actual += ((int)incoming_message[25] - 48) * (long)100;                                   //Filter the minutes for the GGA line multiplied by 10.
      lat_actual += ((int)incoming_message[26] - 48) * (long)10;                                    //Filter the minutes for the GGA line multiplied by 10.
      lat_actual /= (long)6;                                                                         //To convert the minutes to degrees we need to divide the minutes by 6.
      lat_actual += ((int)incoming_message[17] - 48) *  (long)100000000;                            //Add the degrees multiplied by 10.
      lat_actual += ((int)incoming_message[18] - 48) *  (long)10000000;                             //Add the degrees multiplied by 10.
      //      lat_actual /= 10;                                                                              //Divide everything by 10.

      lon_actual = ((int)incoming_message[33] - 48) *  (long)10000000;                              //Filter the minutes for the GGA line multiplied by 10.
      lon_actual += ((int)incoming_message[34] - 48) * (long)1000000;                               //Filter the minutes for the GGA line multiplied by 10.
      lon_actual += ((int)incoming_message[36] - 48) * (long)100000;                                //Filter the minutes for the GGA line multiplied by 10.
      lon_actual += ((int)incoming_message[37] - 48) * (long)10000;                                 //Filter the minutes for the GGA line multiplied by 10.
      lon_actual += ((int)incoming_message[38] - 48) * (long)1000;                                  //Filter the minutes for the GGA line multiplied by 10.
      lon_actual += ((int)incoming_message[39] - 48) * (long)100;                                   //Filter the minutes for the GGA line multiplied by 10.
      lon_actual += ((int)incoming_message[40] - 48) * (long)10;                                    //Filter the minutes for the GGA line multiplied by 10.
      lon_actual /= (long)6;                                                                         //To convert the minutes to degrees we need to divide the minutes by 6.
      lon_actual += ((int)incoming_message[30] - 48) * (long)1000000000;                            //Add the degrees multiplied by 10.
      lon_actual += ((int)incoming_message[31] - 48) * (long)100000000;                             //Add the degrees multiplied by 10.
      lon_actual += ((int)incoming_message[32] - 48) * (long)10000000;                              //Add the degrees multiplied by 10.
      //      lon_actual /= 10;                                                                              //Divide everything by 10.
      lat_actual %= 10000000;
      lon_actual %= 10000000;
      lat_actual *= 100;
      lon_actual *= 100;
      //      Serial.print("actual value:     ") ; Serial.print(lat_actual); Serial.print("    "); Serial.println(lon_actual);
      lat_addition = (lat_actual - lat_previous) / 20 ;
      lon_addition = (lon_actual - lon_previous) / 20;
      //      Serial.print("lat_addition:  "); Serial.println(lat_addition);


      lat_previous = lat_actual;
      lon_previous = lon_actual;
      counter = 0;
    }
    else {
      gps_available = false;
      lat_actual = 0;
      lon_actual = 0;
      Serial.println("NO GPS");
    }
  }
  new_line_found = false;
  if (gps_available == true) {
    lon = counter * lon_addition + lon_actual;
    lat = counter * lat_addition + lat_actual;
    //    Serial.print("counter:  ") ; Serial.println(counter);
    counter ++;
        Serial.print(lat); Serial.print( "     "); Serial.println(lon);
  }


}

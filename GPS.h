/*
   GPS functions
*/

void sendUBX(uint8_t *MSG, uint8_t len)
{
  Serial.flush();
  Serial.write(0xFF);
  delay(500);
  for (int i = 0; i < len; i++) {
    Serial.write(MSG[i]);
  }
}

boolean getUBX_ACK(uint8_t *MSG)
{
  uint8_t b;
  uint8_t ackByteID = 0;
  uint8_t ackPacket[10];
  unsigned long startTime = millis();

  // Construct the expected ACK packet
  ackPacket[0] = 0xB5; // header
  ackPacket[1] = 0x62; // header
  ackPacket[2] = 0x05; // class
  ackPacket[3] = 0x01; // id
  ackPacket[4] = 0x02; // length
  ackPacket[5] = 0x00;
  ackPacket[6] = MSG[2]; // ACK class
  ackPacket[7] = MSG[3]; // ACK id
  ackPacket[8] = 0; // CK_A
  ackPacket[9] = 0; // CK_B

  // Calculate the checksums
  for (uint8_t ubxi = 2; ubxi < 8; ubxi++) {
    ackPacket[8] = ackPacket[8] + ackPacket[ubxi];
    ackPacket[9] = ackPacket[9] + ackPacket[8];
  }

  while (1) {

    // Test for success
    if (ackByteID > 9) {
      // All packets in order!
      return true;
    }
    // Timeout if no valid response in 3 seconds
    if (millis() - startTime > 3000) {
      return false;
    }
    // Make sure data is available to read
    if (Serial.available()) {
      b = Serial.read();

      // Check that bytes arrive in sequence as per expected ACK packet
      if (b == ackPacket[ackByteID]) {
        ackByteID++;
      }
      else {
        ackByteID = 0; // Reset and look again, invalid order
      }
    }
  }
}

void setGPS_AirBorne()
{
  int gps_set_sucess = 0;
  uint8_t setdm6[] = {
    0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x06,
    0x03, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00,
    0x05, 0x00, 0xFA, 0x00, 0xFA, 0x00, 0x64, 0x00, 0x2C,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0xDC
  };
  // while(!gps_set_sucess)
  //{
  sendUBX(setdm6, sizeof(setdm6) / sizeof(uint8_t));
  gps_set_sucess = getUBX_ACK(setdm6);
  //}
}

void gps_reset()
{
  /*
    Forced (Watchdog)
    Coldstart
  */
  int gps_set_sucess = 0;
  uint8_t set_reset[] = {0xB5, 0x62, 0x06, 0x04, 0x04, 0x00, 0xFF, 0x87, 0x00, 0x00, 0x94, 0xF5};
  sendUBX(set_reset, sizeof(set_reset) / sizeof(uint8_t));
  gps_set_sucess = getUBX_ACK(set_reset);
}

void gps_set_max_performance_mode()
{
  int gps_set_sucess = 0;
  uint8_t setMax[] = {
    0xB5, 0x62, 0x06, 0x11, 0x02, 0x00, 0x08, 0x00, 0x21, 0x91
  };
  // while (!gps_set_sucess)
  //{
  sendUBX(setMax, sizeof(setMax) / sizeof(uint8_t));
  gps_set_sucess = getUBX_ACK(setMax);
  //}
}

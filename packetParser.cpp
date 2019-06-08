

#include <Adafruit_BLE.h>
#define PACKET_ACC_LEN                  (15)
#define PACKET_GYRO_LEN                 (15)
#define PACKET_MAG_LEN                  (15)
#define PACKET_QUAT_LEN                 (19)
#define PACKET_BUTTON_LEN               (5)
#define PACKET_COLOR_LEN                (6)
#define PACKET_LOCATION_LEN             (15)


//    READ_BUFSIZE            Size of the read buffer for incoming packets

#define READ_BUFSIZE                    (20)


/* Buffer to hold incoming characters */

uint8_t packetbuffer[READ_BUFSIZE+1];


float parsefloat(uint8_t *buffer) 
{
 
 float f = ((float *)buffer)[0];
  return f;
}


void printHex(const uint8_t * data, const uint32_t numBytes)
{

uint32_t szPos;
  for (szPos=0; szPos < numBytes; szPos++) 
  {
   
   Serial.print(F("0x"));
    // Append leading 0 for small values
    if (data[szPos] <= 0xF)
    {
      Serial.print(F("0"));
      Serial.print(data[szPos] & 0xf, HEX);
    }
    else
    {
      Serial.print(data[szPos] & 0xff, HEX);
  
  }
    // Add a trailing space if appropriate
    if ((numBytes > 1) && (szPos != numBytes - 1))
    {
      Serial.print(F(" "));
    }
  }
  Serial.println();
}


uint8_t readPacket(Adafruit_BLE *ble, uint16_t timeout) 
{
  uint16_t origtimeout = timeout, replyidx = 0;

 
 memset(packetbuffer, 0, READ_BUFSIZE);
 while (timeout--) {
    if (replyidx >= 20) break;
    if ((packetbuffer[1] == 'B') && (replyidx == PACKET_BUTTON_LEN))
      break;


    while (ble->available()) {
      char c =  ble->read();
      if (c == '!') {//Check for our message signature
        replyidx = 0;
      }
    
    packetbuffer[replyidx] = c;
      replyidx++;
      timeout = origtimeout;
    } 
    if (timeout == 0) break;
    delay(1);
  }
  packetbuffer[replyidx] = 0;  // null term

  if (!replyidx)  // no data or timeout 
    return 0;
  if (packetbuffer[0] != '!')  // doesn't start with '!' packet beginning
    return 0;
  // check checksum!
  uint8_t xsum = 0;
  uint8_t checksum = packetbuffer[replyidx-1];
  
 
 for (uint8_t i=0; i<replyidx-1; i++) {
    xsum += packetbuffer[i];
 
 }
  xsum = ~xsum;
  // Throw an error message if the checksum's don't match
  if (xsum != checksum)
  {
    Serial.print("Checksum mismatch in packet : ");
    printHex(packetbuffer, replyidx+1);
    return 0;
  } 
  // checksum passed!
  return replyidx;
  
  
}

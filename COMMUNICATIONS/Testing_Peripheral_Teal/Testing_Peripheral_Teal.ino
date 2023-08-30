 
 /*
 * Kevin Brannan
  Combined Funcitonality:
  //DR Interrupt
  Filters
  High Speed I2C
  Raw Data
  

*/

#include <Wire.h>
#include "SparkFun_ISM330DHCX.h"
#include <SparkFun_MMC5983MA_Arduino_Library.h>
#include <H3LIS331DL.h>

//For BLE:
#include <bluefruit.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>

// BLE Service
BLEDfu  bledfu;  // OTA DFU service
BLEDis  bledis;  // device information
BLEUart bleuart; // uart over ble
BLEBas  blebas;  // battery
BLECharacteristic myCharacteristic;

#define PACKET_SIZE 30
/////////////////////////////////////////////////////////////////////
/*
//Magnetometer (from sensor hub example)

// 8 bit addressing is required for the 6DoF
// to communicate with its' external sensors.

#define MAG_ADDR_READ 0x61 // (0x30 << 1) | 1)
#define MAG_ADDR_WRITE 0x60 // (0x30 << 1)

#define MAG_READ_REG 0x00 // Read from 0x00
#define MAG_READ_LEN 0x07 // Read seven times consecutively

// INT_CTRL0 (0x09) - contains the bit to initiate measurement. 
// It must be written before each read and is cleared automatically.
#define MAG_WRITE_REG 0x09 
#define MAG_WRITE_DATA 0x01 // Value to write to INT_CTRL0 

/////////////////////////////////////////////////////////////////////
//ISM330 Accel + Gyro
#define interrupt_pin D0
#define mag_int_pin   D1

SparkFun_ISM330DHCX myISM; 
SFE_MMC5983MA myMag; 
H3LIS331DL h3lis;

//From standalone lib example
volatile bool newDataAvailable = true;
uint32_t rawValueX = 0; 
uint32_t rawValueY = 0;
uint32_t rawValueZ = 0;
*/
struct mag_data_t
{
  uint16_t scaledX;
  uint16_t scaledY;
  uint16_t scaledZ;
};


 struct all_data_t
 {  
   char headerMSB;
   char headerLSB;
   // Structs for X,Y,Z data
   sfe_ism_raw_data_t   accelData; 
   sfe_ism_raw_data_t   gyroData;     //sets of 3 int16_t vars.
   mag_data_t           magData;
   char header_1MSB;
   char header_1LSB;
   AxesRaw_t            high_g_data;
 };
 
const int union_size = sizeof(all_data_t);

union bitPacket_t{
  all_data_t allData;
 int16_t buf[union_size];
};
bitPacket_t data; 


////////////////////////////////////////////////////////////////

void setup(void)
{
  data.allData.headerMSB = '%';
  data.allData.headerLSB = '%';
  data.allData.header_1MSB = '?';
  data.allData.header_1LSB = '?';
//  pinMode(interrupt_pin, INPUT);
  
  //h3lis.init();
  
  Serial.begin(115200);   
  Wire.begin();
  Wire.setClock(400000);

///////////////////////////////////////////////////////////////////////////

  Bluefruit.autoConnLed(true);
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);
  //Bluefruit.configUuid128Count(15);
  Bluefruit.begin();
  Bluefruit.setTxPower(8);    //8 is max power (see bluefruit.h)
  //TxPower nRF52840: -40dBm, -20dBm, -16dBm, -12dBm, -8dBm, -4dBm, 0dBm, +2dBm, +3dBm, +4dBm, +5dBm, +6dBm, +7dBm and +8dBm.
  // Bluefruit.setInterval(6, 80);

  

  //Use Bluefruit.requestMtuExchange() to change packet size.

  Bluefruit.setName("Teal Incisor");      //(getMcuUniqueID()); // B5BBCD582C988315
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);
  Bluefruit.Periph.setConnInterval(6, 12); // 7.5 - 15 ms (units of 1.25)    
  // Fast settings from 'throughput' example. Slow down as necessary when multiple peripherials are present? 
  
  // To be consistent OTA DFU should be added first if it exists
  bledfu.begin();

  // Configure and Start Device Information Service
  bledis.setManufacturer("HIMU Industries");
  bledis.setModel("9-DOF Hoof IMU");
  bledis.begin();

  // Configure and Start BLE Uart Service
  bleuart.begin();

//  bleuart.setRxCallback(bleuart_rx_callback);
//  bleuart.setNotifyCallback(bleuart_notify_callback);


//  myCharacteristic.setProperties(CHR_PROPS_NOTIFY);                 // Set characteristic to NOTIFY
  //myCharacteristic.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);  // Read is open, Write is closed
//  myCharacteristic.setFixedLen(sizeof(myCharacteristic));           // <- THIS IS NECESSARY
//  myCharacteristic.begin();


  
  // Start BLE Battery Service
  blebas.begin();
  blebas.write(100);    //Current battery level.

  // Set up and start advertising
  startAdv();

//////////////////////////////////////////////////////////////////////////
  /*
  if( !myISM.begin() )
  {
    Serial.println("Did not begin.");
    while(1);
  }

  // Reset the device to default settings. This if helpful is you're doing multiple
  // uploads testing different settings. 
  myISM.deviceReset();
  myISM.resetSensorHub();

  while( !myISM.getDeviceReset() ){ 
    yield();
  } 

  Serial.println("Reset.");
  delay(100);
  
  myISM.setDeviceConfig();
  myISM.setBlockDataUpdate();
  
///////////////////////////////////////////////////////////////////////////
// Configure the interrupt pin for the "Measurement Done" interrupt
    pinMode(mag_int_pin, INPUT);
    //attachInterrupt(digitalPinToInterrupt(mag_int_pin), interruptRoutine, RISING);

    if (myMag.begin() == false)
    {
        Serial.println("MMC5983MA did not respond - check your wiring. Freezing.");
        while (true);
    }

    myMag.softReset();
    
    Serial.println(myMag.getTemperature());
    
    myMag.setFilterBandwidth(100);    //800, 400, 200 or 100.
    myMag.setContinuousModeFrequency(1000);   //1000, 200, 100, 50, 20, 10, 1 and 0.
    myMag.enableAutomaticSetReset();
    myMag.enableContinuousMode();
    myMag.enableInterrupt();
    // Set our interrupt flag, just in case we missed the rising edge
    newDataAvailable = true;
    
//////////////////////////////////////////////////////////////////////////////

  // Set the output data rate and precision of the accelerometer
  myISM.setAccelDataRate(ISM_XL_ODR_6667Hz);
  myISM.setAccelFullScale(ISM_8g); 

  // Set the output data rate and precision of the gyroscope
  myISM.setGyroDataRate(ISM_GY_ODR_6667Hz);
  myISM.setGyroFullScale(ISM_500dps); 

  // Turn on the accelerometer's filter and apply settings. 
  myISM.setAccelFilterLP2();
  myISM.setAccelSlopeFilter(ISM_LP_ODR_DIV_100);

  // Turn on the gyroscope's filter and apply settings. 
  myISM.setGyroFilterLP1();
  myISM.setGyroLP1Bandwidth(ISM_MEDIUM);

  // Set the accelerometer's status i.e. the data ready to interrupt one. 
  // Commented out just below is the function to send the data ready
  // to interrupt two. 

  myISM.setAccelStatustoInt1();
  
  //myISM.setAccelStatustoInt2();

  // We can just as easily set the gyroscope's data read signal to either interrupt

  //myISM.setGyroStatustoInt1();
  //myISM.setGyroStatustoInt2();
  
  
  // Uncommenting the function call below will change the interrupt to 
  // active LOW instead of HIGH.

  //myISM.setPinMode();

  // This function call will modify which "events" trigger an interrupt. No 
  // argument has been given, please refer to the datasheet for more 
  // information.

  // myISM.setIntNotification(uint8_t val);


*/
}
void loop(){
/*
  while(!digitalRead(mag_int_pin))
  {yield;}    //wait for the int!

  myMag.clearMeasDoneInterrupt(); // Clear the MMC5983 interrupt

  myMag.readFieldsXYZ(&rawValueX, &rawValueY, &rawValueZ);
  
  data.allData.magData.scaledX = rawValueX >> 2;
  data.allData.magData.scaledY = rawValueY >> 2;
  data.allData.magData.scaledZ = rawValueZ >> 2;


              
  // Check if both gyroscope and accelerometer data is available.
  if( myISM.checkStatus() )
  {
    myISM.getRawAccel(&data.allData.accelData);
    myISM.getRawGyro(&data.allData.gyroData);
    h3lis.getAccAxesRaw(&data.allData.high_g_data);   //HIGH_G
  }
*/

//PRINT STATEMENTS.



/*
  for(int i=0;i < union_size>>1; i++)
  {
    String str = String(data.buf[i]);
    int str_length = str.length();
    char buff[str_length + 1];
    str.toCharArray(buff, str_length + 1);
    
    bleuart.write(buff,sizeof(buff));    //mag data is supposed to be unsigned and will not look correct.
    bleuart.write(";");     //("\t");
  }
  bleuart.write("\n");
*/  
  //myCharacteristic.notify(&data.buf, union_size);                 // Set characteristic message, and notify client

  data.allData.magData.scaledX = 0x01;
  data.allData.magData.scaledY = 0x02;
  data.allData.magData.scaledZ = 0x03;
  data.allData.gyroData.xData = 0xD4;
  data.allData.gyroData.yData = 0xD5;
  data.allData.gyroData.zData = 0xD6;
  data.allData.accelData.xData = 0x07;
  data.allData.accelData.yData = 0x08;
  data.allData.accelData.zData = 0x09;
  
  data.allData.high_g_data.AXIS_X = 0xEE;
  data.allData.high_g_data.AXIS_Y = 0xEE;
  data.allData.high_g_data.AXIS_Z = 0xEE;
  
  size_t len = bleuart.write((const char *)&data.buf, union_size);
  Serial.print(len);
  Serial.print("\t");
  Serial.println(union_size);
  
    //Serial.write((const char *)&data.buf, union_size);

 
  for(int i=0;i < union_size>>1; i++)
    {
      Serial.print(data.buf[i]);    //mag data is supposed to be unsigned and will not look correct.
      Serial.print("\t");
    }
    Serial.println();  
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms (originally 32)
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

//////////////////////////////////////////////////////////////////

// callback invoked when central connects
void connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);
/////////////////////////////////////////////////////////////////
  // request PHY changed to 2MB
  // This doubles the physical layer speed, increasing packet speed, but doesnt change connection interval so throughput is not necessarily changed.
  //Serial.println("Request to change PHY");
  connection->requestPHY();

  // request to update data length
  Serial.println("Request to change Data Length");
  connection->requestDataLengthUpdate(); 
    
  // request mtu exchange
  // 247 is max packet size. adjust to actual sensor data size.
  Serial.println("Request to change MTU");
  connection->requestMtuExchange(40);    //(sizeof(myData) + 3)

  // request connection interval of 7.5 ms
  connection->requestConnectionParameter(6); // in unit of 1.25

  // delay a bit for all the request to complete
  delay(1000);
  
  Serial.println(connection->getMtu());
  //Serial.print(Bluefruit.getMaxMtu(conn_handle));
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.println();
  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);

  bleuart.flush();
}

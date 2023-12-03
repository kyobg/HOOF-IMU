  /*
 * Kevin Brannan
 * Kyle Garcia
  Combined Funcitonality:
  Data Rate (DR) Interrupt
  Filters
  High Speed I2C
  Raw Data, No Conversions
*/

#include <Wire.h>
#include "src/SparkFun_ISM330DHCX.h"                        // 6 Degree-of-Freedom (6DOF) Accel + Gyro Library
#include "src/SparkFun_MMC5983MA_Arduino_Library.h"         // Magnetometer Library
#include "src/SparkFun_LIS331.h"                            // 3-Axis High G Accelerometer Library
#include "src/SparkFunBQ27441.h"                            // Battery Management System Library

//For Bluetooth Low Energy (BLE):
#include <bluefruit.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>

// BLE Service
BLEDfu  bledfu;  // OTA DFU service
BLEDis  bledis;  // device information
BLEUart bleuart; // uart over ble
BLEBas  blebas;  // battery

/////////////////////////////////////////////////////////////////////
// BMS System
const uint16_t BATTERY_CAPACITY = 650;      //battery capacity in mAh.
const uint16_t TERMINATE_VOLTAGE = 3000;    //lowest operational voltage in mV.
const uint16_t TAPER_CURRENT = 12;          //Control charging rate. this is the lowest value.
const int PERCENTAGE_INTERVAL = 1;
uint32_t lasttime = 0;
#define GPOUT_PIN D2
#define BAS_TIME_INTERVAL 60000 

/////////////////////////////////////////////////////////////////////
//Magnetometer (from sensor hub example)

// 8 bit addressing is required for the 6DoF
// to communicate with its' external sensors.

#define MAG_ADDR_READ 0x61 // (0x30 << 1) | 1)
#define MAG_ADDR_WRITE 0x60 // (0x30 << 1)

#define MAG_READ_REG 0x00 // Read from 0x00
#define MAG_READ_LEN 0x07 // Read seven times consecutively

#define MAG_WRITE_REG 0x09 
#define MAG_WRITE_DATA 0x01 // Value to write to INT_CTRL0 

/////////////////////////////////////////////////////////////////////
//ISM330 Accel + Gyro (6DOF)
#define interrupt_pin D0
#define mag_int_pin   D1

/////////////////////////////////////////////////////////////////////

SparkFun_ISM330DHCX myISM; 
SFE_MMC5983MA myMag; 
LIS331 myLis;

//From standalone lib example
volatile bool newDataAvailable = true;
uint32_t rawValueX = 0; 
uint32_t rawValueY = 0;
uint32_t rawValueZ = 0;

// for high g read
int16_t rawAccelX = 0;
int16_t rawAccelY = 0;
int16_t rawAccelZ = 0;

struct mag_data_t
{
  uint16_t scaledX;
  uint16_t scaledY;
  uint16_t scaledZ;
};

struct accel_high_data_t
{
  uint16_t X;
  uint16_t Y;
  uint16_t Z;  
};

 struct all_data_t
 {  
   char headerMSB;                            // Packet Identifier
   char headerLSB;                            // Packet Identifier
   uint16_t millisecondsMSB;                  // Time High
   uint16_t millisecondsLSB;                  // Time Low
   // Structs for X,Y,Z data
   sfe_ism_raw_data_t   accelData;            // Low-g Accelerometer Raw Data
   sfe_ism_raw_data_t   gyroData;             // Gyroscope Raw Data
   mag_data_t           magData;              // Magnetometer Raw Data
   accel_high_data_t    high_g_data;          // High-g Accelerometer Raw Data
 };
 
const int union_size = sizeof(all_data_t);    // Size of the struct for union

union bitPacket_t{
  all_data_t allData;
  int16_t buf[union_size];                    // The union allocates space equal to the amount of data +  the size of the data as an array of int16_t
};
bitPacket_t data;                             // Union referred to as data from this point on


////////////////////////////////////////////////////////////////

void setup(void)
{
  data.allData.headerMSB = '%';
  data.allData.headerLSB = '%';

  
  pinMode(interrupt_pin, INPUT);

  Serial.begin(115200);   
  Wire.begin();
  Wire.setClock(400000);
  delay(3000);
  myLis.setI2CAddr(0x19);                   // This must be called before .begin()
  myLis.setODR(LIS331::DR_100HZ);                   // Sample at 100 Hz
  myLis.setFullScale(LIS331::LOW_RANGE);           // HIGH_RANGE sets to 400g
  myLis.begin(LIS331::USE_I2C);             // Uses I2C rather than SPI
  Serial.println("STARTUP ROUTINE");    // Usually does not print. Serial comms is async and maybe gets clobbered in the soft device startup. 
///////////////////////////////////////////////////////////////////////////
  if (!lipo.begin())
  {
      // If communication fails, print an error message and loop forever.
      Serial.println("Error: Unable to communicate with BMS.");
      delay(5000);
  }
  if (lipo.itporFlag()) //write config parameters only if needed
    {
        Serial.println("BMS: Writing gauge config");

  //Advanced config settings are untested. Using defaults if commented out.
        //lipo.enterConfig();                     // To configure the values below, you must be in config mode. 
        lipo.setCapacity(BATTERY_CAPACITY);       // Set the battery capacity
        //lipo.setGPOUTPolarity(LOW);             // Set GPOUT to active-low
        //lipo.setGPOUTFunction(SOC_INT);         // Set GPOUT to SOC_INT mode
        //lipo.setSOCIDelta(PERCENTAGE_INTERVAL); // Set percentage change integer.
        //lipo.setDesignEnergy(BATTERY_CAPACITY * 3.7f);
        //lipo.setTerminateVoltage(TERMINATE_VOLTAGE);
        //lipo.setTaperRate(10 * BATTERY_CAPACITY / TAPER_CURRENT);
        //lipo.exitConfig(); // Exit config mode to save changes
    }
    else
    {
        Serial.println("BMS: Using existing gauge config");
    }

    // Use lipo.GPOUTPolarity to read from the chip and confirm the changes
//    if (lipo.GPOUTPolarity())
//      Serial.println("BMS GPOUT set to active-HIGH");
//    else
//      Serial.println("BMS GPOUT set to active-LOW");
//
//    Serial.println("BMS SOCI Delta: " + String(lipo.sociDelta()));
    
///////////////////////////////////////////////////////////////////////////

  Bluefruit.autoConnLed(true);
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);
  //Bluefruit.configUuid128Count(15);
  Bluefruit.begin();
  Bluefruit.setTxPower(8);    //8 is max power (see bluefruit.h)
  //TxPower nRF52840: -40dBm, -20dBm, -16dBm, -12dBm, -8dBm, -4dBm, 0dBm, +2dBm, +3dBm, +4dBm, +5dBm, +6dBm, +7dBm and +8dBm.
  // Bluefruit.setInterval(6, 80);

  

  //Use Bluefruit.requestMtuExchange() to change packet size.

  Bluefruit.setName("Indigo Molar");      //(getMcuUniqueID()); // B5BBCD582C988315
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

//  myCharacteristic.setProperties(CHR_PROPS_NOTIFY);                 // Set characteristic to NOTIFY
  //myCharacteristic.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);  // Read is open, Write is closed
//  myCharacteristic.setFixedLen(sizeof(myCharacteristic));           // <- THIS IS NECESSARY
//  myCharacteristic.begin();


  
  // Start BLE Battery Service
  blebas.begin();
  Serial.println("BATTERY STATUS:");
  printBatteryStats();
  
  // Set up and start advertising
  startAdv();

//////////////////////////////////////////////////////////////////////////
  
  if( !myISM.begin() )
  {
    Serial.println("6dof ISM Did not begin.");
    while(1);
  }
  
  // Reset the device to default settings. This if helpful is you're doing multiple
  // uploads testing different settings. 
  myISM.deviceReset();
  myISM.resetSensorHub();

  while( !myISM.getDeviceReset() ){ 
    yield();
  } 

  Serial.println("6dof ISM Reset.");
  delay(100);
  
  myISM.setDeviceConfig();
  myISM.setBlockDataUpdate();
  
///////////////////////////////////////////////////////////////////////////
// Configure the interrupt pin for the "Measurement Done" interrupt
    pinMode(mag_int_pin, INPUT);
    //attachInterrupt(digitalPinToInterrupt(mag_int_pin), interruptRoutine, RISING);

    if (myMag.begin() == false)
    {
        Serial.println("MAGNETOMETER did not respond - check your wiring. Freezing.");
        while (true);
    }

    myMag.softReset();

    Serial.print("TEMP: ");
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
  
  // Uncommenting the function call below will change the interrupt to 
  // active LOW instead of HIGH.

  //myISM.setPinMode();

  // This function call will modify which "events" trigger an interrupt. No 
  // argument has been given, please refer to the datasheet for more 
  // information.

  // myISM.setIntNotification(uint8_t val);
}



void loop(){
//  if (digitalRead(GPOUT_PIN) == LOW)
//  {
//      printBatteryStats();    // SOC_INT occurred. Print battery stats.
//      blebas.notify(lipo.soc());      //State of charge: Current battery level as %.
//      
//  }

  
  while(!digitalRead(mag_int_pin))
  {yield;}    //wait for the int!

  myMag.clearMeasDoneInterrupt();                                        // Clear the MMC5983 interrupt
  uint32_t timenow = millis();                                           // uint32_t overflows at 49 days, uint16_t would overflow at 65.5 seconds.
  data.allData.millisecondsMSB = timenow >> 16;                      
  data.allData.millisecondsLSB = timenow & 0x0000FFFF;
  myMag.readFieldsXYZ(&rawValueX, &rawValueY, &rawValueZ);
  
  data.allData.magData.scaledX = rawValueX >> 2;
  data.allData.magData.scaledY = rawValueY >> 2;
  data.allData.magData.scaledZ = rawValueZ >> 2;

  if( (timenow - lasttime) > BAS_TIME_INTERVAL )
  {
    printBatteryStats();          // Print battery stats locally.
    blebas.notify(lipo.soc());    //State of charge: Current battery level as %.
    lasttime = timenow;
  }
              
  // Check if both gyroscope and accelerometer data is available.
  if( myISM.checkStatus() )
  {
    myISM.getRawAccel(&data.allData.accelData);                 // Low-g
    myISM.getRawGyro(&data.allData.gyroData);                   // Gyroscope
    myLis.readAxes(rawAccelX, rawAccelY, rawAccelZ);         // High-g
    data.allData.high_g_data.X = (uint16_t) rawAccelX;
    data.allData.high_g_data.Y = (uint16_t) rawAccelY;
    data.allData.high_g_data.Z = (uint16_t) rawAccelZ;
  }


//PRINT STATEMENTS.

  //myCharacteristic.notify(&data.buf, union_size);                 // Set characteristic message, and notify client

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
  connection->requestMtuExchange(100);    //(sizeof(myData) + 3)

  // request connection interval of 7.5 ms
  connection->requestConnectionParameter(6); // in unit of 1.25

  // delay a bit for all the request to complete
  delay(1000);
  
  // Serial.println(connection->getMtu());
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



void printBatteryStats()
{
    // Read battery stats from the BQ27441-G1A
    unsigned int soc = lipo.soc();                   // Read state-of-charge (%)
    unsigned int volts = lipo.voltage();             // Read battery voltage (mV)
    int current = lipo.current(AVG);                 // Read average current (mA)
    unsigned int fullCapacity = lipo.capacity(FULL); // Read full capacity (mAh)
    unsigned int capacity = lipo.capacity(REMAIN);   // Read remaining capacity (mAh)
    int power = lipo.power();                        // Read average power draw (mW)
    int health = lipo.soh();                         // Read state-of-health (%)

    // Assemble a string to print
    String toPrint = "[" + String(millis() / 1000) + "] ";
    toPrint += String(soc) + "% | ";
    toPrint += String(volts) + " mV | ";
    toPrint += String(current) + " mA | ";
    toPrint += String(capacity) + " / ";
    toPrint += String(fullCapacity) + " mAh | ";
    toPrint += String(power) + " mW | ";
    toPrint += String(health) + "%";

    //fast charging allowed
    if (lipo.chgFlag())
        toPrint += " CHG";

    //full charge detected
    if (lipo.fcFlag())
        toPrint += " FC";

    //battery is discharging
    if (lipo.dsgFlag())
        toPrint += " DSG";

    // Print the string
    Serial.println(toPrint);
}

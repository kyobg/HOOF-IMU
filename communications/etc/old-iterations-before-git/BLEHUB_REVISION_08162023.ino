/**************************************************************************
 * Author: Kyle Garcia                                                    *
 *                                                                        *
 * Title: Data Receiver using CLIENT Libraries                            *
 *                                                                        *
 * Purpose: To collect information from BLE Servers, which are the IMUs,  *
 * then output them into MATLAB for processing                            *
 **************************************************************************/

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>> LIBRARIES <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/

#include <Wire.h>
#include <bluefruit.h>
//#include <Adafruit_LittleFS.h>
//#include <InternalFileSystem.h>

// BLE Services Instantiation
BLEDfu bledfu;
BLEClientDis bledis;
BLEClientUart bleuart;
BLEClientBas blebas;

// A struct that contains the peripheral info
typedef struct
{
  char name[16+1];
  uint16_t connection_handle;
  BLEClientUart bleuart; // Each prph needs its own bleuart client service
  bool PACKET_FULL;
  uint16_t counter;
  int16_t newPacket[13];
} prph_info_t;

prph_info_t prphs[BLE_MAX_CONNECTION];
uint8_t connection_num = 0; // for blink pattern

void setup() {
  Serial.begin(115200);
  while(!Serial) delay(10); // for nrf5820 with native USB
  Bluefruit.configCentralBandwidth(BANDWIDTH_MAX);
  Bluefruit.begin(1, 1); // was 0, 4
  Bluefruit.setName("Central Denture"); // Set name of hub

//
//

  // initialize the peripheral pool, scan through and collect up to 20 BLE devices
  for (uint8_t index = 0; index < BLE_MAX_CONNECTION; index++)
  {
      // Invalidate all connetion handles
      prphs[index].connection_handle = BLE_CONN_HANDLE_INVALID;

      // All of the BLE Central UART Service
      prphs[index].bleuart.begin();
      prphs[index].bleuart.setRxCallback(bleuart_rx_callback);
  }
      // Callbacks for Central
      Bluefruit.Central.setConnectCallback(connect_callback);
      Bluefruit.Central.setDisconnectCallback(disconnect_callback);
       /* Start Central Scanning
   * - Enable auto scan if disconnected
   * - Interval = 100 ms, window = 80 ms
   * - Filter only accept bleuart service in advertising
   * - Don't use active scan (used to retrieve the optional scan response adv packet)
   * - Start(0) = will scan forever since no timeout is given
   */
    Bluefruit.Scanner.setRxCallback(scan_callback);
    Bluefruit.Scanner.restartOnDisconnect(true);
    Bluefruit.Scanner.setInterval(160, 80); // had it as 24, 12 //Originally 160 for fast mode       // in units of 0.625 ms // 24, 12
    Bluefruit.Scanner.filterUuid(BLEUART_UUID_SERVICE);
    Bluefruit.Scanner.useActiveScan(false);       // Don't request scan response data
    Bluefruit.Scanner.start(0);                   // 0 = Don't stop scanning after n seconds
}

/**
 * Callback invoked when scanner picks up an advertising packet
 * @param report Structural advertising data
 */
void scan_callback(ble_gap_evt_adv_report_t* report)
{
  // Since we configure the scanner with filterUuid()
  if (Bluefruit.Scanner.checkReportForService(report, peer->bleuart))
  {
    // Scan callback only invoked for device with bleuart service advertised  
    // Connect to the device with bleuart service in advertising packet
    Bluefruit.Central.connect(report);
  }
  else
  {
    Bluefruit.Scanner.resume();
  }
}

/*
 * Callback invoked when an connection is established
 * @param conn_handle
 */

void connect_callback(uint16_t conn_handle)
{
  // Find an available ID to use
  int id  = findConnHandle(BLE_CONN_HANDLE_INVALID);

  // Eeek: Exceeded the number of connections !!!
  if ( id < 0 ) return;
  
  prph_info_t* peer = &prphs[id];
  peer->connection_handle = conn_handle;
  
  Bluefruit.Connection(conn_handle)->getPeerName(peer->name, sizeof(peer->name)-1);

  Serial.print("Connected to ");
  Serial.println(peer->name);
  
  Serial.print("Discovering BLE UART service ... ");

  if ( peer->bleuart.discover(conn_handle) )
  {
    Serial.println("Found it");
    Serial.println("Enabling TXD characteristic's CCCD notify bit");
    peer->bleuart.enableTXD();
    //Bluefruit.Connection(conn_handle)->requestDataLengthUpdate();
    //Bluefruit.Connection(conn_handle)->requestMtuExchange(30);
    Serial.println("Continue scanning for more peripherals");
    Bluefruit.Scanner.start(0);

    Serial.println("Enter some text in the Serial Monitor to send it to all connected peripherals:");
  } else
  {
    Serial.println("Found ... NOTHING!");

    // disconnect since we couldn't find bleuart service
    Bluefruit.disconnect(conn_handle);
  }  

  connection_num++;
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  connection_num--;

  // Mark the ID as invalid
  int id  = findConnHandle(conn_handle);

  // Non-existant connection, something went wrong, DBG !!!
  if ( id < 0 ) return;

  // Mark conn handle as invalid
  prphs[id].connection_handle = BLE_CONN_HANDLE_INVALID;

  Serial.print(prphs[id].name);
  Serial.println(" disconnected!");
}

/**
 * Callback invoked when BLE UART data is received
 * @param uart_svc Reference object to the service where the data 
 * arrived.
 */
void bleuart_rx_callback(BLEClientUart& uart_svc)
{
  // uart_svc is prphs[conn_handle].bleuart
  uint16_t conn_handle = uart_svc.connHandle();
  int id = findConnHandle(conn_handle);
  prph_info_t* peer = &prphs[id];

  // Read then forward to all peripherals
  if ( uart_svc.available() )
  {
    // default MTU with an extra byte for string terminator <- CHANGED.
    char buf[20];
    int16_t packetSize = sizeof(buf);
    int16_t bytes = uart_svc.read(buf, packetSize);
    int16_t sentPacketSize = packetSize >> 1;
    int16_t data[sentPacketSize];
    for (int k = 0; k < sentPacketSize; k++){
      data[k] = (buf[k+k]) | (buf[(k+k)+1]<<8);
    }
  if (data[0] == 9509) {
    // Print sender's name
    //Serial.printf("[From %s]: ", peer->name);
    for (int i = 0; i < sentPacketSize; i++){
      peer->newPacket[i] = data[i];
    }
    peer->newPacket[0] = peer->connection_handle; // this will return the 
    peer->PACKET_FULL = 0;
  }
  if (data[0] == 16191) {
    for (int i = 1; i < 4; i++){
      peer->newPacket[i+9] = data[i]; 
    }
    peer->PACKET_FULL = 1;
  }
  }


  if (peer->PACKET_FULL) {
    for (int j = 0; j < 12; j++){
      Serial.print(peer->newPacket[j]);
      Serial.print("\t");
      peer->PACKET_FULL = 0;
    }
    Serial.print(peer->newPacket[12]);
    Serial.println();
  }
}

/**
 * Find the connection handle in the peripheral array
 * @param conn_handle Connection handle
 * @return array index if found, otherwise -1
 */
int findConnHandle(uint16_t conn_handle)
{
  for(int id=0; id<BLE_MAX_CONNECTION; id++)
  {
    if (conn_handle == prphs[id].connection_handle)
    {
      return id;
    }
  }

  return -1;  
}

void loop()
{  // First check if we are connected to any peripherals
  if ( Bluefruit.Central.connected() )
  {
    // default MTU with an extra byte for string terminator
    //char buf[24] = { 0 };
  }
}


/******************************************************************************
Demonstrates how to set up the BQ27441 and read state-of-charge (soc),
battery voltage, average current, remaining capacity, average power, and
state-of-health (soh).
After uploading, open up the serial monitor to 115200 baud to view your 
battery's stats.
Development environment specifics:
Arduino 1.8.19
SparkFun Battery Babysitter v1.0
******************************************************************************/
#include <SparkFunBQ27441.h>
// the capacity of our battery
const unsigned int BATTERY_CAPACITY = 650;
//MAIN CODE
void setup()
{
  Serial.begin(115200);
  setupBQ27441();
}
void loop() 
{
  printBatteryStats();
  delay(1000);
}
//battery charge and discharge
void setupBQ27441(void)
{
  if (!lipo.begin()) // begin() will return true if communication is successful
  {
  // If communication fails, print an error message and loop forever.
    Serial.println("Error: Unable to communicate with BQ27441.");
    Serial.println("  Check wiring and try again.");
    Serial.println("  (Battery must be plugged into Battery Babysitter!)");
    while (1) ;
  }
  Serial.println("Connected to BQ27441!");
  
  lipo.setCapacity(BATTERY_CAPACITY);
}

void printBatteryStats()
{
  // Read battery stats from the BQ27441-G1A
  unsigned int soc = lipo.soc();  // Read state-of-charge (%)
  unsigned int volts = lipo.voltage(); // Read battery voltage (mV)
  int current = lipo.current(AVG); // Read average current (mA)
  unsigned int fullCapacity = lipo.capacity(FULL); // Read full capacity (mAh)
  unsigned int capacity = lipo.capacity(REMAIN); // Read remaining capacity (mAh)
  int power = lipo.power(); // Read average power draw (mW)
  int health = lipo.soh(); // Read state-of-health (%)
  int tempK= lipo.temperature(); // Read state-of-health (.1K)
  float tempC = tempK*.1 -273.15; //change the temperature from kelvin it celsius
  
  //there is only an space between the data values to allow is to graph then in EXCEL
  String toPrint = String(soc) + " ";
  toPrint += String(volts) + " ";
  toPrint += String(current) + " ";
  toPrint += String(capacity) + " ";
  toPrint += String(fullCapacity) + " ";
  toPrint += String(power) + " ";
  toPrint += String(health) + " ";
  toPrint += String(tempC) + " ";
  Serial.println(toPrint);
}

//Elena code
#include <SparkFunBQ27441.h>
#include <Arduino.h>
#include <Adafruit_TinyUSB.h> // for Serial

const unsigned int BATTERY_CAPACITY = 650; 
const int numValues = 10;
int sensorValues[numValues];

void setupBQ27441(void)
{
  // Use lipo.begin() to initialize the BQ27441-G1A and confirm that it's
  // connected and communicating.
  if (!lipo.begin()) // begin() will return true if communication is successful
  {
	// If communication fails, print an error message and loop forever.
    Serial.println("Error: Unable to communicate with BQ27441.");
    Serial.println("  Check wiring and try again.");
    Serial.println("  (Battery must be plugged into Battery Babysitter!)");
    while (1) ;
  }
  Serial.println("Connected to BQ27441!");
  
  // Uset lipo.setCapacity(BATTERY_CAPACITY) to set the design capacity
  // of your battery.
  lipo.setCapacity(BATTERY_CAPACITY);

  for (int i = 0; i < numValues; i++) {
    sensorValues[i] = 0;
  }

}


void BatteryStats()
{
  // Read battery stats from the BQ27441-G1A
  unsigned int soc = lipo.soc();  // Read state-of-charge (%)*****************************************
  //we will have a trigger at 50 and 15 percent to charge, standard devitation for significant change in battery 
  unsigned int volts = lipo.voltage(); // Read battery voltage (mV)
  int current = lipo.current(AVG); // Read average current (mA)
  unsigned int fullCapacity = lipo.capacity(FULL); // Read full capacity (mAh)
  unsigned int capacity = lipo.capacity(REMAIN); // Read remaining capacity (mAh)
  int power = lipo.power(); // Read average power draw (mW)
  int health = lipo.soh(); // Read state-of-health (%)***********************************************
  //
 
}

void updateSensorValues(unsigned int newValue) {
  // Shift values in the array to make room for the new value
  for (int i = 0; i < numValues - 1; i++) {
    sensorValues[i] = sensorValues[i + 1];
  }

  // Add the new value to the array
  sensorValues[numValues - 1] = newValue;
}

float calculateStandardDeviation(int data[], int n) {
  float mean = 0.0, sum = 0.0, variance = 0.0, stdDeviation = 0.0;

  // Calculate mean
  for (int i = 0; i < n; i++) {
    sum += data[i];
  }
  mean = sum / n;

  // Calculate variance
  for (int i = 0; i < n; i++) {
    variance += pow(data[i] - mean, 2);
  }
  variance /= n;

  // Calculate standard deviation
  stdDeviation = sqrt(variance);

  return stdDeviation;
}


void loop() {
  // put your main code here, to run repeatedly:
  unsigned int soc = lipo.soc();  // Read state-of-charge (%)
  updateSensorValues(soc);


  delay(1000);

    if (numValues % 10 == 0) {
    float stdDeviation = calculateStandardDeviation(sensorValues, numValues);
    Serial.print("Standard Deviation: ");
    Serial.println(stdDeviation);



  if (soc == 50)
    Serial.println("battery at 50%");
  if (soc == 15)
    Serial.println("battery at 15%");

  }

}

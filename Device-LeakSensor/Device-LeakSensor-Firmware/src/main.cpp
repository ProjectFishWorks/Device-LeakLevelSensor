#include <Arduino.h>
#include <NodeControllerCore.h>

#define NODE_ID 0xB2              // 178
#define LEAK_DETECTED_MESSAGE_ID 0x0A00    // 2560
#define HIGH_WATER_LEVEL_MESSAGE_ID 0x0A01   // 2561

#define debuging

#define LEAK_SENSOR_PIN 4
#define LEVEL_SWITCH_PIN 2

#define MessageDelay 1000
#define MessageGap 5000


// Node controller core object
NodeControllerCore core;

// put function declarations here:

// Function to send message
void send_message(uint8_t nodeID, uint16_t messageID, uint64_t data);

// Function to check the leak sensor
void chkLeakSensor();

// Function to check the level switch
void chkLevelSwitch();


void setup() 
{
  // put your setup code here, to run once:

  // Initialize serial communication
  Serial.begin(115200);
  
  pinMode(LEVEL_SWITCH_PIN, INPUT_PULLUP);
  pinMode(LEAK_SENSOR_PIN, INPUT);

    // Create the node controller core object
  core = NodeControllerCore();
  Serial.println("test");
  // Initialize the node controller core object
  if (core.Init(send_message, NODE_ID))
  {
    Serial.println("Driver device initialized");
  }
  else
  {
    Serial.println("Failed to initialize driver");
  }
  delay(MessageDelay);
/*
xTaskCreate(chkLeakSensor,
            "chkLeakSensor",
            10000,
            NULL,
            1,
            NULL
            );
*/
}



void loop() 
{
  // put your main code here, to run repeatedly:
  chkLeakSensor();
  chkLevelSwitch();
  delay(MessageGap);
}

// put function definitions here:

void send_message(uint8_t nodeID, uint16_t messageID, uint64_t data)
{
  
  Serial.print("Received message with ID: ");
  Serial.println(messageID, HEX);
  Serial.print("Data: ");
  for (int i = 0; i < data; i++)
  {
    Serial.print(data);
    Serial.println(" ");
  }
  Serial.println();
  
}

// Function to check the leak sensor
void chkLeakSensor()
{
  Serial.println("Checking Leak Sensor");
  int sensorValue = analogReadMilliVolts(LEAK_SENSOR_PIN);
  
  Serial.println(sensorValue);
  if(sensorValue > 1)
  {
    #ifdef debuging
    Serial.println("Leak Detected");
    #endif
  }
  else
  {
    #ifdef debuging
    Serial.println("No Leak Detected");
    #endif
  }
  delay(MessageDelay);
}

// Function to check the level switch
void chkLevelSwitch()
{
  int levelSwitchValue = digitalRead(LEVEL_SWITCH_PIN);
  if(levelSwitchValue == LOW)
  {
    #ifdef debuging
    Serial.println("Level Switch is triggered");
    #endif
  }
  else
  {
    #ifdef debuging
    Serial.println("Level Switch is not triggered");
    #endif
  }
  delay(MessageDelay);
}
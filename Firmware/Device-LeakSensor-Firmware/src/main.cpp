#include <Arduino.h>
#include <NodeControllerCore.h>

#define NODE_ID 0xB2 // 178

#define LEAK_SENSOR1_SENSITIVITY_MESAGE_ID 2956 // 0xB8C
#define LEAK_SENSOR2_SENSITIVITY_MESAGE_ID 2957 // 0xB8D
#define LEAK_SENSOR3_SENSITIVITY_MESAGE_ID 2958 // 0xB8E
#define LEAK_SENSOR4_SENSITIVITY_MESAGE_ID 2959 // 0xB8F

#define LEAK_DETECTED_1_MESSAGE_ID 0x0A01 // 2561
#define LEAK_DETECTED_2_MESSAGE_ID 0x0A02 // 2562
#define LEAK_DETECTED_3_MESSAGE_ID 0x0A03 // 2563
#define LEAK_DETECTED_4_MESSAGE_ID 0x0A04 // 2564

#define HIGH_WATER_1_LEVEL_MESSAGE_ID 0x0B01 // 2817
#define HIGH_WATER_2_LEVEL_MESSAGE_ID 0x0B02 // 2818
#define HIGH_WATER_3_LEVEL_MESSAGE_ID 0x0B03 // 2819
#define HIGH_WATER_4_LEVEL_MESSAGE_ID 0x0B04 // 2820

#define ALARM_MESSAGE_ID 901

#define debuging

#define LEAK_SENSOR_PIN_1 0
#define LEAK_SENSOR_PIN_2 1
#define LEAK_SENSOR_PIN_3 2
#define LEAK_SENSOR_PIN_4 3

#define LEVEL_SWITCH_PIN_1 4
#define LEVEL_SWITCH_PIN_2 5
#define LEVEL_SWITCH_PIN_3 8
#define LEVEL_SWITCH_PIN_4 9

#define updateInterval 2000

int leakSensor1Sensitivity = 100;
int leakSensor2Sensitivity = 100;
int leakSensor3Sensitivity = 100;
int leakSensor4Sensitivity = 100;

uint8_t hasSentHighWaterLevelAlarm1 = 0;
uint8_t hasSentHighWaterLevelAlarm2 = 0;
uint8_t hasSentHighWaterLevelAlarm3 = 0;
uint8_t hasSentHighWaterLevelAlarm4 = 0;

uint8_t hasSentLeak1Alarm = 0;
uint8_t hasSentLeak2Alarm = 0;
uint8_t hasSentLeak3Alarm = 0;
uint8_t hasSentLeak4Alarm = 0;

uint8_t hasSentNoLeakAlarm1 = 0;
uint8_t hasSentNoLeakAlarm2 = 0;
uint8_t hasSentNoLeakAlarm3 = 0;
uint8_t hasSentNoLeakAlarm4 = 0;

uint8_t hasSentNoHighWaterLevelAlarm1 = 0;
uint8_t hasSentNoHighWaterLevelAlarm2 = 0;
uint8_t hasSentNoHighWaterLevelAlarm3 = 0;
uint8_t hasSentNoHighWaterLevelAlarm4 = 0;

uint8_t hasSentGeneralAlarm = 0;
uint8_t hasSentNoGeneralAlarm = 0;

// Node controller core object
NodeControllerCore core;

// put function declarations here:-------------------------------------------------------------------------------------------

// Function to send message
void messageReceived(uint8_t nodeID, uint16_t messageID, uint64_t data);

//
void checkForGlobalAlerts(void *parameters);

// Function to check the tank level sensor
void checkForLevelAlerts(void *parameters);

// Function to check the leak sensors
void checkForLeakAlerts(void *parameters);

void setup() //---------------------------------------------------------------------------------------------------------------
{
  // put your setup code here, to run once:

  // Initialize serial communication
  Serial.begin(115200);

  pinMode(LEVEL_SWITCH_PIN_1, INPUT_PULLUP);
  pinMode(LEVEL_SWITCH_PIN_2, INPUT_PULLUP);
  pinMode(LEVEL_SWITCH_PIN_3, INPUT_PULLUP);
  pinMode(LEVEL_SWITCH_PIN_4, INPUT_PULLUP);

  pinMode(LEAK_SENSOR_PIN_1, INPUT);
  pinMode(LEAK_SENSOR_PIN_2, INPUT);
  pinMode(LEAK_SENSOR_PIN_3, INPUT);
  pinMode(LEAK_SENSOR_PIN_4, INPUT);

  // Create the node controller core object
  core = NodeControllerCore();
  Serial.println("Create-> core = NodeControllerCore();");

  // Initialize the node controller core object
  if (core.Init(messageReceived, NODE_ID))
  {
    Serial.println("Node Contoler core.Init(messageReceived, NODE_ID");
    Serial.println("Driver device initialized");
  }
  else
  {
    Serial.println("Node Contoler core.Init(messageReceived, NODE_ID");
    Serial.println("Failed to initialize driver");
  }

  xTaskCreatePinnedToCore(
      checkForGlobalAlerts,   /* Function to implement the task */
      "checkForGlobalAlerts", /* Name of the task */
      10000,                  /* Stack size in words */
      NULL,                   /* Task input parameter */
      1,                      /* Priority of the task */
      NULL,                   /* Task handle. */
      1);

  xTaskCreatePinnedToCore(
      checkForLevelAlerts, /* Function to implement the task */
      "checkForAlerts",    /* Name of the task */
      10000,               /* Stack size in words */
      NULL,                /* Task input parameter */
      1,                   /* Priority of the task */
      NULL,                /* Task handle. */
      1);
  xTaskCreatePinnedToCore(
      checkForLeakAlerts,   /* Function to implement the task */
      "checkForLeakAlerts", /* Name of the task */
      10000,                /* Stack size in words */
      NULL,                 /* Task input parameter */
      1,                    /* Priority of the task */
      NULL,                 /* Task handle. */
      1);
}

void loop()
{
}

// put function definitions here:

void messageReceived(uint8_t nodeID, uint16_t messageID, uint64_t data)
{
  Serial.print("Received message with ID: ");
  Serial.println(messageID, HEX);
  Serial.print("Data: " + String(data));
  Serial.println();

  // Check if the message is for this node
  if (nodeID == NODE_ID)
  {
    Serial.println("Message is for this node");

    if (messageID == LEAK_SENSOR1_SENSITIVITY_MESAGE_ID)
    {
      leakSensor1Sensitivity = data;
      Serial.println("Leak Sensor 1 Sensitivity = " + String(leakSensor1Sensitivity));
    }
    else if (messageID == LEAK_SENSOR2_SENSITIVITY_MESAGE_ID)
    {
      leakSensor2Sensitivity = data;
      Serial.println("Leak Sensor 2 Sensitivity = " + String(leakSensor2Sensitivity));
    }
    else if (messageID == LEAK_SENSOR3_SENSITIVITY_MESAGE_ID)
    {
      leakSensor3Sensitivity = data;
      Serial.println("Leak Sensor 3 Sensitivity = " + String(leakSensor3Sensitivity));
    }
    else if (messageID == LEAK_SENSOR4_SENSITIVITY_MESAGE_ID)
    {
      leakSensor4Sensitivity = data;
      Serial.println("Leak Sensor 4 Sensitivity = " + String(leakSensor4Sensitivity));
    }
  }
  else
  {
    Serial.println("Message is not for this node");
    return;
  }
}

// Function to check the tank level sensor
void checkForGlobalAlerts(void *parameters)
{
  Serial.println("Checking Tank Level Switch Task");

  uint64_t generalErrorData1 = 0x1;
  uint64_t generalErrorData0 = 0x0;

  while (1)
  {
    uint8_t levelSwitchStatus1 = (digitalRead(LEVEL_SWITCH_PIN_1));
    uint8_t levelSwitchStatus2 = (digitalRead(LEVEL_SWITCH_PIN_2));
    uint8_t levelSwitchStatus3 = (digitalRead(LEVEL_SWITCH_PIN_3));
    uint8_t levelSwitchStatus4 = (digitalRead(LEVEL_SWITCH_PIN_4));

    int leakSensorStatus1 = analogReadMilliVolts(LEAK_SENSOR_PIN_1);
    Serial.println("Leak Sensor 1 = " + String(leakSensorStatus1));
    int leakSensorStatus2 = analogReadMilliVolts(LEAK_SENSOR_PIN_2);
    Serial.println("Leak Sensor 2 = " + String(leakSensorStatus2));
    int leakSensorStatus3 = analogReadMilliVolts(LEAK_SENSOR_PIN_3);
    Serial.println("Leak Sensor 3 = " + String(leakSensorStatus3));
    int leakSensorStatus4 = analogReadMilliVolts(LEAK_SENSOR_PIN_4);
    Serial.println("Leak Sensor 4 = " + String(leakSensorStatus4));

    if (leakSensorStatus1 > leakSensor1Sensitivity ||
        leakSensorStatus2 > leakSensor2Sensitivity ||
        leakSensorStatus3 > leakSensor3Sensitivity ||
        leakSensorStatus4 > leakSensor4Sensitivity ||
        levelSwitchStatus1 == 0 ||
        levelSwitchStatus2 == 0 ||
        levelSwitchStatus3 == 0 ||
        levelSwitchStatus4 == 0)
    {
      if (hasSentGeneralAlarm == 0)
      {
        core.sendMessage(ALARM_MESSAGE_ID, &generalErrorData0);
        core.sendMessage(ALARM_MESSAGE_ID, &generalErrorData1);
        hasSentGeneralAlarm = 1;
        hasSentNoGeneralAlarm = 0;
#ifdef debuging
        Serial.println("General Alarm sent!!!!");
#endif
      }
      else
      {
#ifdef debuging
        Serial.println("General Alarm Not sent");
#endif
      }
    }
    else
    {
      if (hasSentNoGeneralAlarm == 0)
      {
        core.sendMessage(ALARM_MESSAGE_ID, &generalErrorData0);
        hasSentGeneralAlarm = 0;
        hasSentNoGeneralAlarm = 1;
#ifdef debuging
        Serial.println("No General Alarm not sent");
#endif
      }
      else
      {
#ifdef debuging
        Serial.println("No General Alarm not sent");
#endif
      }
    }
    delay(updateInterval);
  }
}

void checkForLevelAlerts(void *parameters)
{

  uint64_t errorData1 = 0x1;
  uint64_t errorData0 = 0x0;

  Serial.println("Checking Tank Level Switch Task");

  while (1)
  {
    uint8_t levelSwitchStatus1 = (digitalRead(LEVEL_SWITCH_PIN_1));
    uint8_t levelSwitchStatus2 = (digitalRead(LEVEL_SWITCH_PIN_2));
    uint8_t levelSwitchStatus3 = (digitalRead(LEVEL_SWITCH_PIN_3));
    uint8_t levelSwitchStatus4 = (digitalRead(LEVEL_SWITCH_PIN_4));

    if (levelSwitchStatus1 == 0)
    {
      if (hasSentHighWaterLevelAlarm1 == 0)
      {
        core.sendMessage(HIGH_WATER_1_LEVEL_MESSAGE_ID, (uint64_t)0);
        core.sendMessage(HIGH_WATER_1_LEVEL_MESSAGE_ID, &errorData1);
        hasSentHighWaterLevelAlarm1 = 1;
        hasSentNoHighWaterLevelAlarm1 = 0;
      }
#ifdef debuging
      Serial.println("Tank 1 is High");
#endif
    }
    else
    {
      if (hasSentNoHighWaterLevelAlarm1 == 0)
      {
        core.sendMessage(HIGH_WATER_1_LEVEL_MESSAGE_ID, (uint64_t)0);
        hasSentHighWaterLevelAlarm1 = 0;
        hasSentNoHighWaterLevelAlarm1 = 1;
      }
#ifdef debuging
      Serial.println("Tank 1 level is normal");
#endif
    }

    if (levelSwitchStatus2 == 0)
    {
      if (hasSentHighWaterLevelAlarm2 == 0)
      {
        core.sendMessage(HIGH_WATER_2_LEVEL_MESSAGE_ID, (uint64_t)0);
        core.sendMessage(HIGH_WATER_2_LEVEL_MESSAGE_ID, &errorData1);
        hasSentHighWaterLevelAlarm2 = 1;
        hasSentNoHighWaterLevelAlarm2 = 0;
      }
#ifdef debuging
      Serial.println("Tank 2 is High");
#endif
    }
    else
    {
      if (hasSentNoHighWaterLevelAlarm2 == 0)
      {
        core.sendMessage(HIGH_WATER_2_LEVEL_MESSAGE_ID, (uint64_t)0);
        hasSentHighWaterLevelAlarm2 = 0;
        hasSentNoHighWaterLevelAlarm2 = 1;
      }
#ifdef debuging
      Serial.println("Tank 2 level is normal");
#endif
    }

    if (levelSwitchStatus3 == 0)
    {
      if (hasSentHighWaterLevelAlarm3 == 0)
      {
        core.sendMessage(HIGH_WATER_3_LEVEL_MESSAGE_ID, (uint64_t)0);
        core.sendMessage(HIGH_WATER_3_LEVEL_MESSAGE_ID, &errorData1);
        hasSentHighWaterLevelAlarm3 = 1;
        hasSentNoHighWaterLevelAlarm3 = 0;
      }
#ifdef debuging
      Serial.println("Tank 3 is High");
#endif
    }
    else
    {
      if (hasSentNoHighWaterLevelAlarm3 == 0)
      {
        core.sendMessage(HIGH_WATER_3_LEVEL_MESSAGE_ID, (uint64_t)0);
        hasSentHighWaterLevelAlarm3 = 0;
        hasSentNoHighWaterLevelAlarm3 = 1;
      }
#ifdef debuging
      Serial.println("Tank 3 level is normal");
#endif
    }

    if (levelSwitchStatus4 == 0)
    {
      if (hasSentHighWaterLevelAlarm4 == 0)
      {
        core.sendMessage(HIGH_WATER_4_LEVEL_MESSAGE_ID, (uint64_t)0);
        core.sendMessage(HIGH_WATER_4_LEVEL_MESSAGE_ID, &errorData1);
        hasSentHighWaterLevelAlarm4 = 1;
        hasSentNoHighWaterLevelAlarm4 = 0;
      }
#ifdef debuging
      Serial.println("Tank 4 is High");
#endif
    }
    else
    {
      if (hasSentNoHighWaterLevelAlarm4 == 0)
      {
        core.sendMessage(HIGH_WATER_4_LEVEL_MESSAGE_ID, (uint64_t)0);
        hasSentHighWaterLevelAlarm4 = 0;
        hasSentNoHighWaterLevelAlarm4 = 1;
      }
#ifdef debuging
      Serial.println("Tank 4 level is normal");
#endif
    }

    delay(updateInterval);
  }
}

void checkForLeakAlerts(void *parameters)
{

  uint64_t errorData1 = 0x1;
  uint64_t errorData0 = 0x0;

  while (1)
  {

    int leakSensorStatus1 = analogReadMilliVolts(LEAK_SENSOR_PIN_1);
    Serial.println("Leak Sensor 1 = " + String(leakSensorStatus1));
    int leakSensorStatus2 = analogReadMilliVolts(LEAK_SENSOR_PIN_2);
    ;
    Serial.println("Leak Sensor 2 = " + String(leakSensorStatus2));
    int leakSensorStatus3 = analogReadMilliVolts(LEAK_SENSOR_PIN_3);
    ;
    Serial.println("Leak Sensor 3 = " + String(leakSensorStatus3));
    int leakSensorStatus4 = analogReadMilliVolts(LEAK_SENSOR_PIN_4);
    ;
    Serial.println("Leak Sensor 4 = " + String(leakSensorStatus4));

    if (leakSensorStatus1 > leakSensor1Sensitivity)
    {
      if (hasSentLeak1Alarm == 0)
      {
        core.sendMessage(LEAK_DETECTED_1_MESSAGE_ID, (uint64_t)0);
        core.sendMessage(LEAK_DETECTED_1_MESSAGE_ID, &errorData1);
        hasSentLeak1Alarm = 1;
        hasSentNoLeakAlarm1 = 0;
      }
#ifdef debuging
      Serial.println("Leak 1 detected");
#endif
    }
    else
    {
      if (hasSentNoLeakAlarm1 == 0)
      {
        core.sendMessage(LEAK_DETECTED_1_MESSAGE_ID, (uint64_t)0);
        hasSentLeak1Alarm = 0;
        hasSentNoLeakAlarm1 = 1;
      }
#ifdef debuging
      Serial.println("No leak 1 detected");
#endif
    }

    if (leakSensorStatus2 > leakSensor2Sensitivity)
    {
      if (hasSentLeak2Alarm == 0)
      {
        core.sendMessage(LEAK_DETECTED_2_MESSAGE_ID, (uint64_t)0);
        core.sendMessage(LEAK_DETECTED_2_MESSAGE_ID, &errorData1);
        hasSentLeak2Alarm = 1;
        hasSentNoLeakAlarm2 = 0;
      }
#ifdef debuging
      Serial.println("Leak 2 detected");
#endif
    }
    else
    {
      if (hasSentNoLeakAlarm2 == 0)
      {
        core.sendMessage(LEAK_DETECTED_2_MESSAGE_ID, (uint64_t)0);
        hasSentLeak2Alarm = 0;
        hasSentNoLeakAlarm2 = 1;
      }
#ifdef debuging
      Serial.println("No leak 2 detected");
#endif
    }

    if (leakSensorStatus3 > leakSensor3Sensitivity)
    {
      if (hasSentLeak3Alarm == 0)
      {
        core.sendMessage(LEAK_DETECTED_3_MESSAGE_ID, (uint64_t)0);
        core.sendMessage(LEAK_DETECTED_3_MESSAGE_ID, &errorData1);
        hasSentLeak3Alarm = 1;
        hasSentNoLeakAlarm3 = 0;
      }
#ifdef debuging
      Serial.println("Leak 3 detected");
#endif
    }
    else
    {
      if (hasSentNoLeakAlarm3 == 0)
      {
        core.sendMessage(LEAK_DETECTED_3_MESSAGE_ID, (uint64_t)0);
        hasSentLeak3Alarm = 0;
        hasSentNoLeakAlarm3 = 1;
      }
#ifdef debuging
      Serial.println("No leak 3 detected");
#endif
    }

    if (leakSensorStatus4 > leakSensor4Sensitivity)
    {
      if (hasSentLeak4Alarm == 0)
      {
        core.sendMessage(LEAK_DETECTED_3_MESSAGE_ID, (uint64_t)0);
        core.sendMessage(LEAK_DETECTED_4_MESSAGE_ID, &errorData1);
        hasSentLeak4Alarm = 1;
        hasSentNoLeakAlarm4 = 0;
      }
#ifdef debuging
      Serial.println("Leak 4 detected");
#endif
    }
    else
    {
      if (hasSentNoLeakAlarm4 == 0)
      {
        core.sendMessage(LEAK_DETECTED_4_MESSAGE_ID, (uint64_t)0);
        hasSentLeak4Alarm = 0;
        hasSentNoLeakAlarm4 = 1;
      }
#ifdef debuging
      Serial.println("No leak 4 detected");
#endif
    }
    delay(updateInterval);
  }
}
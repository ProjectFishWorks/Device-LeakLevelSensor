#include <Arduino.h>
#include <NodeControllerCore.h>

#define NODE_ID 0xB2                       // 178
#define LEAK_DETECTED_MESSAGE_ID 0x0A00    // 2560
#define HIGH_WATER_LEVEL_MESSAGE_ID 0x0A01 // 2561
#define PUMP_MAX_SPEED_MESSAGE_ID 0x0A02   // 2562
#define PUMP_MIN_SPEED_MESSAGE_ID 0x0A03   // 2563
#define ALARM_MESSAGE_ID 901               //

#define debuging

#define LEAK_SENSOR_PIN 4
#define LEVEL_SWITCH_PIN 10
#define PUMP_IN1_PIN 8
#define PUMP_IN2_PIN 9
#define PUMP_PWM_PIN 5
int levelSwitchStatus;
int leakSensorStatus;
int minPumpSpeed = 50;
int maxPumpSpeed = 155;

#define sendMessageDelay 1000
#define MessageGap 2000

// Node controller core object
NodeControllerCore core;

// put function declarations here:

// Function to send message
void send_message(uint8_t nodeID, uint16_t messageID, uint64_t data);

// Function to enable the pump
void pumpEnabled();

// Function to disable the pump
void pumpDisabled();

// Function to check the tank level sensor
void chkTankLevelSwitch();

// Function to chk the leak sensor
//void chkLeakSensor();

void setup()
{
  // put your setup code here, to run once:

  // Initialize serial communication
  Serial.begin(115200);

  pinMode(LEVEL_SWITCH_PIN, INPUT_PULLUP);
  pinMode(LEAK_SENSOR_PIN, INPUT);
  pinMode(PUMP_IN1_PIN, OUTPUT);
  pinMode(PUMP_IN2_PIN, OUTPUT);
  pinMode(PUMP_PWM_PIN, OUTPUT);

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
  delay(sendMessageDelay);
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
  chkTankLevelSwitch();
  //delay(MessageGap);
  //digitalWrite(PUMP_IN1_PIN, 1);
  //digitalWrite(PUMP_IN2_PIN, 1);
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

void pumpEnabled()
{
  digitalWrite(PUMP_IN1_PIN, HIGH);
  digitalWrite(PUMP_IN2_PIN, LOW);
}

void pumpDisabled()
{
  digitalWrite(PUMP_IN1_PIN, LOW);
  digitalWrite(PUMP_IN2_PIN, LOW);
}

// Function to check the tank level sensor
void chkTankLevelSwitch()
{
  levelSwitchStatus = digitalRead(LEVEL_SWITCH_PIN);
  leakSensorStatus = analogReadMilliVolts(LEAK_SENSOR_PIN);
  if (levelSwitchStatus == 1 )//&& leakSensorStatus > 150)
  {
    pumpEnabled();
    analogWrite(PUMP_PWM_PIN, minPumpSpeed);

#ifdef debuging
    Serial.println("Tank is High");
    Serial.println("Pump is reduced speed");
    Serial.print("pumpIN1   ");
    //Serial.print(digitalRead(PUMP_IN1_PIN));
    Serial.print("    pumpIN2  ");
    //Serial.println(digitalRead(PUMP_IN2_PIN));
    Serial.print("Leak Sensor =  ");
    Serial.println(leakSensorStatus);
    Serial.println("");
#endif
  }
  else if(levelSwitchStatus == 1 )//&& leakSensorStatus > 150)
  {
    pumpEnabled();
    analogWrite(PUMP_PWM_PIN, maxPumpSpeed);

#ifdef debuging
    Serial.println("Tank level is normal");
    Serial.println("Pump is enabled");
    Serial.print("pumpIN1   ");
    //Serial.print(digitalRead(PUMP_IN1_PIN));
    Serial.print("    pumpIN2   ");
    //Serial.println(digitalRead(PUMP_IN2_PIN));
    Serial.print("Leak Sensor =  ");
    Serial.println(leakSensorStatus);
    Serial.println("");
#endif
  }
}

// Function to chk the leak sensor
void chkLeakSensor()
{
  leakSensorStatus = analogReadMilliVolts(LEAK_SENSOR_PIN);
  if (leakSensorStatus > 150)
  {
    pumpDisabled();
    analogWrite(PUMP_PWM_PIN, 0);

#ifdef debuging
    Serial.println("Leak detected");
    Serial.println("Pump is disabled");
    Serial.print("pumpIN1   ");
    //Serial.print(digitalRead(PUMP_IN1_PIN));
    Serial.print("    pumpIN2  ");
    //Serial.println(digitalRead(PUMP_IN2_PIN));
    Serial.print("Leak Sensor =  ");
    Serial.println(leakSensorStatus);
    Serial.println("");
#endif
  }
  else
  {
    pumpEnabled();
    analogWrite(PUMP_PWM_PIN, maxPumpSpeed);

#ifdef debuging
    Serial.println("No leaks detected");
    Serial.println("Pump is enabled");
    Serial.print("pumpIN1   ");
    //Serial.print(digitalRead(PUMP_IN1_PIN));
    Serial.print("    pumpIN2   ");
    //Serial.println(digitalRead(PUMP_IN2_PIN));
    Serial.print("Leak Sensor =  ");
    Serial.println(leakSensorStatus);
    Serial.println("");
#endif
  }
}
/*
// Function to send message
void SendTempHumMessage(void *parameters)
{
  while (1)
  {
    if (canopyTempAlarmOnOff == 1)
    {
      if (CanopyTemp >= canopyTempAlarmHigh || CanopyTemp <= canopyTempAlarmLow)
      {
        Serial.println("------------Canopy Temp Message triggered");
        Serial.println("Has sent alarm is = " + String(hasSentCanTempAlarm));
        if (hasSentCanTempAlarm == 0)
        {
          core.sendMessage(ALARM_MESSAGE_ID, &errorData1);
          hasSentCanTempAlarm = 1;
          hasSentNoCanTempAlarm = 0;
          Serial.println("-----------Not Can Temp Alarm  = " + String(hasSentCanTempAlarm));
        }
      }
      else
      {
        if (hasSentNoCanTempAlarm == 0)
        {
          core.sendMessage(ALARM_MESSAGE_ID, &errorData0);
          hasSentCanTempAlarm = 0;
          hasSentNoCanTempAlarm = 1;
          Serial.println("Canopy Temperture no alarm");
          Serial.println("Canopy Temperture = " + String(CanopyTemp));
        }
      }
    }
  }
}
*/
/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/Vishal/Documents/CTDProjects/Captsone/src/Captsone.ino"
/*
 * Project Captsone
 * Description:
 * Author:
 * Date:
 */
void myThread1(void *args);
void myThread2(void *args);
void myThread3(void *args);
void setup();
void loop();
#line 7 "c:/Users/Vishal/Documents/CTDProjects/Captsone/src/Captsone.ino"
SYSTEM_THREAD(ENABLED);

// libraries
#include <blynk.h>
#include "MQTT.h"
#include "SparkFun_VCNL4040_Arduino_Library.h"

// MQTT
void callback(char *topic, byte *payload, unsigned int length);

uint64_t reading;
double voltage;
double temperature;
unsigned int proxValue;
unsigned int ambientValue;
MQTT client("lab.thewcl.com", 1883, callback);
os_thread_t thread1;
os_thread_t thread2;
os_thread_t thread3;
VCNL4040 proximitySensor;
// bool interruptOccured = false;

const size_t UART_TX_BUF_SIZE = 20;


const BleUuid serviceUuid("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
const BleUuid rxUuid("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
const BleUuid txUuid("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");


// recieve from MQTT
void callback(char *topic, byte *payload, unsigned int length)
{
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  // if ((String)topic == "groupB/triggerInterrupt")
  // {
  //   Serial.println(p[0]);
  //   if (p[0] == '0')
  //   {
  //     // interruptOccured = false;
  //     digitalWrite(D7, LOW);
  //   }
  //   else if(p[0] == '1')
  //   {
  //     // interruptOccured = true;
  //     digitalWrite(D7, HIGH);
  //   }
  // }
}

// THREADS

// TEMP
void myThread1(void *args)
{
  while (1)
  {
    // if (interruptOccured == false)
    // {
      // temp in farenheit
      reading = analogRead(A5);
      voltage = (reading * 3.3) / 4095.0;   //converts the reading to a voltage value
      temperature = (voltage - 0.5) * 100;  //converts the voltage value to temperature in celsius
      temperature = temperature * 1.8 + 32; //converts to farenheit
      delay(1000);
    // }
  }
}

// prox values
void myThread2(void *args)
{
  while (1)
  {
    // if (interruptOccured == false)
    // {
      proxValue = proximitySensor.getProximity();
      delay(1000);
    // }
  }
}

// light values
void myThread3(void *args)
{
  while (1)
  {
    // if (interruptOccured == false)
    // {
      ambientValue = proximitySensor.getAmbient();
      delay(1000);
    // }
  }
}

void setup()
{
  Serial.begin(9600);
  Blynk.begin("W0CPcWAtiUAoJquOpPU_hkXKvNsr3xAQ", IPAddress(167, 172, 234, 162), 9090);
  pinMode(D7, OUTPUT); //LED for interupts
  pinMode(A5, INPUT);  // TMP36

  os_thread_create(&thread1, "thread1", OS_THREAD_PRIORITY_DEFAULT, myThread1, NULL, 1024);
  os_thread_create(&thread1, "thread2", OS_THREAD_PRIORITY_DEFAULT, myThread2, NULL, 1024);
  os_thread_create(&thread1, "thread3", OS_THREAD_PRIORITY_DEFAULT, myThread3, NULL, 1024);

  proximitySensor.begin();
  proximitySensor.powerOnProximity();
  proximitySensor.powerOnAmbient();

  if (proximitySensor.begin() == false)
  {
    Serial.println("Device not found. Please check wiring.");
    while (1); //Freeze!
  }

  BLE.on();

  BleAdvertisingData data;
  data.appendServiceUUID(serviceUuid);
  BLE.advertise(&data);
}

void loop()
{
  Blynk.run();
  client.loop();
  // if(interruptOccured == false){

    if (client.isConnected() && BLE.connected())
    {

      Blynk.virtualWrite(V0, temperature);  //update temp gauge on blynk
      Blynk.virtualWrite(V1, proxValue);    //update prox gauge on blynk
      Blynk.virtualWrite(V2, ambientValue); //update light gauge on blynk

      client.publish("groupB/vishalTemp", (String)temperature);
      client.publish("groupB/vishalProx", (String)proxValue);
      client.publish("groupB/vishalLux", (String)ambientValue);

      delay(1000);

    }
    else if(!client.isConnected())
    {
      client.connect(System.deviceID());
      client.subscribe("groupB/triggerInterrupt");
      client.subscribe("groupB/vishalTemp");
      client.subscribe("groupB/vishalProx");
      client.subscribe("groupB/vishalLux");
    }

    else if(!BLE.connected()){
        client.publish("groupB/triggerInterrupt", "Vishal's BLE is disconnected");
        delay(2000);
    }
  // }
}
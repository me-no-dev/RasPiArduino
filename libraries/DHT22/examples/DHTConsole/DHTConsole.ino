#include <DHT22.h>
// Only used for sprintf
#include <stdio.h>

// Data wire is plugged into port 7 on the Arduino
// Connect a 4.7K resistor between VCC and the data pin (strong pullup)
#define DHT22_PIN 7

// Setup a DHT22 instance
DHT22 myDHT22(DHT22_PIN);

void setup()
{
  Console.println("DHT22 Library Demo");
}

void loop()
{ 
  DHT22_ERROR_t errorCode;
  
  // The sensor can only be read from every 1-2s, and requires a minimum
  // 2s warm-up after power-on.
  delay(2000);
  
  Console.print("Requesting data...");
  errorCode = myDHT22.readData();
  switch(errorCode)
  {
    case DHT_ERROR_NONE:
      Console.print("Got Data ");
      Console.print(myDHT22.getTemperatureC());
      Console.print("C ");
      Console.print(myDHT22.getHumidity());
      Console.println("%");
      // Alternately, with integer formatting which is clumsier but more compact to store and
	  // can be compared reliably for equality:
	  //	  
      char buf[128];
      sprintf(buf, "Integer-only reading: Temperature %hi.%01hi C, Humidity %i.%01i %% RH",
                   myDHT22.getTemperatureCInt()/10, abs(myDHT22.getTemperatureCInt()%10),
                   myDHT22.getHumidityInt()/10, myDHT22.getHumidityInt()%10);
      Console.println(buf);
      break;
    case DHT_ERROR_CHECKSUM:
      Console.print("check sum error ");
      Console.print(myDHT22.getTemperatureC());
      Console.print("C ");
      Console.print(myDHT22.getHumidity());
      Console.println("%");
      break;
    case DHT_BUS_HUNG:
      Console.println("BUS Hung ");
      break;
    case DHT_ERROR_NOT_PRESENT:
      Console.println("Not Present ");
      break;
    case DHT_ERROR_ACK_TOO_LONG1:
    case DHT_ERROR_ACK_TOO_LONG2:
    case DHT_ERROR_ACK_TOO_LONG3:
      Console.println("ACK time out ");
      break;
    case DHT_ERROR_SYNC_TIMEOUT:
      Console.println("Sync Timeout ");
      break;
    case DHT_ERROR_DATA_TIMEOUT:
      Console.println("Data Timeout ");
      break;
    case DHT_ERROR_TOOQUICK:
      Console.println("Polled to quick ");
      break;
  }
}
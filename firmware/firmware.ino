#include <SPI.h>
#include <HttpClient.h>
#include <Ethernet.h>
#include <EthernetClient.h>

// Name of the server we want to connect to
const char kHostname[] = "api.metro.net";
// Path to download (this is the bit after the hostname in the URL
// that you want to download
const char kPath[] = "/agencies/lametro-rail/stops/80136/predictions/";

byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x32, 0xFE };

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30 * 1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1000;

void getData() {
  int err = 0;
  boolean endBrace = false;
  boolean readingJson = true;
  char json[400];
  int index = 0;
  int countBrace = 0;
  EthernetClient c;
  HttpClient http(c);

  err = http.get(kHostname, kPath);

  if (err == 0) {
    Serial.println("startedRequest ok");
    err = http.responseStatusCode();
    if (err >= 0) {
      Serial.print("Got status code: ");
      Serial.println(err);
      err = http.skipResponseHeaders();
      if (err >= 0) {
        int bodyLen = http.contentLength();
        Serial.print("Content length is: ");
        Serial.println(bodyLen);
        Serial.println();
        Serial.println("Body returned follows:");

        // Now we've got to the body, so we can print it out
        unsigned long timeoutStart = millis();
        char c;

        // Whilst we haven't timed out & haven't reached the end of the body
        while ( (http.connected() || http.available()) && ((millis() - timeoutStart) < kNetworkTimeout) ) {
          c = http.read();
          // if quotes, add a slash then quotes
          if (c == '\"' ) {
            // insert slash
            json[index] = '\\';
            index++;
            // Store it
            json[index] = c;
            index++;
          } else if (c == '}') { // if end bracket, add bracket and then quotes and break loop
            // Store it
            json[index] = c;
            index++;
            json[index] = '\"';
            index++;
            endBrace = true;
          } else { // if no special characters, add to array
            // Store it
            json[index] = c;
            index++;
          }

          // second bracket, add quotes
          if (c == "{" && countBrace == 1) {
            json[index] = '\"';
            index++;
            json[index] = c; // Store it
            index++;
            readingJson = true;
            countBrace == 2;
          }

          // first brace detected, skipped
          if (c == "{" && countBrace == 0) {
            countBrace = 1;
          }

          bodyLen--;
          // We read something, reset the timeout counter
          timeoutStart = millis();
          } else {
            // We haven't got any data, so let's pause to allow some to arrive
            delay(kNetworkDelay);
          }

          //checking index
          Serial.println(" ");
          Serial.println("index is ");
          Serial.println(index);
          Serial.println(" ");

          // checking char stored
          Serial.println(" ");
          Serial.println("json READ ");
          Serial.println(" ");

          for (int i = 0; i < index; i++) {
            Serial.print(json[i]);
          } else {
            Serial.print("Failed to skip response headers: ");
            Serial.println(err);
          }

      } else {
          Serial.print("Getting response failed: ");
          Serial.println(err);
        }
    } else {
        Serial.print("Connect failed: ");
        Serial.println(err);
      }

      http.stop();

      // And just stop, now that we've tried a download
      while (1);
}

void setup()
{
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);

  while (Ethernet.begin(mac) != 1)
  {
    Serial.println("Error getting IP address via DHCP, trying again...");
    delay(15000);
  }

  // get data from api.metro.com
  getData();
}

void loop()
{
  // not used
}


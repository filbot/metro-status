#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>

#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

const char* ssid     = "sm-1337";
const char* password = "hacktheplanet";

const char* host = "metro-data.herokuapp.com";

int value = 0;

void setup() {
  Serial.begin(115200);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3D);

  // Clear the buffer.
  display.clearDisplay();
  display.display();

  delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println(".");
    display.display();
    delay(1);
    display.startscrollright(0x00, 0x0F);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  delay(60000);
  display.clearDisplay();
  display.display();
  ++value;

  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  String url = "/";

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 60000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  boolean currentLineIsBlank = true;
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    char c = client.read();
    if (c == '\n' && currentLineIsBlank) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.setTextSize(1);
      display.println("");
      display.println("Trains every");
      display.println("");
      display.setTextSize(3);
      display.setCursor(18, 20);
      display.println(line);
      display.setTextSize(1);
      display.setCursor(14, 45);
      display.println("minutes");
      display.display();
      delay(1);
      display.startscrollright(0x00, 0x0F);
    }
    if (c == '\n') {
      // you're starting a new line
      currentLineIsBlank = true;
    } else if (c != '\r') {
      // you've gotten a character on the current line
      currentLineIsBlank = false;
    }
  }

  Serial.println();
  Serial.println("closing connection");
}

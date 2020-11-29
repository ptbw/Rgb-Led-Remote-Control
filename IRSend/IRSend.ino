
 /*
 * Common mistakes & tips:
 *   * Don't just connect the IR LED directly to the pin, it won't
 *     have enough current to drive the IR LED effectively.
 *   * Make sure you have the IR LED polarity correct.
 *     See: https://learn.sparkfun.com/tutorials/polarity/diode-and-led-polarity
 *   * Typical digital camera/phones can be used to see if the IR LED is flashed.
 *     Replace the IR LED with a normal LED if you don't have a digital camera
 *     when debugging.
 *   * Avoid using the following pins unless you really know what you are doing:
 *     * Pin 0/D3: Can interfere with the boot/program mode & support circuits.
 *     * Pin 1/TX/TXD0: Any serial transmissions from the ESP8266 will interfere.
 *     * Pin 3/RX/RXD0: Any serial transmissions to the ESP8266 will interfere.
 *   * ESP-01 modules are tricky. We suggest you use a module with more GPIOs
 *     for your first time. e.g. ESP-12 etc.
 * 
 *   Based on example from ESPWebServer
 */
#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <WiFiClient.h>

#include <RgbIrLed.h>

RgbIrLed rgbled(1);

const char* ssid = "ChangeMe";
const char* password = "ChangeMe";
MDNSResponder mdns;

ESP8266WebServer server(80);

IRsend irsend(4);  // An IR LED is controlled by GPIO pin 4 (D2)

// kFrequency is the modulation frequency all messages will be replayed at.
const uint16_t kFrequency = 38000;  // in Hz. e.g. 38kHz.

void handleRoot() {
  server.send(200, "text/html",
              "<html>" \
                "<head><title>ESP8266 Demo</title></head>" \
                "<body>" \
                  "<h1>Hello from ESP8266, you can send NEC encoded IR" \
                      "signals from here!</h1>" \
                  "<p><a href=\"ir?code=on\">On</a></p>" \
                  "<p><a href=\"ir?code=red\">Send Red</a></p>" \
                  "<p><a href=\"ir?code=green\">Send Green</a></p>" \
                  "<p><a href=\"ir?code=blue\">Send Blue</a></p>" \
                  "<p><a href=\"ir?code=smooth\">Smooth</a></p>" \
                  "<p><a href=\"ir?code=fade\">Fade</a></p>" \
                "</body>" \
              "</html>");
}

void sendCode(const String code)
{
  if (code == "on")
    irsend.sendNEC(rgbled.On, 32);  
  if (code == "red")
    irsend.sendNEC(rgbled.Red,32);
  if (code == "blue")
    irsend.sendNEC(rgbled.Blue,32);
  if (code == "green")
    irsend.sendNEC(rgbled.Green,32);
  if (code == "smooth")
    irsend.sendNEC(rgbled.Smooth,32);
  if (code == "fade")
    irsend.sendNEC(rgbled.Fade,32);
}

void handleIr() {
  for (uint8_t i = 0; i < server.args(); i++) {
    if (server.argName(i) == "code")
      sendCode(server.arg(i));
      
//    if (server.argName(i) == "code" && server.arg(i) == "red") {      
//      Serial.println("Red");
//      irsend.sendNEC(rgbled.On, 32);
//      irsend.sendNEC(rgbled.Red,32);
//    }
//    if (server.argName(i) == "code" && server.arg(i) == "green") {      
//      Serial.println("Green");
//      irsend.sendNEC(rgbled.On, 32);
//      irsend.sendNEC(rgbled.Green,32);
//    }
//    if (server.argName(i) == "code" && server.arg(i) == "blue") {      
//      Serial.println("Blue");
//      irsend.sendNEC(rgbled.On, 32);
//      irsend.sendNEC(rgbled.Blue,32);
//    }
//    if (server.argName(i) == "code" && server.arg(i) == "smooth") {      
//      Serial.println("Smooth");
//      irsend.sendNEC(rgbled.On, 32);
//      irsend.sendNEC(rgbled.Smooth,32);
//    }
  }
  handleRoot();
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++)
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  server.send(404, "text/plain", message);
}

void setup(void) {
  irsend.begin();

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (mdns.begin("esp8266", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/ir", handleIr);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}

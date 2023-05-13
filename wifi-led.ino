#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// Replace with your network credentials
const char* ssid = "Jio fiber 257";
const char* password = "xxxxxxxxx";

ESP8266WebServer server(80);


void handleRoot() {
  String html = "<html><body>";
  html += "<h1>Color Picker</h1>";
  html += "<input type='color' id='colorPicker' onchange='updateColor(this.value)' value='#FFFFFF '>";
  html += "<script>";
  html += "function updateColor(color) {";
  html += "  var xhr = new XMLHttpRequest();";
  html += "  xhr.open('GET', '/color?color=' + encodeURIComponent(color), true);";
  html += "  xhr.send();";
  html += "}";
  html += "</script>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleColor() {
  if (server.hasArg("color")) {
    String colorValue = server.arg("color");
    String red = String((int) strtol(&colorValue[1], NULL, 16) >> 16, HEX);
    String green = String((int) strtol(&colorValue[1], NULL, 16) >> 8 & 0xFF, HEX);
    String blue = String((int) strtol(&colorValue[1], NULL, 16) & 0xFF, HEX);
    String rgbValue = red + "," + green + "," + blue;

    // Convert RGB values to integers
    int redValue = strtol(red.c_str(), NULL, 16);
    int greenValue = strtol(green.c_str(), NULL, 16);
    int blueValue = strtol(blue.c_str(), NULL, 16);

    // Set analog values for RGB pins
    analogWrite(2, redValue);
    analogWrite(14, greenValue);
    analogWrite(12, blueValue);

    server.send(200, "text/plain", "RGB value: " + rgbValue);
  } else {
    server.send(400, "text/plain", "Invalid request");
  }
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Set up mDNS responder with the hostname "esp8266"
  if (!MDNS.begin("esp8266")) {
    Serial.println("Error setting up mDNS responder!");
  }

  // Print ESP8266 IP address
  Serial.println("");
  Serial.print("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/color", handleColor);

  server.begin();
  Serial.println("HTTP server started");

  // Advertise the mDNS service
  MDNS.addService("http", "tcp", 80);
}

void loop() {
  server.handleClient();
}

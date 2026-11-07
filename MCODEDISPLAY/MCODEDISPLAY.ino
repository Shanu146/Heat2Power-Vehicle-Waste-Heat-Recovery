#include <Arduino_GFX_Library.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "bike_bitmap.h"


// =====================================================
// WIFI
// =====================================================

const char* WIFI_SSID = "Shan's Galaxy";
const char* WIFI_PASSWORD = "Sunshine";

WiFiUDP udp;
const uint16_t UDP_PORT = 4210;


// =====================================================
// LCD - YOUR PROVEN WORKING CONFIGURATION
// DO NOT CHANGE
// =====================================================

Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(

  // LCD control
  5,    // DE
  3,    // VSYNC
  46,   // HSYNC
  7,    // PCLK

  // RED
  1,
  2,
  42,
  41,
  40,

  // GREEN
  39,
  0,
  45,
  48,
  47,
  21,

  // BLUE
  14,
  38,
  18,
  17,
  10,

  // Horizontal timing
  0,
  40,
  48,
  88,

  // Vertical timing
  0,
  13,
  3,
  32,

  // Pixel clock
  1,
  16000000,

  false,
  0,
  0,
  0
);


Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
  800,
  480,
  rgbpanel,
  0,
  true
);


// =====================================================
// SENSOR VALUES
// =====================================================

float objectTemp = 0.0;
float ambientTemp = 0.0;
float voltage = 0.0;

unsigned long lastPacketTime = 0;


// =====================================================
// WIFI CONNECTION
// =====================================================

void connectWiFi()
{
  Serial.print("Connecting to WiFi");

  WiFi.mode(WIFI_STA);

  // Prevent WiFi power saving from interfering
  // with continuous UDP communication.
  WiFi.setSleep(false);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected!");

  Serial.print("Display ESP32 IP: ");
  Serial.println(WiFi.localIP());

  udp.begin(UDP_PORT);

  Serial.print("UDP listening on port ");
  Serial.println(UDP_PORT);
}


// =====================================================
// DRAW DASHBOARD
// =====================================================

void drawDashboard()
{
  // ===================================================
  // BACKGROUND
  // ===================================================

  gfx->fillScreen(0x0000);


  // ===================================================
  // TITLE
  // ===================================================

  gfx->setTextColor(0x07FF);
  gfx->setTextSize(4);

  // Shortened so it fits the 800 pixel screen
  gfx->setCursor(35, 25);
  gfx->println("BIKE SYSTEM - TEAM SYNTARA");


  // ===================================================
  // TOP DIVIDER
  // ===================================================

  gfx->drawFastHLine(
    30,
    80,
    740,
    0x07FF
  );


  // ===================================================
  // OBJECT TEMPERATURE
  // ===================================================

  gfx->setTextColor(0xFFFF);
  gfx->setTextSize(2);

  gfx->setCursor(60, 125);
  gfx->println("OBJECT TEMP");

  gfx->setTextColor(0xF800);
  gfx->setTextSize(5);

  gfx->setCursor(60, 160);

  gfx->print(objectTemp, 1);
  gfx->println(" C");


  // ===================================================
  // AMBIENT TEMPERATURE
  // ===================================================

  gfx->setTextColor(0xFFFF);
  gfx->setTextSize(2);

  gfx->setCursor(300, 125);
  gfx->println("AMBIENT");

  gfx->setTextColor(0xFFE0);
  gfx->setTextSize(5);

  gfx->setCursor(300, 160);

  gfx->print(ambientTemp, 1);
  gfx->println(" C");


  // ===================================================
  // VOLTAGE
  // ===================================================

  gfx->setTextColor(0xFFFF);
  gfx->setTextSize(2);

  gfx->setCursor(540, 125);
  gfx->println("VOLTAGE");

  gfx->setTextColor(0x07E0);
  gfx->setTextSize(5);

  gfx->setCursor(540, 160);

  gfx->print(voltage, 2);
  gfx->println(" V");


  // ===================================================
  // SECOND DIVIDER
  // ===================================================

  gfx->drawFastHLine(
    30,
    270,
    740,
    0x07FF
  );


  // ===================================================
  // SENSOR STATUS
  // ===================================================

  gfx->setTextSize(3);

  if (millis() - lastPacketTime < 10000)
  {
    gfx->setTextColor(0x07E0);

    gfx->setCursor(60, 320);
    gfx->println("SENSOR ONLINE");
  }
  else
  {
    gfx->setTextColor(0xF800);

    gfx->setCursor(60, 320);
    gfx->println("SENSOR OFFLINE");
  }


  // ===================================================
  // NETWORK INFORMATION
  // ===================================================

  gfx->setTextColor(0x8410);
  gfx->setTextSize(2);

  gfx->setCursor(60, 390);

  gfx->print("UDP: ");
  gfx->print(UDP_PORT);


  gfx->setCursor(60, 430);

  gfx->print("IP: ");
  gfx->print(WiFi.localIP());


  // ===================================================
  // MOTORCYCLE IMAGE
  // LOWER RIGHT
  // ===================================================

  gfx->draw16bitRGBBitmap(
    500,              // X
    305,              // Y
    bikeImage,
    BIKE_IMAGE_W,
    BIKE_IMAGE_H
  );
}


// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(115200);

  delay(2000);

  Serial.println();
  Serial.println("==============================");
  Serial.println("      BIKE DASHBOARD");
  Serial.println("==============================");


  // ===================================================
  // LCD INITIALIZATION
  // ===================================================

  if (!gfx->begin())
  {
    Serial.println("LCD INIT FAILED!");

    while (true)
    {
      delay(1000);
    }
  }

  Serial.println("LCD INIT OK");


  // ===================================================
  // STARTING SCREEN
  // ===================================================

  gfx->fillScreen(0x0000);

  gfx->setTextColor(0x07FF);
  gfx->setTextSize(4);

  gfx->setCursor(220, 210);
  gfx->println("M CODE START...");

  delay(1000);


  // ===================================================
  // WIFI
  // ===================================================

  connectWiFi();


  // ===================================================
  // INITIAL STATE
  // ===================================================

  lastPacketTime = millis();

  drawDashboard();
}


// =====================================================
// LOOP
// =====================================================

void loop()
{
  // ===================================================
  // WIFI RECONNECT
  // ===================================================

  if (WiFi.status() != WL_CONNECTED)
  {
    connectWiFi();
  }


  // ===================================================
  // RECEIVE UDP PACKET
  // ===================================================

  int packetSize = udp.parsePacket();

  if (packetSize)
  {
    char packet[100];

    int len = udp.read(
      packet,
      sizeof(packet) - 1
    );

    if (len > 0)
    {
      packet[len] = '\0';


      // =================================================
      // EXPECTED PACKET:
      //
      // objectTemp,ambientTemp,voltage
      //
      // Example:
      // 37.52,28.31,12.64
      // =================================================

      float newObjectTemp;
      float newAmbientTemp;
      float newVoltage;


      int result = sscanf(
        packet,
        "%f,%f,%f",
        &newObjectTemp,
        &newAmbientTemp,
        &newVoltage
      );


      // =================================================
      // VALID PACKET
      // =================================================

      if (result == 3)
      {
        objectTemp = newObjectTemp;

        ambientTemp = newAmbientTemp;

        voltage = newVoltage;

        lastPacketTime = millis();

        drawDashboard();
      }
    }
  }


  // ===================================================
  // SENSOR TIMEOUT
  // ===================================================

  if (millis() - lastPacketTime > 10000)
  {
    static unsigned long lastOfflineDraw = 0;

    if (millis() - lastOfflineDraw > 1000)
    {
      drawDashboard();

      lastOfflineDraw = millis();
    }
  }


  // ===================================================
  // SMALL LOOP DELAY
  // ===================================================

  delay(10);
}
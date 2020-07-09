#include <ILI9341_t3.h>
#include <ILI9341_t3_PrintScreen_SdFat.h>

#define TFT_DC  9
#define TFT_CS 10
#define LED_PIN A9
#define SD_CS 7

ILI9341_t3 Display = ILI9341_t3(TFT_CS, TFT_DC);


int i;

void setup() {

  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);


  Display.begin();

  Display.setRotation(1);
  Display.fillScreen(ILI9341_BLACK);
  digitalWrite(LED_PIN, HIGH);

  for (i = 0; i < 200; i += 4) {
    Display.drawRect(i, i, 200 - i, 200 - i, ILI9341_RED);
  }

  Serial.print("Saving file...");
  SaveBMP24(&Display, SD_CS, "ExampleSdFat.BMP");
  Serial.print("file saved.");

  Serial.print("Drawing BMP file...");
  Display.fillScreen(ILI9341_BLACK);
  DrawBMP24(&Display, SD_CS, "ExampleSdFat.BMP");
  Serial.print("Drawing complete.");

}

void loop() {


}
#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SD.h>

/* Connect HC-05 TX to Arduino RX and HC-05 RX to Arduino TX */
SoftwareSerial Bluetooth(26, 27); /* RX, TX */

File f;

#define OLED_MOSI   32    //11
#define OLED_CLK    33    //13
#define OLED_DC     30    //39
#define OLED_CS     29    //38
#define OLED_RESET  31    //10
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


bool dir = 0;
int disp_len = 21;
int scrolling_speed = 300;
String msg = "KEOA DK-42";


void setup() 
{ 
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println(msg);
  display.display();
  
  Serial.begin(9600);
  Serial.println("Board is ready");

  if (!SD.begin(52)) {
    Serial.println("sd card initialization failed!");
  }
  Serial.println("sd card initialized");
  f = SD.open("1.txt");
  if (f) {
    Serial.println("1.txt:");
    // read from the file until there's nothing else in it:
    if (f.available()) {
      msg = f.readString();
    }
    msg.concat(" ");
    while (msg.length() < disp_len) {
      msg.concat(" ");
    }
    
    // close the file:
    f.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening 1.txt");
  }
 
  Bluetooth.begin(9600);  
  Serial.println("Bluetooth is ready");
  delay(500); 
}


void loop()
{
  String displayed = msg.substring(0,disp_len); 
  if (Bluetooth.available()) {
    String inp = Bluetooth.readString();
    if (inp == "->") {
      dir = 1;
    } else if (inp == "<-") {
      dir = 0;
    } else {
      msg = inp;
      msg.concat(" ");
    }
  }
  while (msg.length() < disp_len) {
    msg.concat(" ");
   }
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(displayed);
  display.display();
  if (dir) {
    msg = msg.charAt(msg.length()-1) + msg.substring(0, msg.length()-1);
  } else { 
    msg = msg.substring(1, msg.length()) + msg.charAt(0);
  }
  delay(scrolling_speed);
}

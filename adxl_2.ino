#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
# include <ThingSpeak.h>
// BUZZER PIN
const int buzzer_pin = D6;  //pin connected to buzzer
// LED PIN
const int led_pin_red = D5; // pin connected to LED
const int led_pin_green = D7; // pin connected to red led
const int button_pin = D8;
// TEMPERATURE SENSOR PIN
const int lm35 = A0;
// current state of the push button
int button_state = 0;
// THRESHOLD VALUE
const float threshold = 8.0; // setting up threshold value for accelerometer
const  float x = 0;
const float y = 0;
const float z = 0;
// initializing the lcd screen
LiquidCrystal_I2C lcd(0x27, 16, 2);
// Wi-Fi credentials and thingspeak connections
const char* ssid = "OnePlus Nord";
const char* password = "avanig1834";
const char* host = "maker.ifttt.com";
WiFiClient client;
unsigned long channel_number = 2197854;
const char* API_key = "D8Z585ICCMUBQL07";
const char* server = "api.thingspeak.com";
// ADXL345
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

void setup(void) {
  Serial.begin(115200);
  Serial.println("email from nodemcu");
  delay(100);
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  while (!Serial) {
  delay(10);}
  Serial.println("Accelerometer Test");
  Serial.println("");
  if (!accel.begin()) {
    Serial.println("Could not find a valid ADXL345 sensor, check wiring!");
    while (1);}
  //  set up the buzzer pin as an output
  pinMode(buzzer_pin, OUTPUT);
  // set up the led pin as an output
  pinMode(led_pin_red, OUTPUT);
  pinMode(led_pin_green, OUTPUT);
  // set up the push button
  pinMode(button_pin, INPUT);
  // set up the tempearature sensor
  pinMode(lm35, INPUT);
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
}
  // ip address of nodemcu
  Serial.println("Connected to WiFi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  // initializing lcd
  lcd.init(); 
  lcd.clear();
  lcd.backlight(); // turn on the backlight
  lcd.setCursor(0,0);
  lcd.print("ACCIDENT");
  lcd.setCursor(0,1);
  lcd.print("DETECTION");
}

void loop(void) {
  

          WiFiClient client; 
          const int httpPort = 80;  
            if (!client.connect(host, httpPort)) {  
                  Serial.println("connection failed");  
            return;} 
          if(Serial.available())  {
                        char a=Serial.read();
            
            if (a=='y'){
    
          
                    String url = "trigger/accident_detected/with/key/dJ2Wj4ucAs8wrHqGq5THK4"; 

          
                    Serial.print("Requesting URL: ");
                    Serial.println(url);
                 
                     client.print(String("GET ") + url + " HTTP/1.1\r\n" + 
                                    "Host: " + host + "\r\n" +   
                                           "Connection: close\r\n\r\n");    
                                    }  
          
          
          else{
            Serial.println("Correct character not pressed.Try again");
            
          }

          }
  sensors_event_t event;
  accel.getEvent(&event);
  
  float x = event.acceleration.x;
  float y = event.acceleration.y;
  float z = event.acceleration.z;

  Serial.print("X: "); Serial.print(x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(z); Serial.print("  ");
  Serial.println("m/s^2 ");
  ThingSpeak.writeField(2197854, 2, x, "D8Z585ICCMUBQL07");
  ThingSpeak.writeField(2197854, 3, y, "D8Z585ICCMUBQL07");
  ThingSpeak.writeField(2197854, 4, z, "D8Z585ICCMUBQL07");
  if(x > threshold || y > threshold || z > threshold)
  {
    button_state = digitalRead(button_pin);
    if(button_state == 1)
    {
      digitalWrite(led_pin_green, HIGH);
      delay(200);
    }
    // message on lcd screen
    lcd.init(); 
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("accident");
    lcd.setCursor(0,1);
    lcd.print("detected");
    
    // message on serial monitor
    Serial.println("accident detected!");
    // buzzer buzzes
    soundBuzzer();
    // blink the RED LED
    lightLED_red();
    // send email
    
    delay(500);
  }
  
  else
  { 
    lcd.init(); 
    lcd.clear();
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("no accident");
    lcd.setCursor(0,1);
    lcd.print("detected");
    // blink the GREEN LED
    lightLED_green();
    delay(500);
  }

  // reading temperature value
  int lm_value = analogRead(lm35);
  float temp = ((lm_value*3)/10)+26.5;
  Serial.print("temp: ");
  Serial.print(temp);
  Serial.print(" celcius");
  Serial.println("\xC2\xB0");
  delay(500);
  if(temp>30.0)
  {
    soundBuzzer();
    lightLED_red();
    delay(500);
  }
  // sending temperature sensore data to thingspeak
  ThingSpeak.writeField(2197854, 1, temp, "D8Z585ICCMUBQL07");
  delay(500);
}
void soundBuzzer()
{
  tone(buzzer_pin, 400, 400);
}
void lightLED_red()
{
  digitalWrite(led_pin_red, HIGH);
  delay(300);
  digitalWrite(led_pin_red, LOW);
  delay(300);
}
void lightLED_green()
{
  digitalWrite(led_pin_green, HIGH);
  delay(300);
  digitalWrite(led_pin_green, LOW);
  delay(300);
}

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <cactus_io_DHT22.h>
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE); /* Abre o LiquidCrystal_I2C na local 0x27 do barramento*/

/* RTC BARRAMENTO = 0x68 */
#define DS3231_I2C_ADDRESS 0x68
unsigned long time0;

/* DHT 22 */
#define DHT22_PIN 2
DHT22 dht(DHT22_PIN); /*Intialize DHT22 */

/* SD CARD READER */
#include <SPI.h>
#include <SD.h>
const int chipSelect = 10; // indica o pino digital onde está o chip select do leitor de cartão
File dataFile; // Declara um objeto do tipo File;

void setup() {
// CODE THAT WILL ONLY RUN ONCE
lcd.begin (16,2);
dht.begin();
Serial.begin(9600);
Wire.begin();
Serial.print("Initializing SD card module...\n");
   if (!SD.begin(chipSelect)) {
    lcd.print("SDCARD NOT FOUND");
    delay(5000);
  }
Serial.print("Card recognized.");
sucellusMsg();


}

void loop() {
// MAIN CODE - WILL REPEAT FOREVER
//setDS3231time(45,26,16,1,18,11,18);

lcd.setCursor(0,0);
lcd.clear();
dht.readHumidity();   // MUST NOT BE CALLED WITHIN SPAN OF LESS THAN 2 secs
dht.readTemperature(); // same

if (isnan(dht.humidity) || isnan(dht.temperature_C)) {
  lcd.print("DHT sensor read failure!");
  return;
 }

// SOMETIMES BEING ELEGANT JUST TAKES TOO MUCH TIME. I HOPE NO ONE SEES THIS.
lcd.setCursor(0,0); lcd.print("HR = "); lcd.setCursor(6,0); lcd.print(dht.humidity);
lcd.setCursor(0,1); lcd.print("TempC = "); lcd.setCursor(9,1); lcd.print(dht.temperature_C);
Serial.print(dht.humidity); Serial.print(" %\t\t");
Serial.print(dht.temperature_C); Serial.print(" *C\t");
Serial.print(readTime()); Serial.print("\t\n");  
delay(4000);
lcd.clear();
lcd.setCursor(0,0);
lcd.print(readTime());
lcd.setCursor(0,1);
lcd.print(readDate());
delay(2000);


logData("sucellus.txt", dht.humidity, dht.temperature_C, readTime(), readDate());



}

/* FUNCTIONS FOR RTC MODULE: https://tronixstuff.com/2014/12/01/tutorial-using-ds1307-and-ds3231-real-time-clock-modules-with-arduino/  */
/* ===============================================*/
/*                                                */
// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
  return( (val/10*16) + (val%10) );
}
// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
  return( (val/16*10) + (val%16) );
}
// SET TIME 
void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte
dayOfMonth, byte month, byte year)
{
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}

void readDS3231time(byte *second,
byte *minute,
byte *hour,
byte *dayOfWeek,
byte *dayOfMonth,
byte *month,
byte *year)
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}


String readTime ()
{
   byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231

  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);
  
String concatTime = "";
concatTime = "";
if(hour<10){
  concatTime.concat("0");
  concatTime.concat(hour);
  
} else { 
concatTime.concat(hour);
}

concatTime.concat(":");

if(minute<10){
  concatTime.concat("0");
  concatTime.concat(minute);
} else {
  concatTime.concat(minute);
}
concatTime.concat(":"); 
if(second<10){
  concatTime.concat("0");
  concatTime.concat(second);
} else {
concatTime.concat(second);
}

  return concatTime;

}

String readDate ()
  {
   byte second, minute, hour, dayOfWeek, dayOfMonth, month, year;
  // retrieve data from DS3231

  readDS3231time(&second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month,
  &year);
  
String concatDate = "";
concatDate = "";
concatDate.concat(year);
concatDate.concat("-");
concatDate.concat(month);
concatDate.concat("-");
concatDate.concat(dayOfMonth);

return concatDate;

}
/*               OTHER FUNCTIONS                  */
/* ===============================================*/
/*                                                */
void sucellusMsg() 
{
  lcd.clear();
  lcd.setCursor(5,0); lcd.print("ONDE HA"); delay(1000);
  lcd.setCursor(3,0); lcd.print("UMA VONTADE"); delay(1000);
  lcd.setCursor(1,0); lcd.print("HA UM CAMINHO"); delay(1000);
  lcd.clear(); 
  lcd.setCursor(4,0); lcd.print("SUCELLUS"); 
  lcd.setCursor(3,1); lcd.print("innovation");
  delay(4000);
}

void logData(String fileName, float fileHumidity, float fileTemperature, String fileTime, String fileDate) 
{
  String dataLine ="";
  dataLine.concat(fileHumidity); dataLine.concat(";");
  dataLine.concat(fileTemperature); dataLine.concat(";");
  dataLine.concat(fileTime); dataLine.concat(";");
  dataLine.concat(fileDate); dataLine.concat(";");

  File dataFile = SD.open(fileName, FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataLine);
    dataFile.close();
    // print to the serial port too:
    Serial.println(dataLine);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening file");
    lcd.clear(); lcd.print("FILE NOT FOUND");
    delay(1000);
  }
}

/* MATH FUNCTIONS */
/* ============== */

float average (float * array, int len)  // assuming array is int.
{
  long sum = 0L ;  // sum will be larger than an item, long for safety.
  for (int i = 0 ; i < len ; i++)
    sum += array [i] ;
  return  ((float) sum) / len ;  // average will be fractional, so float may be appropriate.
}



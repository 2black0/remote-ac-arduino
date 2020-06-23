// library yang dipakai
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <RTClib.h>
#include <SPI.h>
#include <SD.h>
#include <IRremote.h>
#include <ezButton.h>

// definisi pin dan tipe dari DHT
#define DHTPIN 8
#define CSPIN 53
#define BUTTON1PIN 2 // active low
#define BUTTON2PIN 3 // active low
#define DHTTYPE DHT22

// instance name dari library yang dipakai
LiquidCrystal_I2C lcd(0x27, 20, 4);
DHT dht(DHTPIN, DHTTYPE);
RTC_DS3231 rtc;
File dataFile;
IRsend irsend;
ezButton button1(BUTTON1PIN);
ezButton button2(BUTTON2PIN);

// deklrasi variable
int set_point = 25;
int low_temp = 20;
int khz=38;
int t_later = 0;
int atur_ac = 0;

int h, t;
String date, time, suhu_ac_C;
String dataSimpan;
String setDate, setTime;

int count;
int period = 1000;
unsigned long time_now = 0;

// deklarasi variable untuk kendali AC (Panasonic)
unsigned ON[]    = {3400,1800,350,500,350,1350,400,500,350,500,350,500,350,500,350,500,350,500,400,450,400,500,350,500,400,450,400,500,350,1350,350,500,350,500,350,500,400,500,350,500,350,500,350,500,350,1350,400,1350,350,1350,350,500,400,500,350,1350,350,500,350,500,400,500,350,500,350,500,350,500,350,500,350,500,400,450,400,450,400,500,350,500,350,500,400,500,350,500,350,500,350,500,350,500,400,500,350,500,350,500,350,500};
unsigned OFF[]   = {3400,1800,350,500,400,1300,400,450,400,450,400,450,400,500,350,500,400,450,400,450,400,450,450,450,400,450,400,450,400,1350,350,500,400,450,400,450,400,450,400,450,400,450,400,500,350,1350,400,1300,400,1350,350,500,400,450,400,1350,400,450,400,450,400,450,400,450,450,400,400,500,350,500,400,450,400,450,400,450,400,450,400,500,350,500,400,450,400,500,350,500,400,450,400,450,400,450,400,450,400,450,400,500};
unsigned ON_16[] = {800,200,1000,3000,200,550,300,1450,250,650,200,700,150,650,250,600,250,700,150,650,250,600,250,700,150,600,300,600,200,2300,250,4950,150,700,150,3300,200,1500,250,750,150,700,200,1400,250,600,300,600,250,650,200,550,250,750,150,650,200,1500,200,2450,100,650,250,4100,200,550,300,750,100,600,300,550,250,2400,200,2300,300,650,200,3300,200,1400,250,600,250,1550,200,550,300,600,250};
unsigned ON_17[] = {1800,150,1400,1850,300,600,200,1550,250,500,300,650,250,700,150,550,150,1700,200,550,250,600,300,500,300,650,250,650,200,1450,300,550,300,550,300,650,250,600,200,750,100,700,100,650,300,1350,350,1450,250,1450,300,600,250,700,100,1500,250,750,150,600,250,700,200,550,250,600,300,550,250,650,250,500,350,550,300,650,200,600,250,600,250,700,200,550,250,750,200,650,200,600,250,550,200,650,300,600,250,650,200,650};
unsigned ON_18[] = {350,200,2700,1950,300,550,350,1450,200,650,150,1500,250,700,150,700,250,600,200,600,250,700,200,600,300,550,250,700,100,1500,300,550,300,650,250,650,150,600,300,650,200,600,250,600,250,1500,150,1550,200,1450,150,800,250,600,100,1500,350,600,250,600,250,600,300,550,300,650,150,650,250,600,200,650,250,600,300,600,250,600,250,600,250,700,200,550,250,600,300,550,200,850,100,800,100,550,250,650,250,600,200,650};
unsigned ON_19[] = {950,150,850,200,250,150,400,2300,250,2400,150,600,250,1550,200,600,300,550,250,650,250,550,100,700,350,600,250,700,200,2250,250,3250,250,600,50,1700,200,1550,150,1450,300,1450,200,700,150,800,100,1450,300,750,100,1450,300,500,300,600,300,1500,150,700,200,600,250,700,100,700,250,550,200,650,300,700,250,550,200,650,100,1550,300,1550,200,650,100,700,250,650,100,700,250,650,100,650,300,600,350,650,200,550,200,650};
unsigned ON_20[] = {3350,1850,250,650,250,1400,350,550,250,650,250,600,200,750,200,600,200,650,200,550,350,550,300,550,300,600,250,650,250,1400,300,600,250,650,100,700,250,550,350,650,200,600,300,550,250,1450,250,1500,250,1350,350,600,300,550,150,1600,300,500,300,600,250,600,300,650,200,550,300,650,200,600,200,1500,100,1650,250,700,150,600,300,600,250,600,300,600,250,600,200,750,150,600,200,700,200,650,150,700,250,550,100,700};
unsigned ON_21[] = {3400,1750,400,450,400,1350,400,450,400,450,400,450,400,450,400,500,350,500,400,450,400,450,400,500,400,450,400,450,400,1300,400,450,400,500,350,500,400,450,400,450,400,450,400,450,400,1350,400,1300,400,1300,400,450,450,450,400,1300,400,500,450,400,400,450,400,450,400,450,400,450,400,500,400,450,400,450,450,400,400,450,400,450,400,450,450,450,400,450,400,450,400,500,350,500,400,450,400,450,400,450,400,450};
unsigned ON_22[] = {3400,1750,400,450,400,1350,350,500,350,500,400,450,400,450,400,500,350,450,400,500,350,500,400,450,400,500,350,500,400,1300,400,450,400,450,400,500,350,500,350,500,400,450,400,450,400,1300,400,1350,400,1300,400,450,450,450,450,1250,400,450,450,450,350,500,350,500,450,400,400,450,400,450,450,400,400,500,350,500,400,450,400,450,400,450,450,450,450,400,400,450,400,450,400,500,350,500,350,500,450,400,400,450};
unsigned ON_23[] = {3450,1750,400,450,400,1300,400,450,400,500,350,500,400,450,400,450,400,450,400,450,400,450,450,450,400,450,400,450,400,1350,400,450,400,450,400,450,400,450,400,500,350,500,400,450,400,1300,400,1300,400,1350,400,450,400,500,350,1350,400,450,400,450,400,450,400,500,350,500,400,450,400,450,400,450,400,450,400,450,400,500,350,500,400,450,400,500,350,500,400,450,400,450,400,450,400,450,400,500,350,500,400,450};
unsigned ON_24[] = {3400,1750,400,500,350,1350,350,500,350,500,400,450,400,500,350,500,350,500,350,500,350,500,400,500,350,500,350,500,350,1400,350,500,350,500,350,500,350,500,350,500,400,450,400,500,350,1350,350,1350,350,1350,400,500,350,500,400,1350,350,500,350,500,350,500,350,500,400,450,400,500,350,500,350,500,350,500,350,500,350,500,400,450,400,500,350,500,400,450,400,500,350,500,350,500,350,500,350,500,400,450,400,500};
unsigned ON_25[] = {3400,1800,300,600,250,1450,250,600,300,500,350,550,300,550,300,600,250,750,150,550,250,600,300,600,200,650,200,700,200,2350,250,550,300,600,250,650,250,600,250,550,250,650,200,1500,250,1450,300,1350,350,550,350,600,250,1400,300,650,200,600,250,650,250,500,300,800,100,650,150,750,150,600,250,1450,300,600,250,700,200,500,350,550,300,550,300,550,300,600,300,550,250,600,250,700,200,600,250,550,300,650,150,650};
unsigned ON_26[] = {3350,2000,200,550,250,1450,250,600,300,600,200,600,350,500,300,600,200,650,250,650,250,600,250,600,250,650,250,550,300,1500,200,600,250,600,250,600,250,650,250,550,300,600,250,550,350,1400,250,1450,250,1450,300,550,300,700,200,1450,250,600,250,550,200,700,300,550,250,600,250,650,250,600,250,650,200,650,250,550,300,600,250,550,250,650,250,600,300,550,350,550,250,600,250,550,350,550,250,650,250,700,150,600};
unsigned ON_27[] = {3450,1750,400,450,400,1350,350,500,400,450,400,450,400,450,400,450,450,400,400,500,350,500,400,450,400,500,450,400,450,1250,400,450,400,450,400,500,450,400,400,450,400,450,400,450,400,1300,400,1350,450,1250,400,450,450,450,400,1300,400,450,400,500,450,400,450,400,400,450,450,400,400,450,400,450,400,500,450,400,400,450,400,450,400,450,450,450,400,450,400,450,400,450,450,450,350,500,400,450,400,450,400,450};
unsigned ON_28[] = {3450,1700,450,400,450,1250,450,450,450,400,450,400,450,400,450,400,450,400,450,450,400,450,450,400,450,450,400,450,450,1250,450,400,450,400,450,400,450,450,450,400,450,400,450,400,450,1250,450,1300,450,1250,450,400,500,400,450,1250,450,400,450,400,450,450,450,400,450,400,450,400,450,400,450,400,500,400,400,450,450,400,450,400,450,400,500,400,450,400,450,400,450,400,450,400,450,450,450,400,450,400,450,400};
unsigned ON_29[] = {3450,1750,400,450,400,1350,350,500,400,450,400,450,400,450,400,450,400,500,350,500,400,450,400,500,350,500,400,450,400,1300,400,450,400,450,400,500,350,500,400,450,400,450,400,450,400,1350,350,1350,400,1300,400,450,450,450,400,1300,400,500,350,500,350,500,400,450,400,500,350,450,400,450,400,500,350,500,350,500,400,450,400,450,400,500,400,450,400,500,350,450,400,500,350,500,350,500,400,450,400,500,350,500};
unsigned ON_30[] = {3400,1750,400,450,400,1350,350,500,400,450,400,450,400,450,400,450,400,500,350,500,400,450,400,500,350,500,400,450,400,1300,400,450,400,500,350,500,350,500,400,450,400,450,400,450,400,1350,350,1350,400,1300,400,450,450,450,400,1300,400,500,350,500,350,500,400,450,400,450,400,450,400,450,400,500,350,500,400,450,400,450,400,450,400,450,450,450,400,450,400,450,400,500,350,500,350,500,400,450,400,450,400,450};

// fungsi setup
void setup(){
  // mengaktifkan fitur-fitur
  Serial.begin(9600);
  pinMode(CSPIN, OUTPUT);
  button1.setDebounceTime(50);
  button2.setDebounceTime(50);
  lcd.begin();
  lcd.backlight();
  dht.begin();
  rtc.begin();
  SD.begin();

  // mencatat waktu compile
  /*Serial.print("Compiled Time: ");
  setDate = __DATE__;
  setTime = __TIME__;
  Serial.print(F(__DATE__));
  Serial.println(F(__TIME__));*/

  // set waktu RTC dengan waktu compile ketika RTC kehilangan daya
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // tampilan awal
  Serial.println("");
  Serial.println(F("********************"));
  Serial.println(F("Program Pengatur Suhu Ruangan Otomatis berbasis Arduino dan DHT22"));
  showLCD(1, 0, 0, "Program Pengatur", 0, 1, "Suhu Ruangan", 0, 2, "Otomatis Berbasis", 0, 3, "Arduino Mega", 2500);
}

// fungsi loop
void loop() {
  button1.loop();
  button2.loop();

  cek_tombol(); // menunggu tombol rtc setting ditekan atau tidak

  time_now = millis(); // ambil waktu mili second sekarang
  showLCD(1, 0, 0, "", 0, 1, "", 0, 2, "", 0, 3, "", 1); // clear tampilan lcd

  DateTime now = rtc.now(); // mengambil waktu RTC
  h = dht.readHumidity(); // baca humidity
  t = dht.readTemperature(); // baca temperature

  date = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year()); // menyimpan tanggal pada variable date
  time = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()); // menyimpan waktu pada variable time

  if (count >= 30){ // update suhu AC tiap 30 detik
    count = 0;
    if (t > set_point) { // cek suhu apakah lebih dari set point
      if (t == t_later) { // cek apakah suhu sama dengan suhu sebelumnya, apabila sama maka langsung keluar dari if
        goto exit;
      }
      atur_ac = set_point - (t - set_point); // nilai atur_ac = 25 - (suhu terbaca - 25); misal suhu terbaca 30 maka, atur_ac = 25 - (30 - 25) = 20
      suhu_ac_C = String(atur_ac) + "*C"; // variable suhu_ac_C digunakan untuk tampilan saja tentu sudah termasuk dengan simbol derajat
      set_ac(atur_ac); // memanggil fungsi set_ac dengan argument nilai atur_ac
      t_later = t; // menyimpan nilai suhu sekarang pada suhu_later agar nanti bisa digunakan untuk program diatas
    }
    else if (t <= low_temp) { // cek apakah suhu kurang dari 20
      suhu_ac_C = "AC OFF"; // tampilan ke serial ataupun ke lcd
      set_ac_off(); // memanggil fungsi set_ac_off yang digunakan untuk mematikan AC
    }
    else { // apabila suhu diantara 21-25 maka tidak mengerjakan apa2
      suhu_ac_C = suhu_ac_C;
    }
  }

  exit:
  // tampialn pada serial monitor
  Serial.println("");
  Serial.println(F("********************"));
  Serial.print(F("Date: ")); Serial.println(date);
  Serial.print(F("Time: ")); Serial.println(time);
  Serial.print(F("Humidity: ")); Serial.print(h); Serial.println(F("%"));
  Serial.print(F("Temperature: ")); Serial.print(t); Serial.println(F("*C"));
  Serial.print(F("AC Temperature: ")); Serial.println(suhu_ac_C);
  Serial.println("");

  // tampilan pada LCD
  showLCD(0, 0, 0, "Date: " + date, 0, 1, "Time: " + time, 0, 2, "Hum: " + String(h) + "%", 0, 3, "Temp: " + String(t) + "*C", 1);
  showLCD(0, 0, 0, "", 0, 1, "", 0, 2, "", 11, 3, "AC: " + suhu_ac_C, 1);

  dataSimpan = date + ";" + time + ";" + String(h) + ";" + String(t) + ";" + String(atur_ac); // mengisi variable dataSimpan dengan data yang akan disimpan pada SDCard
  simpanSD(dataSimpan); // memanggil fungsi simpanSD dengan argument nilai dataSimpan

  while(millis() < time_now + period){ // menunggu 1detik

  }
  count = count +1 ;
}

// fungsi tampilan LCD 20x4
// clr -> 1=clear lcd, 0=tidak clear lcd
// rx dan cx untuk atur posisi kursor
// charax untuk kata yang akan ditampilkan
// tdelay untuk jeda waktu tampil dalam ms
void showLCD(bool clr, int r1, int c1, String chara1, int r2, int c2, String chara2, int r3, int c3, String chara3, int r4, int c4, String chara4, int tdelay) {
  if (clr == 1){
    lcd.clear();
    delay(10);
  }
  lcd.setCursor(r1, c1); lcd.print(chara1);
  lcd.setCursor(r2, c2); lcd.print(chara2);
  lcd.setCursor(r3, c3); lcd.print(chara3);
  lcd.setCursor(r4, c4); lcd.print(chara4);
  delay(tdelay);
}

// fungsi untuk simpan data ke SDCard
void simpanSD(String dataSimpan){
  dataFile = SD.open("logger.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataSimpan);
    dataFile.close();
    Serial.println(F("Save data to logger.txt in SDCard"));
  }
  else {
    Serial.println(F("Error opening logger.txt"));
  }
}

// fungsi untuk atur suhu AC
void set_ac(int atur_ac){
  if (atur_ac <= 16){
    atur_ac = 1;
  }
  else if (atur_ac >= 30){
    atur_ac = 15;
  }
  else {
    atur_ac = atur_ac - 15;
  }

  switch(atur_ac){
    case 1: irsend.sendRaw(ON_16, sizeof(OFF)/sizeof(int),khz); break;
    case 2: irsend.sendRaw(ON_17, sizeof(OFF)/sizeof(int),khz); break;
    case 3: irsend.sendRaw(ON_18, sizeof(OFF)/sizeof(int),khz); break;
    case 4: irsend.sendRaw(ON_19, sizeof(OFF)/sizeof(int),khz); break;
    case 5: irsend.sendRaw(ON_20, sizeof(OFF)/sizeof(int),khz); break;
    case 6: irsend.sendRaw(ON_21, sizeof(OFF)/sizeof(int),khz); break;
    case 7: irsend.sendRaw(ON_22, sizeof(OFF)/sizeof(int),khz); break;
    case 8: irsend.sendRaw(ON_23, sizeof(OFF)/sizeof(int),khz); break;
    case 9: irsend.sendRaw(ON_24, sizeof(OFF)/sizeof(int),khz); break;
    case 10: irsend.sendRaw(ON_25, sizeof(OFF)/sizeof(int),khz); break;
    case 11: irsend.sendRaw(ON_26, sizeof(OFF)/sizeof(int),khz); break;
    case 12: irsend.sendRaw(ON_27, sizeof(OFF)/sizeof(int),khz); break;
    case 13: irsend.sendRaw(ON_28, sizeof(OFF)/sizeof(int),khz); break;
    case 14: irsend.sendRaw(ON_29, sizeof(OFF)/sizeof(int),khz); break;           
    case 15: irsend.sendRaw(ON_30, sizeof(OFF)/sizeof(int),khz); break;
  }
}

// fungsi untuk mematikan AC
void set_ac_off() {
  irsend.sendRaw(OFF, sizeof(OFF)/sizeof(int),khz);
}

// fungsi untuk cek apakah masuk rtc setup atau tidak
void cek_tombol() {
  Serial.println(F("Tekan tombol 1 untuk Setting RTC dan 2 untuk Lanjut"));
  showLCD(1, 0, 0, "Tekan 1: Set RTC", 0, 1, "Tekan 2: Lanjut", 0, 2, "", 0, 3, "", 2500);
  while(1) {
    if (button1.isPressed()) { // cek tombol 1 ditekan = rtc setup
      rtcSetup();
      break;
    }
    if (button2.isPressed()) { // cek tombol 2 ditekan = no rtc setup
      break;
    }
  }
}

// fungsu untuk setting rtc
void rtcSetup() {
  int cursorStatus = 1;
  int hour = 0;
  int minute = 0;
  int second = 0;

  while(1) {
    showLCD(1, 0, 0, "RTC Setting", 0, 1, "Hour: " + String(hour), 0, 2, "Minute: " + String(minute), 0, 3, "Second: " + String(second), 100);

    // cek tombol 1 dan 2 ditekan bersama, untuk keluar dari mode setup RTC
    if (button1.isPressed() && button2.isPressed()) { 
      showLCD(1, 0, 0, "Setting RTC Done", 0, 1, String(hour) + ":" + String(minute) + ":" + String(second), 0, 2, "Tekan 1 Lanjut", 0, 3, "", 100);
      while(1) {
        if (button1.isPressed()){
          break;
        }
      }
      break;
    }

    // cek tombol 1 ditekan? jika iya, naikkan nilai hour / minute / second tergantung nilai kursor
    if (button1.isPressed()) {
      if (cursorStatus == 1) {
        hour = hour + 1;
        if (hour > 23) {
          hour = 0;
        }
        showLCD(1, 0, 0, "hour:", 0, 1, String(hour), 0, 2, "", 0, 3, "", 100);
      }
      if (cursorStatus == 2) {
        minute = minute + 1;
        if (minute > 59) {
          minute = 0;
        }
        showLCD(1, 0, 0, "minute:", 0, 1, String(minute), 0, 2, "", 0, 3, "", 100);
      }
      if (cursorStatus == 3) {
        second = second + 1;
        if (second > 59) {
          second = 0;
        }
      }
      showLCD(1, 0, 0, "second:", 0, 1, String(second), 0, 2, "", 0, 3, "", 100);
    }

    // cek tombol 2 ditekan? jika iya naikkan nilai kursor
    if (button2.isPressed()) {
      cursorStatus = cursorStatus + 1;
      if (cursorStatus > 3) {
        cursorStatus = 1;
      }
    }
  }

  DateTime now = rtc.now(); // mengambil waktu RTC
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  rtc.adjust(DateTime(now.year(), now.month(), now.day(), hour, minute, second));
}
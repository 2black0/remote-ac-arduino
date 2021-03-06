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
#define BUTTON1PIN A2 // active low
#define BUTTON2PIN A3 // active low
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
int khz = 38;
int t_later = 0;
int atur_ac = 0;

int h, t;
String date, time;
String suhu_ac_C = "AC OFF";
String dataSimpan;
String setDate, setTime;
bool status;

int count;
int period = 1000;
unsigned long time_now = 0;

// deklarasi variable untuk kendali AC (Panasonic)
unsigned int OFF[] = {3400, 1750, 400, 450, 400, 1300, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 450, 450, 400, 450, 400, 450, 400, 1350, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 500, 350, 1350, 350, 1350, 400, 1350, 350, 450, 400, 500, 400, 1300, 400, 500, 350, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 500, 350, 450, 400, 500, 350, 500, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 500, 350, 500, 350, 450};   // UNKNOWN 68D13F41
unsigned int ON[] = {3400, 1750, 400, 450, 400, 1350, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 500, 400, 450, 400, 450, 400, 450, 400, 1350, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 1350, 400, 1300, 400, 1300, 400, 450, 450, 450, 400, 1300, 400, 450, 400, 450, 400, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450};    // UNKNOWN 68D13F41
unsigned int ON_16[] = {3450, 1750, 400, 450, 400, 1300, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 450, 450, 400, 450, 400, 450, 400, 1300, 400, 450, 400, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 1300, 400, 1350, 350, 1350, 400, 450, 400, 500, 400, 1300, 400, 450, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 450, 450, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500, 400, 450, 400, 450, 400, 450}; // UNKNOWN 68D13F41
unsigned int ON_17[] = {3400, 1750, 400, 450, 400, 1350, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 500, 400, 450, 400, 450, 400, 450, 400, 1350, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 1350, 400, 1300, 400, 1300, 400, 450, 450, 450, 400, 1300, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450}; // UNKNOWN 68D13F41
unsigned int ON_18[] = {3400, 1750, 400, 450, 400, 1350, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500, 400, 450, 400, 450, 400, 500, 350, 1350, 400, 450, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 500, 350, 1350, 400, 1300, 400, 1300, 400, 450, 450, 450, 400, 1300, 400, 450, 400, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450}; // UNKNOWN 68D13F41
unsigned int ON_19[] = {3450, 1750, 350, 500, 400, 1300, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 500, 350, 500, 400, 450, 400, 1300, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 1300, 400, 1300, 400, 1350, 400, 450, 400, 500, 350, 1350, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 450, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450}; // UNKNOWN 68D13F41
unsigned int ON_20[] = {3400, 1750, 400, 450, 400, 1300, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 450, 450, 400, 450, 400, 450, 400, 1300, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 1300, 400, 1350, 350, 1350, 400, 450, 400, 500, 350, 1350, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 450, 450, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450}; // UNKNOWN 68D13F41
unsigned int ON_21[] = {3400, 1750, 400, 450, 400, 1300, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 500, 400, 450, 400, 450, 400, 1300, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 1350, 350, 1350, 350, 1350, 400, 450, 400, 500, 350, 1350, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 450, 450, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450}; // UNKNOWN 68D13F41
unsigned int ON_22[] = {3450, 1750, 350, 500, 400, 1300, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 500, 350, 500, 400, 450, 400, 1300, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500, 400, 450, 400, 450, 400, 1300, 400, 1300, 400, 1350, 350, 500, 400, 450, 400, 1350, 350, 500, 400, 450, 400, 450, 400, 450, 450, 400, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 450, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500, 400, 450}; // UNKNOWN 68D13F41
unsigned int ON_23[] = {3450, 1750, 350, 500, 350, 1350, 400, 450, 400, 450, 400, 500, 350, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 500, 400, 450, 350, 500, 400, 1300, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 350, 500, 400, 1300, 400, 1300, 400, 1350, 350, 500, 400, 450, 400, 1350, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 450, 400, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 500, 400, 450, 400, 450, 400, 500, 350, 450, 400, 450, 400, 500, 350, 500, 350, 500}; // UNKNOWN 68D13F41
unsigned int ON_24[] = {3450, 1750, 400, 450, 400, 1300, 400, 450, 400, 500, 350, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 500, 400, 450, 400, 500, 350, 1300, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 1350, 350, 1350, 350, 1350, 400, 450, 400, 500, 350, 1350, 400, 450, 400, 450, 400, 500, 350, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 450, 450, 400, 500, 350, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450}; // UNKNOWN 68D13F41
unsigned int ON_25[] = {3450, 1800, 350, 450, 400, 1350, 350, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 500, 400, 450, 400, 450, 400, 450, 400, 1350, 350, 500, 350, 500, 400, 450, 400, 500, 350, 500, 350, 450, 400, 450, 400, 1350, 350, 1350, 400, 1300, 400, 500, 350, 500, 400, 1350, 350, 450, 400, 500, 350, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 500, 350, 500, 350, 500, 350, 450, 400, 500, 350, 500, 400, 500, 350, 500, 350, 500, 350, 500, 350, 500, 400, 450, 400, 500, 350, 450, 400, 450}; // UNKNOWN 68D13F41
unsigned int ON_26[] = {3450, 1750, 400, 450, 400, 1300, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 450, 450, 400, 450, 400, 450, 400, 1350, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 1350, 350, 1350, 400, 1300, 400, 450, 450, 450, 400, 1300, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 500, 350, 450, 450, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450}; // UNKNOWN FF86BA58
unsigned int ON_27[] = {3450, 1750, 350, 500, 400, 1300, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 500, 350, 500, 400, 450, 400, 1300, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 1300, 400, 1300, 400, 1350, 350, 500, 400, 450, 400, 1350, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 450, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450}; // UNKNOWN 68D13F41
unsigned int ON_28[] = {3400, 1750, 400, 500, 350, 1350, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 500, 350, 500, 350, 1350, 400, 450, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 1300, 400, 1300, 400, 1350, 350, 500, 400, 450, 400, 1350, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500}; // UNKNOWN 68D13F41
unsigned int ON_29[] = {3450, 1750, 400, 450, 400, 1300, 400, 500, 350, 500, 350, 500, 400, 450, 400, 500, 350, 450, 400, 500, 350, 450, 450, 450, 400, 450, 400, 500, 350, 1350, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 1350, 350, 1350, 400, 1300, 400, 450, 400, 500, 400, 1300, 400, 450, 400, 500, 350, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 500, 350, 450, 400, 500, 350, 500, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 500, 350, 450}; // UNKNOWN C78FA89C
unsigned int ON_30[] = {3450, 1700, 400, 500, 350, 1350, 350, 500, 400, 450, 400, 450, 400, 450, 400, 500, 350, 450, 400, 500, 350, 500, 400, 450, 400, 450, 400, 500, 350, 1350, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 500, 350, 1350, 400, 1350, 350, 1300, 400, 450, 450, 450, 400, 1300, 400, 450, 450, 450, 350, 500, 350, 500, 400, 450, 400, 450, 400, 450, 400, 450, 400, 450, 400, 500, 350, 500, 350, 500, 350, 500, 400, 500, 350, 500, 350, 500, 400, 450, 400, 450, 400, 500, 350, 450, 400, 450, 400, 500}; // UNKNOWN F3621E56

// fungsi setup
void setup()
{
  // mengaktifkan fitur-fitur
  Serial.begin(9600);
  pinMode(CSPIN, OUTPUT);
  button1.setDebounceTime(50);
  button2.setDebounceTime(50);
  lcd.begin();
  lcd.backlight();
  dht.begin();
  rtc.begin();
  //SD.begin();
  Serial.print("Inisialisasi SD Card");
  showLCD(1, 0, 0, "Inisialisasi", 0, 1, "SD Card", 0, 2, "", 0, 3, "", 1000);
  if (!SD.begin(10))
  {
    Serial.println("Inisialisasi Gagal!");
    showLCD(1, 0, 0, "Inisialisasi", 0, 1, "Gagal", 0, 2, "", 0, 3, "", 1000);
    while (1)
      ;
  }
  Serial.println("Inisialisasi Selesai");
  showLCD(1, 0, 0, "Inisialisasi", 0, 1, "Selesai", 0, 2, "", 0, 3, "", 1000);

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // tampilan awal
  Serial.println("");
  Serial.println(F("********************"));
  Serial.println(F("Program Pengatur Suhu Ruangan Otomatis berbasis Arduino dan DHT22"));
  showLCD(1, 0, 0, "Program Pengatur", 0, 1, "Suhu Ruangan", 0, 2, "Otomatis Berbasis", 0, 3, "Arduino Mega", 2500);

  cek_tombol(); // menunggu tombol rtc setting ditekan atau tidak
}

// fungsi loop
void loop()
{
  time_now = millis();                                   // ambil waktu mili second sekarang
  showLCD(1, 0, 0, "", 0, 1, "", 0, 2, "", 0, 3, "", 1); // clear tampilan lcd

  DateTime now = rtc.now();  // mengambil waktu RTC
  h = dht.readHumidity();    // baca humidity
  t = dht.readTemperature(); // baca temperature

  date = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year());     // menyimpan tanggal pada variable date
  time = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second()); // menyimpan waktu pada variable time

  if (count >= 30)
  { // update suhu AC tiap 30 detik
    count = 0;
    if (t > set_point)
    { // cek suhu apakah lebih dari set point
      if (t == t_later)
      { // cek apakah suhu sama dengan suhu sebelumnya, apabila sama maka langsung keluar dari if
        goto exit;
      }
      atur_ac = set_point - (t - set_point); // nilai atur_ac = 25 - (suhu terbaca - 25); misal suhu terbaca 30 maka, atur_ac = 25 - (30 - 25) = 20
      suhu_ac_C = String(atur_ac) + "*C";    // variable suhu_ac_C digunakan untuk tampilan saja tentu sudah termasuk dengan simbol derajat
      set_ac(atur_ac);                       // memanggil fungsi set_ac dengan argument nilai atur_ac
      t_later = t;                           // menyimpan nilai suhu sekarang pada suhu_later agar nanti bisa digunakan untuk program diatas
    }
    else if (t <= low_temp)
    {                       // cek apakah suhu kurang dari 20
      suhu_ac_C = "AC OFF"; // tampilan ke serial ataupun ke lcd
      set_ac_off();         // memanggil fungsi set_ac_off yang digunakan untuk mematikan AC
    }
    else
    { // apabila suhu diantara 21-25 maka tidak mengerjakan apa2
      suhu_ac_C = suhu_ac_C;
    }
  }

exit:
  // tampialn pada serial monitor
  Serial.println("");
  Serial.println(F("********************"));
  Serial.print(F("Date: "));
  Serial.println(date);
  Serial.print(F("Time: "));
  Serial.println(time);
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.println(F("%"));
  Serial.print(F("Temperature: "));
  Serial.print(t);
  Serial.println(F("*C"));
  Serial.print(F("AC Temperature: "));
  Serial.println(suhu_ac_C);
  Serial.println("");

  // tampilan pada LCD
  showLCD(1, 0, 0, "Date: " + date, 0, 1, "Time: " + time, 0, 2, "Hum : " + String(h) + "%", 0, 3, "Temp: " + String(t) + "*C", 1);
  showLCD(0, 0, 0, "", 0, 1, "", 0, 2, "", 11, 3, "AC:" + suhu_ac_C, 1);

  dataSimpan = date + ";" + time + ";" + String(h) + ";" + String(t) + ";" + String(atur_ac); // mengisi variable dataSimpan dengan data yang akan disimpan pada SDCard
  simpanSD(dataSimpan);                                                                       // memanggil fungsi simpanSD dengan argument nilai dataSimpan

  while (millis() < time_now + period)
  { // menunggu 1detik
    if (status == 0)
    {
      Serial.print("count: ");
      Serial.println(count);
      showLCD(0, 18, 0, String(count), 0, 1, "", 0, 2, "", 0, 3, "", 1);
    }
    status = 1;
  }
  status = 0;
  count = count + 1;
}

// fungsi tampilan LCD 20x4
// clr -> 1=clear lcd, 0=tidak clear lcd
// rx dan cx untuk atur posisi kursor
// charax untuk kata yang akan ditampilkan
// tdelay untuk jeda waktu tampil dalam ms
void showLCD(bool clr, int r1, int c1, String chara1, int r2, int c2, String chara2, int r3, int c3, String chara3, int r4, int c4, String chara4, int tdelay)
{
  if (clr == 1)
  {
    lcd.clear();
    delay(10);
  }
  lcd.setCursor(r1, c1);
  lcd.print(chara1);
  lcd.setCursor(r2, c2);
  lcd.print(chara2);
  lcd.setCursor(r3, c3);
  lcd.print(chara3);
  lcd.setCursor(r4, c4);
  lcd.print(chara4);
  delay(tdelay);
}

// fungsi untuk simpan data ke SDCard
void simpanSD(String dataSimpan)
{
  dataFile = SD.open("logger.txt", FILE_WRITE);
  if (dataFile)
  {
    dataFile.println(dataSimpan);
    dataFile.close();
    Serial.println(F("Save data to logger.txt in SDCard"));
    showLCD(1, 0, 0, "Simpan Data", 0, 1, "ke logger.txt", 0, 2, "", 0, 3, "", 1000);
  }
  else
  {
    Serial.println(F("Error opening logger.txt"));
    showLCD(1, 0, 0, "Error", 0, 1, "ke logger.txt", 0, 2, "", 0, 3, "", 1000);
  }
}

// fungsi untuk atur suhu AC
void set_ac(int atur_ac)
{
  irsend.sendRaw(ON, sizeof(ON) / sizeof(ON[0]), khz);
  delay(1000);
  if (atur_ac <= 16)
  {
    atur_ac = 1;
  }
  else if (atur_ac >= 30)
  {
    atur_ac = 15;
  }
  else
  {
    atur_ac = atur_ac - 15;
  }

  switch (atur_ac)
  {
  case 1:
    irsend.sendRaw(ON_16, sizeof(ON_16) / sizeof(ON_16[0]), khz);
    delay(1000);
    break;
  case 2:
    irsend.sendRaw(ON_17, sizeof(ON_17) / sizeof(ON_17[0]), khz);
    delay(1000);
    break;
  case 3:
    irsend.sendRaw(ON_18, sizeof(ON_18) / sizeof(ON_18[0]), khz);
    delay(1000);
    break;
  case 4:
    irsend.sendRaw(ON_19, sizeof(ON_19) / sizeof(ON_19[0]), khz);
    delay(1000);
    break;
  case 5:
    irsend.sendRaw(ON_20, sizeof(ON_20) / sizeof(ON_20[0]), khz);
    delay(1000);
    break;
  case 6:
    irsend.sendRaw(ON_21, sizeof(ON_21) / sizeof(ON_21[0]), khz);
    delay(1000);
    break;
  case 7:
    irsend.sendRaw(ON_22, sizeof(ON_22) / sizeof(ON_22[0]), khz);
    delay(1000);
    break;
  case 8:
    irsend.sendRaw(ON_23, sizeof(ON_23) / sizeof(ON_23[0]), khz);
    delay(1000);
    break;
  case 9:
    irsend.sendRaw(ON_24, sizeof(ON_24) / sizeof(ON_24[0]), khz);
    delay(1000);
    break;
  case 10:
    irsend.sendRaw(ON_25, sizeof(ON_25) / sizeof(ON_25[0]), khz);
    delay(1000);
    break;
  case 11:
    irsend.sendRaw(ON_26, sizeof(ON_26) / sizeof(ON_26[0]), khz);
    delay(1000);
    break;
  case 12:
    irsend.sendRaw(ON_27, sizeof(ON_27) / sizeof(ON_27[0]), khz);
    delay(1000);
    break;
  case 13:
    irsend.sendRaw(ON_28, sizeof(ON_28) / sizeof(ON_28[0]), khz);
    delay(1000);
    break;
  case 14:
    irsend.sendRaw(ON_29, sizeof(ON_29) / sizeof(ON_29[0]), khz);
    delay(1000);
    break;
  case 15:
    irsend.sendRaw(ON_30, sizeof(ON_30) / sizeof(ON_30[0]), khz);
    delay(1000);
    break;
  }
}

// fungsi untuk mematikan AC
void set_ac_off()
{
  irsend.sendRaw(OFF, sizeof(OFF) / sizeof(int), khz);
}

// fungsi untuk cek apakah masuk rtc setup atau tidak
void cek_tombol()
{
  Serial.println(F("Tekan tombol 1 untuk Setting RTC dan 2 untuk Lanjut"));
  showLCD(1, 0, 0, "Tekan 1: Set RTC", 0, 1, "Tekan 2: Lanjut", 0, 2, "", 0, 3, "", 1000);
  while (1)
  {
    button1.loop();
    button2.loop();

    if (button1.isReleased())
    { // cek tombol 1 ditekan = rtc setup
      //showLCD(1, 0, 0, "Tombol 1 ditekan", 0, 1, "", 0, 2, "", 0, 3, "", 1000);
      rtcSetup();
      break;
    }
    if (button2.isReleased())
    { // cek tombol 2 ditekan = no rtc setup
      //showLCD(1, 0, 0, "Tombol 2 ditekan", 0, 1, "", 0, 2, "", 0, 3, "", 1000);
      break;
    }
  }
}

// fungsi untuk setting rtc
void rtcSetup()
{
  int cursorStatus = 1;
  int hour = 0;
  int minute = 0;
  int second = 0;
  int year = 1990;
  int month = 0;
  int day = 0;

  while (1)
  {
    button1.loop();
    button2.loop();

    showLCD(1, 0, 0, "RTC Setting", 0, 1, "Year: " + String(year), 0, 2, "Month: " + String(month), 0, 3, "Day: " + String(day), 100);

    // cek tombol 1 dan 2 ditekan bersama, untuk keluar dari mode setup RTC
    if (button1.isReleased() && button2.isReleased())
    {
      showLCD(1, 0, 0, "Setting RTC Done", 0, 1, String(year) + ":" + String(month) + ":" + String(day), 0, 2, "Tekan 1 & 2", 0, 3, "", 100);
      while (1)
      {
        if (button1.isReleased())
        {
          break;
        }
      }
      break;
    }

    // cek tombol 1 ditekan? jika iya, naikkan nilai hour / minute / second tergantung nilai kursor
    if (button1.isReleased())
    {
      if (cursorStatus == 1)
      {
        year = year + 1;
      }
      if (cursorStatus == 2)
      {
        month = month + 1;
        if (month > 12)
        {
          month = 1;
        }
      }
      if (cursorStatus == 3)
      {
        day = day + 1;
        if (day > 31)
        {
          day = 1;
        }
      }
    }

    // cek tombol 2 ditekan? jika iya naikkan nilai kursor
    if (button2.isReleased())
    {
      cursorStatus = cursorStatus + 1;
      if (cursorStatus > 3)
      {
        cursorStatus = 1;
      }
    }
  }

  while (1)
  {
    button1.loop();
    button2.loop();

    showLCD(1, 0, 0, "RTC Setting", 0, 1, "Hour: " + String(hour), 0, 2, "Minute: " + String(minute), 0, 3, "Second: " + String(second), 100);

    // cek tombol 1 dan 2 ditekan bersama, untuk keluar dari mode setup RTC
    if (button1.isReleased() && button2.isReleased())
    {
      showLCD(1, 0, 0, "Setting RTC Done", 0, 1, String(hour) + ":" + String(minute) + ":" + String(second), 0, 2, "Tekan 1 Lanjut", 0, 3, "", 100);
      while (1)
      {
        if (button1.isReleased())
        {
          break;
        }
      }
      break;
    }

    // cek tombol 1 ditekan? jika iya, naikkan nilai hour / minute / second tergantung nilai kursor
    if (button1.isReleased())
    {
      if (cursorStatus == 1)
      {
        hour = hour + 1;
        if (hour > 23)
        {
          hour = 0;
        }
      }
      if (cursorStatus == 2)
      {
        minute = minute + 1;
        if (minute > 59)
        {
          minute = 0;
        }
      }
      if (cursorStatus == 3)
      {
        second = second + 1;
        if (second > 59)
        {
          second = 0;
        }
      }
    }

    // cek tombol 2 ditekan? jika iya naikkan nilai kursor
    if (button2.isReleased())
    {
      cursorStatus = cursorStatus + 1;
      if (cursorStatus > 3)
      {
        cursorStatus = 1;
      }
    }
  }

  //DateTime now = rtc.now(); // mengambil waktu RTC
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  //rtc.adjust(DateTime(now.year(), now.month(), now.day(), hour, minute, second));
  rtc.adjust(DateTime(year, month, day, hour, minute, second));
}
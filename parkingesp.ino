#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

// LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Servo
Servo entryServo;
Servo exitServo;

// WiFi
const char* ssid = "A55";
const char* password = "namaguaa";

// PIN
const int IR_ENTRY = 16;
const int IR_EXIT = 17;
const int TRIG_PIN = 18;
const int ECHO_PIN = 19;
const int SLOT_PINS[4] = {33, 32, 35, 25};

// Slot & status
bool slotStatus[4] = {false};
int availableSlots = 4;
bool isTruck = false;
unsigned long lastSlotUpdate = 0;
int lastKendaraanId = -1;  // Simpan ID kendaraan masuk terakhir

// Server & Telegram
const char* server = "http://192.168.134.222/smart_parking/api.php";
const char* telegramAPI = "http://192.168.134.222/smart_parking/bot_telegram.php";

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();

  WiFi.begin(ssid, password);
  lcd.setCursor(0, 0);
  lcd.print("Menghubungkan WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  lcd.clear();
  lcd.print("WiFi Terhubung");
  Serial.println("\nWiFi Connected");
  delay(1000);

  entryServo.attach(26);
  exitServo.attach(14);
  entryServo.write(0);
  exitServo.write(0);

  pinMode(IR_ENTRY, INPUT_PULLUP);
  pinMode(IR_EXIT, INPUT_PULLUP);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  for (int i = 0; i < 4; i++) pinMode(SLOT_PINS[i], INPUT_PULLUP);

  updateSlots();
  tampilkanStatus();
  kirimTelegram("🔌 <b>Sistem Parkir Aktif!</b>\nSistem siap digunakan.");
}

void loop() {
  if (millis() - lastSlotUpdate > 1000) {
    updateSlots();
    tampilkanStatus();
    kirimSlotKeServer();
    lastSlotUpdate = millis();
  }

  if (digitalRead(IR_ENTRY) == LOW) {
    delay(300);
    if (digitalRead(IR_ENTRY) == LOW) prosesMasuk();
  }

  if (digitalRead(IR_EXIT) == LOW) {
    delay(300);
    if (digitalRead(IR_EXIT) == LOW) prosesKeluar();
  }
}

void updateSlots() {
  availableSlots = 4;
  for (int i = 0; i < 4; i++) {
    slotStatus[i] = digitalRead(SLOT_PINS[i]) == LOW;
    if (slotStatus[i]) availableSlots--;
  }
}

void tampilkanStatus() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Slot:");
  for (int i = 0; i < 4; i++) {
    lcd.setCursor(6 + (i * 3), 0);
    lcd.print(i + 1);
    lcd.print(":");
    lcd.print(slotStatus[i] ? "X" : "O");
  }
  lcd.setCursor(0, 1);
  lcd.print("Tersedia: ");
  lcd.print(availableSlots);
  lcd.print("/4");
}

void prosesMasuk() {
  int distance = ukurJarak();
  isTruck = (distance > 0 && distance < 10);

  if (isTruck) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("TRUK DITOLAK!");
    kirimAPI("truk_ditolak", "");
    kirimTelegram("🚛 <b>TRUK DITOLAK!</b>\nHanya mobil diperbolehkan.");
    delay(2500);
    tampilkanStatus();
    return;
  }

  if (availableSlots > 0) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Pintu Masuk Buka");
    entryServo.write(90);
    delay(5000);
    entryServo.write(0);

    String response = kirimAPI("masuk", "jenis=mobil");
    if (response.indexOf("success") > 0) {
      int idStart = response.indexOf("\"id\":") + 5;
      int idEnd = response.indexOf("}", idStart);
      lastKendaraanId = response.substring(idStart, idEnd).toInt();
    }

    updateSlots();
    tampilkanStatus();
    kirimTelegram("🚗 <b>MOBIL MASUK!</b>\nSlot tersedia: " + String(availableSlots) + "/4");
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PARKIR PENUH!");
    kirimTelegram("🅿️ <b>PARKIR PENUH!</b>\nTidak ada slot tersedia.");
    delay(2000);
    tampilkanStatus();
  }
}

void prosesKeluar() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pintu Keluar Buka");
  exitServo.write(90);
  delay(5000);
  exitServo.write(0);

  String postData = "id=" + String(lastKendaraanId);
  String response = kirimAPI("keluar", postData);
  if (response.indexOf("success") > 0) {
    kirimTelegram("🔚 <b>MOBIL KELUAR!</b>\nSlot tersedia: " + String(availableSlots) + "/4");
  } else {
    kirimTelegram("⚠️ <b>GAGAL KELUAR!</b>\nPeriksa sistem.");
  }

  updateSlots();
  tampilkanStatus();
}

int ukurJarak() {
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long durasi = pulseIn(ECHO_PIN, HIGH, 20000);
  return durasi * 0.034 / 2;
}

void kirimSlotKeServer() {
  String data = "[";
  for (int i = 0; i < 4; i++) {
    data += slotStatus[i] ? "true" : "false";
    if (i < 3) data += ",";
  }
  data += "]";
  kirimAPI("slot", "data=" + data);
}

String kirimAPI(String action, String postData) {
  if (WiFi.status() != WL_CONNECTED) return "WiFi Error";

  HTTPClient http;
  String url = String(server) + "?action=" + action;
  http.begin(url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  int code = http.POST(postData);
  String resp = "";
  if (code > 0) {
    resp = http.getString();
    Serial.println("API: " + url + " CODE: " + code + " RESP: " + resp);
  } else {
    Serial.println("API Error: " + String(code));
  }
  http.end();
  return resp;
}

void kirimTelegram(String message) {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  String encoded = urlEncode(message);
  String url = String(telegramAPI) + "?action=sendNotification&message=" + encoded;
  http.begin(url);
  int code = http.GET();
  Serial.println("Telegram: " + url);
  if (code != 200) Serial.println("Telegram Error: " + String(code));
  http.end();
}

String urlEncode(String str) {
  String encoded = "";
  char c, code0, code1;
  for (unsigned int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == ' ') encoded += '+';
    else if (isalnum(c)) encoded += c;
    else {
      code1 = (c & 0xf) + '0'; if ((c & 0xf) > 9) code1 = (c & 0xf) - 10 + 'A';
      code0 = ((c >> 4) & 0xf) + '0'; if (((c >> 4) & 0xf) > 9) code0 = ((c >> 4) & 0xf) - 10 + 'A';
      encoded += '%'; encoded += code0; encoded += code1;
    }
  }
  return encoded;
}

 /* --------------------------------------------------------------------
   Projet : ESP32-C3 avec écran OLED intégré 0.42" et capteur SHT30
   Auteur : Claude Dufourmont
   Description :
     Ce programme permet la saisie directe du SSID et du mot de passe WiFi
     à l’aide des trois boutons intégrés (UP / NEXT / OK) et leur stockage
     en mémoire NVS via la librairie Preferences.

     Une invitation s’affiche au démarrage : un appui long sur le bouton OK
     (≈ 2 secondes) permet d’effacer les identifiants enregistrés et d’entrer
     en mode configuration.

     Une fois connecté au WiFi :
       - L’adresse IP locale (partiellement masquée pour confidentialité)
         s’affiche à l’écran.
       - L’heure est synchronisée automatiquement via un serveur NTP.
       - Le passage heure d’été / heure d’hiver est géré automatiquement
         selon la règle européenne (dernier dimanche de mars / octobre).
       - L'heure et la température s'affichent par alternance.

     Le code est conçu pour l’ESP32-C3 Mini avec écran OLED 0.42" intégré,
     et une configuration de boutons sur les GPIO :
        - UP   → GPIO 4
        - NEXT → GPIO 7
        - OK   → GPIO 10

   -------------------------------------------------------------------- */

#include <WiFi.h>
#include <Preferences.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Adafruit_SHT31.h>

// --------------------- OLED / I2C ---------------------
#define SDA_PIN 5
#define SCL_PIN 6
U8G2_SSD1306_72X40_ER_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// --------------------- BOUTONS ---------------------
const int BTN_UP   = 4;  // à câbler
const int BTN_NEXT = 7;  // à câbler
const int BTN_OK   = 10; // à câbler

// --------------------- SHT30 ---------------------
#define SHT30_ADDR 0x45
Adafruit_SHT31 sht30 = Adafruit_SHT31();

// --------------------- VARIABLES ---------------------
Preferences prefs;
const char* CHAR_LINES[] = {
  "abcdefghijklmnopqrstuvwxyz",
  "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
  "0123456789",
  " !@#$_-.,:/+*()?=<"
};
const int NUM_LINES = 4;

String ssid = "";
String password = "";
String tempInput = "";
int lineIndex = 0;
int charIndex = 0;

bool okbuttonPressed = false;
unsigned long okpressStart = 0;
bool inConfig = false;

// --------------------- NTP ---------------------
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

// --------------------- FONCTIONS UTILES ---------------------
bool readBtn(int pin) { return digitalRead(pin) == LOW; }
bool waitRelease(int pin) { while(readBtn(pin)) delay(10); return true; }

void drawCenteredText(const char* txt, int y) {
  int16_t w = u8g2.getUTF8Width(txt);
  int x = (72 - w) / 2;
  if (x < 0) x = 0;
  u8g2.drawUTF8(x, y, txt);
}

String maskedIP(IPAddress ip){
  return String(ip[0]) + "." + String(ip[1]) + ".xx.xx";
}

// --------------------- AFFICHAGE FENETRE SAISIE ---------------------
void showOLEDInput() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);

  const int maxVisible = 12; // max caractères visibles
  String visibleInput;

  // On prend les derniers caractères saisis pour rester dans la fenêtre
  if(tempInput.length() <= maxVisible) {
    visibleInput = tempInput + "_";
  } else {
    visibleInput = tempInput.substring(tempInput.length() - maxVisible) + "_";
  }

  // Affichage uniquement de la fenêtre (plus de "Saisie:" qui prend trop de place)
  u8g2.drawUTF8(0, 10, visibleInput.c_str());

  // Ligne type + caractère courant
  char buf[32];
  const char *typeStr = (lineIndex==0)?"Min":(lineIndex==1)?"Maj":(lineIndex==2)?"Num":"Sym";
  char cur = CHAR_LINES[lineIndex][charIndex % strlen(CHAR_LINES[lineIndex])];
  snprintf(buf, sizeof(buf), "T:%s C:%c", typeStr, cur);
  u8g2.drawUTF8(0, 25, buf);

  // Aide rapide
  u8g2.setFont(u8g2_font_5x7_tr);
  u8g2.drawUTF8(0, 38, "UP NEXT OK");

  u8g2.sendBuffer();
}

// --------------------- MODE CONFIGURATION ---------------------
void enterConfigMode() {
  tempInput = "";
  lineIndex = 0;
  charIndex = 0;

  // Saisie SSID
  while(ssid == "") {
    showOLEDInput();

    if(readBtn(BTN_UP)){
      charIndex++;
      if(charIndex >= (int)strlen(CHAR_LINES[lineIndex])) charIndex=0;
      waitRelease(BTN_UP);
    }
    if(readBtn(BTN_NEXT)){
      lineIndex++;
      if(lineIndex >= NUM_LINES) lineIndex=0;
      charIndex=0;
      waitRelease(BTN_NEXT);
    }
    if(readBtn(BTN_OK)){
      unsigned long start=millis();
      while(readBtn(BTN_OK));
      unsigned long dur=millis()-start;

      if(dur>1000){ ssid=tempInput; tempInput=""; break; }
      else {
        char c = CHAR_LINES[lineIndex][charIndex];
        if(c=='<'){ if(tempInput.length()>0) tempInput.remove(tempInput.length()-1); }
        else tempInput += c;
      }
    }
    delay(10);
  }

  // Saisie MOT DE PASSE
  while(password == "") {
    showOLEDInput();
    if(readBtn(BTN_UP)){ charIndex++; if(charIndex >= (int)strlen(CHAR_LINES[lineIndex])) charIndex=0; waitRelease(BTN_UP);}
    if(readBtn(BTN_NEXT)){ lineIndex++; if(lineIndex >= NUM_LINES) lineIndex=0; charIndex=0; waitRelease(BTN_NEXT);}
    if(readBtn(BTN_OK)){
      unsigned long start=millis();
      while(readBtn(BTN_OK));
      unsigned long dur=millis()-start;
      if(dur>1000){ password=tempInput; tempInput=""; break; }
      else { char c = CHAR_LINES[lineIndex][charIndex]; if(c=='<'){ if(tempInput.length()>0) tempInput.remove(tempInput.length()-1); } else tempInput += c; }
    }
    delay(10);
  }

  prefs.putString("ssid",ssid);
  prefs.putString("pass",password);

  u8g2.clearBuffer();
  drawCenteredText("Identifiants sauvegardes !",20);
  u8g2.sendBuffer();
  delay(1000);
}

// --------------------- WIFI ---------------------
bool connectWiFi(unsigned long timeout=30000){
  WiFi.begin(ssid.c_str(),password.c_str());
  unsigned long start=millis();
  while(WiFi.status()!=WL_CONNECTED && millis()-start<timeout) delay(500);
  return WiFi.status()==WL_CONNECTED;
}

void showWiFiStatus(){
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  if(WiFi.status()==WL_CONNECTED){
    drawCenteredText("GOOD !",12);
    String ip = maskedIP(WiFi.localIP());
    u8g2.setFont(u8g2_font_6x10_tr);
    int16_t w = u8g2.getUTF8Width(ip.c_str());
    int x = (72-w)/2; if(x<0)x=0;
    u8g2.drawUTF8(x,30,ip.c_str());
  } else drawCenteredText("NO GOOD !",24);
  u8g2.sendBuffer();
}

// --------------------- APPUI LONG ---------------------
void showAppuiLongInvitation(int durationSec) {
  unsigned long startTime = millis();
  const int barX = 4;
  const int fullW = 64;
  const int barY = 24;  // légèrement plus haut qu'avant

  while (millis() - startTime < (unsigned long)durationSec * 1000) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x10_tr);

    // Message court et centré
    if (((millis() / 500) % 2) == 0)
      drawCenteredText("Appui long ->", 10);

    // Barre de progression
    unsigned long elapsed = millis() - startTime;
    int curW = map(elapsed, 0, durationSec * 1000, 0, fullW);
    u8g2.drawFrame(barX, barY, fullW, 6);
    if (curW > 0)
      u8g2.drawBox(barX, barY, curW, 6);

    // Décompte visible, bien séparé
    int secondsLeft = durationSec - (elapsed / 1000);
    char ssec[8];
    snprintf(ssec, sizeof(ssec), "%2ds", secondsLeft);
    u8g2.setFont(u8g2_font_5x7_tr);
    int textW = u8g2.getUTF8Width(ssec);
    int sx = (72 - textW) / 2;
    u8g2.drawUTF8(sx, 38, ssec);  // plus bas que la barre, bien dégagé

    u8g2.sendBuffer();
    delay(50);

    // Si appui long pendant le décompte → mode config
    if (readBtn(BTN_OK)) {
      unsigned long ps = millis();
      while (readBtn(BTN_OK));
      if (millis() - ps > 2000) {
        inConfig = true;
        prefs.clear();
        ssid = "";
        password = "";
        enterConfigMode();
        break;
      }
    }
  }
}

// --------------------- TEMPERATURE ---------------------
float readTemperatureC() {
  float t = sht30.readTemperature();
  if(isnan(t)) return NAN;
  return t;
}

// --------------------- HEURE LOCALE ---------------------
bool isDST(int y, int m, int d, int w) {
  // Règle Europe : heure d'été du dernier dimanche de mars à 2h
  // jusqu'au dernier dimanche d'octobre à 3h
  if (m < 3 || m > 10) return false;                      // Jan–Fév & Nov–Déc = heure d’hiver
  if (m > 3 && m < 10) return true;                       // Avr–Sept = heure d’été
  int lastSunday = d - w;                                 // dernier dimanche du mois
  if (m == 3)  return (d - lastSunday) >= 25;             // dernier dimanche de mars
  if (m == 10) return (d - lastSunday) < 25;              // avant dernier dimanche d'octobre

  return false;
}

int getUTCDSTOffset(unsigned long epoch){
  time_t t=epoch;
  struct tm * now= gmtime(&t);
  return isDST(now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_wday)?3600:0;
}
String getLocalTimeString(){
  timeClient.update();
  unsigned long epoch=timeClient.getEpochTime();
  int offset=3600+getUTCDSTOffset(epoch);
  

  time_t local=epoch+offset;
  struct tm *t= gmtime(&local);
  char buf[9]; 
  sprintf(buf,"%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
  return String(buf);
}

// --------------------- SETUP ---------------------
void setup() {
  Serial.begin(115200); 
  delay(50);
  pinMode(BTN_UP, INPUT_PULLUP); 
  pinMode(BTN_NEXT, INPUT_PULLUP); 
  pinMode(BTN_OK, INPUT_PULLUP);

  Wire.begin(SDA_PIN, SCL_PIN);
  u8g2.begin();

  u8g2.clearBuffer(); 
  u8g2.setFont(u8g2_font_6x10_tr);
  drawCenteredText("Boot...", 20); 
  u8g2.sendBuffer();

  prefs.begin("wifi", false);
  ssid = prefs.getString("ssid", "");
  password = prefs.getString("pass", "");

  sht30.begin(SHT30_ADDR);  // Init SHT30

  showAppuiLongInvitation(12);

  bool wifiOK = false;
  if(ssid!="" && password!="") wifiOK = connectWiFi(30000);
  showWiFiStatus();

  if(wifiOK) {
    timeClient.begin(); 
    timeClient.update(); 
    delay(1000);
  }
}

// --------------------- LOOP ---------------------
void loop() {
  static unsigned long lastSwitch = 0;
  static bool showTime = true;

  // Appui long pour mode config
  if(readBtn(BTN_OK)){
    if(!okbuttonPressed) okpressStart = millis();
    okbuttonPressed = true;
    if(millis() - okpressStart > 2000 && !inConfig){
      inConfig = true; 
      prefs.clear(); ssid=""; password=""; 
      enterConfigMode(); 
      WiFi.begin(ssid.c_str(), password.c_str());
      unsigned long start = millis();
      while(WiFi.status() != WL_CONNECTED && millis() - start < 30000) delay(500);
      timeClient.begin(); timeClient.update(); 
      inConfig = false;
    }
  } else okbuttonPressed = false;

  u8g2.clearBuffer();

  // Lecture température
  float temp = readTemperatureC();

  if(WiFi.status() == WL_CONNECTED) {
    // Alternance heure / température toutes les 3s
    if(millis() - lastSwitch > 3000) {
      lastSwitch = millis();
      showTime = !showTime;
    }

    u8g2.setFont(u8g2_font_7x14B_tr);
    if(showTime) {
      String ts = getLocalTimeString();
      int16_t w = u8g2.getUTF8Width(ts.c_str()); 
      int x = (72 - w)/2; if(x<0)x=0;
      u8g2.drawUTF8(x, 12, ts.c_str());
    } else {
      if(!isnan(temp)) {
        char tbuf[16]; 
        snprintf(tbuf, sizeof(tbuf), "%.1f C", temp);
        int16_t w = u8g2.getUTF8Width(tbuf); 
        int x = (72 - w)/2; if(x<0)x=0;
        u8g2.drawUTF8(x, 12, tbuf);
      } else drawCenteredText("Err SHT30", 12);
    }
  } else {
    drawCenteredText("NO GOOD !", 10);
    if(!isnan(temp)) {
      char tbuf[16]; 
      snprintf(tbuf, sizeof(tbuf), "%.1f C", temp);
      int16_t w = u8g2.getUTF8Width(tbuf); 
      int x = (72 - w)/2; if(x<0)x=0;
      u8g2.drawUTF8(x, 28, tbuf);
    }
  }

  u8g2.sendBuffer();
  delay(200);
}


#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h> 

LiquidCrystal_I2C lcd(0x27, 16, 2);

String password = "7355608";

int vyhral = 0;
int prohral = 0;
int TrueCS2Mode = 0; 
int TrueCS2ModeCas = 0;

const int bzucak = 12;

const byte radky = 4;
const byte sloupce = 3;

char keys[radky][sloupce] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte pinyRadku[radky] = {9, 8, 7, 6};
byte pinySloupcu[sloupce] = {5, 4, 3};

byte prvniStrana[] = {
  B00000,
  B00100,
  B01100,
  B00100,
  B00100,
  B01110,
  B00000,
  B11111
};

byte druhaStrana[] = {
  B00000,
  B01110,
  B00010,
  B00110,
  B01000,
  B01110,
  B00000,
  B11111
};

byte tretiStrana[] = {
  B00000,
  B01110,
  B00010,
  B00110,
  B00010,
  B01110,
  B00000,
  B11111
};

byte C41[] = {
  B10000,
  B10111,
  B10101,
  B10111,
  B10101,
  B10111,
  B10000,
  B11111
};

byte C42[] = {
  B00001,
  B11101,
  B10101,
  B11101,
  B10101,
  B11101,
  B00001,
  B11111
};

byte C43[] = {
  B11111,
  B10000,
  B10000,
  B10111,
  B10100,
  B10111,
  B10000,
  B10000
};

byte C44[] = {
  B11111,
  B00001,
  B00001,
  B11101,
  B00101,
  B11101,
  B00001,
  B00001
};


void selectBuzz(){
  digitalWrite(LED_BUILTIN,HIGH);
  tone(bzucak,554);
  delay(150);
  digitalWrite(LED_BUILTIN,LOW);
  noTone(bzucak);
}

void ulozStats() {
  EEPROM.put(0, vyhral);
  EEPROM.put(sizeof(int), prohral);
  EEPROM.put(2 * sizeof(int), TrueCS2Mode);
  EEPROM.put(3 * sizeof(int), TrueCS2ModeCas);
}

void nactiStats() {
  EEPROM.get(0, vyhral);
  EEPROM.get(sizeof(int), prohral);
  EEPROM.get(2 * sizeof(int), TrueCS2Mode);
  EEPROM.get(3 * sizeof(int), TrueCS2ModeCas);
}

void TrueCS2ModeFunc() {
  nactiStats();
  String heslo = "";
  String displayheslo = "";
  selectBuzz();
  delay(500);
  selectBuzz();
  lcd.clear();

  Keypad klavesnice = Keypad(makeKeymap(keys), pinyRadku, pinySloupcu, radky, sloupce);
  while (klavesnice.getKey() != NO_KEY) {
    delay(10);
  }

  while (true) {
    char klavesa = klavesnice.getKey();
    if (klavesa >= '0' && klavesa <= '9') {
      lcd.clear();
      selectBuzz();
      lcd.setCursor(4,0);
      heslo += klavesa;
      displayheslo += "*";
      lcd.print(displayheslo);
    }
    
    if (klavesa == '#'){
      if (TrueCS2ModeCas < 0){
        lcd.clear();
        TrueCS2Countdown(45, heslo);
        return;
      }
      else{
        lcd.clear();
        TrueCS2Countdown(TrueCS2ModeCas, heslo);
        return;
      }
    }
    if (klavesa == '*'){
      selectBuzz();
      mainMenu();
      return;
    }
  }
}

void TrueCS2Countdown(int doba, String heslo) {
  Keypad klavesnice = Keypad(makeKeymap(keys), pinyRadku, pinySloupcu, radky, sloupce);
  while (klavesnice.getKey() != NO_KEY) {
    delay(10);
  }

  int Time = doba;
  String zadano = "";
  lcd.clear();
  unsigned long lastTick = millis();
  unsigned long lastBuzz = millis();
  bool canDefuse = false;
  int defusePresses = 0;
  int defuseNeeded = 10;
  unsigned long lastDefusePress = 0;

  while (Time > 0) {
    if (millis() - lastTick >= 1000) {
      lastTick += 1000;
      Time--;
      lcd.clear();
    }

    if (!canDefuse) {
      int hodiny = Time / 3600;
      int minuty = (Time % 3600) / 60;
      int sekundy = Time % 60;
      lcd.setCursor(4, 0);
      if (hodiny < 10) lcd.print("0");
      lcd.print(hodiny);
      lcd.print(":");
      if (minuty < 10) lcd.print("0");
      lcd.print(minuty);
      lcd.print(":");
      if (sekundy < 10) lcd.print("0");
      lcd.print(sekundy);
    } else {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Zneskodnovani:");
      lcd.setCursor(0,1);
      lcd.print(defusePresses);
      lcd.print("/");
      lcd.print(defuseNeeded);
    }

    char klavesa = klavesnice.getKey();
    if (!canDefuse) {
      if (klavesa) {
        if (klavesa >= '0' && klavesa <= '9') {
          zadano += klavesa;
        }
        if (klavesa == '#') {
          if (zadano == heslo) {
            zadano = "";
            canDefuse = true;
            lcd.clear();
            lcd.print("Zneskodneni:");
            lcd.setCursor(0, 1);
            lcd.print("Stiskni 1");
            lcd.setCursor(0, 0);
            delay(1000);
          }
          else{
            lcd.clear();
            lcd.print("Neodpovida heslu");
            tone(bzucak,223);
            delay(100);
            noTone(bzucak);
            delay(100);
            tone(bzucak,223);
            delay(600);
            noTone(bzucak);
            delay(100);
            //Time = Time - 5; tohle zatím nechám zakomentovaný - moc to nefunguje
            zadano = "";
          }
        }
      }
    } else {
      if (klavesa == '1') {
        unsigned long now = millis();
        if (now - lastDefusePress >= 1000) { 
          defusePresses++;
          lastDefusePress = now;
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Zneskodnuje:");
          lcd.setCursor(0,1);
          lcd.print(defusePresses);
          lcd.print("/");
          lcd.print(defuseNeeded);
          if (defusePresses >= defuseNeeded) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Bomba");
            lcd.setCursor(0, 1);
            lcd.print("Zneskodnena!");
            TrueCS2Mode = 0;
            vyhral += 1;
            ulozStats();
            tone(bzucak,920);
            delay(100);
            noTone(bzucak);
            delay(100);
            tone(bzucak,920);
            delay(600);
            noTone(bzucak);
            mainMenu();
            return;
          }
        }
      }
    }

    if (Time <= 0) {
      lcd.clear();
      lcd.print("Bomba vybuchla");
      TrueCS2Mode = 0;
      prohral += 1;
      ulozStats();
      tone(bzucak,1046);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(5000);
      noTone(bzucak);
      digitalWrite(LED_BUILTIN, LOW);
      mainMenu();
      return;
    }

    if (Time > 10) {
      if (millis() - lastBuzz >= 1000) {
        lastBuzz += 1000;
        tone(bzucak, 523);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(200);
        noTone(bzucak);
        digitalWrite(LED_BUILTIN, LOW);
      }
    } else if (Time > 5) {
      if (millis() - lastBuzz >= 200) {
        lastBuzz += 200;
        tone(bzucak, 880);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(50);
        noTone(bzucak);
        digitalWrite(LED_BUILTIN, LOW);
      }
    } else if (Time > 2) {
      if (millis() - lastBuzz >= 100) {
        lastBuzz += 100;
        tone(bzucak, 1046);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(30);
        noTone(bzucak);
        digitalWrite(LED_BUILTIN, LOW);
      }
    } else if (Time <= 2 && Time > 0) {
      tone(bzucak, 1046);
      digitalWrite(LED_BUILTIN, HIGH);
    }
  }
}

void Stats() {
  nactiStats(); 
  selectBuzz();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Vyhral:");
  lcd.setCursor(8,0);
  lcd.print(vyhral);
  lcd.setCursor(0,1);
  lcd.print("Prohral:");
  lcd.setCursor(9,1);
  lcd.print(prohral);
  Keypad klavesnice = Keypad(makeKeymap(keys), pinyRadku, pinySloupcu, radky, sloupce);

  while (klavesnice.getKey() != NO_KEY) {
    delay(10);
  }

  while (true) {
    char klavesa = klavesnice.getKey();

    if (klavesa == '#'){
      selectBuzz();
      vyhral = 0;
      prohral = 0;
      ulozStats(); 
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Vyhral:");
      lcd.setCursor(8,0);
      lcd.print(vyhral);
      lcd.setCursor(0,1);
      lcd.print("Prohral:");
      lcd.setCursor(9,1);
      lcd.print(prohral);
    }

    if (klavesa == '*'){
      selectBuzz();
      noTone(bzucak);
      mainMenu();
      return;
    }
    delay(10);
  }

}

void CS2Hra(){

  String heslo = "";
  String displayheslo = "";

  selectBuzz(); 
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("CS2 Hra");
  delay(500);
  lcd.clear();
  lcd.print("Zadej heslo:");

  Keypad klavesnice = Keypad(makeKeymap(keys), pinyRadku, pinySloupcu, radky, sloupce);

  while (klavesnice.getKey() != NO_KEY) {
    delay(10);
  }

  while (true) {
    char klavesa = klavesnice.getKey();

    if (klavesa >= '0' && klavesa <= '9') {
      lcd.clear();
      selectBuzz();
      lcd.setCursor(0,0);
      lcd.print("Zadej heslo:");
      lcd.setCursor(0,1);
      heslo += klavesa;
      displayheslo += "*";
      lcd.print(displayheslo);
    }
    if (klavesa == '#'){
      lcd.clear();
      odpocetBomby(45, "CS2", heslo);
      heslo = "";
      displayheslo = "";
      return;
    }
    if (klavesa == '*'){
      selectBuzz();
      mainMenu();
      return;
    }
  }
}

void scrollText(String text) {
  int lcdWidth = 16;
  String displayText = text;
  for (int i = 0; i < lcdWidth; i++) displayText += " ";
  int len = displayText.length();

  for (int i = 0; i < len; i++) {
    lcd.clear();
    int startIdx = max(0, i - lcdWidth);
    int endIdx = min(len, i);
    String toShow = displayText.substring(startIdx, endIdx);
    while (toShow.length() < lcdWidth) {
      toShow = " " + toShow;
    }
    lcd.setCursor(0, 0);
    lcd.print(toShow);
    delay(350);
  }
  lcd.clear();
}

void Credits(){
  selectBuzz();
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Udelal: Rodrick_");
  lcd.setCursor(0,1); lcd.print("A Ocasnik");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Rodrick:kod");
  lcd.setCursor(0,1); lcd.print("Ocasnik:zapojeni");
  delay(3000);
  lcd.clear();
  scrollText("github.com/rodrickhmmm/C4BombaArduinoProjekt/");
  mainMenu();
}

void Manual(){
  selectBuzz();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("* se vracis do");
  lcd.setCursor(0, 1);
  lcd.print("main menu");
  lcd.setCursor(15, 0);
  lcd.write(byte(0));
  lcd.setCursor(15, 1);
  lcd.print("3");

  Keypad klavesnice = Keypad(makeKeymap(keys), pinyRadku, pinySloupcu, radky, sloupce);

  while (klavesnice.getKey() != NO_KEY) {
    delay(10);
  }

  while (true) {
    char klavesa = klavesnice.getKey();

    if (klavesa == '1'){
      selectBuzz();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("* se vracis do");
      lcd.setCursor(0, 1);
      lcd.print("main menu");
      lcd.setCursor(15, 0);
      lcd.write(byte(0));
      lcd.setCursor(15, 1);
      lcd.print("3");
    }
    
    if (klavesa == '2'){
      selectBuzz();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("# potvrzujes");
      lcd.setCursor(0, 1);
      lcd.print("zadani psw,cas");
      lcd.setCursor(15, 0);
      lcd.write(byte(1));
      lcd.setCursor(15, 1);
      lcd.print("3");
    }

    if (klavesa == '3'){
      selectBuzz();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Pro vice info -");
      lcd.setCursor(0, 1);
      lcd.print("repo na githubu");
      lcd.setCursor(15, 0);
      lcd.write(byte(2));
      lcd.setCursor(15, 1);
      lcd.print("3");
    }

    if (klavesa == '*'){
      selectBuzz();
      noTone(bzucak);
      mainMenu();
      return;
    }
    delay(10);
  }

}
void MoreModes(){
  lcd.clear();            
  selectBuzz();
  lcd.setCursor(0, 0); lcd.print("Stats");
  lcd.setCursor(2,1); lcd.print("1");
  lcd.setCursor(6, 0); lcd.print("Man.");
  lcd.setCursor(7,1); lcd.print("2");
  lcd.setCursor(12, 0); lcd.print("Cred"); 
  lcd.setCursor(13,1); lcd.print("3");


  Keypad klavesnice = Keypad(makeKeymap(keys), pinyRadku, pinySloupcu, radky, sloupce);

  while (klavesnice.getKey() != NO_KEY) {
    delay(10);
  }

  while (true) {
    char klavesa = klavesnice.getKey();
    if (klavesa == '1') {
      Stats();
      return;
    }
    if (klavesa == '3') {
      Credits();
      return;
    }

    if (klavesa == '2'){
      Manual();
      return;
    }

    if (klavesa == '*'){
      selectBuzz();
      noTone(bzucak);
      mainMenu();
      return;
    }
    delay(10);
  }
}

void customHra(){

  selectBuzz(); 

  lcd.clear();

  lcd.print("Zadej dobu (s):");

  String doba = "";
  String heslo = "";
  bool zadanadoba = false;
  bool zadanoheslo = false;
  int dobaVint = 0;

  Keypad klavesnice = Keypad(makeKeymap(keys), pinyRadku, pinySloupcu, radky, sloupce);
  char klavesa = klavesnice.getKey();

  while (klavesnice.getKey() != NO_KEY) {
    delay(10);
  }
  
  while (true) {
    while (zadanadoba = true) {
      char klavesa = klavesnice.getKey();

      if (klavesa >= '0' && klavesa <= '9') {
        lcd.clear();
        selectBuzz();
        lcd.setCursor(0,0);
        lcd.print("Zadej dobu (s):");
        lcd.setCursor(0,1);
        doba += klavesa;
        lcd.print(doba);
      }

      if (klavesa == '#'){
        if (doba != ""){
          dobaVint = doba.toInt();
          lcd.clear();
          zadanadoba = false; 
          selectBuzz();
          lcd.setCursor(0,0);
          lcd.print("Zadej heslo:");
          lcd.setCursor(0,1);
          break;
        }
        else{
          selectBuzz();
          lcd.clear();
          lcd.print("Vypln dobu!");
          delay(1000);
          lcd.clear();
          lcd.print("Zadej dobu (s):");
        }
      }

      if (klavesa == '*'){
        selectBuzz();
        mainMenu();
        return;
      }
    }

    while (zadanoheslo = true) {
      char klavesa = klavesnice.getKey();

      if (klavesa >= '0' && klavesa <= '9') {
        lcd.clear();
        selectBuzz();
        lcd.setCursor(0,0);
        lcd.print("Zadej heslo:");
        lcd.setCursor(0,1);
        heslo += klavesa;
        lcd.print(heslo);
      }

      if (klavesa == '#'){
        if (heslo != ""){
          selectBuzz();
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Doba:");
          lcd.setCursor(5,0);
          lcd.print(doba);
          lcd.setCursor(0,1);
          lcd.print("Heslo:");
          lcd.setCursor(6,1);
          lcd.print(heslo);
          delay(2000);
          zadanoheslo = false; 
          aktivujBombu(dobaVint, "CTM", heslo);
          heslo = "";
          doba = "";
          return;
        }
        else{
          selectBuzz();
          lcd.clear();
          lcd.print("Vypln heslo!");
          delay(1000);
          lcd.clear();
          lcd.print("Zadej heslo:");
        }
      }

      if (klavesa == '*'){
        selectBuzz();
        mainMenu();
        return;
      }

      delay(10);
    }
  }
}

void aktivujBombu(int doba, String mod, String heslo){
  selectBuzz();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Nastaveno na");
  lcd.setCursor(13, 0);
  lcd.print(mod);
  lcd.setCursor(0,1);
  lcd.print("Doba:");
  lcd.print(doba);
  delay(1500);

  lcd.clear();
  lcd.print("Zmackni 1 pro");
  lcd.setCursor(0,1);
  lcd.print("Aktivaci");

  Keypad klavesnice = Keypad(makeKeymap(keys), pinyRadku, pinySloupcu, radky, sloupce);

  while (klavesnice.getKey() != NO_KEY) {
    delay(10);
  }

  while (true) {
    char klavesa = klavesnice.getKey();
    if (klavesa == '1') {
      selectBuzz();
      odpocetBomby(doba, mod, heslo);
      return;
    }
    if (klavesa == '*') {
      selectBuzz();
      mainMenu();
      return;
    }
    delay(10);
  }
}

void odpocetBomby(int doba, String mod, String heslo) {
  Keypad klavesnice = Keypad(makeKeymap(keys), pinyRadku, pinySloupcu, radky, sloupce);

  while (klavesnice.getKey() != NO_KEY) {
    delay(10);
  }

  int Time = doba;
  String zadano = "";

  lcd.clear();
  unsigned long lastTick = millis();
  unsigned long lastBuzz = millis();

  while (Time > 0) {
    if (millis() - lastTick >= 1000) {
      lastTick += 1000;
      Time--;
      lcd.clear();
    }

    int hodiny = Time / 3600;
    int minuty = (Time % 3600) / 60;
    int sekundy = Time % 60;
    lcd.setCursor(4, 0);
    if (hodiny < 10) lcd.print("0");
    lcd.print(hodiny);
    lcd.print(":");
    if (minuty < 10) lcd.print("0");
    lcd.print(minuty);
    lcd.print(":");
    if (sekundy < 10) lcd.print("0");
    lcd.print(sekundy);

    lcd.setCursor(0, 1);
    lcd.print(zadano);

    char klavesa = klavesnice.getKey();
    if (klavesa) {
      if (klavesa >= '0' && klavesa <= '9') {
        zadano += klavesa;
      }

      if (klavesa == '#') {
        if (zadano == heslo) {
          lcd.clear();
          lcd.print("Odpovida heslu");
          vyhral += 1;
          ulozStats();
          tone(bzucak,920);
          delay(100);
          noTone(bzucak);
          delay(100);
          tone(bzucak,920);
          delay(600);
          noTone(bzucak);
          mainMenu();
          return;
        } 

        // tuff cisla
        if (zadano == "67"){
          lcd.clear();
          lcd.print("SIX SEVEEEENNNNNN");
          delay(1500);
          mainMenu();
          return;
        }
        if (zadano == "420"){
          lcd.clear();
          lcd.print("SMOKE WEED EVERY");
          lcd.setCursor(0,1);
          lcd.print("DAY");
          delay(1500);
          mainMenu();
          return;
        }
        if (zadano == "69"){
          lcd.clear();
          lcd.print("I CAN'T GOON");
          delay(1500);
          mainMenu();
          return;
        }
        if (zadano == "41"){
          lcd.clear();
          lcd.print("TS IS SO TUFF");
          lcd.setCursor(0,1);
          lcd.print("BOI");
          delay(1500);
          mainMenu();
          return;
        }
        else {
          lcd.clear();
          lcd.print("Neodpovida heslu");
          tone(bzucak,223);
          delay(100);
          noTone(bzucak);
          delay(100);
          tone(bzucak,223);
          delay(600);
          noTone(bzucak);
          delay(100);
          //Time = Time - 5; tohle zatím nechám zakomentovaný - moc to nefunguje
          zadano = "";
        }
      }
    }

    if (Time > 10) {
      if (millis() - lastBuzz >= 1000) {
        lastBuzz += 1000;
        tone(bzucak, 523);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(200);
        noTone(bzucak);
        digitalWrite(LED_BUILTIN, LOW);
      }
    } else if (Time > 5) {
      if (millis() - lastBuzz >= 200) {
        lastBuzz += 200;
        tone(bzucak, 880);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(50);
        noTone(bzucak);
        digitalWrite(LED_BUILTIN, LOW);
      }
    } else if (Time > 2) {
      if (millis() - lastBuzz >= 100) {
        lastBuzz += 100;
        tone(bzucak, 1046);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(30);
        noTone(bzucak);
        digitalWrite(LED_BUILTIN, LOW);
      }
    } else if (Time <= 2 && Time > 0) {
      tone(bzucak, 1046);
      digitalWrite(LED_BUILTIN, HIGH);
    } else if (Time == 0) {
      lcd.clear();
      lcd.print("Bomba vybuchla");
      prohral += 1;
      ulozStats();
      tone(bzucak,1046);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(5000);
      noTone(bzucak);
      digitalWrite(LED_BUILTIN, LOW);
      mainMenu();
      return;
    }
  }
}

void StndMenu(){

  Keypad klavesnice = Keypad(makeKeymap(keys), pinyRadku, pinySloupcu, radky, sloupce);

  while (klavesnice.getKey() != NO_KEY) {
    delay(10);
  }

  selectBuzz();
  lcd.clear();
  lcd.print("Qck Stnd Lng");
  lcd.setCursor(1, 1); lcd.print("1");
  lcd.setCursor(5, 1); lcd.print("2");
  lcd.setCursor(10,1); lcd.print("3");

  while (true){
    char klavesa = klavesnice.getKey();
    if (klavesa == '1'){
      aktivujBombu(30, "Qck", password);
      return;
    }

    if (klavesa == '2') {
      aktivujBombu(40, "Stnd", password);
      return;
    }

    if (klavesa == '3'){
      aktivujBombu(60, "Lng", password);
      return;
    }

    if (klavesa == '*'){
      selectBuzz();
      mainMenu();
      return;
    }
  }
}

void TrueCS2VybratCas(){
  lcd.clear();
  selectBuzz();

  Keypad klavesnice = Keypad(makeKeymap(keys), pinyRadku, pinySloupcu, radky, sloupce);
  char klavesa = klavesnice.getKey();

  lcd.print("Zadej cas (s):");
  lcd.setCursor(0,1);
  lcd.print(TrueCS2ModeCas);

  while (klavesnice.getKey() != NO_KEY) {
    delay(10);
  }

  while (true){
    char klavesa = klavesnice.getKey();
    if (klavesa >= '0' && klavesa <= '9'){
      TrueCS2ModeCas = TrueCS2ModeCas * 10 + (klavesa - '0'); 
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Zadej cas (s):");
      lcd.setCursor(0,1);
      lcd.print(TrueCS2ModeCas);
    }
    if (klavesa == '#'){
      lcd.clear();
      lcd.print("Nastaveno na:");
      lcd.setCursor(0,1);
      lcd.print(TrueCS2ModeCas);
      delay(1500);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Vypni bombu");
      TrueCS2ModeCas = TrueCS2ModeCas;
      ulozStats();
      return;
    }
  }
}

void Cs2Menu(){

  Keypad klavesnice = Keypad(makeKeymap(keys), pinyRadku, pinySloupcu, radky, sloupce);

  while (klavesnice.getKey() != NO_KEY) {
    delay(10);
  }

  selectBuzz();
  lcd.clear();
  lcd.print("Stnd TrueCS TrueCSCTM");
  lcd.setCursor(1, 1); lcd.print("1");
  lcd.setCursor(8, 1); lcd.print("2");
  lcd.setCursor(14,1); lcd.print("3");

  while (true){
    char klavesa = klavesnice.getKey();
    if (klavesa == '1'){
      CS2Hra();
      return;
    }

    if (klavesa == '2') {
      TrueCS2Mode = 1;
      ulozStats();
      lcd.clear();
      lcd.print("Vypni bombu");
      return;
    }

    if (klavesa == '3'){
      TrueCS2Mode = 1;
      ulozStats();
      TrueCS2VybratCas();
      return;
    }

    if (klavesa == '*'){
      selectBuzz();
      mainMenu();
      return;
    }
  }
}

void mainMenu() {
  lcd.clear();
  lcd.print("Stnd CS2 CTM Mor");
  lcd.setCursor(1, 1); lcd.print("1");
  lcd.setCursor(6, 1); lcd.print("2");
  lcd.setCursor(10,1); lcd.print("3");
  lcd.setCursor(14,1); lcd.print("4");
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, prvniStrana);
  lcd.createChar(1, druhaStrana);
  lcd.createChar(2, tretiStrana);
  lcd.createChar(3, C41);
  lcd.createChar(4, C42);
  lcd.createChar(5, C43);
  lcd.createChar(6, C44);
  pinMode(bzucak,OUTPUT);

  if (EEPROM.read(100) != 123) {
    vyhral = 0;
    prohral = 0;
    ulozStats();
    EEPROM.write(100, 123);
  } else {
    nactiStats();
  }

  if (TrueCS2Mode == 0){
    lcd.setCursor(0, 0);
    lcd.print("C4 Bomba");
    lcd.setCursor(14, 1);
    lcd.write(byte(3));
    lcd.setCursor(15, 1);
    lcd.write(byte(4));
    lcd.setCursor(14, 0);
    lcd.write(byte(5));
    lcd.setCursor(15, 0);
    lcd.write(byte(6));
    delay(1500);

    mainMenu();

    pinMode(LED_BUILTIN, OUTPUT);
  }
  else{
    TrueCS2ModeFunc();
  }
}

Keypad klavesnice = Keypad(makeKeymap(keys), pinyRadku, pinySloupcu, radky, sloupce);

void loop() {

  char klavesa = klavesnice.getKey();

  if (klavesa == '1') {
    StndMenu();
  }
  if (klavesa == '2') {
    Cs2Menu();
  }
  if (klavesa == '3') {
    customHra();
  }
  if (klavesa == '4') {
    MoreModes();
  }

}

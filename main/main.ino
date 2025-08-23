#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

String password = "7355608";

int vyhral = 0;
int prohral = 0;

const int bzucak = 12;

// nastavení klávesnice 4x3
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

void selectBuzz(){
  tone(bzucak,554);
  delay(150);
  noTone(bzucak);
}

void Stats(){
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
  delay(2500);
  lcd.clear();
  mainMenu();
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

  // Počkej, dokud není žádná klávesa stisknutá
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
    if (klavesa == '#'){
      selectBuzz;
      mainMenu();
      return;
    }
  }
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
  mainMenu();
}
void MoreModes(){
  lcd.clear();            
  selectBuzz();
  lcd.setCursor(0, 0); lcd.print("Stats");
  lcd.setCursor(2,1); lcd.print("1");
  lcd.setCursor(10, 0); lcd.print("Credits"); 
  lcd.setCursor(12,1); lcd.print("4");


  Keypad klavesnice = Keypad(makeKeymap(keys), pinyRadku, pinySloupcu, radky, sloupce);

  // Počkej, dokud není žádná klávesa stisknutá
  while (klavesnice.getKey() != NO_KEY) {
    delay(10);
  }

  // Čekej na stisknutí '6'
  while (true) {
    char klavesa = klavesnice.getKey();
    if (klavesa == '1') {
      Stats();
      return;
    }
    if (klavesa == '4') {
      Credits();
      return;
    }

    if (klavesa == '#'){
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
    while (zadanadoba = true) {  // porovnání, ne přiřazení
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
        dobaVint = doba.toInt();
        lcd.clear();
        zadanadoba = false;  // ukončí smyčku
        selectBuzz();
        lcd.setCursor(0,0);
        lcd.print("Zadej heslo:");
        lcd.setCursor(0,1);
        break;
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
        zadanoheslo = false;  // ukončí smyčku
        aktivujBombu(dobaVint, "CTM", heslo);
        heslo = "";
        doba = "";
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

  // Počkej, dokud není žádná klávesa stisknutá
  while (klavesnice.getKey() != NO_KEY) {
    delay(10);
  }

  // Čekej na stisknutí '6'
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

  // Počkej, dokud není žádná klávesa stisknutá
  while (klavesnice.getKey() != NO_KEY) {
    delay(10);
  }

  int Time = doba;
  String zadano = "";

  lcd.clear();
  unsigned long lastTick = millis();
  unsigned long lastBuzz = millis();

  while (Time > 0) {
    // Časový odpočet
    if (millis() - lastTick >= 1000) {
      lastTick += 1000;
      Time--;
      lcd.clear();
    }

    // Zobrazení času
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

    // Zobrazení zadaného hesla
    lcd.setCursor(0, 1);
    lcd.print(zadano);

    // Čtení klávesnice
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
          tone(bzucak,920);
          delay(100);
          noTone(bzucak);
          delay(100);
          tone(bzucak,920);
          delay(600);
          noTone(bzucak);
          mainMenu();
          return;
          // tuff cisla
        } 
        if (zadano == "67"){
          lcd.clear();
          lcd.print("SIX SEVEEEENNNNNN");
          delay(1500);
          mainMenu();
          return;
        }
        if (zadano == "420"){
          lcd.clear();
          lcd.print("SMOKE WEED EVERY DAY");
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
          lcd.print("TS IS SO TUFF BOI");
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
          time = time - 5
          zadano = "";
        }
      }
    }

    // Bzučák – neblokující
    if (Time > 10) {
      if (millis() - lastBuzz >= 1000) {
        lastBuzz += 1000;
        tone(bzucak, 523, 200);
      }
    } else if (Time > 5) {
      if (millis() - lastBuzz >= 200) {
        lastBuzz += 200;
        tone(bzucak, 880, 50);
      }
    } else if (Time > 2) {
      if (millis() - lastBuzz >= 100) {
        lastBuzz += 100;
        tone(bzucak, 1046, 30); // ještě rychlejší a vyšší tón
      }
    } else if (Time > 0) {
      lcd.clear();
      lcd.print("Bomba vybuchla");
      prohral += 1;
      tone(bzucak,1046);
      delay(5000);
      noTone(bzucak);
      mainMenu();
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

    if (klavesa == '#'){
      selectBuzz();
      mainMenu();
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
  lcd.print("Stnd TrueCS");
  lcd.setCursor(1, 1); lcd.print("1");
  lcd.setCursor(8, 1); lcd.print("2");

  while (true){
    char klavesa = klavesnice.getKey();
    if (klavesa == '1'){
      CS2Hra();
      return;
    }

    if (klavesa == '2') {
      //odpocetTrueCS2(45,"TrueCS2", password);
      return;
    }

    if (klavesa == '#'){
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
  pinMode(bzucak,OUTPUT);

  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  
  lcd.setCursor(0, 0);
  lcd.print("C4 Bomba");
  delay(1500);

  mainMenu();

  pinMode(LED_BUILTIN, OUTPUT);
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
/*
    * Typical pin layout used:
     * ----------------------------------------------------------------------------------
     *                          Arduino Uno   |*                          Arduino Uno      
     * Signal      Pin          Pin           |* Signal      Pin          Pin    
     * ---------------------------------------|------------------------------------------
     * KEYPAD      R1            0            |* KEYPAD      C3            A0
     * KEYPAD      R2            2            |* KEYPAD      C4            A3
     * KEYPAD      R3            3            |* CBKC        trig          A2
     * KEYPAD      R4            4            |* CBKC        echo          A1  
     * SERVO1      SCK           5            |* LCD                       A4
     * SERVO2      MOSI          6            |* LCD                       A5
     * KEYPAD      C1            7            |                           
     * KEYPAD      C2            8            |                           
     * RST/Reset   RST           9            |                           
     * SPI SS      SDA(SS)      10            |
     * SPI MOSI    MOSI         11 / ICSP-4   |
     * SPI MISO    MISO         12 / ICSP-1   |
     * SPI SCK     SCK          13 / ICSP-3   |
     * ----------------------------------------------------------------------------------
*/

#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <Servo.h> 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>      
LiquidCrystal_I2C lcd(0x27, 16, 2); // 0x27 địa chỉ LCD, 16 cột và 2 hàng

// RFID 
int PinRST = 9;
int PinSDA = 10;
int UID[4], i;

int ID1[4] = {115, 213, 56, 252}; 
int ID2[4] = {147, 185, 225, 246}; 
int ID3[4] = {013, 25, 64, 193}; 

MFRC522 RFID(PinSDA,PinRST);

// KEYPAD
const byte ROWS = 4; 
const byte COLS = 4; 
char password[5];
char On_equip[]="2002"; // Mật khẩu mở cửa
char Off_equip[]="6363"; // Mật khẩu đóng cửa
int a = 0;
int on = 0;

char MatrixKey[ROWS][COLS] = 
{
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {0,2,3,4}; // R1,R2,R3,R4
byte colPins[COLS] = {7,8,A0,A3}; // C1,C2,C3,C4
Keypad Mykeys = Keypad( makeKeymap(MatrixKey), rowPins, colPins, ROWS, COLS);

// SERVO 5 6 ~
Servo myservo1; //Tạo biến myServo của loại Servo
Servo myservo2;

int servo1 = 5;
int servo2 = 6;

// CBKC
const int trig = A2;
const int echo = A1;
unsigned long duration;
float d_cm;

//=================================================================================================================

void setup() {
  Serial.begin(9600);
  Serial.println("----- Bắt đầu chương trình -----");
  Serial.println();
  
  // LCD 
  lcd.init(); // Khởi tạo màn hình Màn hình
  lcd.backlight(); // Bật đèn màn hình Màn hình
  lcd.setCursor(5, 0);
  lcd.print("Hello");
  lcd.setCursor(0, 1);
  lcd.print("Have A Nice Day!");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" Enter Password");
  delay(2000);
  lcd.setCursor(0, 0);
  lcd.print(" Or Swipe Card!");

  // RFID
  SPI.begin();    
  RFID.PCD_Init(); // Khởi tạo RFID

  // SERVO (Digital PWM 5,6)
  myservo1.attach(servo1); 
  myservo1.write (90); 
  myservo2.attach(servo2); 
  myservo2.write (90); 

  // CBKC
  pinMode(trig, OUTPUT); //chân xuất tín hiệu
  pinMode(echo, INPUT); //chân nhận tín hiệu
  

}  
//=================================================================================================================

void rfID() {
  if ( ! RFID.PICC_IsNewCardPresent()) { return; } // Đọc thẻ 1 lần
  if ( ! RFID.PICC_ReadCardSerial()) {  return;  } // Đọc toàn bộ dl thẻ
  
  Serial.println();
  Serial.print("UID của thẻ: ");   
  
  for (byte i = 0; i < RFID.uid.size; i++) { 
    Serial.print(RFID.uid.uidByte[i] < 0x10 ? " 0" : " ");   // thêm 0 cho số có 1 chữ số
    UID[i] = RFID.uid.uidByte[i]; // Gán
    Serial.print(UID[i]);
    Serial.print(" ");
  }
  Serial.println();
  if (UID[i] == ID1[i]) {
    lcd.clear();
    lcd.print("    Correct!");
    lcd.clear();
    delay(5000); 
    myservo1.write(180); // Mở cửa!
    myservo2.write(0); // Mở cửa!    
    delay(5000);    
    lcd.print("    Opened !");
    Serial.println("[RFID] - Thẻ hợp lệ !");
  }
  else if (UID[i] == ID2[i]) {
    lcd.clear();
    lcd.print("    Correct!");
    lcd.clear(); 
    delay(1000);    
    myservo1.write(180); // Mở cửa!
    myservo2.write(0); // Mở cửa!   
    delay(2000);   
    lcd.print("    Opened !");
    Serial.println("[RFID] - Thẻ hợp lệ !");
  }
  else if (UID[i] == ID3[i]){
    lcd.clear();
    lcd.print("    Correct!");
    lcd.clear();
    delay(1000);     
    myservo1.write(180); // Mở cửa!
    myservo2.write(0); // Mở cửa!    
    delay(2000);
    lcd.print("    Opened !");
    Serial.println("[RFID] - Thẻ hợp lệ !");
  }
  else {
    lcd.clear();
    Serial.println("[RFID] - Thẻ không hợp lệ !");
    lcd.print("   Khong hop le!");
    delay(2000);
    lcd.clear();
    lcd.print("   Try Again!");
    delay(2000);
    lcd.clear();
  }
  Serial.println();

  RFID.PICC_HaltA();  // Vô hiệu hóa thẻ tránh xung đột với thẻ khác
  RFID.PCD_StopCrypto1(); 
}

void KEYPAD() {
  char EnterKey = Mykeys.getKey();
 if (EnterKey)
 {
    password[a]=EnterKey; // Nhập lần lượt các ký tự vào biến chuỗi ký tự Psssword
    a++;
    on++;
    Serial.print("[KEYPAD] - ");
    Serial.println(password);
    // Nhập mật khẩu
    if (a == 0) {
      password[0] = EnterKey;
      lcd.setCursor(0, 0);
      lcd.print(" Enter Password ");
      lcd.setCursor(5, 1);
      lcd.print(password[0]);
      delay(500); // Ký tự hiển thị trên màn hình LCD trong 0.5s
      lcd.setCursor(5, 1);
      lcd.print("*"); // Ký tự được thay bởi dấu *
    }
    if (a == 1) {
      password[1] = EnterKey;
      lcd.setCursor(0, 0);
      lcd.print(" Enter Password");
      lcd.setCursor(6, 1);
      lcd.print(password[1]);
      delay(500);
      lcd.setCursor(6, 1);
      lcd.print("*");
    }
    if (a == 2) {
      password[2] = EnterKey;
      lcd.setCursor(0, 0);
      lcd.print(" Enter Password");
      lcd.setCursor(7, 1);
      lcd.print(password[2]);
      delay(500);
      lcd.setCursor(7, 1);
      lcd.print("*");
    }
    if (a == 3) {
      password[3] = EnterKey;
      lcd.setCursor(0, 0);
      lcd.print(" Enter Password");
      lcd.setCursor(8, 1);
      lcd.print(password[3]);
      delay(500);
      lcd.setCursor(8, 1);
      lcd.print("*");
      delay(1000);
    }
 }
             if( on == 4 ) 
                 {
                if(!strcmp(password,On_equip))
                  {
                    lcd.clear();
                    lcd.print("    Correct!");
                    delay(500);
                    myservo1.write(180); // Mở cửa!
                    myservo2.write(0); // Mở cửa!    
                    delay(1000);
                    lcd.clear();
                    lcd.print("    Opened !");
                    a = 0;
                    Serial.println("[KEYPAD] - Dung mat khau mo cua!");
                    
                  }
                  
                 if(!strcmp(password,Off_equip))
                 {
                    lcd.clear();
                    delay(500);
                    myservo1.write(90); // Đóng cửa!
                    myservo2.write(90); // Đóng cửa!
                    lcd.print("     Closed!");
                    delay(3000);
                    lcd.clear();
                    lcd.print(" Enter Password");
                    a = 0;
                  }
                  
                  if(strcmp(password,On_equip))
                  {
                    if(strcmp(password,Off_equip)) {
                    lcd.clear();
                    lcd.print("   Incorrect!");
                    delay(2000);
                    lcd.clear();
                    lcd.print("   Try Again!");
                    delay(2000);
                    lcd.clear();
                    lcd.print(" Enter Password");
                    a = 0;
                    Serial.println("[KEYPAD] - Sai mat khau nhap lai.............");
                    }
                  }
                  on=0;
                  }
}

void CBKC() {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(5);
  digitalWrite(trig, LOW);

  duration = pulseIn(echo, HIGH);
  d_cm = duration/2/29.412;

  // Serial.print("Distance: ");
  // Serial.print(d_cm);
  // Serial.println(" cm");
  // delay(200);

  if ( d_cm<10 ) {
      myservo1.write(90); // Đóng cửa!
      myservo2.write(90); // Đóng cửa!   
      delay(3000);
  }

}

void loop() {  
  rfID();
  KEYPAD();
  CBKC();
}

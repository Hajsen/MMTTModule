#include <Ethernet.h>
#include <SPI.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10, 160, 238, 17);
IPAddress server(10,160,238,15); 
byte gateway[] = {10,160,238,1}; 
byte subnet[]  = {255,255,255,0};
bool debug = 1;

#define DE_MUX_A 5
#define DE_MUX_B 6
#define DE_MUX_C 13

EthernetClient client;

void setup()
{
  Ethernet.begin(mac, ip, gateway, subnet);
  Serial.begin(9600);

  delay(1000);
  
  Serial.println(Ethernet.localIP());
  Serial.println(server);
  connect: Serial.println("connecting...");
  char functionCall[] = "setDO";
  

  if(connectToServer()){
    executeTestcases();
    execMTFunctionCall("quit", sizeof("quit"));
  } else {
    Serial.println("connection failed");
    Serial.println("retrying in 3...");
    delay(1000);
    Serial.println("retrying in 2...");
    delay(1000);
    Serial.println("retrying in 1...");
    delay(1000);
    Serial.println("retrying...");
    goto connect;
  }
}

bool connectToServer(){
    if (client.connect(server, 6340)) {
      client.write(1);
      while(!client.available()){}
      if(client.read()){
        Serial.println("connected to server");
        return true;
      }
    }
    return false;
}

bool execMTFunctionCall(char *functionCall, size_t len){
      len -= 1;
      client.write(len);
      
      if(debug)
        Serial.print("Functioncall size of  ");
        Serial.print(functionCall);
        Serial.print(": ");
        Serial.print(len);
        Serial.println();
        
      client.print(functionCall);
      while(!client.available()){}
      if(client.read() == len){        
        Serial.println("Successfull functionCall");
        return true;
      }
      return false;
}

void executeTestcases(){
  /*
  enum pinmapping {
    DO22 = 
  }
  testDO(); //klar-ish
  testDI(); //klar-ish 
  testPWM(); //klar-ish
  testPT100(); //klar-ish
   
  */
  /*
  
  test20mAO(); //klar-ish
  */
}

#define DO_PINS 8
#define DIGITAL_OUT 2
#define LAST_TEST_COMBINATION pow(2, (DO_PINS + 1) - 1)

bool testDO(){
  int pinsToTest[] = {};
  int setPins[DO_PINS];
  int readPins[DO_PINS];
  int testCombination = 0;

  // DE_MUX_A = 0; DE_MUX_B = 0 => DO_1
  // DE_MUX_A = 0; DE_MUX_B = 1 => DO_0
  // DE_MUX_A = 1; DE_MUX_B = 0 => DO_2
  // DE_MUX_A = 1; DE_MUX_B = 1 => DO_3
  DOTests: // comparing if the pins set are set to correct status
    for(int i = 0; i < (DO_PINS - 1); i++){
      execMTFunctionCall("setDO", sizeof("setDO"));  
      client.write(pinsToTest[i]);
      client.write(setPins[i]);
      digitalWrite(DE_MUX_A, (i/((int)pow(2, 0)))%2); //takes the binary value of position 0 and assigns DE_MUX_A
      digitalWrite(DE_MUX_B, (i/((int)pow(2, 1)))%2); //takes the binary value of position 1 and assigns DE_MUX_B
      digitalWrite(DE_MUX_C, (i/((int)pow(2, 2)))%2); //takes the binary value of position 2 and assigns DE_MUX_B
      readPins[i] = digitalRead(DIGITAL_IN);
    }
    
    if(memcmp(setPins, readPins, sizeof(a)) != 0){
      //handle not passing test
    }
  
    //set new DO pin configuration (from all 0:s to all 1:s binary)
    for (int i = (DO_PINS - 1); i >= 0; i--) {                
        setPins[i] = (testCombination/((int)pow(2, i)))%2;
    }
    
    if(testCombination != LAST_TEST_COMBINATION){
      testCombination++;
      goto DOTests;
    }
    
    return true;
}

#define DI_PINS 4
//SKA ÄNDRAS DÅ LATCHAR INFÖRS TILL NÄSTA REVISION PÅ DIGITAL_OUT
bool testDI(){  
  for(int i = 0; i < (DI_PINS - 1); i++){
    digitalWrite(DE_MUX_A, (i/((int)pow(2, 0)))%2); //takes the binary value of position 0 and assigns DE_MUX_A
    digitalWrite(DE_MUX_B, (i/((int)pow(2, 1)))%2); //takes the binary value of position 1 and assigns DE_MUX_B
    
    digitalWrite(DIGITAL_OUT, HIGH);
    execMTFunctionCall("setDI", sizeof("setDI"));
    client.write(i); //read from pin
    while(!client.available()){}
    if(client.read() != HIGH){
      //handle failure
    }
    digitalWrite(DIGITAL_OUT, LOW);
    execMTFunctionCall("setDI", sizeof("setDI"));
    client.write(i); //read from pin
    while(!client.available()){}
    if(client.read() != LOW){
      //handle failure
    }
  }

  return true;
}

#define PWM1 11
#define PWM2 12
bool testPWM(){
  pinMode(PWM1, INPUT);
  pinMode(PWM2, INPUT);
  int duty_cycle;
  
  pwm_high = pulseIn(PWM1, HIGH);
  pwm_low = pulseIn(PWM1, LOW);
  duty_cycle = pwm_high/pwm_low;
  if( !(0.301 > duty_cycle && duty_cycle > 0.299)){
    //handle failure
  }
  pwm_high = pulseIn(PWM2, HIGH);
  pwm_low = pulseIn(PWM2, LOW);
  duty_cycle = pwm_high/pwm_low;
  if( !(0.301 > duty_cycle && duty_cycle > 0.299)){
    //handle failure
  }
  return true;
}

#define PT100 23;
#define PT100_PINS 4;
bool testPT100(){
  
  for(int i = 0; i < (PT100_PINS - 1); i++){
    digitalWrite(DE_MUX_A, (i/((int)pow(2, 0)))%2); //takes the binary value of position 0 and assigns DE_MUX_A
    digitalWrite(DE_MUX_B, (i/((int)pow(2, 1)))%2); //takes the binary value of position 1 and assigns DE_MUX_B
    digitalWrite(DE_MUX_C, (i/((int)pow(2, 2)))%2); //takes the binary value of position 2 and assigns DE_MUX_B
    if(!(515 < analogRead(PT100) && analogRead(PT100) < 506)){
      //handle failure
    }
  }
  return true;
}

#define 20mAO_PINS 6;
#define 20mAO 24;
bool test20mAO(){
  for(int i = 0; i < (20mAO_PINS - 1); i++){
    digitalWrite(DE_MUX_A, (i/((int)pow(2, 0)))%2); //takes the binary value of position 0 and assigns DE_MUX_A
    digitalWrite(DE_MUX_B, (i/((int)pow(2, 1)))%2); //takes the binary value of position 1 and assigns DE_MUX_B
    digitalWrite(DE_MUX_C, (i/((int)pow(2, 2)))%2); //takes the binary value of position 2 and assigns DE_MUX_B
    if(!(515 < analogRead(20mAO) && analogRead(20mAO) < 506)){
      //handle failure
    }
  }
  return true;
}

void loop()
{
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    for(;;)
      ;
  }
}

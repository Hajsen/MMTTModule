#include "module.h"

void setup()
{
  Serial.begin(9600);
  canInit();
}

bool execMTFunctionCall(char *functionCall, size_t funclen){
    funclen -= 1;
    sndCan(funclen, 1, 1);
    
    if(debug)
      Serial.print("Functioncall size of  ");
      for(int i = 0; i < funclen; i++){
        Serial.print(functionCall[i]);
      }
      Serial.print(": ");
      Serial.print(funclen);
      Serial.println();
      
    sndCan(functionCall, funclen, 1);
    sndCan(wfa, 1, 2);
    while(!(len = rcvCan()));
    if(funclen == len){        
      Serial.println("Successfull functionCall");
      return true;
    }
    return false;
}


#define DO_PINS 8
#define DIGITAL_OUT 2
#define DIGITAL_IN 3
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
      sndCan(pinsToTest[i], 1, 2);
      sndCan(setPins[i], 1, 2);
      sndCan(wfa, 1, 2);
      while(!(len = rcvCan())); 
      if(rxBuf[0] != setPins[i]){
        //error setting on MT
        Serial.println("Error MT");
      }
      digitalWrite(DE_MUX_A, (i/((int)pow(2, 0)))%2); //takes the binary value of position 0 and assigns DE_MUX_A
      digitalWrite(DE_MUX_B, (i/((int)pow(2, 1)))%2); //takes the binary value of position 1 and assigns DE_MUX_B
      digitalWrite(DE_MUX_C, (i/((int)pow(2, 2)))%2); //takes the binary value of position 2 and assigns DE_MUX_B
      readPins[i] = digitalRead(DIGITAL_IN);
    }
    
    if(memcmp(setPins, readPins, sizeof(setPins)) != 0){
      //handle not passing test
    } else {
      
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
    sndCan(i, 1, 2); //read from pin
    sndCan(wfa, 1, 2);
    while(!(len = rcvCan()));
    if(rxBuf[0] != HIGH){
      //handle failure
    } 
    digitalWrite(DIGITAL_OUT, LOW);
    execMTFunctionCall("setDI", sizeof("setDI"));
    sndCan(i, 1, 1); //read from pin
    sndCan(wfa, 1, 2);
    while(!(len = rcvCan()));
    if(rxBuf[0] != LOW){
      //handle failure
    }
  }

  return true;
}

#define PWM1 11
#define PWM2 12
float pwm_high;
float pwm_low;
float duty_cycle;

bool testPWM(){
  pinMode(PWM1, INPUT);
  pinMode(PWM2, INPUT);
  
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

#define PT100 23
#define PT100_PINS 4
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

#define mAO_PINS 6
#define mAO 24
bool test20mAO(){
  for(int i = 0; i < (mAO_PINS - 1); i++){
    digitalWrite(DE_MUX_A, (i/((int)pow(2, 0)))%2); //takes the binary value of position 0 and assigns DE_MUX_A
    digitalWrite(DE_MUX_B, (i/((int)pow(2, 1)))%2); //takes the binary value of position 1 and assigns DE_MUX_B
    digitalWrite(DE_MUX_C, (i/((int)pow(2, 2)))%2); //takes the binary value of position 2 and assigns DE_MUX_B
    if(!(515 < analogRead(mAO) && analogRead(mAO) < 506)){
      //handle failure
    }
  }
  return true;
}

void loop() {
  sndCan("testDO", 6, 1);
  sndCan(eof, 1, 1);
  while(rcvFunc){
    while(!(len = rcvCan()));
    for(int i = 0; i < len; i++){
      if(rxBuf[i] == EOF){
        rcvFunc = false;
      } else if(rxBuf[i] == 4){
        continue;
      } else {
        functionToRun_len++;
        functionToRun[functionToRun_len - 1] = rxBuf[i];
      }
    }
  }
  runTest(functionToRun, functionToRun_len);
  functionToRun_len = 0;
  delay(10000);
}

void runTest(char *functionToRun, int functionToRun_len){
  while(!Serial);
  Serial.println();
  Serial.println("Running test");
  for(int i = 0; i < functionToRun_len; i++){
    Serial.print(functionToRun[i]);
  }
  Serial.println();
  if(cmpstr(functionToRun, "testDO", functionToRun_len)){
    Serial.println("Running: testDO");
    testDO();
  }
  else if(cmpstr(functionToRun, "testDI", functionToRun_len)){
    Serial.println("Running: testDI");
    testDI();
  }
  else if(cmpstr(functionToRun, "testPWM", functionToRun_len)){
    Serial.println("Running: testPWM");
    testPWM();
  }
  else if(cmpstr(functionToRun, "testPT100", functionToRun_len)){
    Serial.println("Running: testPT100");
    testPT100();
  }
  else if(cmpstr(functionToRun, "test20mAO", functionToRun_len)){
    Serial.println("Running: test20mAO");
    test20mAO();
  }
}

// Very simple string comparison. 
boolean cmpstr(char* function, char* func, int length)
{
  boolean result = false;
  int tmp = 0;

  // Compare length of string. Use the length of the longest string.
  tmp = strlen(func);
  if(tmp > length)
    length = tmp;

  // Compare the strings
  for (int i = 0; i < length; i++)
  {
    if(function[i] == func[i])
      result = true;
    else 
      return false;
  }
  return result;
}

//#################### CAN #################################################

int rcvCan(){
  if(!digitalRead(CAN0_INT)){                          // If CAN0_INT pin is low, read receive buffer
    CAN0.readMsgBuf(&rxId, &len, rxBuf);              // Read data: len = data length, buf = data byte(s)
    /*
    if((rxId & 0x80000000) == 0x80000000)             // Determine if ID is standard (11 bits) or extended (29 bits)
      sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
    else
      sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
    //Serial.print(len);
    */
    
    if((rxId & 0x40000000) == 0x40000000){            // Determine if message is a remote request frame.
      sprintf(msgString, " REMOTE REQUEST FRAME");
      //Serial.print(msgString);
    } else {
      for(byte i = 0; i<len; i++){
        msgString[i] = rxBuf[i];
        Serial.print(rxBuf[i]);
      }
    }    
    return len;
  }
  return 0;
}

bool sndCan(byte *msg, int msg_len, int dest_id){
  byte canbuffer[8];
  int i = 0;
  int q = dest_id;
  for(i = 0;i < msg_len; i++){
    if(!(i%8) && i > 0){
      if(CAN0.sendMsgBuf(q, 8, canbuffer) == CAN_OK)        
        Serial.println("CAN - Message Sent Successfully!");
      else
        Serial.println("Error Sending CAN - Message...");
      
      q++;
    }
    Serial.println(msg[i]);
    canbuffer[i%8] = msg[i];
  }
  
  canbuffer[msg_len % 8] = eot;
  if(CAN0.sendMsgBuf(q, (msg_len)%8 + 1, canbuffer) == CAN_OK)
    Serial.println("CAN - Message Sent Successfully!");
  else
    Serial.println("Error Sending CAN - Message...");
}

void canInit(){
  while(!Serial){};
  // Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");
  
  // Since we do not set NORMAL mode, we are in loopback mode by default.
  //CAN0.setMode(MCP_NORMAL);

  pinMode(CAN0_INT, INPUT);                           // Configuring pin for /INT input
}

//#################### CAN #################################################

#include "module.h"

void setup()
{
  Serial.begin(115200);
  canInit();
  while(!Serial){}
}

bool sanitizepayload(int nextbyte){
  //check if number, uppercase or lowercase
  return ((48 <= nextbyte & nextbyte <= 57 ) | (65 <= nextbyte & nextbyte <= 90) | (97 <= nextbyte & nextbyte <= 122));
}

bool execMTFunctionCall(char *functionCall, size_t funclen){
  byte charlen = funclen;
  sndCan(&charlen, 1, 2);
  
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
  Serial.print("Len rcvd back: ");
  Serial.println(rxBuf[0]);
  if(funclen == rxBuf[0]){        
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

  char result[128];
  // DE_MUX_A = 0; DE_MUX_B = 0 => DO_1
  // DE_MUX_A = 0; DE_MUX_B = 1 => DO_0
  // DE_MUX_A = 1; DE_MUX_B = 0 => DO_2
  // DE_MUX_A = 1; DE_MUX_B = 1 => DO_3
  DOTests: // comparing if the pins set are set to correct status
    for(int i = 0; i < (DO_PINS - 1); i++){
      execMTFunctionCall("setDO", 5); 
      
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
      results[testCombination] = false;
    }
  
    //set new DO pin configuration (from all 0:s to all 1:s binary)
    for (int i = (DO_PINS - 1); i >= 0; i--) {                
        setPins[i] = (testCombination/((int)pow(2, i)))%2;
    }
    
    if(testCombination != LAST_TEST_COMBINATION){
      testCombination++;
      goto DOTests;
    }

    //start of results
    sndCan(sor, 1, 1);
    sndCan("<TestResponse> <TestCase> testDO </TestCase> <Data>", strlen("<TestResponse> <TestCase> testDO </TestCase> <Data>"), 1);
    for(int i = 0; i < testCombination; i++){
      sndCan("<DataPoint type=\"boolean\" name=\"", strlen("<DataPoint type=\"boolean\" name=\""), 1);
      sndCan(i, 1, 1);
      sndCan("\">", strlen("\">"), 1); 
      sndCan(results[i], 1, 1);
      sndCan("</DataPoint>", strlen("</DataPoint>"), 1);
    }
    sndCan("</Data></TestResponse>", strlen("</Data></TestResponse>"), 1);
    sndCan(eor, 1, 1);
    
    return true;
}

#define DI_PINS 4
//SKA ÄNDRAS DÅ LATCHAR INFÖRS TILL NÄSTA REVISION PÅ DIGITAL_OUT
bool testDI(){  
  for(byte i = 0; i < DI_PINS; i++){
    digitalWrite(DE_MUX_A, (i/((int)pow(2, 0)))%2); //takes the binary value of position 0 and assigns DE_MUX_A
    digitalWrite(DE_MUX_B, (i/((int)pow(2, 1)))%2); //takes the binary value of position 1 and assigns DE_MUX_B
    
    digitalWrite(DIGITAL_OUT, HIGH);
    execMTFunctionCall("readA", 5);
    Serial.print("sending HIGH Pin: ");
    sndCan((int)8, 1, 2); //read from pin
    sndCan(wfa, 1, 2);
    while(!(len = rcvCan()));
    int result = 0;
    int result_pos = 0;
    for(int i = len; i >= 0; i--){
      Serial.print("rxBuf ");
      Serial.print(i);
      Serial.print(" : ");
      Serial.println(rxBuf[i]);
      if(rxBuf[i] > 47 && rxBuf[i] < 58){
        result += (rxBuf[i] - 48) * pow(10, result_pos++);
        //sndCan(wfa, 1, 2);
        Serial.print("PART Result: ");
        Serial.println(result);
        rcvCan();
        delay(100);
      }
    }
    Serial.print("Result: ");
    Serial.println(result);
    if(result < 1020){
      results[i] = false;
    } else {
      results[i] = true;
    }
    
    result_pos = 0;
    result = 0;
    
    digitalWrite(DIGITAL_OUT, LOW);
    execMTFunctionCall("readA", 5);
    
    Serial.print("sending LOW Pin: ");
    sndCan(0x08, 1, 1); //read from pin
    sndCan(wfa, 1, 2);
    
    while(!(len = rcvCan()));
       for(int i = len; i >= 0; i--){
      Serial.print("rxBuf ");
      Serial.print(i);
      Serial.print(" : ");
      Serial.println(rxBuf[i]);
      if(rxBuf[i] > 47 && rxBuf[i] < 58){
        result += (rxBuf[i] - 48) * pow(10, result_pos++);
        //sndCan(wfa, 1, 2);
        Serial.print("PART Result: ");
        Serial.println(result);
        rcvCan();
        delay(100);
      }
    }
    if(result < 1020){
      results[i + DI_PINS] = false;
    } else{
      results[i + DI_PINS] = true;
    }
  }

  //start of results
  sndCan(sor, 1, 1);
  sndCan("<TestResponse> <TestCase> testDI </TestCase> <Data>", strlen("<TestResponse> <TestCase> testDI </TestCase> <Data>"), 1);
  for(byte i = 0; i < DI_PINS*2; i++){
    sndCan("<DataPoint type=\"boolean\" name=\"", strlen("<DataPoint type=\"boolean\" name=\""), 1);
    temp = i+48;
    sndCan(&temp, 1, 1); 
    sndCan("\">", strlen("\">"), 1); 
    temp = results[i]+48;
    sndCan(&temp, 1, 1);
    sndCan("</DataPoint>", strlen("</DataPoint>"), 1);
  }
  sndCan("</Data></TestResponse>", strlen("</Data></TestResponse>"), 1);
  sndCan(eor, 1, 1);
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
    results[0] = false;
  }
  pwm_high = pulseIn(PWM2, HIGH);
  pwm_low = pulseIn(PWM2, LOW);
  duty_cycle = pwm_high/pwm_low;
  if( !(0.301 > duty_cycle && duty_cycle > 0.299)){
    //handle failure
    results[1] = false;
  }

  //start of results
  sndCan(sor, 1, 1);
  sndCan("<TestResponse> <TestCase> testPWM </TestCase> <Data>", strlen("<TestResponse> <TestCase> testPWM </TestCase> <Data>"), 1);
  for(byte i = 0; i < 2; i++){
    sndCan("<DataPoint type=\"boolean\" name=\"", strlen("<DataPoint type=\"boolean\" name=\""), 1);
    sndCan(&i, 1, 1);
    sndCan("\">", "\">", 1);
    temp = results[i];
    sndCan(&temp, 1, 1);
    sndCan("</DataPoint>", strlen("</DataPoint>"), 1);
  }
  sndCan("</Data></TestResponse>", strlen("</Data></TestResponse>"), 1);
  sndCan(eor, 1, 1);
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
      results[i] = false;
    }
  }
  
  //start of results
  sndCan(sor, 1, 1);
  sndCan("<TestResponse> <TestCase> testPT100 </TestCase> <Data>", strlen("<TestResponse> <TestCase> testPT100 </TestCase> <Data>"), 1);
  for(byte i = 0; i < 2; i++){
    sndCan("<DataPoint type=\"boolean\" name=\"", strlen("<DataPoint type=\"boolean\" name=\""), 1);
    sndCan(&i, 1, 1);
    sndCan("\">", strlen("\">"), 1);
    temp = results[i];
    sndCan(&temp, 1, 1);
    sndCan("</DataPoint>", strlen("</DataPoint>"), 1);
  }
  sndCan("</Data></TestResponse>", strlen("</Data></TestResponse>"), 1);
  sndCan(eor, 1, 1);
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
      results[i] = false;
    }
  }
  
  //start of results
  sndCan(sor, 1, 1);
  sndCan("<TestResponse> <TestCase> testPWM </TestCase> <Data>", strlen("<TestResponse> <TestCase> testPWM </TestCase> <Data>"), 1);
  for(int i = 0; i < mAO_PINS; i++){
    sndCan("<DataPoint type=\"boolean\" name=\"", strlen("<DataPoint type=\"boolean\" name=\""), 1);
    sndCan(i, 1, 1);
    sndCan("\">", strlen("\">"), 1);
    temp = results[i];
    sndCan(&temp, 1, 1);
    sndCan("</DataPoint>", strlen("</DataPoint>"), 1);
  }
  sndCan("</Data></TestResponse>", strlen("</Data></TestResponse>"), 1);
  sndCan(eor, 1, 1);
  return true;
}

void loop() {
  while(rcvFunc){
    Serial.println("lol");
    while(!(len = rcvCan()));
    for(int i = 0; i < len; i++){
      if(rxBuf[i] == EOF){
        Serial.println("LOLOLOLL");
        rcvFunc = false;
      } else if(rxBuf[i] == 4){
        continue;
      } else if(sanitizepayload(rxBuf[i])){
        functionToRun[functionToRun_len] = rxBuf[i];
        Serial.println(functionToRun[functionToRun_len]);
        functionToRun_len++;
      }
    }
    delay(50);
  }
  runTest(functionToRun, functionToRun_len);
  functionToRun_len = 0;
}

void runTest(char *functionToRun, int functionToRun_len){
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
        while(!Serial){}
        Serial.print(rxBuf[i]);
      }
      Serial.println(" - RCVD CAN");
    }    
    delay(500);
    return len;
  }
  return 0;
}

bool sndCan(byte *msg, int msg_len, int dest_id){
  byte canbuffer[8];
  int i = 0;
  int q = dest_id;
  for(i = 0;i < msg_len; i++){
    Serial.println((char)msg[i]);
    canbuffer[i%8] = msg[i];
    if(!((i+1)%8) && i > 0){
      if(CAN0.sendMsgBuf(q, 8, canbuffer) == CAN_OK)        
        Serial.println("CAN - Message Sent Successfully!");
      else
        Serial.println("Error Sending CAN - Message...");
      
      q++;
    }
  }
        
  canbuffer[msg_len % 8] = eot[0];
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
  CAN0.setMode(MCP_NORMAL);
  //CAN0.setMode(MCP_LOOPBACK);

  pinMode(CAN0_INT, INPUT);                           // Configuring pin for /INT input
}

//#################### CAN #################################################

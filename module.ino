#include "module.h"

void setup()
{
  Serial.begin(115200);
  canInit();
  setPinModes();
  while(!Serial){}
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
#define DIGITAL_IN 2

bool testDI(){
  int readPins[DO_PINS];
  int testCombination = 0;

  char result[128];
  for(int q = 0; q < 2; q++){
    Serial.println();
    Serial.print("DI WHEN STATE IS : ");
    Serial.print(q);
    Serial.println();
    Serial.println("347 | result");
    for(int i = 0; i < (DO_PINS); i++){
      digitalWrite(DE_MUX_A, (i/((int)pow(2, 0)))%2); //takes the binary value of position 0 and assigns DE_MUX_A
      digitalWrite(DE_MUX_B, (i/((int)pow(2, 1)))%2); //takes the binary value of position 1 and assigns DE_MUX_B
      digitalWrite(DE_MUX_C, (i/((int)pow(2, 2)))%2); //takes the binary value of position 2 and assigns DE_MUX_B
      readPins[i] = digitalRead(DIGITAL_IN);
      Serial.println();
      Serial.print((i/((int)pow(2, 0)))%2);
      Serial.print((i/((int)pow(2, 1)))%2);
      Serial.print((i/((int)pow(2, 2)))%2);
      Serial.print(" | ");
      Serial.print(readPins[i]);
    }
    Serial.println();
    Serial.println("Press enter to switch state!");
    while(Serial.available() == 0){}
    Serial.read();
  }
  Serial.println("Press enter to go to next test!");
  while(Serial.available() == 0){}
  Serial.read();  
  return true;
}

#define DIGITAL_OUT 0
#define DI_PINS 4
bool testDO(){
  Serial.println("Testing DO HIGH");    
  digitalWrite(DIGITAL_OUT, HIGH);
  for(int i = 0; i < (DI_PINS - 1); i+=2){
    Serial.println("Testing pin: ");
    Serial.print(i);
    digitalWrite(DE_MUX_A, (i/((int)pow(2, 0)))%2); //takes the binary value of position 0 and assigns DE_MUX_A
    digitalWrite(DE_MUX_B, (i/((int)pow(2, 1)))%2); //takes the binary value of position 1 and assigns DE_MUX_B
    Serial.println("Press enter to go to next pin!");
    while(Serial.available() == 0){}
  }
  Serial.println("Press enter to go to next test!");
  while(Serial.available() == 0){}
  Serial.println("Testing DO HIGH");
  digitalWrite(DIGITAL_OUT, LOW);
  for(int i = 0; i < (DI_PINS - 1); i+=2){
    Serial.println("Testing pin: ");
    Serial.print(i);
    digitalWrite(DE_MUX_A, (i/((int)pow(2, 0)))%2); //takes the binary value of position 0 and assigns DE_MUX_A
    digitalWrite(DE_MUX_B, (i/((int)pow(2, 1)))%2); //takes the binary value of position 1 and assigns DE_MUX_B
  }
}

  return true;
}

#define PWM1 5
#define PWM2 6
float pwm_high;
float pwm_low;
float duty_cycle;

bool testPWM(){
  pinMode(PWM1, INPUT);
  pinMode(PWM2, INPUT);

  Serial.println("Testing PWM1");
  pwm_high = pulseIn(PWM1, HIGH);
  pwm_low = pulseIn(PWM1, LOW);
  duty_cycle = pwm_high/pwm_low;
  if( !(0.301 > duty_cycle && duty_cycle > 0.299)){
    //handle failure
    results[0] = false;
  }
  Serial.println("Result: ");
  Serial.print(results[0]);

  Serial.println("Testing PWM2");
  pwm_high = pulseIn(PWM2, HIGH);
  pwm_low = pulseIn(PWM2, LOW);
  duty_cycle = pwm_high/pwm_low;
  if( !(0.301 > duty_cycle && duty_cycle > 0.299)){
    //handle failure
    results[1] = false;
  }
  Serial.println("Result: ");
  Serial.print(results[1]);
  
  return true;
}

#define PT100 0
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
  /*
  sndCan(sor, 1, 1);
  sndCan("<TestResponse> <TestCase> testPWM </TestCase> <Data>", strlen("<TestResponse> <TestCase> testPWM </TestCase> <Data>"), 1);
  for(int i = 0; i < mAO_PINS; i++){
    sndCan("<DataPoint type=\"boolean\" name=\"", strlen("<DataPoint type=\"boolean\" name=\""), 1);
    sndCan(i, 1, 1);
    sndCan("\">", strlen("\">"), 1);
    sndCan(results[i], 1, 1);
    sndCan("</DataPoint>", strlen("</DataPoint>"), 1);
  }
  sndCan("</Data></TestResponse>", strlen("</Data></TestResponse>"), 1);
  sndCan(eor, 1, 1);
  */
  return true;
}

void setPinModes(){
 pinMode(DE_MUX_A, OUTPUT);
 pinMode(DE_MUX_B, OUTPUT);
 pinMode(DE_MUX_C, OUTPUT);
}
void loop() {
  Serial.println("Running testDI()");
  testDI();
  Serial.println("Press enter to run PWM");
  while(Serial.available() == 0){}
  Serial.read();  
  testPWM();
  Serial.println("Press enter to run PWM");
  while(Serial.available() == 0){}
  Serial.read();  
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
    Serial.print("RxBuf: ");
    if((rxId & 0x40000000) == 0x40000000){            // Determine if message is a remote request frame.
      sprintf(msgString, " REMOTE REQUEST FRAME");
      //Serial.print(msgString);
    } else {
      for(byte i = 0; i<len; i++){
        msgString[i] = rxBuf[i];
        while(!Serial){}
        Serial.print(rxBuf[i]);
      }
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
  CAN0.setMode(MCP_LOOPBACK);

  pinMode(CAN0_INT, INPUT);                           // Configuring pin for /INT input
}

//#################### CAN #################################################

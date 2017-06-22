#include "module.h"

void setup()
{
  Serial.begin(115200);
  canInit();
  setPinModes();
  while(!Serial){}
  Serial.print("Waiting for input");
  
  pinMode(DIGITAL_OUT, OUTPUT);
  Serial.println("Testing DO HIGH");    
  digitalWrite(DIGITAL_OUT, LOW);
  
  while(!Serial.available()){}
  Serial.println(" OK");
}


bool execMTFunctionCall(char *functionCall, size_t funclen){
    funclen -= 1;
    sndCan(funclen, 1, 1);
    
    if(debug)
      Serial.print("Functioncall size of ");
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
  pinMode(DIGITAL_IN, INPUT);

  char result[128];

    Serial.println();
    Serial.println("347 | result");
    for(int i = 0; i < (DO_PINS); i++){
      digitalWrite(DE_MUX_A, (i/((int)pow(2, 0)))%2); //takes the binary value of position 0 and assigns DE_MUX_A
      digitalWrite(DE_MUX_B, (i/((int)pow(2, 1)))%2); //takes the binary value of position 1 and assigns DE_MUX_B
      digitalWrite(DE_MUX_C, (i/((int)pow(2, 2)))%2); //takes the binary value of position 2 and assigns DE_MUX_B
      delay(100);
      readPins[i] = digitalRead(DIGITAL_IN);
      Serial.println();
      Serial.print((i/((int)pow(2, 0)))%2);
      Serial.print((i/((int)pow(2, 1)))%2);
      Serial.print((i/((int)pow(2, 2)))%2);
      Serial.print(" | ");
      Serial.print(readPins[i]);
      delay(100);
    }
    Serial.println();
  
    delay(1000);
  Serial.read();  
  return true;
}


bool testDO(){
  pinMode(DIGITAL_OUT, OUTPUT);
  Serial.println("Testing DO HIGH");    
  digitalWrite(DIGITAL_OUT, HIGH);
    Serial.print("Testing pin: ");
    Serial.print(1);
    Serial.println();
    digitalWrite(DE_MUX_A, 1); //takes the binary value of position 0 and assigns DE_MUX_A
    digitalWrite(DE_MUX_B, 0); //takes the binary value of position 1 and assigns DE_MUX_B
    Serial.println("Press enter to go to next pin!");
    while(Serial.available() == 0){}
  /*
  Serial.println("Press enter to go to next test!");
  while(Serial.available() == 0){}
  Serial.println("Testing DO HIGH");
  digitalWrite(DIGITAL_OUT, LOW);
  for(int i = 0; i < (DI_PINS - 1); i+=2){
    Serial.println("Testing pin: ");
    Serial.print(i);
    digitalWrite(DE_MUX_A, (i/((int)pow(2, 0)))%2); //takes the binary value of position 0 and assigns DE_MUX_A
    digitalWrite(DE_MUX_B, (i/((int)pow(2, 1)))%2); //takes the binary value of position 1 and assigns DE_MUX_B
  }*/
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
  duty_cycle = pwm_high/(pwm_high + pwm_low);
  results[0] = (0.31 > duty_cycle && duty_cycle > 0.29);
  Serial.print("Result: ");
  Serial.print(pwm_high);
  Serial.print(" | ");
  Serial.print(pwm_low);
  Serial.print(" | Passed: ");
  Serial.print(results[0]);
  Serial.print(" | Duty-cycle: ");
  Serial.print(duty_cycle);
  Serial.println();

  Serial.println("Testing PWM2");
  pwm_high = pulseIn(PWM2, HIGH);
  pwm_low = pulseIn(PWM2, LOW);
  duty_cycle = pwm_high/(pwm_high + pwm_low);
  results[1] = (0.31 > duty_cycle && duty_cycle > 0.29);
  Serial.print("Result: ");
  Serial.print(pwm_high);
  Serial.print(" | ");
  Serial.print(pwm_low);
  Serial.print(" | Passed: ");
  Serial.print(results[1]);
  Serial.print(" | Duty-cycle: ");
  Serial.print(duty_cycle);
  Serial.println();
  
  return true;
}

#define PT100 0
#define PT100_PINS 4
bool testPT100(){
  pinMode(PT100, INPUT);
  Serial.println("Checking if voltage is correct from pt100, should be 2.5V");
  for(int i = 0; i < PT100_PINS; i++){
    digitalWrite(DE_MUX_A, (i/((int)pow(2, 0)))%2); //takes the binary value of position 0 and assigns DE_MUX_A
    digitalWrite(DE_MUX_B, (i/((int)pow(2, 1)))%2); //takes the binary value of position 1 and assigns DE_MUX_B
    digitalWrite(DE_MUX_C, (i/((int)pow(2, 2)))%2); //takes the binary value of position 2 and assigns DE_MUX_B
    int pt100AValue = analogRead(PT100);
    //between 2.46V and 2.54V
    results[i] = (347 > pt100AValue && pt100AValue > 327);
    Serial.println("On PT100 pin: ");
    Serial.print(i);
    Serial.print(" | Result: ");
    Serial.print(pt100AValue);
    Serial.print(" | Passed: ");
    Serial.print(results[i]);
    Serial.println();
    delay(3000);
  }
  return true;
}

#define mAO_PINS 6
#define mAO 1
int valuemAO;
bool test20mAO(){
  pinMode(mAO, INPUT);
  for(int i = 0; i < mAO_PINS; i++){
    digitalWrite(DE_MUX_A, (i/((int)pow(2, 0)))%2); //takes the binary value of position 0 and assigns DE_MUX_A
    digitalWrite(DE_MUX_B, (i/((int)pow(2, 1)))%2); //takes the binary value of position 1 and assigns DE_MUX_B
    digitalWrite(DE_MUX_C, (i/((int)pow(2, 2)))%2); //takes the binary value of position 2 and assigns DE_MUX_B
    valuemAO = analogRead(mAO);
    delay(500);
    results[i] = (515 < valuemAO && valuemAO < 506);
    Serial.println("On mAO pin: ");
    Serial.print(i);
    Serial.print(" | Result: ");
    Serial.print(valuemAO);
    Serial.print(" | Passed: ");
    Serial.print(results[i]);
    Serial.println();
  }
  return true;
}

void setPinModes(){
 pinMode(DE_MUX_A, OUTPUT);
 pinMode(DE_MUX_B, OUTPUT);
 pinMode(DE_MUX_C, OUTPUT);
}

void loop() {
  
  //Serial.println("Running testDO()");
  //testDO();
  /*
  Serial.println("Press enter to run DI");
  while(Serial.available() == 0){}
  Serial.read();
  */
  Serial.println("Running testDI()");
  testDI();
  /*
  Serial.println("Press enter to run 20mAO");
  while(Serial.available() == 0){}
  Serial.read();
  Serial.println("Running test20mAO()"); 
  test20mAO();
  Serial.println("Press enter to run PWM");
  while(Serial.available() == 0){}
  Serial.read();
  Serial.println("Running testPWM()"); 
  testPWM();
  Serial.println("Press enter to run pt100");
  while(Serial.available() == 0){}
  Serial.read();  
  Serial.println("Running pt100()");
  testPT100();
  */
  delay(1000);
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

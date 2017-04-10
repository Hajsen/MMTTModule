#include <Ethernet.h>
#include <SPI.h>


byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(10, 160, 238, 17);
IPAddress server(10,160,238,15); 
byte gateway[] = {10,160,238,1}; 
byte subnet[]  = {255,255,255,0};
bool debug = 1;

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

void executeTestcases(){
  /*
  testDO();
  testDI();
  testPWM();
  testPT100();
  test20mAO();
  validateMTFunctionCall(functionCall, sizeof(functionCall));
  */
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

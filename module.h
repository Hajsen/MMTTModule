
#include <SPI.h>

#include <mcp_can.h>
#include <mcp_can_dfs.h>

bool debug = 1;

byte eof[1] = {255}; //end of function
byte eot[1] = {254}; //end of transmission
byte sor[1] = {253}; //start of result
byte eor[1] = {252}; //end of result
byte wfa[1] = {251}; //wait for answer

#define EOF 255 //end of function
#define EOT 254 //end of transmission
#define SOR 253 //start of result
#define EOR 252 //end of result
#define WFA 251 //wait for answer

#define DE_MUX_A 5
#define DE_MUX_B 6
#define DE_MUX_C 13
// CAN0 INT and CS
#define CAN0_INT 8                              // Set INT to pin 2

MCP_CAN CAN0(10);                               // Set CS to pin 11

char functionToRun[30];
size_t functionToRun_len;
bool rcvFunc = true;
void runTest(char *functionToRun);
bool sanitizepayload(int nextbyte);

int results[20] = {true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true};

// CAN RX Variables
long unsigned int rxId;
unsigned char len;
unsigned char rxBuf[8];
byte temp;

// Serial Output String Buffer
char msgString[128];
size_t msgString_len;
byte msg[] = {0x01, 0x04, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0xF1, 0xF2, 0xF3};

int rcvCan();
bool sndCan(byte *msg, int msg_len, int dest_id);
void canInit();

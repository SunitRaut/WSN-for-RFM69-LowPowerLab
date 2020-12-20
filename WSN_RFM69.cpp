#include <WSN_RFM69.h>
#include <RFM69registers.h>
#include <SPI.h>

int WSN_RFM69::backoff;
char WSN_RFM69::pbuffer[PSIZE];
char WSN_RFM69::rbuffer[61];
byte WSN_RFM69::pwrite;
byte WSN_RFM69::pread;
bool WSN_RFM69::pflag;
bool WSN_RFM69::sink;
String WSN_RFM69::message;


WSN_RFM69::WSN_RFM69()	//Constructor
{
    randomSeed(analogRead(A1));
    backoff = random(50);	

}

void WSN_RFM69::setSink(bool is_sink)
{
	sink = is_sink;
}

void WSN_RFM69::sendToSink()
{

}

void WSN_RFM69::fetchPacket()
{

  byte len = pbuffer[pread];
  for (byte i = 0; i < pbuffer[pread]; i++)
  {
    rbuffer[i]=pbuffer[(pread+1+i)%PSIZE];               //copy from RFM buffer to rbuffer
   }//end of for
   pread=(pread+2+pbuffer[pread])%PSIZE;


   int rssi= int(pbuffer[pread]&0xff00)+int(pbuffer[(pread+1)%PSIZE]&0x00ff);

   pread=(pread+2)%PSIZE;
 
   
   if(pread==pwrite) pflag=false;


}


//interruptHook is called every time receiveDone checkpoint is called and a packet is pending to be received through SPI
void WSN_RFM69::interruptHook(uint8_t CTLbyte) 
{



    pbuffer[pwrite]=DATALEN;
    pwrite=(pwrite+1)%PSIZE;

    //Serial.println(F("Interrupt New"));
    for (uint8_t i = 0; i < DATALEN; i++)
{
      //DATA[i] = SPI.transfer(0);				//Commented original implementation
      pbuffer[(pwrite+i)%PSIZE] = SPI.transfer(0);
}//end of for loop
    //if (DATALEN < RF69_MAX_DATA_LEN) DATA[DATALEN] = 0; 	// add null at end of string, commented
    pwrite=(pwrite+DATALEN+1)%PSIZE;
    pflag=true;
  //2/8/2020
    int RSSI = readRSSI();
    //Serial.println("RSSI_write = "+String(RSSI));
    pbuffer[pwrite]=(char)RSSI>>8;
    pbuffer[(pwrite+1)%PSIZE]=RSSI & 0xff;
    pwrite=(pwrite+2)%PSIZE;   
    //Serial.println("\npwrite = "+String(pwrite)); 

    if(ACKRequested())
    {
	sendACK();
    }  

}//end of interruptHook()




//send() sends across a packet and implements MAC protocol while doing so
void WSN_RFM69::send(uint16_t toAddress, const void* buffer, uint8_t bufferSize, bool requestACK)
{
  writeReg(REG_PACKETCONFIG2, (readReg(REG_PACKETCONFIG2) & 0xFB) | RF_PACKET2_RXRESTART); // avoid RX deadlocks
  uint32_t now = millis();

  //while (!canSend() && millis() - now < RF69_CSMA_LIMIT_MS) receiveDone();
 //while (canSend() && millis() - now < backoff) receiveDone();
 //sendFrame(toAddress, buffer, bufferSize, requestACK, false);

// Modified CSMA //////////////
//Persistent channel sensing
while (!canSend()) receiveDone();
bool CSMA_flag=false;
byte csma_count=0;
while(!CSMA_flag)
{
now = millis();
//Initial Waiting Period: Precautionary Backoff
while ((millis()-now) < backoff) {receiveDone();}
if (canSend())  
{
CSMA_flag=true;
//Serial.println(F("sending frame"));
sendFrame(toAddress, buffer, bufferSize, requestACK, false);
}//end of if canSend
//else wait before sensing channel
else
{
while ((millis()-now) < 5) {receiveDone();}
}
if((csma_count++)>100) break;
}//end of while !CSMA_flag

//Modified CSMA End////////////////////

}//end of WSN_RFM69::send


//sendWithRetry() attempts retransmission if no ACK received
bool WSN_RFM69::sendWithRetry(uint16_t toAddress, const void* buffer, uint8_t bufferSize, uint8_t retries, uint8_t retryWaitTime) {
  uint32_t sentTime;
  for (uint8_t i = 0; i <= retries; i++)
  {
	//Serial.println("Attempt = "+String(i));
	if(i>0)
	{
	randomSeed(analogRead(A1));
	backoff =  random(50);
	uint32_t now=millis();
	while ((millis()-now) < backoff) {receiveDone();}
	}
    send(toAddress, buffer, bufferSize, true);
    sentTime = millis();
    while (millis() - sentTime < retryWaitTime)
    {
      if (ACKReceived(toAddress)) {backoff++;return true;}
      //receiveDone();
    }
  }
  return false;
}



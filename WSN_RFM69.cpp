#include <WSN_RFM69.h>
#include <RFM69registers.h>
#include <SPI.h>

int WSN_RFM69::backoff;
char WSN_RFM69::pbuffer[PSIZE];
char WSN_RFM69::rbuffer[61];
char WSN_RFM69::sendbuffer[61];
byte WSN_RFM69::pwrite;
byte WSN_RFM69::pread;
bool WSN_RFM69::pflag;
byte WSN_RFM69::pLen;
int WSN_RFM69::pRSSI;
int WSN_RFM69::pSenderID;

bool WSN_RFM69::sink=false;
String WSN_RFM69::message;
int WSN_RFM69::neighbours[10]={0,0,0,0,0,0,0,0,0,0};
byte WSN_RFM69::hops_count[10]={0,0,0,0,0,0,0,0,0,0};           //Array to store hop_distance of neighbouring nodes
int WSN_RFM69::neighbourCount=0;                                //Count of neighbours
byte WSN_RFM69::hops_from_sink=-1; 				//Unknown value during initialization		  

bool WSN_RFM69::networkMode;

WSN_RFM69::WSN_RFM69()	//Constructor
{
    	randomSeed(analogRead(A1));
    	backoff = random(50);	
	networkMode = false; 					//false by default
}

void WSN_RFM69::setNetworkMode(bool flag)
{
	networkMode = flag;
}

void WSN_RFM69::setSink(bool is_sink)
{
	sink = is_sink;
	if(sink==true)
	{
		hops_from_sink = 0;
		discoveryRESP();	
	}
	else
	{
		discoveryREQ();
	}
	
}



void WSN_RFM69::discoveryREQ()
{ 
	//Serial.println(F("discoveryREQ"));
	sendbuffer[0]=1;
  	tx_PHY(1,0,false);
	//_address=1+_address;
}

void WSN_RFM69::discoveryRESP()
{
	sendbuffer[0]=2;
    	unsigned char id_high = (unsigned char)(_address>>8);             //Assign the higher byte of Node ID. Shift nodeID to left by 8 bits. (x variable remains unaffected) 
	unsigned char id_low  = _address & 0xff;                          //Assign the lower byte of Node ID. NodeID&(00FF) will mask off higher byte. 
	sendbuffer[1]=id_high;sendbuffer[2]=id_low;           //Node Id
    	sendbuffer[3]=hops_from_sink;                         //Hops from sink
    	//int y=256*int(sendbuffer[3])+int(sendbuffer[4]);
    	tx_PHY(4,0,false);                                 //Broadcast message to discovery requesting node

}


void WSN_RFM69::calculate_hops()
{
  	byte x_min=hops_count[0];
  	for(byte i=0;i<neighbourCount;i++)          //find minimum hops_from_sink distance from all neighbours
  	{
    		if(hops_count[i]<x_min)
			x_min=hops_count[i];  //Check for minimum
  	}
  	if(!sink)                 //assign hops_from_sink, only if not sink. 
  	{
    		hops_from_sink=1+x_min;                     //This node is 1+minimum hop distance from sink   
  	}
}


int WSN_RFM69::route()
{
	int nodeList[neighbourCount]={};byte listCount=0;         //temporary variables for processing
  	for(byte ik=0;ik<neighbourCount;ik++)         //loop through complete neighbour list
  	{
    		if(hops_count[ik]<hops_from_sink)          //find a neighbour which is less hops away than itself from sink
    		{
      			nodeList[listCount++]=neighbours[ik];    //add it as an candidate for forwarding the packet
    		}

  	}

  	if (listCount==0) 
  	{
  		hops_from_sink = -1;
  		discoveryREQ();
  		return -1;
  	}//return -1 in case no suitable neighbours
	return(nodeList[random(listCount)]);        //select a random relay node from above selected candidates
	
}



void WSN_RFM69::sendToSink()
{
	//form packet
	sendbuffer[0]=3;
	for(byte i=1;i<=message.length();i++)
	{
	  if(i>60) break;
	  sendbuffer[i]=message[i-1];
	}
	//send to next neighbour
	for(byte i=0;i<NEIGHBOUR_RETRY_LIMIT;i++)
	{
		
		if(tx_PHY(message.length()+1,route(),true))
		{
			//Serial.println(F("break"));
			i=NEIGHBOUR_RETRY_LIMIT;
		}
	}
}



bool WSN_RFM69::tx_PHY(byte L1_length,int toNode, bool requestACK)
{
	//Serial.println(F("tx_PHY"));
	if(toNode<0) return true;
	if(toNode==0)
	{
		send(toNode,sendbuffer,L1_length, false);
	}
	else
	{
		if(sendWithRetry(toNode,sendbuffer,L1_length,RETRY_TIMES,RETRY_INTERVAL))
		{
			//Serial.println(F("ACK received"));
			return true;	
		}
		else
		{	
			bool temp=false;
			for(byte iik=0;iik<neighbourCount;iik++)    //loop through routing table
        		{
          			if(toNode==neighbours[iik])      //continue looping till destination node is found in list
           			{
            				temp=true;
				}
          			if(temp)
           			{
            				if(iik>=1)
            				{
              					neighbours[iik-1]=neighbours[iik];
              					hops_count[iik-1]=hops_count[iik];
             				}
           			}
         		}//end of for
       			neighbourCount=neighbourCount-1;
			return false;
		}
	}

}



bool WSN_RFM69::fetchPacket()
{
	//Serial.println("Fetch");
	byte len = pbuffer[pread];

	pread = (pread+1)%PSIZE;
	char a = pbuffer[pread];
	pread = (pread+1)%PSIZE;
	char b = pbuffer[pread];
	byte packet_type = 0;
	if(networkMode)
	{	
		pread = (pread+1)%PSIZE;
		if(pbuffer[pread]>0)
			packet_type = pbuffer[pread];
		else
			packet_type = 0;
		len = len-1;
		//Serial.println(String(packet_type));
	}
	
  	for (byte i = 0; i < (len); i++)
  	{
    		rbuffer[i]=pbuffer[(pread+1+i)%PSIZE];               //copy from RFM buffer to rbuffer
  	}//end of for
   	pread=(pread+2+len)%PSIZE;


   	int rssi = int(pbuffer[pread]&0xff00)+int(pbuffer[(pread+1)%PSIZE]&0x00ff);
	
   	pread=(pread+2)%PSIZE;
   	
	//Serial.println("pread,pwrite"+String(pread)+","+String(pwrite));
   	if(pread==pwrite) pflag=false;

	//complete fetching the whole packet and only then proceed for further processing
	//Packet Processing
	

	if (!networkMode)
	{	
		pLen = len;
		pRSSI = rssi;
		pSenderID = SENDERID;
		return true;
	}
	
	if (networkMode)
		{
		//Serial.println(char(packet_type));
		if (packet_type==3)
		{	
			if(!sink)
			{
				//relay the packet
				//send to next neighbour
				//message = "";
				//message = message + 3;
				sendbuffer[0] = 3;
				for(byte i=0; i<len; i++)
				{
				  sendbuffer[i+1]=rbuffer[i];	
				}
				//sendToSink();
				tx_PHY(message.length()+1,route(),true);
			}
			pLen = len;
			pRSSI = rssi;
			pSenderID = SENDERID;
			return true;
		}
		else if (packet_type==1)
		{	
			if(hops_from_sink>=0) discoveryRESP();
			pLen = len;
			pRSSI = rssi;
			pSenderID = SENDERID;
			return false;
		}
		else if (packet_type==2)
		{
			//process discoveryRESP here
			//Serial.println("processNeighbour");
			unsigned char high = rbuffer[0];
    			unsigned char low  = rbuffer[1];
			int y = 256*(int(high))+int(low);  
			for(byte i=0;i<neighbourCount;i++)      //loop through existing neighbour list
    			{
      				if ((y==neighbours[i]))          //check if incoming node already exists in neighbour list
      				{ 
        		 		if((hops_count[i]==rbuffer[2]))
        				{
          	 				return false;                             //if duplicate entry found, abort procesing, do not add to neighbourlist
        				}
        				else
        				{
						hops_count[i]=rbuffer[2];             //update hop count
        					byte temp = hops_from_sink;
						calculate_hops();                     //function to calculate node's own hops_from_sink distance
        					if(temp!=hops_from_sink)
						{
							discoveryRESP();	
						}
						return false;
        				}
        
      				}
      
    			}//end of for
			if(neighbourCount==(sizeof(neighbours)/2))  return false;  
			neighbours[neighbourCount]=y;
			hops_count[neighbourCount++]=rbuffer[2];

			pLen = len;
			pRSSI = rssi;
			pSenderID = SENDERID;
			return false;
		}
	}
}//end of fetchPacket()




//interruptHook is called every time receiveDone checkpoint is called and a packet is pending to be received through SPI
void WSN_RFM69::interruptHook(uint8_t CTLbyte) 
{

	if(CTLbyte & RFM69_CTL_SENDACK) return;

	pbuffer[pwrite]=DATALEN;
	//Serial.println(DATALEN);
    	pwrite=(pwrite+1)%PSIZE;
	
	unsigned char id_high = (unsigned char)(SENDERID>>8);             //Assign the higher byte of Node ID. Shift nodeID to left by 8 bits. (x variable remains unaffected) 
	unsigned char id_low  = SENDERID & 0xff;                          //Assign the lower byte of Node ID. NodeID&(00FF) will mask off higher byte. 	

	pbuffer[pwrite] = id_high;
    	pwrite=(pwrite+1)%PSIZE;
	pbuffer[pwrite] = id_low;	
    	pwrite=(pwrite+1)%PSIZE;

	
    	//Serial.println(F("Interrupt New"));
    	for (uint8_t i = 0; i < DATALEN; i++)
	{
      		//DATA[i] = SPI.transfer(0);				//Commented original implementation
      		pbuffer[(pwrite+i)%PSIZE] = SPI.transfer(0);
		//Serial.print(pbuffer[(pwrite+i)%PSIZE]);
	}//end of for loop
    	//if (DATALEN < RF69_MAX_DATA_LEN) DATA[DATALEN] = 0; 	// add null at end of string, commented
    	pwrite=(pwrite+DATALEN+1)%PSIZE;
    	pflag=true;
      	int RSSI = readRSSI();
    	//Serial.println("RSSI_write = "+String(RSSI));
    	pbuffer[pwrite]=(char)RSSI>>8;
    	pbuffer[(pwrite+1)%PSIZE]=RSSI & 0xff;
    	pwrite=(pwrite+2)%PSIZE;   
    	//Serial.println("\npwrite = "+String(pwrite)); 
	
	//Serial.println("pread,pwrite:"+String(pread)+","+String(pwrite));
	
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
bool WSN_RFM69::sendWithRetry(uint16_t toAddress, const void* buffer, uint8_t bufferSize, uint8_t retries=3, uint8_t retryWaitTime=50) 
{
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



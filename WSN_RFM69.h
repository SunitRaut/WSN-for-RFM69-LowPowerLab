// **********************************************************************************
// Author: Sunit Raut   github.com/SunitRaut
// **********************************************************************************
// License
// **********************************************************************************
// This program is free software; you can redistribute it 
// and/or modify it under the terms of the GNU General    
// Public License as published by the Free Software       
// Foundation; either version 3 of the License, or        
// (at your option) any later version.                    
//                                                        
// This program is distributed in the hope that it will   
// be useful, but WITHOUT ANY WARRANTY; without even the  
// implied warranty of MERCHANTABILITY or FITNESS FOR A   
// PARTICULAR PURPOSE. See the GNU General Public        
// License for more details.                              
//                                                        
// Licence can be viewed at                               
// http://www.gnu.org/licenses/gpl-3.0.txt
//
// Please maintain this license information along with authorship
// and copyright notices in any redistribution of this code
// **********************************************************************************


#ifndef WSN_RFM69_h
#define WSN_RFM69_h
#include <RFM69.h>

#define PSIZE 200		//Size of packet buffer in bytes. Set this according to your hardware RAM limit and your estimated RAM usage
// 200 bytes is optimal for Arduino Nano, Uno, etc with 2kB RAM. For ESP8266 or other controllers with more RAM, ou can increase the packet buffer size. 
#define RETRY_TIMES 	1	// How many times to retry a transmission?
#define RETRY_INTERVAL	50	// At what intervals to retry?
#define NEIGHBOUR_RETRY_LIMIT 5	// How many neighbours to try in case of failure

class WSN_RFM69 : public RFM69
{
public:
static char pbuffer[PSIZE];
static char rbuffer[61];
static byte pwrite;
static byte pread;
static bool pflag;
static byte pLen;
static int pRSSI;
static int pSenderID;

static int backoff;
static bool sink;
static String message;
static int neighbours[10];
static byte hops_count[10];                      //Array to store hop_distance of neighbouring nodes
static int neighbourCount;                                           //Count of neighbours
static byte hops_from_sink; 

static bool networkMode;

WSN_RFM69();


bool fetchPacket();

void setNetworkMode(bool flag);
void setSink(bool is_sink); 
void sendToAllNeighbours();
bool sendToNeighbour(uint16_t to_node, bool requestACK);

void sendToSink();

bool tx_PHY(byte L1_length,int toNode,bool requestACK);   

protected:

static char sendbuffer[61];

void send(uint16_t toAddress, const void* buffer, uint8_t bufferSize, bool requestACK);
bool sendWithRetry(uint16_t toAddress, const void* buffer, uint8_t bufferSize, uint8_t retries, uint8_t retryWaitTime); 
static void send_ACK(const void* buffer = "", uint8_t bufferSize=0);
static void check();			//not in use (undefined)
static void isr_modif();		//not in use (undefined)

void interruptHandler();
void interruptHook(uint8_t CTLbyte);

void discoveryREQ();
void discoveryRESP();

void calculate_hops();
int route();
    
};

#endif

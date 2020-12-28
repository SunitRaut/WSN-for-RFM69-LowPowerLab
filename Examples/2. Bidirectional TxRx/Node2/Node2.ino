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

//Project: Send data from Node 1 to Node 2 and vice versa periodically at 5 seconds inteerval
//This file: Node 2

#include <WSN_RFM69.h>


WSN_RFM69 node;

#define NETWORKID     0           // Must be the same for all nodes
#define MYNODEID      2           // My node ID
#define TONODEID      1           //Destination Node ID

#define FREQUENCY     RF69_915MHZ //Required for intialization only
#define USEACK        true       //Requet Acknowledgement or not

String message = "Hello from Node 1"; 
char sendbuffer[61] = "";        
long int now = 0;   //timer for periodically transmitting 

void setup() {

  Serial.begin(9600);//Set baudrate to 9600 bps
  node.initialize(FREQUENCY, MYNODEID, NETWORKID);
  node.setHighPower(); // Always use this for RFM69HCW 
  Serial.println("Node Started");
  now = millis();
}

void loop() {
node.receiveDone();

if(node.pflag) 
 {
  if(node.fetchPacket())
  {
    Serial.println("Received from Node ID: "+String(node.pSenderID));
    for(int i=0;i<node.pLen;i++)
    {
      Serial.print(node.rbuffer[i]);
    }
    Serial.print(F("\n"));
    Serial.println("RSSI: "+String(node.pRSSI));
  }
 }
node.receiveDone();
//Call node.receiveDone() as frequently as you wish. Ideally should be called in between processes taking a long time to complete.
// receiveDone() receives any packets pending in RFM69's buffer and moves it to the 200-byte packet buffer. This prevents loss of packets due to long processing times of certain processes within your program.

if( millis() - now > 5000 )
{
  node.message = F("Hello from Node 2");
  
  Serial.println("Sending Message: "+node.message);
  if (USEACK)
      {
        if(node.sendToNeighbour(TONODEID,true))  
          Serial.println(F("ACK received!"));
        else
          Serial.println(F("no ACK received"));
      }
    else // don't use ACK
      {
        node.sendToNeighbour(TONODEID,false);
      }
      node.message=F("This is broadcast from Node 2");
      Serial.println("Sending Broadcast Message: "+node.message);
      node.sendToAllNeighbours();
      
    now = millis();

}

}

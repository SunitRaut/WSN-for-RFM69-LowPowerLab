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

//Project: Send data from Node(s) to Sink Node every 5 seconds.
//This file: Sink Node. Upload this code to all sink nodes in the network. (Assign a unique node id to each)

#include <WSN_RFM69.h>


WSN_RFM69 node;

#define NETWORKID     0           // Must be the same for all nodes
#define MYNODEID      1           // My node ID
#define FREQUENCY     RF69_915MHZ //Required for intialization only


void setup() {

  Serial.begin(9600);//Set baudrate to 9600 bps
  node.initialize(FREQUENCY, MYNODEID, NETWORKID);
  node.setHighPower(); // Always use this for RFM69HCW 
  node.setPowerLevel(0);
  Serial.println("Node Started");
  node.setNetworkMode(true);
  node.setSink(true);
  
}

void loop() {

node.receiveDone(); //Check if packet has been received and load it in packet buffer node.pbuffer[]
//node.pflag checks whether a new packet is waiting in the packet buffer to be processed
if(node.pflag) 
 {
  if(node.fetchPacket())
  {
    for(int i=0;i<node.pLen;i++)
    {
      Serial.print(node.rbuffer[i]);
    }
    Serial.print(F("\n"));
  }
 }

}

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

//Project: Send data from Node 1 to Node 2 periodically at 5 seconds inteerval
//This file: Transmitter - Node 1

#include <WSN_RFM69.h>


WSN_RFM69 node;

#define NETWORKID     0           // Must be the same for all nodes
#define MYNODEID      1           // My node ID
#define TONODEID      2           //Destination Node ID

#define FREQUENCY     RF69_915MHZ //Required for intialization only
#define USEACK        true       //Requet Acknowledgement or not

     
long int now = 0;   //timer for periodically transmitting 

void setup() {

  Serial.begin(9600);//Set baudrate to 9600 bps
  node.initialize(FREQUENCY, MYNODEID, NETWORKID);
  node.setHighPower(); // Always use this for RFM69HCW 
  Serial.println(F("Node Started"));
  now = millis();
}

void loop() {
node.receiveDone();
if( millis() - now > 5000 )
{
  node.message = F("Hello from Node 1");
  
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
      node.message=F("This is broadcast");
      Serial.println("Sending Broadcast Message: "+node.message);
      node.sendToAllNeighbours();
      
    now = millis();

}




}

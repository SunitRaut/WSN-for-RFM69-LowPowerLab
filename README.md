# WSN for RFM69 LowPowerLab Library
[![GitHub release](https://img.shields.io/github/release/SunitRaut/WSN-for-RFM69-LowPowerLab.svg)](https://github.com/SunitRaut/WSN-for-RFM69-LowPowerLab)
[![license](https://img.shields.io/github/license/SunitRaut/WSN-for-RFM69-LowPowerLab.svg)](https://github.com/SunitRaut/WSN-for-RFM69-LowPowerLab/blob/master/LICENSE.txt)

This is a library for developing Wireless Sensor Network using RFM69 LowPowerLab Library. 

## Features:
- Packet Buffer added to avoid loss of packets occuring due to single 61 byte buffer in original LowPowerLab library
- Improved CSMA mechanism to reduce probability of packet collisions and losses
- Form a Wireless Network of Nodes
- Send messages from Nodes to Sink Node
- Have Multiple Sink Nodes within a network
- Multi-hop Routing capability
- Self-Organizing Network for Node-to-Sink Communication
- Plus all functionality provided by LowPowerLab RFM69 Library 


# How to use Library

Create object of WSN_RFM69 Class.
```
WSN_RFM69 node; // object node of class WSN_RFM69
```
The library can be run in two modes: simple and networkMode. 
By default simple mode is enabled.

- In simple mode, you can use this library exactly like LowPowerLab's RFM69 plus 200-byte packet buffer minus networking ability. 

- In simple mode, you can send max. 61 byte message. (exactly like in LowPowerLab RFM library).

- Please Check the examples '1. Unidirectional TxRx' and '2. Bidirectional TxRx' to know more about usage of simple mode.

- In networkMode, you can form a wireless network of nodes automatically and send messages towards sink node (nearest sink node, in case of multiple sinks). 

- In networkMode, you can send max. 60 byte message. (due to 1-byte network header).

- Please Check the example '3. Wireless Network with Nodes and Sink' to know more about usage of networkMode.

To enable networkMode: ```node.setNetworkMode(true);```
To enable simple mode: ```node.setNetworkMode(false);```

# Simple Mode

To enable simple mode: node.setNetworkMode(false);  //By default, even if you don't declare this statement, the node will operate in simple mode only.

- To send to all Neighbouring Nodes:
```
node.message = "Hello World! Neighbours";

node.sendToAllNeighbours();
```
- To send to a particular Neighbouring Node:
```
node.message = "Hello World! Neighbour";

node.sendToNeighbour(NeighbourNodeId);
```
Note: Neighbour refers to a node which is within the range of transmitting node. 


# networkMode

To enable networkMode: node.setNetworkMode(true);

- In Setup():
```
node.setNetworkMode(true);
```
Sink Node: node.setSink(true) | Node: node.setSink(false)

- How to send messages in networkMode:

-- First assign to message
```
node.message = "Hello World!";      //max. 60 byte message
```
-- Then send towards the nearest sink
```
node.sendToSink();
```
- To send to all Neighbouring Nodes:
```
node.message = "Hello World! Neighbours";

node.sendToAllNeighbours();
```
- To send to a particular Neighbouring Node:
```
node.message = "Hello World! Neighbour";

node.sendToNeighbour(NeighbourNodeId);
```
### Some Important Notes:
- 0 is the broadcast address. Do not assign 0 as node id for any node.
- If there are any functions / processes in your code that take a long time to execute, add node.receiveDone() between the function / process. This ensures that packets are not lost due to such lengthy processes / functions. node.receiveDone() checks for arrival of new packet and sends it to packet buffer, so that it can be processed later in the main loop. 

### Dependencies:

In order to use this library, you need to frst install the following libraries: 
- LowPowerLab RFM69 (https://github.com/LowPowerLab/RFM69)
- LowPowerLab SPIFlash (https://github.com/LowPowerLab/SPIFlash)

You can download these libraries through Arduino Library Manager.

# WSN for RFM69 LowPowerLab Library
This is a library for developing Wireless Sensor Network using RFM69 LowPowerLab Library. 

Features:
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
WSN_RFM69 node; // object node of class WSN_RFM69
You can use all methods/functions provided by LowPowerLab plus additional methods/functions provided by our library.

The library can be run in two modes: simple and networkMode
By default simple mode is enabled.
In simple mode, you can use this library exactly like LowPowerLab's RFM69 plus 200-byte packet buffer minus networking ability.
In simple mode, you can send max. 61 byte message. (exactly like in LowPowerLab RFM library).
In networkMode, you can form a wireless network of nodes automatically and send messages towards sink node (nearest sink node, in case of multiple sinks). 
In networkMode, you can send max. 60 byte message. (due to 1-byte network header).

Other Features Coming Soon:
- Broadcast a message to all neighbouring nodes (in networkMode)
- Broadcast a message to all nodes in network (in networkMode)
- Send packet to a particular neighbouring node (in networkMode)

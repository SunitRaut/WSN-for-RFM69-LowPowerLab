# WSN for RFM69 LowPowerLab Library
This is a library for developing Wireless Sensor Network using RFM69 LowPowerLab Library. 

Features:
- Packet Buffer added to avoid loss of packets occuring due to single 61 byte buffer in original LowPowerLab library
- Improved CSMA mechanism to reduce probability of packet collisions and losses
- [Coming Soon] Form a Wireless Network of Nodes
- [Coming Soon] Send messages from Nodes to Sink Node
- You can use all functions/methods provided by RFM69 LowPowerLab library through object of WSN_RFM69 class.

More features under Development.....

# How to use Library

Create object of WSN_RFM69 Class.
WSN_RFM69 node; // object node of class WSN_RFM69
You can use all methods/functions provided by LowPowerLab plus additional methods/functions provided by our library.

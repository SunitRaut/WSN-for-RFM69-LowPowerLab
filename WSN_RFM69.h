#ifndef WSN_RFM69_h
#define WSN_RFM69_h
#include <RFM69.h>

#define PSIZE 200

class WSN_RFM69 : public RFM69
{
public:
static char pbuffer[PSIZE];
static char rbuffer[61];
static byte pwrite;
static byte pread;
static bool pflag;
static int backoff;
static bool sink;
static String message;


WSN_RFM69();

void send(uint16_t toAddress, const void* buffer, uint8_t bufferSize, bool requestACK);
bool sendWithRetry(uint16_t toAddress, const void* buffer, uint8_t bufferSize, uint8_t retries, uint8_t retryWaitTime); 
static void send_ACK(const void* buffer = "", uint8_t bufferSize=0);
static void check();			//not in use (undefined)
static void isr_modif();		//not in use (undefined)

static void fetchPacket();
void setSink(bool is_sink); 
void sendToSink();    

protected:
void interruptHandler();
void interruptHook(uint8_t CTLbyte);
    
};

#endif
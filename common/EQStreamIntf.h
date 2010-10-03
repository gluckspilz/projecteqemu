#ifndef EQSTREAMINTF_H_
#define EQSTREAMINTF_H_

//this is the only part of an EQStream that is seen by the application.

#include <string>

typedef enum {
	ESTABLISHED,
	CLOSING,		//waiting for pending data to flush.
	DISCONNECTING,	//have sent disconnect, waiting for their disconnect reply.
	CLOSED,			//received a disconnect from remote side.
	UNESTABLISHED
} EQStreamState;

class EQApplicationPacket;

class EQStreamInterface {
public:
	virtual ~EQStreamInterface() {}
	
	virtual void QueuePacket(const EQApplicationPacket *p, bool ack_req=true) = 0;
	virtual void FastQueuePacket(EQApplicationPacket **p, bool ack_req=true) = 0;
	virtual EQApplicationPacket *PopPacket() = 0;
	virtual void Close() = 0;
	virtual void ReleaseFromUse() = 0;
	virtual void RemoveData() = 0;
	virtual uint32 GetRemoteIP() const = 0;
	virtual uint16 GetRemotePort() const = 0;
	virtual bool CheckState(EQStreamState state) = 0;
	virtual std::string Describe() const = 0;

	virtual const int32 GetBytesSent() const { return 0; }
	virtual const int32 GetBytesRecieved() const { return 0; }
	virtual const int32 GetBytesSentPerSecond() const { return 0; }
	virtual const int32 GetBytesRecvPerSecond() const { return 0; }
};

#endif /*EQSTREAMINTF_H_*/

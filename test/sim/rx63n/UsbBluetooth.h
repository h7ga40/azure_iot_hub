//------------------------------------------------------------------------------
//  TOPPERS/ASP Windows Debug Environment
//  Copyright (C) 2010-2019 Cores Co., Ltd. Japan
//------------------------------------------------------------------------------
// $Id: UsbBluetooth.h 1970 2019-07-10 00:36:30Z coas-nagasima $
#ifndef UsbBluetoothH
#define UsbBluetoothH

//---------------------------------------------------------------------------
#include "Kernel.h"
#include "RX63NReg.h"
#include "SIFLayer.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#pragma pack(push, 1)
struct TSetupPacket
{
	uint8_t bmRequestType;
	uint8_t bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
};
#pragma pack(pop)

#define REQUEST_TYPE_DIRECTION_OFFSET	7
#define REQUEST_TYPE_DIRECTION_MASK		(0x01 << REQUEST_TYPE_DIRECTION_OFFSET)
#define REQUEST_TYPE_HOST_TO_DEVICE		(0 << REQUEST_TYPE_DIRECTION_OFFSET)
#define REQUEST_TYPE_DEVICE_TO_HOST		(1 << REQUEST_TYPE_DIRECTION_OFFSET)

#define REQUEST_TYPE_TYPE_OFFSET		5
#define REQUEST_TYPE_TYPE_MASK			(0x03 << REQUEST_TYPE_TYPE_OFFSET)
#define REQUEST_TYPE_STANDARD			(0 << REQUEST_TYPE_TYPE_OFFSET)
#define REQUEST_TYPE_CLASS				(1 << REQUEST_TYPE_TYPE_OFFSET)
#define REQUEST_TYPE_VENDOR				(2 << REQUEST_TYPE_TYPE_OFFSET)

#define REQUEST_TYPE_RECIPIENT_OFFSET	0
#define REQUEST_TYPE_RECIPIENT_MASK		(0x1F << REQUEST_TYPE_RECIPIENT_OFFSET)
#define REQUEST_TYPE_DEVICE				(0 << REQUEST_TYPE_RECIPIENT_OFFSET)
#define REQUEST_TYPE_INTERFACE			(1 << REQUEST_TYPE_RECIPIENT_OFFSET)
#define REQUEST_TYPE_ENDPOINT			(2 << REQUEST_TYPE_RECIPIENT_OFFSET)
#define REQUEST_TYPE_OTHER				(3 << REQUEST_TYPE_RECIPIENT_OFFSET)

#define REQUEST_TYPE_GET_STATUS			0
#define REQUEST_TYPE_CLEAR_FEATURE		1
#define REQUEST_TYPE_SET_FEATURE		3
#define REQUEST_TYPE_SET_ADDRESS		5
#define REQUEST_TYPE_GET_DESCRIPTOR		6
#define REQUEST_TYPE_SET_DESCRIPTOR		7
#define REQUEST_TYPE_GET_CONFIGURATION	8
#define REQUEST_TYPE_SET_CONFIGURATION	9
#define REQUEST_TYPE_GET_INTERFACE		10
#define REQUEST_TYPE_SET_INTERFACE		11
#define REQUEST_TYPE_SYNCH_FRAME		12

#define DESCRIPTOR_TYPE_DEVICE			1
#define DESCRIPTOR_TYPE_CONFIGURATION	2
#define DESCRIPTOR_TYPE_STRING			3
#define DESCRIPTOR_TYPE_INTERFACE		4
#define DESCRIPTOR_TYPE_ENDPOINT		5
#define DESCRIPTOR_TYPE_DEVICE_QUALIFIER 6
#define DESCRIPTOR_TYPE_OTHER_SPEED_CONFIGURATION 7
#define DESCRIPTOR_TYPE_INTERFACE_POWER	8

#define FEATURE_SELECTOR_ENDPOINT_HALT 0		// Endpoint
#define FEATURE_SELECTOR_DEVICE_REMOTE_WAKEUP 1	// Device
#define FEATURE_SELECTOR_TEST_MODE 2			// Device

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#pragma pack(push, 1)
struct TBluetoothHCICommand
{
	uint16_t Command : 10;
	uint16_t Group : 6;
	uint8_t Length;
	uint8_t Parameters[1];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TBluetoothHCIEvent
{
	uint8_t EventCode;
	uint8_t Length;
	uint8_t Parameters[1];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TBluetoothACLData
{
	uint16_t ConnectionHandle : 12;
	uint16_t PacketBoundaryFlag : 2;
	uint16_t BroadcastFlag : 2;
	uint16_t Length;
	uint8_t Data[1];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TBluetoothSCOData
{
	uint16_t ConnectionHandle : 12;
	uint16_t : 4;
	uint8_t Length;
	uint8_t Data[1];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct TBluetoothDataNode
{
	int Size;
	uint8_t Header[2];
	union{
		TBluetoothHCICommand HCICommand;
		TBluetoothHCIEvent HCIEvent;
		TBluetoothACLData ACLData;
		TBluetoothSCOData SCOData;
	};
};
#pragma pack(pop)

typedef list<TBluetoothDataNode *> CBluetoothDataQueue;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class CSifUSB;

enum TUSBBluetoothTerm
{
	ubttIdle,
	ubttSetupStageToken,
	ubttSetupStageData,
	ubttSetupStageHandshake,
	ubttDataStageTokenIn,
	ubttDataStageTokenOut,
	ubttDataStageDataIn,
	ubttDataStageDataOut,
	ubttDataStageHandshakeIn,
	ubttDataStageHandshakeOut,
	ubttStatusStageTokenIn,
	ubttStatusStageTokenOut,
	ubttStatusStageDataIn,
	ubttStatusStageDataOut,
	ubttStatusStageHandshakeIn = ubttIdle,
	ubttStatusStageHandshakeOut = ubttIdle,
	ubttDataTokenIn = ubttStatusStageDataOut + 1,
	ubttDataTokenOut,
	ubttDataDataIn,
	ubttDataDataOut,
	ubttDataHandshakeIn = ubttIdle,
	ubttDataHandshakeOut = ubttIdle,
};

class CUSBBluetooth : public ISysTimerSync
{
public:
	CUSBBluetooth(CSifUSB *owner);
	virtual ~CUSBBluetooth();
private:
	CSifUSB *m_Owner;
	int m_State;
	TUSBBluetoothTerm m_Term;
	__int64 m_Timer;
	int m_Addr;
	int m_EndPoint;
	TSetupPacket m_SetupPacket;
	CBluetoothDataQueue m_HCIEvent;
	CBluetoothDataQueue m_ACLData;
	uint8_t m_RecvACLData[2048];
	int m_RecvACLDataPos;
	bool m_Data1;
	void SendSetupData();
	bool SendDataIn();
	void SendStatusData();
	void SendHandshake();
	void RecvControlData(const void *data, int size);
	void RecvData(const void *data, int size);
	void GetStateOfDevice(TSetupPacket *packet, int size);
	void GetStateOfInterface(TSetupPacket *packet, int size);
	void GetStateOfEndpoint(TSetupPacket *packet, int size);
	void ClearFeatureOfDevice(TSetupPacket *packet, int size);
	void ClearFeatureOfInterface(TSetupPacket *packet, int size);
	void ClearFeatureOfEndpoint(TSetupPacket *packet, int size);
	void SetFeatureOfDevice(TSetupPacket *packet, int size);
	void SetFeatureOfInterface(TSetupPacket *packet, int size);
	void SetFeatureOfEndpoint(TSetupPacket *packet, int size);
	void SetAddressOfDevice(TSetupPacket *packet, int size);
	void GetDescriptorOfDevice(TSetupPacket *packet, int size);
	void SetDescriptorOfDevice(TSetupPacket *packet, int size);
	void GetConfigurationOfDevice(TSetupPacket *packet, int size);
	void SetConfigurationOfDevice(TSetupPacket *packet, int size);
	void GetInterfaceOfDevice(TSetupPacket *packet, int size);
	void SetInterfaceOfInterface(TSetupPacket *packet, int size);
	void SynchFrameOfEndpoint(TSetupPacket *packet, int size);
	void Send(const void *data, int size);
	void SendEmptyData();
	void EnqueueInterruptData(const void *data, int size);
	bool SendInterruptData();
	void EnqueueBulkData(const void *data, int size);
	bool SendBulkData();
public:	// ISysTimerSync
	__int64 GetTimer();
	void Progress(__int64 Timer);
	void CallTimeOut();
public:
	void Receive(const void *data, int size);
	bool HasEvent() { return (m_ACLData.size() > 0) || (m_HCIEvent.size() > 0); }
};

//---------------------------------------------------------------------------
#endif // UsbBluetoothH

#include "GamePacketMaker.h"
#include "Packet.h"
#include "PacketHeader.h"
#include "SerializeBuffer.h"
#include <stdio.h>

void GamePacketMaker::MP_CS_REQ_LOGIN(CPacket* Packet, TCHAR* id, TCHAR* password)
{
	NetHeader Header;
	Header._code = GamePacketCode;
	Header._randKey = rand();

	Packet->PutData((char*)&Header, sizeof(NetHeader));
	uint16 type = PACKET_CS_GAME_REQ_LOGIN;

	*Packet << type;
	Packet->PutData((char*)id, sizeof(TCHAR) * ID_LEN);
	Packet->PutData((char*)password, sizeof(TCHAR) * PASS_LEN);

	uint16 len = (uint16)(Packet->GetDataSize() - sizeof(NetHeader));
	memcpy(Packet->GetBufferPtr() + NET_HEADER_SIZE_INDEX, (void*)&len, sizeof(uint16));
}

void GamePacketMaker::MP_CS_REQ_FIELD_MOVE(CPacket* Packet, uint16& fieldID)
{
	NetHeader Header;
	Header._code = GamePacketCode;
	Header._randKey = rand();

	Packet->PutData((char*)&Header, sizeof(NetHeader));
	uint16 type = PACKET_CS_GAME_REQ_FIELD_MOVE;

	*Packet << type;
	*Packet << fieldID;

	uint16 len = (uint16)(Packet->GetDataSize() - sizeof(NetHeader));
	memcpy(Packet->GetBufferPtr() + NET_HEADER_SIZE_INDEX, (void*)&len, sizeof(uint16));
}


void GamePacketMaker::MP_CS_REQ_CHARACTER_ATTACK(CPacket* packet, int32 AttackerType, int64 AttackerID, int32 TargetType, int64 TargetID)
{
	NetHeader Header;
	Header._code = GamePacketCode;
	Header._randKey = rand();

	packet->PutData((char*)&Header, sizeof(NetHeader));
	uint16 type = PACKET_CS_GAME_REQ_CHARACTER_ATTACK;

	*packet << type;
	*packet << AttackerType;
	*packet << AttackerID;
	*packet << TargetType;
	*packet << TargetID;

	uint16 len = (uint16)(packet->GetDataSize() - sizeof(NetHeader));
	memcpy(packet->GetBufferPtr() + NET_HEADER_SIZE_INDEX, (void*)&len, sizeof(uint16));
}

void GamePacketMaker::MP_SC_REQ_CHARACTER_SKILL(CPacket* packet, FVector& StartLocation, FRotator& StartRotator, int32 SkillID)
{
	NetHeader Header;
	Header._code = GamePacketCode;
	Header._randKey = rand();

	packet->PutData((char*)&Header, sizeof(NetHeader));
	uint16 type = PACKET_CS_GAME_REQ_CHARACTER_SKILL;

	*packet << type;
	*packet << StartLocation;
	*packet << StartRotator;
	*packet << SkillID;

	uint16 len = (uint16)(packet->GetDataSize() - sizeof(NetHeader));
	memcpy(packet->GetBufferPtr() + NET_HEADER_SIZE_INDEX, (void*)&len, sizeof(uint16));
}


void GamePacketMaker::MP_CS_REQ_PLAYER_LIST(CPacket* packet)
{
	NetHeader Header;
	Header._code = GamePacketCode;
	Header._randKey = rand();

	packet->PutData((char*)&Header, sizeof(NetHeader));
	uint16 type = PACKET_CS_GAME_REQ_PLAYER_LIST;

	*packet << type;

	uint16 len = (uint16)(packet->GetDataSize() - sizeof(NetHeader));
	memcpy(packet->GetBufferPtr() + NET_HEADER_SIZE_INDEX, (void*)&len, sizeof(uint16));
}


void GamePacketMaker::MP_CS_REQ_SELECT_PLAYER(CPacket* packet, int64 PlayerID)
{
	NetHeader Header;
	Header._code = GamePacketCode;
	Header._randKey = rand();

	packet->PutData((char*)&Header, sizeof(NetHeader));
	uint16 type = PACKET_CS_GAME_REQ_SELECT_PLAYER;

	*packet << type;
	*packet << PlayerID;

	uint16 len = (uint16)(packet->GetDataSize() - sizeof(NetHeader));
	memcpy(packet->GetBufferPtr() + NET_HEADER_SIZE_INDEX, (void*)&len, sizeof(uint16));
}

void GamePacketMaker::MP_CS_REQ_FIND_PATH(CPacket* packet, FVector& Destination)
{
	NetHeader Header;
	Header._code = GamePacketCode;
	Header._randKey = rand();

	packet->PutData((char*)&Header, sizeof(NetHeader));
	uint16 type = PACKET_CS_GAME_REQ_FIND_PATH;

	*packet << type;
	*packet << Destination;

	uint16 len = (uint16)(packet->GetDataSize() - sizeof(NetHeader));
	memcpy(packet->GetBufferPtr() + NET_HEADER_SIZE_INDEX, (void*)&len, sizeof(uint16));
}

void GamePacketMaker::MP_CS_REQ_CHARACTER_MOVE(CPacket* packet, FVector& Destination, FRotator& StartRotation)
{
	NetHeader Header;
	Header._code = GamePacketCode;
	Header._randKey = rand();

	packet->PutData((char*)&Header, sizeof(NetHeader));
	uint16 type = PACKET_CS_GAME_REQ_CHARACTER_MOVE;

	*packet << type << Destination << StartRotation;

	uint16 len = (uint16)(packet->GetDataSize() - sizeof(NetHeader));
	memcpy(packet->GetBufferPtr() + NET_HEADER_SIZE_INDEX, (void*)&len, sizeof(uint16));

}

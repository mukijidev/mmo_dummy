#pragma once
#include "Type.h"
class CPacket;

class  GamePacketMaker
{
	
public:
	static void MP_CS_REQ_FIELD_MOVE(CPacket* Packet, uint16& fieldID);
	static void MP_CS_REQ_CHARACTER_ATTACK(CPacket* packet, int32 AttackerType, int64 AttackerID, int32 TargetType, int64 TargetID);
	static void MP_SC_REQ_CHARACTER_SKILL(CPacket* packet, FVector& StartLocation, FRotator& StartRotator, int32 SkillID);
	static void MP_CS_REQ_PLAYER_LIST(CPacket* packet);
	static void MP_CS_REQ_SELECT_PLAYER(CPacket* packet, int64 PlayerID);
	static void MP_CS_REQ_FIND_PATH(CPacket* packet, FVector& Destination);
	static void MP_CS_REQ_CHARACTER_MOVE(CPacket* Packet, FVector& Destination, FRotator& StartRotation);

	static void MP_CS_LOGIN_REQ_LOGIN(CPacket* packet, TCHAR* id, TCHAR* password);
	static void MP_CS_GAME_REQ_ENTER(CPacket* packet, int64 accountNo, const char* token);

private:
	inline static uint16 GamePacketCode = 0x77;

};

#include "Dummy.h"
#include <stdio.h>
#include "PacketHeader.h"
#include "SerializeBuffer.h"
#include "Packet.h"
#include "GamePacketMaker.h"
#include "GameData.h"
#include "algorithm"
#include "Util.h"

//int64 _downClient = 0;

static const uint32 FIND_PATH_INTERVAL_MS = 1000;

Dummy::Dummy(int dummyId)
{
	_dummyId = dummyId;
	_state = DS_DISCONNECTED;

	bConnected = false;
}

void Dummy::HandlePacket(CPacket* packet)
{
	uint16 packetType;
	*packet >> packetType;

	switch (packetType)
	{
		//로그인서버
		case PACKET_SC_LOGIN_RES_LOGIN:
		{
			HandleLoginResult(packet);
		}
		break;

		//게임서버 로그인
		case PACKET_SC_GAME_RES_LOGIN:
		{
			//TODO: 게임서버 로그인 성공했으면
			//채팅서버 로그인 하고
			//게임서버 필드 이동
			HandleLogin(packet);
		}
		break;

		case PACKET_SC_GAME_RES_PLAYER_LIST:
		{
			HandlePlayerList(packet);
		}
		break;

		case PACKET_SC_GAME_RES_SELECT_PLAYER:
		{
			HandleSelectPlayer(packet);
		}
		break;

		//게임서버 필드 이동
		case PACKET_SC_GAME_RES_FIELD_MOVE:
		{
			//이거는 일단 할거 없는데
		}
		break;

		//게임서버 내 캐릭터 생성 결과
		case PACKET_SC_GAME_SPAWN_MY_CHARACTER:
		{
			//이거 까지 받으면 이동 가능
			HandleSpawnMyCharacter(packet);
		}
		break;

		//게임서버 다른 캐릭터 생성 결과
		case PACKET_SC_GAME_SPAWN_OTHER_CHAACTER:
		{
			// 다른캐릭터 RemotePlayer에 추가하고
		}
		break;

		// 이거 필요한가? 아직 안쓰고있는데
		case PACKET_SC_GAME_DESPAWN_MY_CHARACTER:
		{
			//이거 어떻게 써야하지
		}
		break;

		//게임 서버 다른 캐릭터 디스폰 결과
		//이거는 다른캐릭터 필드이동같은거나 로그아웃
		case PACKET_SC_GAME_DESPAWN_OTHER_CHARACTER:
		{
			//다른 캐릭터 RemotePlayer에서 제거하고
		}
		break;

		// 내 캐릭터도 이걸로 움직임
		case PACKET_SC_GAME_RES_CHARACTER_MOVE:
		{
			HandleCharacterMove(packet);
		}
		break;

		// 캐릭터 공격받는것 결과
		case PACKET_SC_GAME_RES_DAMAGE:
		{
			//내캐릭터가 다른캐릭터 죽은거 처리하고
		}
		break;

		// 캐릭터 스킬 사용 other character
		case PACKET_SC_GAME_RES_CHARACTER_SKILL:
		{
			// 흠 더미에서 이거는 할거없는데
			// 애니메이션을 틀건 아니니까
		}
		break;

		case PACKET_SC_GAME_RES_MONSTER_SKILL:
		{
			//몬스터 스킬 사용
			//이거 애니메이션 틀려고 한거니까
			//한거 없음
		}
		break;

		case PACKET_SC_GAME_SPAWN_MONSTER:
		{
			HandleSpawnMonster(packet);
		}
		break;

		case PACKET_SC_GAME_MONSTER_MOVE:
		{
			// 몬스터 이동
			// 몬스터 이동 프레임을 돌려야하나?
			// 몬스터 목적지를 그냥 현재위치로 찍을까
			HandleMonterMvoe(packet);
		}
		break;

		case PACKET_SC_GAME_RES_MONSTER_DEATH:
		{
			HandleMonsterDeath(packet);
		}
		break;

		case PACKET_SC_GAME_DESPAWN_MONSTER:
		{
			HandleDespawnMonster(packet);
		}
		break;

		case PACKET_SC_GAME_RES_FIND_PATH:
		{
			HandleFindPath(packet);
		}
		break;

		case PACKET_SC_CHAT_RES_LOGIN:
		{
			//채팅서버 res login 받으면
			//채팅시작할 수 있음
		}
		break;

		case PACKET_SC_CHAT_RES_MESSAGE:
		{
			//채팅 받으면
			//할거 없음
		}
		break;
	}

	CPacket::Free(packet);
	return;
}


void Dummy::Run()
{
	switch(_state)
	{
		case DS_WAIT_FOR_REQUEST:
		{

		}
		break;

		case DS_DISCONNECTED:
		{
			//연결 끊어진 상태면 연결하고
			RequestConnect();
			//printf("Requset Connect %d\n", _dummyId);
		}
		break;

		case DS_LOGIN_CONNECTED:
		{
			RequestLoginToLoginServer();
		}
		break;

		case DS_CONNECTED:
		{
			//연결된 상태면 로그인 하고
			RequestEnterGameServer();
		}
		break;

		case DS_LOGGINED:
		{
			//로그인 된 상태면 내 캐릭터 요청 하고
			RequestCharacterList();
		}
		break;

		case DS_RES_CHARACTER_LIST:
		{
			RequestSelectCharacter();
		}
		break;

		case DS_RES_SELECT_CHARACTER:
		{
			RequestFieldMove();
		}
		break;

		case DS_IN_FIELD:
		{
			//필드까지 온상태에서 랜덤행동
			//RequestMove();
			/*RequestAttack();
			RequestChat();*/
			InFieldAct();
		}
		break;
		
		default:
			__debugbreak();
	}
}


bool Dummy::ConnectTo(const WCHAR* ip, uint16 port)
{
	//	//Connect할대마다 RecvQueue랑 SendQueue 초기화 한번씩 해주고
	_recvQueue.ClearBuffer();
	_sendQueue.ClearBuffer();
	
	int retVal;
	
	memset(&_serverAddr, 0, sizeof(_serverAddr));
	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_port = htons(port);
	InetPtonW(AF_INET, ip, &_serverAddr.sin_addr);
	_serverPort = port;
	
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if (_socket == INVALID_SOCKET)
	{
		wprintf(L"socket create failed\n");
		return false;
	}
	
	
	retVal = connect(_socket, (SOCKADDR*)&_serverAddr, sizeof(_serverAddr));
	if (retVal == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		wprintf(L"connect failed : %d\n", errorCode);
		closesocket(_socket);
		return false;
	}
	
	
	//non block 소켓으로 전환하고
	int setNonBlockError;
	int setNonBlockErrorCode;
	u_long on = 1;
	setNonBlockError = ioctlsocket(_socket, FIONBIO, &on);
	if (setNonBlockError == SOCKET_ERROR)
	{
		setNonBlockErrorCode = WSAGetLastError();
		wprintf(L"set non block failed : %d\n", setNonBlockErrorCode);
		closesocket(_socket);
		bConnected = false;
		return false;
	}
	
	//링거 옵션 적용하고 TIME OUT 없이 바로 fin보내게
	linger _linger;
	_linger.l_onoff = 1;
	_linger.l_linger = 0;
	setsockopt(_socket, SOL_SOCKET, SO_LINGER, (char*)&_linger, sizeof(_linger));
	
	bConnected = true;
	return true;
}

void Dummy::RequestLoginToLoginServer()
{
	TCHAR id[ID_LEN] = { 0 };
	TCHAR password[PASS_LEN] = { 0 };
	wsprintf(id, L"dummy%d", _dummyId);
	wsprintf(password, L"dummy%d", _dummyId);

	CPacket* packet = CPacket::Alloc();
	GamePacketMaker::MP_CS_LOGIN_REQ_LOGIN(packet, id, password);
	SendPacket(packet);
	_state = DS_WAIT_FOR_REQUEST;
}

void Dummy::RequestEnterGameServer()
{
	CPacket* packet = CPacket::Alloc();
	GamePacketMaker::MP_CS_GAME_REQ_ENTER(packet, _accountNo, _authToken);
	SendPacket(packet);
	_state = DS_WAIT_FOR_REQUEST;
}

void Dummy::RequestConnect()
{
	if (ConnectTo(serverIP, loginServerPort))
	{
		//연결 성공했으면
		_state = DS_LOGIN_CONNECTED;
	}
}

void Dummy::RequestDisconnect()
{
	Disconnect();
}

void Dummy::RequestFieldMove()
{
	uint16 fields[3] = { FIELD_LOBBY, FIELD_GUARDIAN, FIELD_SPIDER };
	uint16 fieldId = fields[_dummyId % 3];
	//fieldId = FIELD_LOBBY; 
	currentFieldId = fieldId;


	//printf("[%d] req field move (%s)\n", _dummyId, Util::FieldName(fieldId));

	CPacket* fieldMovePacket = CPacket::Alloc();
	GamePacketMaker::MP_CS_REQ_FIELD_MOVE(fieldMovePacket, fieldId);
	SendPacket(fieldMovePacket);
	_state = DS_WAIT_FOR_REQUEST;
}



void Dummy::RandomMove()
{
	FVector dest = _currentLocation;
	dest.X = std::clamp(dest.X + (rand() % 800 - 400), 100.0, 11900.0);
	dest.Y = std::clamp(dest.Y + (rand() % 800 - 400), 100.0, 11900.0);
	RequestMoveTo(dest);
		
}

bool Dummy::RequestMoveTo(FVector dest)
{
	int now = timeGetTime();
	if (now - _lastFindPathTime < FIND_PATH_INTERVAL_MS)
		return false;

	_lastFindPathTime = now;

	CPacket* p = CPacket::Alloc();
	GamePacketMaker::MP_CS_REQ_FIND_PATH(p, dest);
	SendPacket(p);
	return true;
}

void Dummy::RequestAttackMonster(int64 monsterId)
{
	int now = timeGetTime();
	if (now - _lastAttackTime < 500)
		return;	

	_lastAttackTime = now;

	int32 skillId = 0;      
	_currentRotation.Yaw = Util::CalculateRotation(_currentLocation, _monsters[monsterId].pos);

	CPacket* skillPkt = CPacket::Alloc();
	GamePacketMaker::MP_SC_REQ_CHARACTER_SKILL(skillPkt, _currentLocation, _currentRotation, skillId);
	SendPacket(skillPkt);

	CPacket* pkt = CPacket::Alloc();
	GamePacketMaker::MP_CS_REQ_CHARACTER_ATTACK(pkt, TYPE_PLAYER, _playerId, TYPE_MONSTER, monsterId);
	SendPacket(pkt);
}

//몬스터 근처에 있으면 치고
//없으면 돌아다니고
void Dummy::InFieldAct()
{
	uint32 timeNow = timeGetTime();
	for (auto it = _monsters.begin(); it != _monsters.end(); )
	{
		if (timeNow - it->second.lastSeen > 7000)  
			it = _monsters.erase(it);
		else
			++it;
	}

	if (_monsters.empty())
	{
		RandomMove();
		return;
	}

	int64 targetId = 0;
	int bestDis2 = 0;
	bool first = true;
	for (auto& kv : _monsters)
	{
		int dx = (int)(kv.second.pos.X - _currentLocation.X);
		int dy = (int)(kv.second.pos.Y - _currentLocation.Y);
		int d2 = dx * dx + dy * dy;
		if (first || d2 < bestDis2)
		{
			first = false;
			bestDis2 = d2;
			targetId = kv.first;
		}
	}

	if (_dummyId < 3)
	{
		printf("[d%d] monsters=%d nearest=%lld dist=%.0f\n",
			_dummyId, _monsters.size(), (long long)targetId, sqrt((double)bestDis2));
	}

	if (bestDis2 <= _attackRange * _attackRange)
	{
		RequestAttackMonster(targetId);

		if (timeGetTime() - _lastFindPathTime > 2000)
			RequestMoveTo(_monsters[targetId].pos);
	}
	else {
		RequestMoveTo(_monsters[targetId].pos);
	}
}

void Dummy::RequestAttack()
{
	for (auto it = _remotePlayers.begin(); it != _remotePlayers.end(); ++it)
	{
		FVector remotePlayerLocation = it->second->_currentLocation;
		float distance = GetDistance(remotePlayerLocation);
		if (distance < _attackRange)
		{
			CPacket* packet = CPacket::Alloc();
			GamePacketMaker::MP_CS_REQ_CHARACTER_ATTACK(packet, TYPE_PLAYER, _playerId, 1, it->second->_playerId);
			SendPacket(packet);
		}
	}

}

void Dummy::RequestChat()
{

}

void Dummy::RequestCharacterList()
{
	//printf("request character list\n");
	CPacket* packet = CPacket::Alloc();
	GamePacketMaker::MP_CS_REQ_PLAYER_LIST(packet);
	SendPacket(packet);
	_state = DS_WAIT_FOR_REQUEST;
}

void Dummy::RequestSelectCharacter()
{
	//printf("[%d] req select (id = %lld)\n", _dummyId, _playerId);
	CPacket* packet = CPacket::Alloc();
	// 0번째 캐릭터 선책
	GamePacketMaker::MP_CS_REQ_SELECT_PLAYER(packet, _playerId);
	SendPacket(packet);
	_state = DS_WAIT_FOR_REQUEST;
}


void Dummy::SendPacket(CPacket* packet)
{
	packet->Encode(0x77);
	_sendQueue.Enqueue(packet->GetBufferPtr(), packet->GetDataSize());
	CPacket::Free(packet);
}


float Dummy::GetDistance(FVector& Dest)
{
	float x = _currentLocation.X - Dest.X;
	float y = _currentLocation.Y - Dest.Y;
	return sqrt(x * x + y * y);
}

void Dummy::HandleLoginResult(CPacket* packet)
{
	uint8 status;
	*packet >> _accountNo >> status;

	if (status == 0)
	{
		printf("login server login faield");
		Disconnect();
		_state = DS_DISCONNECTED;
		return;
	}

	packet->GetData((char*)_gameServerIP, IP_LEN * sizeof(WCHAR));
	*packet >> _gameServerPort;
	packet->GetData((char*)_chatServerIP, IP_LEN * sizeof(WCHAR));
	*packet >> _chatServerPort;
	packet->GetData(_authToken, SESSION_KEY_LEN);

	Disconnect();
	if (ConnectTo(_gameServerIP, _gameServerPort))
	{
		_state = DS_CONNECTED;
	}
	else
	{
		_state = DS_DISCONNECTED;
	}
}


void Dummy::HandleLogin(CPacket* packet)
{
	int64 accountNo;
	uint8 status;


	*packet >> accountNo >> status;

	if (status == 1)
	{
		//게임서버로 접속
		//printf("login suceed\n");
		_state = DS_LOGGINED;

	}

	if (status == 0)
	{
		//printf("login failed\n");
		Disconnect();
		_state = DS_DISCONNECTED;
	}
}

void Dummy::HandlePlayerList(CPacket* packet)
{
	uint8 playerCount;
	PlayerInfo playerInfo;
	*packet >> playerCount;

	if (playerCount > 1)
	{
		// dummy's player number is always 1
		__debugbreak();
	}

	*packet >> playerInfo;
	_playerId = playerInfo.PlayerID;
	//printf("dummy playerId : %lld", _playerId);

	// state change to DS_RES_CHARACTER_LIST
	// which means already got character list
	// next step is choose this player
	_state = DS_RES_CHARACTER_LIST;
}

void Dummy::HandleSelectPlayer(CPacket* packet)
{
	//printf("[%d] select  -> field move\n", _dummyId);
	uint8 status;
	*packet >> status;

	if (!status)
	{
		__debugbreak();
	}

	_state = DS_RES_SELECT_CHARACTER;
}

void Dummy::HandleSpawnMyCharacter(CPacket* packet)
{
	FVector SpawnLocation;
	PlayerInfo playerInfo;
	FRotator SpawnRotation;
	*packet >> playerInfo >> SpawnLocation >> SpawnRotation;

	_state = DS_IN_FIELD;
	_currentLocation = SpawnLocation;
	_currentRotation = SpawnRotation;
	//printf("[%d] spawned  (%.0f,%.0f) -> IN_FIELD\n", _dummyId, _currentLocation.Y, _currentLocation.X);

	
}

void Dummy::HandleCharacterMove(CPacket* packet)
{

	int64 characterNo;
	FVector destination;
	FRotator StartRotation;
	*packet >> characterNo >> destination >> StartRotation;

	if (characterNo == _playerId)
	{
		_currentRotation.Yaw = Util::CalculateRotation(_currentLocation, destination);
		_currentLocation.Y = destination.Y;
		_currentLocation.X = destination.X;

	}
}

void Dummy::HandleSpawnMonster(CPacket* packet)
{
	MonsterInfo info;
	FVector spawnLocation;
	FRotator spawnRotator;
	uint32 lastSeen = timeGetTime();
	*packet >> info >> spawnLocation >> spawnRotator;
	_monsters[info.MonsterID] = MonsterSnapshot{ spawnLocation, info.Type, lastSeen };
}

void Dummy::HandleMonterMvoe(CPacket* packet)
{
	int64 monsterId; 
	FVector currentPos;
	*packet >> monsterId >> currentPos;
	auto it = _monsters.find(monsterId);
	if (it != _monsters.end())
	{
		it->second.pos = currentPos;
		it->second.lastSeen = timeGetTime();
	}
}

void Dummy::HandleDespawnMonster(CPacket* packet)
{
	int64 id;
	*packet >> id;
	_monsters.erase(id);
}

void Dummy::HandleMonsterDeath(CPacket* packet)
{
	int64 id;
	*packet >> id;
	_monsters.erase(id);
}

void Dummy::HandleFindPath(CPacket* packet)
{
	int64 characterNo;
	FVector currentPos;
	*packet >> characterNo >> currentPos;
	if (characterNo == _playerId)
		_currentLocation = currentPos;
}




void Dummy::Disconnect()
{
	closesocket(_socket);
	bConnected = false;
	return;
}

void Dummy::SendProcess()
{
	int directDequeueSize = _sendQueue.GetDirectDequeueSize();
	if (directDequeueSize == 0)
	{
		return;
	}

	char* frontPtr = _sendQueue.GetFrontPtr();
	int sendVal = send(_socket, frontPtr, directDequeueSize, 0);
	int sendErrorCode;
	if (sendVal == SOCKET_ERROR)
	{
		sendErrorCode = WSAGetLastError();
		if (sendErrorCode == WSAEWOULDBLOCK)
		{
			return;
		}
		else
		{
			wprintf(L"send failed : %d\n", sendErrorCode);
			OnServerDisconnect();

			return;
		}
	}

	int moveVal = _sendQueue.MoveFront(sendVal);
	// simple error check
	if (moveVal != sendVal)
	{
		__debugbreak();
	}
}

void Dummy::RecvProcess()
{
	int directEnequeueSize = _recvQueue.GetDirectEnqueueSize();
	if (directEnequeueSize == 0)
	{
		__debugbreak();
	}

	char* rearPtr = _recvQueue.GetRearPtr();
	int recvVal = recv(_socket, rearPtr, directEnequeueSize, 0);
	int recvErrorCode;

	if (recvVal == SOCKET_ERROR)
	{
		recvErrorCode = WSAGetLastError();
		if (recvErrorCode == WSAEWOULDBLOCK)
		{
			return;
		}
		else
		{
			wprintf(L"recv failed : %d\n", recvErrorCode);
			OnServerDisconnect();
			//_downClient++;
			return;
		}
	}
	else if (recvVal == 0)
	{
		wprintf(L"server closed\n");
		OnServerDisconnect();

		//fin 받았는데 내가먼저 끊은상황이면
		//_downClient++;		
		return;
	}
	else
	{
		_recvQueue.MoveRear(recvVal);

		for (;;)
		{
			if (_recvQueue.GetUseSize() < sizeof(NetHeader))
			{
				break;
			}

			NetHeader header;
			_recvQueue.Peek((char*)&header, sizeof(NetHeader));

			int dataSize = header._len;
			if (_recvQueue.GetUseSize() < sizeof(NetHeader) + dataSize)
			{
				break;
			}

			_recvQueue.MoveFront(sizeof(NetHeader));
			CPacket* packet = CPacket::Alloc();
			char* buf = packet->GetBufferPtr();
			_recvQueue.Dequeue(buf, dataSize);
			packet->MoveWritePos(dataSize);

			bool bDecodedSucceed = packet->Decode(&header, 0x77);
			if (!bDecodedSucceed)
				__debugbreak();

			HandlePacket(packet);
		}
	}
}


//void Dummy::RequestLogin()
//{
//	printf("request login\n");
//	TCHAR id[16];
//	TCHAR password[16];
//	wsprintf(id, L"dummy%d", _dummyId);
//	wsprintf(password, L"dummy%d", _dummyId);
//
//	CPacket* requestLoginPacket = CPacket::Alloc();
//	GamePacketMaker::MP_CS_REQ_LOGIN(requestLoginPacket, id, password);
//	SendPacket(requestLoginPacket);
//	_state = DS_WAIT_FOR_REQUEST;
//}
//
//
//bool Dummy::Connect()
//{
//	//Connect할대마다 RecvQueue랑 SendQueue 초기화 한번씩 해주고
//	_recvQueue.ClearBuffer();
//	_sendQueue.ClearBuffer();
//
//	int retVal;
//
//	memset(&_serverAddr, 0, sizeof(_serverAddr));
//	_serverAddr.sin_family = AF_INET;
//	_serverAddr.sin_port = htons(serverPort);
//	InetPtonW(AF_INET, serverIP, &_serverAddr.sin_addr);
//	_serverPort = serverPort;
//
//	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//
//	if (_socket == INVALID_SOCKET)
//	{
//		wprintf(L"socket create failed\n");
//		return false;
//	}
//
//
//	retVal = connect(_socket, (SOCKADDR*)&_serverAddr, sizeof(_serverAddr));
//	if (retVal == SOCKET_ERROR)
//	{
//		int errorCode = WSAGetLastError();
//		wprintf(L"connect failed : %d\n", errorCode);
//		closesocket(_socket);
//		return false;
//	}
//
//
//	//non block 소켓으로 전환하고
//	int setNonBlockError;
//	int setNonBlockErrorCode;
//	u_long on = 1;
//	setNonBlockError = ioctlsocket(_socket, FIONBIO, &on);
//	if (setNonBlockError == SOCKET_ERROR)
//	{
//		setNonBlockErrorCode = WSAGetLastError();
//		wprintf(L"set non block failed : %d\n", setNonBlockErrorCode);
//		closesocket(_socket);
//		bConnected = false;
//		return false;
//	}
//
//	//링거 옵션 적용하고 TIME OUT 없이 바로 fin보내게
//	linger _linger;
//	_linger.l_onoff = 1;
//	_linger.l_linger = 0;
//	setsockopt(_socket, SOL_SOCKET, SO_LINGER, (char*)&_linger, sizeof(_linger));
//
//	bConnected = true;
//	_state = DS_CONNECTED;
//
//	return true;
//}

void Dummy::OnServerDisconnect()
{
	Disconnect();
	_monsters.clear();
	_state = DS_DISCONNECTED;
	//_downClient++;

}

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

Dummy::Dummy(int dummyId)
{
	_dummyId = dummyId;
	_state = DS_DISCONNECTED;
}

void Dummy::HandlePacket(CPacket* packet)
{
	uint16 packetType;
	*packet >> packetType;

	switch (packetType)
	{
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

		case PACKET_SC_GAME_MONSTER_MOVE:
		{
			// 몬스터 이동
			// 몬스터 이동 프레임을 돌려야하나?
			// 몬스터 목적지를 그냥 현재위치로 찍을까
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
			printf("Requset Connect %d\n", _dummyId);
		}
		break;

		case DS_CONNECTED:
		{
			//연결된 상태면 로그인 하고
			RequestLogin();
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
			RequestMove();
			/*RequestAttack();
			RequestChat();*/
		}
		break;
		
		default:
			__debugbreak();
	}
}


void Dummy::RequestConnect()
{
	if(Connect())
	{
		//연결 성공 했으면
		_state = DS_CONNECTED;
	}
}

void Dummy::RequestDisconnect()
{
	Disconnect();
}

void Dummy::RequestLogin()
{
	printf("request login\n");
	TCHAR id[16];
	TCHAR password[16];
	wsprintf(id, L"dummy%d", _dummyId);
	wsprintf(password, L"dummy%d", _dummyId);

	CPacket* requestLoginPacket = CPacket::Alloc();
	GamePacketMaker::MP_CS_REQ_LOGIN(requestLoginPacket, id, password);
	SendPacket(requestLoginPacket);
	_state = DS_WAIT_FOR_REQUEST;
}

void Dummy::RequestFieldMove()
{
	//TODO: 필드이동요청
	//나중에는 채팅서버까지 필드이동 요청 같이 해야함
	uint16 fieldId = FIELD_LOBBY;
	CPacket* fieldMovePacket = CPacket::Alloc();
	GamePacketMaker::MP_CS_REQ_FIELD_MOVE(fieldMovePacket, fieldId);
	SendPacket(fieldMovePacket);
	_state = DS_WAIT_FOR_REQUEST;
}

// 더미쓰레드 최소 3초에 작동하게하면
// 현재위치 기억안해도 되게 일단 코드 작성하고
void Dummy::RequestMove()
{
	float range = 400.0f; // 범위 설정

	FVector RandomDestination = _currentLocation;
	RandomDestination.X += (rand() % static_cast<int>(range * 2)) - range;
	RandomDestination.Y += (rand() % static_cast<int>(range * 2)) - range;

	//맵크기 12000 일단 하드코딩
	RandomDestination.X = std::clamp(RandomDestination.X, (double)100, 11900.0);
	RandomDestination.Y = std::clamp(RandomDestination.Y, (double)100, 11900.0);


	CPacket* movePacket = CPacket::Alloc();
	GamePacketMaker::MP_CS_REQ_CHARACTER_MOVE(movePacket, RandomDestination, _currentRotation);
	SendPacket(movePacket);
	//printf("send request move");
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
	printf("request character list\n");
	CPacket* packet = CPacket::Alloc();
	GamePacketMaker::MP_CS_REQ_PLAYER_LIST(packet);
	SendPacket(packet);
	_state = DS_WAIT_FOR_REQUEST;
}

void Dummy::RequestSelectCharacter()
{
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


void Dummy::HandleLogin(CPacket* packet)
{
	int64 accountNo;
	uint8 status;


	*packet >> accountNo >> status;

	if (status == 1)
	{
		//게임서버로 접속
		printf("login suceed\n");
		_state = DS_LOGGINED;

	}

	if (status == 0)
	{
		printf("login failed\n");
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
	printf("dummy playerId : %d", _playerId);

	// state change to DS_RES_CHARACTER_LIST
	// which means already got character list
	// next step is choose this player
	_state = DS_RES_CHARACTER_LIST;
}

void Dummy::HandleSelectPlayer(CPacket* packet)
{
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


bool Dummy::Connect()
{
	//Connect할대마다 RecvQueue랑 SendQueue 초기화 한번씩 해주고
	_recvQueue.ClearBuffer();
	_sendQueue.ClearBuffer();

	int retVal;

	memset(&_serverAddr, 0, sizeof(_serverAddr));
	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_port = htons(serverPort);
	InetPtonW(AF_INET, serverIP, &_serverAddr.sin_addr);
	_serverPort = serverPort;

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
	_state = DS_CONNECTED;

	return true;
}

void Dummy::Disconnect()
{
	closesocket(_socket);
	bConnected = false;
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
			closesocket(_socket);
			bConnected = false;
			//_downClient++;
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
			closesocket(_socket);
			bConnected = false;
			//_downClient++;
			return;
		}
	}
	else if (recvVal == 0)
	{
		wprintf(L"server closed\n");
		closesocket(_socket);
		bConnected = false;

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
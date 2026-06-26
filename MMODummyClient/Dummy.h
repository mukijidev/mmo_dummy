#pragma once
#include "Type.h"
#include "Packet.h"
#include "RingBuffer.h"
#include <unordered_map>

class CPacket;

enum DummyState
{
	//연결 끊어진 상태에서 connect하고
	DS_DISCONNECTED,

	//로그인 부터 connect
	DS_LOGIN_CONNECTED,

    //로그인 끊고 게임 connect
	DS_CONNECTED,

	//login 상태에서 characterlist요청하고
	DS_LOGGINED,

	//characterlist요청 받으면 select player하고
	DS_RES_CHARACTER_LIST,

	//selectplayer요청받으면	field로 이동
	DS_RES_SELECT_CHARACTER,

	//field에서 이동, 공격 등등
	DS_IN_FIELD,

	//request 보내고 응답까지 기다려야하는 경우
	//login, fieldMove, characterList, selectCharacter ...
	DS_WAIT_FOR_REQUEST,
};

class Dummy
{
	friend class DummyThread;

private:
	Dummy(int dummyId);
	//bool Connect();
	void Disconnect();
	void SendProcess();
	void RecvProcess();
	void HandlePacket(CPacket* packet);
	void Run();
	void SendPacket(CPacket* packet);

private:
	bool bConnected;
	SOCKET _socket;
	SOCKADDR_IN _serverAddr;
	uint16 _serverPort;
	RingBuffer _recvQueue;
	RingBuffer _sendQueue;

private:
	const WCHAR* serverIP = L"127.0.0.1";
	uint16 loginServerPort = 10301;
	int64 _accountNo = 0;
	char _authToken[SESSION_KEY_LEN];
	WCHAR _gameServerIP[IP_LEN] = L"127.0.0.1";
	WCHAR _chatServerIP[IP_LEN] = L"127.0.0.1";
	uint16 _gameServerPort = 10303;
	uint16 _chatServerPort = 10302;

private:
	bool ConnectTo(const WCHAR* ip, uint16 port);
	void RequestLoginToLoginServer();
	void RequestEnterGameServer();
	//정리
	void RequestConnect();
	void RequestDisconnect();

private:
	void RequestLogin();
	void RequestFieldMove();
	void RequestMove();
	void RequestAttack();
	void RequestChat();
	void RequestCharacterList();
	void RequestSelectCharacter();

private:
	void HandleLoginResult(CPacket* packet);
	//
	void HandleLogin(CPacket* packet);
	void HandlePlayerList(CPacket* packet);
	void HandleSelectPlayer(CPacket* packet);
	void HandleSpawnMyCharacter(CPacket* packet);
	void HandleCharacterMove(CPacket* packet);

private:
	DummyState _state;
	int64 _playerId;
	int _dummyId;
	//자신의 위치는 언제 업데이트할까? 그냥 서버에서 current pos 받을때마다?
	FVector _currentLocation;
	FRotator _currentRotation;
	float _speed;
	float _attackRange;
	int currentFieldId = 0;



private:
	//나중에 주변 플레이어, 몬스터 공격기능까지 추가하려면 
	std::unordered_map<int64, Dummy*> _remotePlayers;
	std::unordered_map<int64, Dummy*> _monsters;

private:
	float GetDistance(FVector& Dest);
};


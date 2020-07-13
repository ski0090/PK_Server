#include <iostream>
#include <list>
#include "PK_Sock.h"
using namespace std;

void main()
{
	SocketUtil::StaticInit();
	TCPSocketPtr listenSock = SocketUtil::CreateTCPSocket(INET);
	SocketAddress recieveAddress(INADDR_ANY, 9200);
	if (listenSock->Bind(recieveAddress) != NO_ERROR)
	{
		cout << "바인드 실패 시작" << endl;
		return;
	}
	listenSock->Listen();
	list<TCPSocketPtr> mReadBlock;
	list<TCPSocketPtr> mReadAble;

	mReadBlock.push_back(listenSock);//평강:리슨 소켓도 하나의 읽을 것을 받아들이는 소켓이다.
	cout << "서버 시작" << endl;

	while (true)
	{
		//평강:읽을 거리가 있는 소켓이 생길때까지 대기
		if (!SocketUtil::Select(&mReadBlock, &mReadAble, nullptr, nullptr, nullptr, nullptr))
			continue;
		for (const TCPSocketPtr& sock : mReadAble)
		{
			if (sock == listenSock)//평강:리슨 소켓에 메세지가 오면 Accept를 해준다.
			{
				SocketAddress newClient;
				TCPSocketPtr newSocket = listenSock->Accept(newClient);
				mReadBlock.push_back(newSocket);
			}
			else//리슨 소켓을 제외하면 다른 소켓은 클라이언트와 연결되어 있는 소켓이다. 이러할 경우 메세지를 받고 모든 클라이언트 들에게 메세지를 뿌려주자.
			{
				TCHAR szMsg[1024];
				ZeroMemory(szMsg, 1024);
				sock->Receive(szMsg, 1024);//클라이언트가 중간에 꺼질경우 아마 0을 반환할것이다 이에대한 예외처리는 알아서 해주길 바란다.

				for (auto& client : mReadBlock)
				{
					client->Send(szMsg, 1024);
				}
			}
		}
	}
	SocketUtil::CleanUp();
}
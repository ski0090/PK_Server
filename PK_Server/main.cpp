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
		cout << "���ε� ���� ����" << endl;
		return;
	}
	listenSock->Listen();
	list<TCPSocketPtr> mReadBlock;
	list<TCPSocketPtr> mReadAble;

	mReadBlock.push_back(listenSock);//��:���� ���ϵ� �ϳ��� ���� ���� �޾Ƶ��̴� �����̴�.
	cout << "���� ����" << endl;

	while (true)
	{
		//��:���� �Ÿ��� �ִ� ������ ���涧���� ���
		if (!SocketUtil::Select(&mReadBlock, &mReadAble, nullptr, nullptr, nullptr, nullptr))
			continue;
		for (const TCPSocketPtr& sock : mReadAble)
		{
			if (sock == listenSock)//��:���� ���Ͽ� �޼����� ���� Accept�� ���ش�.
			{
				SocketAddress newClient;
				TCPSocketPtr newSocket = listenSock->Accept(newClient);
				mReadBlock.push_back(newSocket);
			}
			else//���� ������ �����ϸ� �ٸ� ������ Ŭ���̾�Ʈ�� ����Ǿ� �ִ� �����̴�. �̷��� ��� �޼����� �ް� ��� Ŭ���̾�Ʈ �鿡�� �޼����� �ѷ�����.
			{
				TCHAR szMsg[1024];
				ZeroMemory(szMsg, 1024);
				sock->Receive(szMsg, 1024);//Ŭ���̾�Ʈ�� �߰��� ������� �Ƹ� 0�� ��ȯ�Ұ��̴� �̿����� ����ó���� �˾Ƽ� ���ֱ� �ٶ���.

				for (auto& client : mReadBlock)
				{
					client->Send(szMsg, 1024);
				}
			}
		}
	}
	SocketUtil::CleanUp();
}
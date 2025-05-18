#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <cstdio>
#include <string>
#include <iostream>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

using namespace std;

int main() {
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		cerr << "WSAStartup failed! Error " << result << endl;
		return 1;
	}

	// 1. 创建socket套接字
	SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == client_socket) {
		cout << "create socket failed!" << endl;
		return -1;
	}

	// 2. 连接服务器
	struct sockaddr_in target;
	target.sin_family = AF_INET;
	target.sin_port = htons(8080);
	target.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (-1 == connect(client_socket, (struct sockaddr*)&target, sizeof(target))) {
		cout << "connect server failed!" << endl;
		closesocket(client_socket);
		return -1;
	}

	// 3. 开始通讯 send recv

	while (1) {
		char sbuffer[1024] = { 0 };
		cout << "发送：";
		cin >> sbuffer;

		send(client_socket, sbuffer, (int)strlen(sbuffer), 0);

		char rbuffer[1024] = { 0 };
		int ret = recv(client_socket, rbuffer, 1024, 0);
		if (ret <= 0) break;
		cout << rbuffer << endl;
	}

	// 4.关闭连接
	closesocket(client_socket);

	return 0;
}

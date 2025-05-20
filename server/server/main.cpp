#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <cstdio>
#include <string.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

using namespace std;

DWORD WINAPI thread_func(LPVOID lpThreadParameter)
{
    SOCKET client_socket = *(SOCKET*)lpThreadParameter;
    free(lpThreadParameter);
    while (1) {
        // 5. 开始通讯
        char buffer[1024] = { 0 };

        int ret = recv(client_socket, buffer, 1024, 0);
        if (ret <= 0)break;
        cout << client_socket << ":" << buffer << endl;
        send(client_socket, buffer, strlen(buffer), 0);
    }

    cout << "socket:" << client_socket << "disconnect" << endl;
    // 6. 关闭连接
    closesocket(client_socket);
    return 0;
}


int main() {
    // windows上使用网络功能需要开始网络权限
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cerr << "WSAStartup failed: Error " << result << endl;
        return -1;
    }
    // 1.创建
    SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (INVALID_SOCKET == listen_socket) {
        cout << "create listen socket failed! errcode:" << GetLastError() << endl;
        return -1;
    }
    // 2.
    struct sockaddr_in local = { 0 };
    local.sin_family = AF_INET;
    local.sin_port = htons(8080); // 大小端问题 中间设备使用的是大端序
    local.sin_addr.s_addr = inet_addr("0.0.0.0");// 服务端 选项 网卡 127.0.0.1(本地环回）只接受哪个网卡的数据 一般写全零地址表示全部都接收0.0.0.0
    if(-1 == bind(listen_socket, (struct sockaddr*)&local, sizeof(local))){
        cout << "bind socket failed! errcode:" << GetLastError() << endl;
        return -1;
    }
    // 3. 给这个socket开启监听属性
    if (-1 == listen(listen_socket, 10)) {
        cout << "start listen failed! errcode:" << GetLastError() << endl;
        return -1;
    }
    // 4. 等待客户端连接
    while (1) {
        SOCKET client_socket = accept(listen_socket, NULL, NULL);
        if (INVALID_SOCKET == client_socket) {
            continue;
        }

        SOCKET* sockfd = (SOCKET*)malloc(sizeof(SOCKET));
        *sockfd = client_socket;

        // 创建线程
        CreateThread(NULL, 0, thread_func, sockfd, 0, NULL);
    }

    return 0;
}
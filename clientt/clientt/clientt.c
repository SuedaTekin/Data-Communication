#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define PORT 8080
#define BUFFER_SIZE 1024

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = { 0 };

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Winsock baþlatýlamadý. Hata: %d\n", WSAGetLastError());
        return -1;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("\n Socket oluþturulamadý \n");
        WSACleanup();
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\n Geçersiz adres/ adresin tipi uyumsuz \n");
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\n Baðlantý hatasý \n");
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    while (1) {
        printf("Mesaj gönderin: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        send(sock, buffer, strlen(buffer), 0);
        memset(buffer, 0, BUFFER_SIZE);
        recv(sock, buffer, BUFFER_SIZE, 0);
        printf("Sunucudan yanýt: %s\n", buffer);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
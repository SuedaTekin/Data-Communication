#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#define PORT 8080
#define BUF_SIZE 1024

void initWinsock();
SOCKET createClientSocket();
void communicateWithServer(SOCKET serverSocket);

int main() {
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;

    // Winsock'ý baþlat
    initWinsock();

    // Client soketini oluþtur
    clientSocket = createClientSocket();

    // Sunucu adresini ayarla
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    // IP adresini dönüþtür
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported.\n");
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // Sunucuya baðlan
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Connect failed: %d\n", WSAGetLastError());
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    printf("Connected to server.\n");

    // Server ile iletiþimi baþlat
    communicateWithServer(clientSocket);

    // Soketi kapat
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}

void initWinsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        exit(1);
    }
}

SOCKET createClientSocket() {
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup();
        exit(1);
    }
    return clientSocket;
}

void communicateWithServer(SOCKET serverSocket) {
    char buffer[BUF_SIZE];

    while (1) {
        printf("Enter message: ");
        fgets(buffer, BUF_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;  // Newline karakterini sil

        // Mesajý server'a gönder
        send(serverSocket, buffer, strlen(buffer), 0);

        if (strcmp(buffer, "exit") == 0) {
            break;
        }
    }
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#define PORT 8080
#define BUFFER_SIZE 1024

#pragma comment(lib, "ws2_32.lib")

// Dosyaya yazma fonksiyonu
void writeToFile(const char* message) {
    FILE* fptr = fopen("messages.txt", "a"); // Dosyaya ekleme (append) modunda a�
    if (fptr == NULL) {
        printf("Dosya a��lamad�.\n");
        return;
    }
    fprintf(fptr, "%s\n", message); // Mesaj� dosyaya yaz
    fclose(fptr);
}

DWORD WINAPI clientHandler(void* socket_desc) {
    int sock = *(int*)socket_desc;
    char buffer[BUFFER_SIZE];
    int valread;

    while ((valread = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[valread] = '\0';
        printf("�stemciden mesaj: %s\n", buffer);

        // Mesaj� dosyaya kaydet
        writeToFile(buffer);

        // Mesaj� istemciye geri g�nder
        send(sock, buffer, strlen(buffer), 0);
    }

    closesocket(sock);
    return 0;
}

int main() {
    WSADATA wsaData;
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Winsock ba�latma
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Winsock ba�lat�lamad�. Hata: %d\n", WSAGetLastError());
        return EXIT_FAILURE;
    }

    // Socket olu�turma
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        perror("Socket olu�turulamad�");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Socket opsiyonlar�
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt)) < 0) {
        perror("Setsockopt hatas�");
        closesocket(server_fd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Sunucu adresi
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Socket'i ba�lama
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        perror("Ba�lama hatas�");
        closesocket(server_fd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Dinlemeye ba�lama
    if (listen(server_fd, 3) < 0) {
        perror("Dinleme hatas�");
        closesocket(server_fd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    printf("Ba�lant� bekleniyor...\n");

    while (1) {
        // Ba�lant� kabul etme
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) == INVALID_SOCKET) {
            perror("Kabul hatas�");
            closesocket(server_fd);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        printf("Ba�lant� kuruldu!\n");

        // Her yeni ba�lant� i�in yeni bir thread olu�tur
        DWORD thread_id;
        CreateThread(NULL, 0, clientHandler, (void*)&new_socket, 0, &thread_id);
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}
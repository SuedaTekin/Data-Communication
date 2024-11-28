#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#define PORT 8080
#define BUFFER_SIZE 1024

#pragma comment(lib, "ws2_32.lib")

// Dosyaya yazma fonksiyonu
void writeToFile(const char* message) {
    FILE* fptr = fopen("messages.txt", "a"); // Dosyaya ekleme (append) modunda aç
    if (fptr == NULL) {
        printf("Dosya açýlamadý.\n");
        return;
    }
    fprintf(fptr, "%s\n", message); // Mesajý dosyaya yaz
    fclose(fptr);
}

// Ýstemci iþlemi için fonksiyon
DWORD WINAPI clientHandler(void* socket_desc) {
    int sock = *(int*)socket_desc;
    char buffer[BUFFER_SIZE];
    int valread;

    while ((valread = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[valread] = '\0';  // Mesajýn sonuna null karakter ekle
        printf("Ýstemciden gelen mesaj: %s\n", buffer);

        // Mesajý dosyaya kaydet
        writeToFile(buffer);

        // Mesajý istemciye geri gönder (echo)
        send(sock, buffer, strlen(buffer), 0);
    }

    if (valread == 0) {
        printf("Baðlantý kapandý.\n");
    }
    else if (valread == SOCKET_ERROR) {
        printf("Mesaj alma hatasý: %d\n", WSAGetLastError());
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

    // Winsock baþlatma
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Winsock baþlatýlamadý. Hata: %d\n", WSAGetLastError());
        return EXIT_FAILURE;
    }

    // Socket oluþturma
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        perror("Socket oluþturulamadý");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Socket opsiyonlarý
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt)) < 0) {
        perror("Setsockopt hatasý");
        closesocket(server_fd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Sunucu adresi
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Socket'i baðlama
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        perror("Baðlama hatasý");
        closesocket(server_fd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Dinlemeye baþlama
    if (listen(server_fd, 3) < 0) {
        perror("Dinleme hatasý");
        closesocket(server_fd);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    printf("Sunucu baslatildi, baglanti bekleniyor...\n");

    while (1) {
        // Baðlantý kabul etme
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) == INVALID_SOCKET) {
            perror("Kabul hatasý");
            closesocket(server_fd);
            WSACleanup();
            exit(EXIT_FAILURE);
        }

        printf("Baðlantý kuruldu!\n");

        // Her yeni baðlantý için yeni bir thread oluþtur
        DWORD thread_id;
        CreateThread(NULL, 0, clientHandler, (void*)&new_socket, 0, &thread_id);
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}

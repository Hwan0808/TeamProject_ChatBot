#define _CRT_SECURE_NO_WARNINGS         // 최신 VC++ 컴파일 시 경고 방지
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <process.h>
#include <windows.h> 
#include <tchar.h>
#include <time.h>
#include <string.h>
#include "resource.h"

#define BUFSIZE 1024
#define SERVERPORT 9000

SOCKET listen_sock = NULL;
SOCKET client_sock = NULL;
SOCKET clntSock[100];

HWND hwndEdit = NULL;

SOCKADDR_IN serveraddr;
SOCKADDR_IN clientaddr;
HANDLE hThread1, hThread2;
HANDLE hMutex;
DWORD dwThreadID1, dwThreadID2;

int addrlen;
int clntNum = 0;

BOOL CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // 메시지 처리 함수 (다이얼로그)
LPTSTR lpszClass = _T("BasicApi"); // 메시지 상자 텍스트 함수

void AddStringToEdit(char* fmt, ...);
void OnClose(HWND hWnd); // 대화상자 종료 함수
void OnCommand(HWND hwnd, WPARAM wParam);
void OnDisConnect(HWND hwnd);

BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM IParam);
unsigned int __stdcall ThreadMain(void* arg);
void SendMsg(char* str, int len);

DWORD WINAPI ProcessClient(void* arg);

HWND hEdit; // 에디트 박스 편집 컨트롤 (채팅)
HWND iEdit; // 리스트 박스 편집 컨트롤 (클라이언트)
HICON hIconS, hIconB; // 아이콘

// 오류 출력 함수
void err_quit(char* msg);

int Time_Hour() { // 시간 출력 함수 (Hour)

    time_t curr_time;
    struct tm* curr_tm;
    curr_time = time(NULL);
    curr_tm = localtime(&curr_time);

    return curr_tm->tm_hour;
}

int Time_Min() { // 시간 출력 함수 (Min)

    time_t curr_time;
    struct tm* curr_tm;
    curr_time = time(NULL);
    curr_tm = localtime(&curr_time);

    return curr_tm->tm_min;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    hIconS = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON_SMALL), IMAGE_ICON, 16, 16, LR_DEFAULTSIZE);
    hIconB = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON_BIG), IMAGE_ICON, 16, 16, LR_DEFAULTSIZE);

    // 대화상자(다이얼로그) 생성 
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);

    WSACleanup();
    return 0;
}

unsigned int __stdcall ThreadMain(void* arg)
{
    char servermsg[256];

    hMutex = CreateMutex(NULL, FALSE, NULL);
    if (hMutex == NULL) {
        err_quit("createmutex()");
    }

    // socket()
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) err_quit("socket()");

    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);

    // bind()
    bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (listen_sock == SOCKET_ERROR) err_quit("bind()");

    // listen()
    listen(listen_sock, SOMAXCONN);
    if (listen_sock == SOCKET_ERROR) err_quit("listen()");

    AddStringToEdit("[TCP 서버] 사용자 접속 대기중 입니다.\r\n");

    while (true)
    {
        // accept()
        addrlen = sizeof(clientaddr);
        client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);

        WaitForSingleObject(hMutex, INFINITE);
        clntSock[clntNum++] = client_sock;
        ReleaseMutex(hMutex);

     
        // 접속한 클라이언트 출력
        sprintf(servermsg, "[TCP 서버] 새로운 사용자가 접속했습니다.\r\n");
        AddStringToEdit(servermsg);
        SendMessage(iEdit, LB_ADDSTRING, 0, (LPARAM)inet_ntoa(clientaddr.sin_addr));
        
        // 쓰레드 생성
        hThread1 = (HANDLE)_beginthreadex(NULL, 0, (unsigned int(__stdcall*)(void*))ProcessClient,
            (void*)client_sock, 0, (unsigned*)&dwThreadID1);

    }

    closesocket(listen_sock);
    return 0; 

}
//  서버 채팅 화면 대화상자
BOOL CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    switch (uMsg) {

    case WM_INITDIALOG:

        OnInitDialog(hWnd, hWnd, lParam);
        SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIconB);
        SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconS);
        break;

    case WM_CLOSE:

        OnClose(hWnd);
        break;

    case WM_COMMAND:

        OnCommand(hWnd, wParam); return TRUE;
       
        }
        return FALSE;
}

void OnCommand(HWND hwnd, WPARAM wParam)
{

    switch (LOWORD(wParam))
    {

    case IDCANCEL:

        OnDisConnect(hwnd);
        break;
    }
}

void OnDisConnect(HWND hwnd)
{
    int retval;

    retval = MessageBox(hwnd, _T("종료하시겠습니까?"), _T("서버 종료"), MB_ICONQUESTION | MB_YESNO);
    if (retval == IDYES) {
        OnClose(hwnd);
    }
}

void OnClose(HWND hWnd)
{
    closesocket(listen_sock);
    EndDialog(hWnd, 0);
}

void AddStringToEdit(char* fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);

    char cbuf[256];
    vsprintf(cbuf, fmt, arg);

    int nLength = GetWindowTextLength(hwndEdit);
    SendMessage(hwndEdit, EM_SETSEL, nLength, nLength);
    SendMessage(hwndEdit, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

    va_end(arg);
}

BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM IParam)
{
    hwndEdit = GetDlgItem(hWnd, IDC_EDIT1);
    iEdit = GetDlgItem(hWnd, IDC_LIST1);
    hThread2 = (HANDLE)_beginthreadex(NULL, 0, (unsigned int(__stdcall*)(void*))ThreadMain,
        (void*)listen_sock, 0, (unsigned*)&dwThreadID2);

    return TRUE;
}

// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
    LocalFree(lpMsgBuf);
    exit(1);
}

// 클라이언트와 데이터 통신
DWORD WINAPI ProcessClient(void* arg)
{
    SOCKET temp = (SOCKET)arg;

    int strlen = 0;

    char msg[BUFSIZE];
    char servermsg[256];

    while ((strlen = recv(temp, msg, BUFSIZE, 0)) > 0)
    {
        SendMsg(msg, strlen);
    }
    WaitForSingleObject(hMutex, INFINITE);

    for (int i = 0; i < clntNum; i++) {
        if (temp == clntSock[i]) {
            strlen = sprintf(servermsg, "[TCP 서버] 사용자가 접속을 종료했습니다.\r\n");
            AddStringToEdit(servermsg);

            for (; i < clntNum - 1; i++) 
                clntSock[i] = clntSock[i + 1];
                break;
         
        }
    }
    SendMsg(servermsg, strlen);
    clntNum--;
    ReleaseMutex(hMutex);
    closesocket(temp);
    SendMessage(iEdit, LB_DELETESTRING, 0, (LPARAM)inet_ntoa(clientaddr.sin_addr));
    return 0;
}

void SendMsg(char* str, int len) 
{
    WaitForSingleObject(hMutex, INFINITE);
    for (int i = 0; i < clntNum; i++) 
        send(clntSock[i], str, len, 0);
    ReleaseMutex(hMutex);
}

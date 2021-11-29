#define _CRT_SECURE_NO_WARNINGS         // 최신 VC++ 컴파일 시 경고 방지
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <time.h>
#include <iostream>
#include <string.h>
#include <process.h>
#include "resource.h"

#define SERVERIP "192.168.0.7"
#define SERVERPORT 9000

SOCKET client_sock = NULL;
HWND hwndName;
HWND hwndSend;
HWND hwndEdit1;
HWND hwndEdit2;
HWND hWnd;
HWND hWndFocus;

char Name[25];
char NameStr[256];
char str[128];
BOOL SEND = FALSE;

HICON hIconS, hIconB; // CHAT 아이콘

HANDLE hThread1, hThread2;
DWORD dwThreadID1, dwThreadID2;

unsigned int __stdcall SendMsg(void* arg); // 메시지 전송 함수
unsigned int __stdcall RecvMsg(void* arg); // 메시지 수신 함수
BOOL CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM IParam); // 메시지 처리 함수
LPTSTR lpszClass = _T("BasicApi"); // 글자 변환 함수

void AddStringToEdit(char* fmt, ...);
void OnClose(HWND hWnd); // 대화상자 종료 함수
void OnCommand(HWND hWnd, WPARAM wParam);
void OnDisConnect(HWND hwnd);
void OnSend(HWND hwnd);
void OnConnect(HWND hwnd);
BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM IParam);

// 오류 출력 함수
void err_quit(char *msg);

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
    LPSTR lpCmdLind, int nCmdShow)
{
    hIconS = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON_SMALL), IMAGE_ICON, 16, 16, LR_DEFAULTSIZE);
    hIconB = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON_BIG), IMAGE_ICON, 16, 16, LR_DEFAULTSIZE);

    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // 대화상자 생성
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc); // 채팅 화면

    // 윈속 종료
    WSACleanup();
    return 0;
}

// 클라이언트 채팅 화면
BOOL CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {

    case WM_INITDIALOG:

        OnInitDialog(hWnd, hWnd, lParam);
        SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconS);
        SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIconB);

    case WM_COMMAND:

        OnCommand(hWnd, wParam); return TRUE;

    case WM_CLOSE:
        
        OnClose(hWnd);

        }
        return FALSE;
}

void OnCommand(HWND hwnd, WPARAM wParam)
{
    switch (LOWORD(wParam))
    {
    case IDC_CONNECT:

        OnConnect(hwnd);
        break;

    case IDC_EXIT:

        OnDisConnect(hwnd);
        break;

    case IDC_SEND:

        OnSend(hwnd);
        break;

    }

}

void OnClose(HWND hWnd)
{
    int retval;

    retval = MessageBox(hWnd, _T("종료하시겠습니까?"), _T("서버 종료"), MB_ICONQUESTION | MB_YESNO);
    if (retval == IDYES) {
        EndDialog(hWnd, 0);
    }
}

void AddStringToEdit(char* fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);

    char cbuf[256];
    vsprintf(cbuf, fmt, arg);

    int nLength = GetWindowTextLength(hwndEdit2);
    SendMessage(hwndEdit2, EM_SETSEL, nLength, nLength);
    SendMessage(hwndEdit2, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

    va_end(arg);
}

BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM IParam)
{
    hwndName = GetDlgItem(hWnd, IDC_ID);
    hwndEdit1 = GetDlgItem(hWnd, IDC_CHATEDIT);
    hwndEdit2 = GetDlgItem(hWnd, IDC_CHATVIEW);

    AddStringToEdit("[TCP 클라이언트] 서버 접속 버튼을 누르세요.\r\n");
    return TRUE;
}

void OnConnect(HWND hwnd)
{
    int retval;

    // socket()
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock == INVALID_SOCKET) err_quit("socket()");

    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
    serveraddr.sin_port = htons(SERVERPORT);

    // connect()
    retval = connect(client_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) {
        err_quit("connect()");
    }
    else {
        SetDlgItemText(hwnd, IDC_CHATVIEW, "");
        MessageBox(hWnd, _T("서버 접속 완료"), _T("서버 접속"), MB_ICONINFORMATION | MB_OK);
        AddStringToEdit("[TCP 클라이언트] 채팅 서버에 접속 되었습니다.\r\n");
    }

    hThread1 = (HANDLE)_beginthreadex(NULL, 0, SendMsg, (void*)client_sock, 0, (unsigned*)&dwThreadID1);
    hThread2 = (HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void*)client_sock, 0, (unsigned*)&dwThreadID2);

}

void OnDisConnect(HWND hwnd)
{
    SetDlgItemText(hwnd, IDC_CHATVIEW, "");
    closesocket(client_sock);
    MessageBox(hWnd, _T("서버 접속 종료"), _T("접속 종료"), MB_ICONINFORMATION | MB_OK);
    AddStringToEdit("[TCP 클라이언트] 채팅 서버와 연결이 끊어졌습니다.\r\n");
}

void OnSend(HWND hwnd)
{
    GetDlgItemText(hwnd, IDC_ID, Name, 25);
    GetDlgItemText(hwnd, IDC_CHATEDIT, str, sizeof(str));

    SetDlgItemText(hwnd, IDC_CHATEDIT, "");
    SetFocus(GetDlgItem(hwnd, IDC_CHATEDIT));

    SEND = TRUE;
}

unsigned int __stdcall SendMsg(void* arg)
{
    while (true) 
    {
        if (SEND) {
            sprintf(NameStr, "[%d:%d][%s]:%s \r\n",Time_Hour(), Time_Min(), Name, str);
            send(client_sock, NameStr, (int)strlen(NameStr), 0);

            SEND = FALSE;
        }
    }
    return 0;
}

unsigned int __stdcall RecvMsg(void* arg)
{
    while (true)
    {
        int retval;
        retval = recv(client_sock, NameStr, sizeof(NameStr) - 1, 0);
        if (retval == -1) return 1;
        NameStr[retval] = 0;

        if (retval > 0) {
            AddStringToEdit(NameStr);

        }
        else {
            MessageBox(hWnd, _T("서버에 접속 되어 있지 않습니다."), _T("접속 종료"), MB_ICONERROR | MB_OK);
            AddStringToEdit("[TCP 클라이언트] 서버에 접속 되어 있지 않습니다.\r\n");
        }
     
    }
    return 0;
}

// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
    MessageBox(NULL, "채팅 서버 연결에 실패하였습니다. SERVER DISCONNET" , "클라이언트 종료", MB_ICONERROR);
    exit(1);
}
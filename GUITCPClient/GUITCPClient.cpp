#define _CRT_SECURE_NO_WARNINGS         // 최신 VC++ 컴파일 시 경고 방지
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <time.h>
#include <process.h>
#include<iostream>
#include<fstream>
#include<string>
#include <string.h>
#include <cstring>
#include "resource.h"

#define MAX_FILENAME_SIZE 100 // 파일 경로와 파일 이름의 최대 크기
#define BUFSIZE 4096
using namespace std;

HINSTANCE hInst;
SOCKET client_sock;
HWND hwndIP; // IP 주소 
HWND hwndPort; // 포트
HWND hwndName; // 닉네임
HWND hwndServConnect; // 접속 버튼
HWND hwndServDisConnect; // 접속 종료 버튼
HWND hwndSend; // 보내기 버튼
HWND hwndEdit1; // 메시지 입력 창
HWND hwndEdit2; // 채팅 화면
HWND hWnd;
HWND hWndFocus;

char IP[25];
char Port[25];
char Name[25]; // 이름
char NameStr[256]; // 이름 + 메시지
char str[128]; // 메시지
BOOL SEND = FALSE;

HICON hIconS, hIconB; // 아이콘
HBITMAP hBitmap1, hBitmap2, hBitmap3;

HANDLE Thread1, Thread2; // 스레드
DWORD ThreadID1, ThreadID2; // 스레드 ID

BOOL CALLBACK DlgProc1(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM IParam); // 메인 대화상자 (다이얼로그)
BOOL CALLBACK DlgProc2(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM IParam); // IP 입력 대화상자 (다이얼로그)
BOOL CALLBACK DlgProc3(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // 클라이언트 정보 대화상자 (다이얼로그)

BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM IParam); // 대화상자 (초기화)
LPTSTR lpszClass = _T("BasicApi"); // 글자 변환 함수

OPENFILENAME OpenFileName;
TCHAR SFilePathName[MAX_FILENAME_SIZE];
static TCHAR SFilter[] = "모든 파일\0*.*\0텍스트 파일\0*.txt\0비트맵 파일\0*.bmp";

void DisplayText(char* fmt, ...); // 메시지 출력 함수
void err_quit(char* msg); // 오류 출력 함수
void err_server(char* msg);

void OnCommand1(HWND hWnd, WPARAM wParam);
void OnCommand2(HWND hwnd, WPARAM wParam);
void OnConnect1(HWND hwnd);
void OnConnect2(HWND hwnd);
void OnDisConnect(HWND hwnd);
void OnSend(HWND hwnd);
void OnClose(HWND hWnd);
void OnInfo(HWND hwnd);

unsigned int __stdcall SendMsg(void* arg); // 메시지 전송 함수
unsigned int __stdcall RecvMsg(void* arg); // 메시지 수신 함수

int Time_Hour(); // 시간 출력 함수 (Hour)
int Time_Min(); // 시간 출력 함수 (Min)

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLind, int nCmdShow)
{
    hIconS = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON_SMALL), IMAGE_ICON, 16, 16, LR_DEFAULTSIZE);
    hIconB = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON_BIG), IMAGE_ICON, 16, 16, LR_DEFAULTSIZE);

    hInst = hInstance;

    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // 대화상자 생성
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG2), NULL, DlgProc2);

    // 윈속 종료
    WSACleanup();
    return 0;
}

BOOL CALLBACK DlgProc1(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HBRUSH hBrush = CreateSolidBrush(RGB(230, 240, 250));
    HBITMAP hBitmap1, hBitmap2, hBitmap3;

    switch (uMsg) {

    case WM_INITDIALOG:

        OnInitDialog(hWnd, hWnd, lParam);
        hBitmap1 = (HBITMAP)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP2));
        hBitmap2 = (HBITMAP)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP3));
        hBitmap3 = (HBITMAP)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP4));
        SendMessage(hwndServConnect, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap1);
        SendMessage(hwndServDisConnect, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap2);
        SendMessage(hwndSend, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap3);
        break;

    case WM_CTLCOLORDLG:

        return (LRESULT)hBrush;

    case WM_CTLCOLORBTN:

        return (LRESULT)hBrush;

    case WM_CTLCOLORSTATIC:

        SetBkColor((HDC)wParam, RGB(230, 240, 250));
        return (LRESULT)hBrush;

    case WM_COMMAND:

        OnCommand1(hWnd, wParam);
        return TRUE;

    case WM_CLOSE:

        OnClose(hWnd);
        break;

    }
    return FALSE;
}

BOOL CALLBACK DlgProc2(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HBRUSH hBrush = CreateSolidBrush(RGB(230, 240, 250));;

    HDC hdc;
    HDC memdc;
    PAINTSTRUCT ps;
    static HBITMAP hBitMap4;

    switch (uMsg) {

    case WM_INITDIALOG:
        
        SetDlgItemText(hWnd, IDC_IPADDRESS, "221.139.96.157");
        SetDlgItemText(hWnd, IDC_PORT, "9000");
        hwndIP = GetDlgItem(hWnd, IDC_IPADDRESS);
        hwndPort = GetDlgItem(hWnd, IDC_PORT);
        SetFocus(hwndIP);
        hBitMap4 = (HBITMAP)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP6));
        SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconS);
        SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIconB);
        break;

    case WM_CTLCOLORDLG:

        return (LRESULT)hBrush;

    case WM_CTLCOLORSTATIC:

        SetBkColor((HDC)wParam, RGB(230, 240, 250));
        return (LRESULT)hBrush;

    case WM_PAINT:

        hdc = BeginPaint(hWnd, &ps);
        memdc = CreateCompatibleDC(hdc);
        SelectObject(memdc, hBitMap4);
        BitBlt(hdc, 68, 25, 128, 128, memdc, 0, 0, SRCCOPY);
        DeleteObject(memdc);
        EndPaint(hWnd, &ps);
        break;

    case WM_COMMAND:

        OnCommand2(hWnd, wParam); return TRUE;

    case WM_CLOSE:

        OnClose(hWnd);
        break;
    }
    return FALSE;
}

BOOL CALLBACK DlgProc3(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));;

    HDC hdc;
    HDC memdc;
    PAINTSTRUCT ps;
    static HBITMAP hBitMap;

    switch (uMsg) {

    case WM_INITDIALOG:
        hBitMap = (HBITMAP)LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP5));
        SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconS);
        SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIconB);
        break;

    case WM_CTLCOLORDLG:

        return (LRESULT)hBrush;

    case WM_CTLCOLORSTATIC:

        return (LRESULT)hBrush;

    case WM_PAINT:

        hdc = BeginPaint(hWnd, &ps);
        memdc = CreateCompatibleDC(hdc);
        SelectObject(memdc, hBitMap);
        BitBlt(hdc, 60, 30, 100, 100, memdc, 0, 0, SRCCOPY);
        DeleteObject(memdc);
        EndPaint(hWnd, &ps);
        break;

    case WM_CLOSE:

        EndDialog(hWnd, 0);
        break;
    }
    return FALSE;
}

BOOL OnInitDialog(HWND hWnd, HWND hWndFocus, LPARAM IParam)
{
    hwndName = GetDlgItem(hWnd, IDC_ID);
    hwndEdit1 = GetDlgItem(hWnd, IDC_CHATEDIT);
    hwndEdit2 = GetDlgItem(hWnd, IDC_CHATVIEW);
    hwndServConnect = GetDlgItem(hWnd, IDC_CONNECT);
    hwndServDisConnect = GetDlgItem(hWnd, IDC_EXIT);
    hwndSend = GetDlgItem(hWnd, IDC_SEND);

    SetFocus(hwndName);

    SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconS); // 아이콘
    SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)hIconB); // 아이콘

    DisplayText("[CHATBOT] 채팅 서버에 오신 것을 환영합니다.\r\n");

    EnableWindow(hwndServConnect, FALSE);

    return TRUE;
}

void OnCommand1(HWND hwnd, WPARAM wParam)
{
    switch (LOWORD(wParam))
    {
    case IDC_CONNECT:

        OnConnect2(hwnd);
        break;

    case IDC_EXIT:

        OnDisConnect(hwnd);
        break;

    case IDC_SEND:

        OnSend(hwnd);
        break;

    case ID_INFO:

        OnInfo(hwnd);
        break;

    case ID_SAVE_FILE: // 파일 저장 하기

        ZeroMemory(&OpenFileName, 0, sizeof(OPENFILENAME));
        OpenFileName.lStructSize = sizeof(OPENFILENAME);
        OpenFileName.hwndOwner = hwnd;
        OpenFileName.lpstrFilter = SFilter;
        OpenFileName.lpstrFile = SFilePathName;
        OpenFileName.nMaxFile = MAX_FILENAME_SIZE;
        OpenFileName.lpstrInitialDir = "C:\\";

        if (GetSaveFileName(&OpenFileName) != 0)
        {
            char msg[BUFSIZE];
            DWORD dwSize = GetWindowTextLength(hwndEdit2);
            GetDlgItemText(hwnd, IDC_CHATVIEW, msg, dwSize);

            ofstream writeFile;
            writeFile.open(OpenFileName.lpstrFile);
            writeFile.write(msg, dwSize);
            writeFile.close();

            MessageBox(hwnd, _T("파일이 저장되었습니다."), _T("파일 저장"), MB_ICONINFORMATION | MB_OK);
        }
        else
        {
            MessageBox(hwnd, _T("저장하기를 취소하였습니다."), _T("파일 저장 취소"), MB_ICONINFORMATION | MB_OK);
        }
        break;

    case ID_LOAD_FILE: // 파일 전송 하기

        MessageBox(hwnd, _T("테스트 중입니다. (미구현)"), _T("파일 전송"), MB_ICONWARNING | MB_OK);

        /* ZeroMemory(&OpenFileName, 0, sizeof(OPENFILENAME));
        OpenFileName.lStructSize = sizeof(OPENFILENAME);
        OpenFileName.hwndOwner = hwnd;
        OpenFileName.lpstrFilter = SFilter;
        OpenFileName.lpstrFile = SFilePathName;
        OpenFileName.nMaxFile = MAX_FILENAME_SIZE;
        OpenFileName.lpstrInitialDir = "C:\\";

        if (GetOpenFileName(&OpenFileName) != 0)
        {
            MessageBox(hwnd, _T("파일이 전송 되었습니다."), _T("파일 전송"), MB_ICONINFORMATION | MB_OK);
        }
        else
        {
            MessageBox(hwnd, _T("전송하기를 취소하였습니다."), _T("파일 전송 취소"), MB_ICONINFORMATION | MB_OK);
        }
        */
        break;
    }

}

void OnCommand2(HWND hwnd, WPARAM wParam)
{
    switch (LOWORD(wParam))
    {
    case IDOK:

        OnConnect1(hwnd);
        DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc1);
        break;

    case IDCANCEL:

        EndDialog(hwnd, 0);
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

void OnConnect1(HWND hwnd)
{
    int retval;

    GetDlgItemText(hwnd, IDC_IPADDRESS, IP, 25);
    GetDlgItemText(hwnd, IDC_PORT, Port, 25);

    // socket()
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock == INVALID_SOCKET) err_quit("socket()");

    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(IP);
    serveraddr.sin_port = htons(atoi(Port));

    // connect()
    retval = connect(client_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

    if (retval == SOCKET_ERROR) {
        err_quit("connect()");
    }
    else {
        MessageBox(hWnd, _T("서버 접속 완료"), _T("서버 접속"), MB_ICONINFORMATION | MB_OK);
        EndDialog(hwnd, 0);
    }

    Thread1 = (HANDLE)_beginthreadex(NULL, 0, SendMsg, (void*)client_sock, 0, (unsigned*)&ThreadID1);
    Thread2 = (HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void*)client_sock, 0, (unsigned*)&ThreadID2);
}

void OnConnect2(HWND hwnd)
{
    TerminateThread(Thread1, ThreadID1);
    TerminateThread(Thread2, ThreadID2);

    EnableWindow(hwndSend, TRUE);

    int retval;

    // socket()
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock == INVALID_SOCKET) err_quit("socket()");

    SOCKADDR_IN serveraddr;
    ZeroMemory(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(IP);
    serveraddr.sin_port = htons(atoi(Port));

    // connect()
    retval = connect(client_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

    if (retval == SOCKET_ERROR) {
        err_quit("connect()");
    }
    else {
        MessageBox(hWnd, _T("서버 접속 완료"), _T("서버 접속"), MB_ICONINFORMATION | MB_OK);
        EnableWindow(hwndName, TRUE);
        EnableWindow(hwndEdit1, TRUE);
        EnableWindow(hwndServDisConnect, TRUE);
        EnableWindow(hwndServConnect, FALSE);
    }

    Thread1 = (HANDLE)_beginthreadex(NULL, 0, SendMsg, (void*)client_sock, 0, (unsigned*)&ThreadID1);
    Thread2 = (HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void*)client_sock, 0, (unsigned*)&ThreadID2);
}

void OnDisConnect(HWND hwnd)
{
    int retval;

    retval = MessageBox(hwnd, _T("접속을 종료합니다"), _T("접속 종료"), MB_ICONINFORMATION | MB_YESNO);

    TerminateThread(Thread1, ThreadID1);
    TerminateThread(Thread2, ThreadID2);

    if (retval == IDYES) {

        EnableWindow(hwndSend, FALSE); // 보내기 버튼 비활성화
        SetDlgItemText(hwnd, IDC_CHATVIEW, "");
        SetDlgItemText(hwnd, IDC_CHATEDIT, "");
        SetDlgItemText(hwnd, IDC_ID, "");
        closesocket(client_sock);
        DisplayText("[CHATBOT] 채팅 서버와 연결이 끊어졌습니다.\r\n");
        EnableWindow(hwndName, FALSE);
        EnableWindow(hwndEdit1, FALSE);
        EnableWindow(hwndServDisConnect, FALSE);
        EnableWindow(hwndServConnect, TRUE);
    }
}

void OnSend(HWND hwnd)
{
    int id;
    int msg;

    id = GetDlgItemText(hwnd, IDC_ID, Name, 25);
    msg = GetDlgItemText(hwnd, IDC_CHATEDIT, str, sizeof(str));

    if (id == NULL || msg == NULL) {
        MessageBox(hwnd, _T("아이디와 메시지를 입력하세요!"), _T("ID and Msg"), MB_ICONWARNING | MB_OK);
        SEND = FALSE;
    }
    else {
        SetDlgItemText(hwnd, IDC_CHATEDIT, "");
        SetFocus(GetDlgItem(hwnd, IDC_CHATEDIT));
        SEND = TRUE;
    }
}

void OnInfo(HWND hwnd)
{
    DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG3), NULL, DlgProc3);
}

unsigned int __stdcall SendMsg(void* arg)
{

    while (true)
    {
        if (SEND) {

            sprintf(NameStr, "[%02d:%02d][%s]:%s \r\n", Time_Hour(), Time_Min(), Name, str);
            send(client_sock, NameStr, (int)strlen(NameStr), 0);
            send(client_sock, Name, (int)strlen(Name), 0);

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
        if (retval == -1) {
            err_server("socket()");
        }
        NameStr[retval] = 0;

        if (retval > 0) {
            DisplayText(NameStr);
        }

    }
    return 0;
}

void DisplayText(char* fmt, ...)
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

void err_quit(char* msg)
{
    MessageBox(NULL, "채팅 서버 연결에 실패하였습니다. SERVER DISCONNET", "접속 종료", MB_ICONERROR);
    exit(1);
}

void err_server(char* msg)
{
    MessageBox(NULL, "서버가 클라이언트를 강제 종료 시켰습니다. SERVER DENYED", "접속 종료", MB_ICONERROR);
    exit(1);
}

int Time_Hour() {

    time_t timer;
    struct tm* now_time;
    timer = time(NULL);
    now_time = localtime(&timer);

    return now_time->tm_hour;
}

int Time_Min() {

    time_t timer;
    struct tm* now_time;
    timer = time(NULL);
    now_time = localtime(&timer);

    return now_time->tm_min;
}

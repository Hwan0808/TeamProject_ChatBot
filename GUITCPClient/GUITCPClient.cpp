#define _CRT_SECURE_NO_WARNINGS         // 최신 VC++ 컴파일 시 경고 방지
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <windows.h>

#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include "resource.h"

#define SERVERIP   "192.168.0.7"
#define SERVERPORT 9000
#define BUFSIZE    512

// 대화상자 프로시저
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM); // 채팅 화면 프로시저 생성

LPTSTR lpszClass = _T("BasicApi");

// 편집 컨트롤 출력 함수
void DisplayText(char *fmt, ...);
// 오류 출력 함수
void err_quit(char *msg);
void err_display(char *msg);
// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags);
// 소켓 통신 스레드 함수
DWORD WINAPI ClientMain(LPVOID arg);

SOCKET sock; // 소켓
char buf[BUFSIZE + 1]; // 데이터 송수신 버퍼
HANDLE hReadEvent, hWriteEvent; // 이벤트
HWND hSendButton; // 보내기 버튼
HWND hEdit1, hEdit2; // 편집 컨트롤
HWND hWnd; // 윈도우 프로시저

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{

    // 이벤트 생성
    hReadEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
    if (hReadEvent == NULL) return 1;
    hWriteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (hWriteEvent == NULL) return 1;

    // 소켓 통신 스레드 생성
    CreateThread(NULL, 0, ClientMain, NULL, 0, NULL);

    // 대화상자 생성
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc); // 채팅 화면

    // 이벤트 제거
    CloseHandle(hReadEvent);
    CloseHandle(hWriteEvent);

    // closesocket()
    closesocket(sock);

    // 윈속 종료
    WSACleanup();
    
    return 0;
}

// 클라이언트 채팅 화면
BOOL CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int Return;
    int retval;

    switch (uMsg) {
    case WM_INITDIALOG:
        hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
        hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);
        hSendButton = GetDlgItem(hDlg, IDOK);
        SendMessage(hEdit1, EM_SETLIMITTEXT, BUFSIZE, 0);
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {

        case IDOK:

            EnableWindow(hSendButton, FALSE); // 보내기 버튼 비활성화
            WaitForSingleObject(hReadEvent, INFINITE); // 읽기 완료 기다리기
            GetDlgItemText(hDlg, IDC_EDIT1, buf, BUFSIZE + 1);
            SetEvent(hWriteEvent); // 쓰기 완료 알리기
            SetFocus(hEdit1);
            SendMessage(hEdit1, EM_SETSEL, 0, -1);
            return TRUE;

        case IDCANCEL:

            Return = MessageBox(hWnd, _T("종료하시겠습니까?"), _T("확인"), MB_YESNO);
            if (Return == IDYES) {
                DestroyWindow(hDlg);
                closesocket(sock);
                WSACleanup();
            }
            else {
                return 0;
            }

        case IDCONNECT:

            SOCKADDR_IN serveraddr;
            ZeroMemory(&serveraddr, sizeof(serveraddr));
            serveraddr.sin_family = AF_INET;
            serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
            serveraddr.sin_port = htons(SERVERPORT);
            retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

            if (retval == SOCKET_ERROR) {
                err_quit("connect()");
            }
            else {
                Return = MessageBox(hWnd, _T("서버 접속 완료."), _T("확인"), MB_OK);
                DisplayText("[채팅 서버] ");
                DisplayText("서버 IP 주소=%s, 포트 번호=%d\r\n",inet_ntoa(serveraddr.sin_addr), ntohs(serveraddr.sin_port));
            }

        }
        return FALSE;
    }
    return FALSE;
}

// 편집 컨트롤 출력 함수
void DisplayText(char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);

    char cbuf[BUFSIZE + 256];
    vsprintf(cbuf, fmt, arg);

    int nLength = GetWindowTextLength(hEdit2);
    SendMessage(hEdit2, EM_SETSEL, nLength, nLength);
    SendMessage(hEdit2, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

    va_end(arg);
}

// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
    MessageBox(NULL, "채팅 서버 연결에 실패하였습니다. SERVER DISCONNET" , "클라이언트 종료", MB_ICONERROR);
    exit(1);
}

// 소켓 함수 오류 출력
void err_display(char *msg)
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, WSAGetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    DisplayText("[%s] %s", msg, (char *)lpMsgBuf);
    LocalFree(lpMsgBuf);
}

// 사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char *buf, int len, int flags)
{
    int received;
    char *ptr = buf;
    int left = len;

    while (left > 0) {
        received = recv(s, ptr, left, flags);
        if (received == SOCKET_ERROR)
            return SOCKET_ERROR;
        else if (received == 0)
            break;
        left -= received;
        ptr += received;
    }

    return (len - left);
}

// TCP 클라이언트 시작 부분
DWORD WINAPI ClientMain(LPVOID arg)
{
    int retval;

    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // socket()
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) err_quit("socket()");

    // 서버와 데이터 통신
    while (1) {
        WaitForSingleObject(hWriteEvent, INFINITE); // 쓰기 완료 기다리기

        // 문자열 길이가 0이면 보내지 않음
        if (strlen(buf) == 0) {
            EnableWindow(hSendButton, TRUE); // 보내기 버튼 활성화
            SetEvent(hReadEvent); // 읽기 완료 알리기
            continue;
        }

        // 데이터 보내기
        retval = send(sock, buf, strlen(buf), 0);
        if (retval == SOCKET_ERROR) {
            err_display("send()");
            break;
        }

        // 데이터 받기
        retval = recvn(sock, buf, retval, 0);
        if (retval == SOCKET_ERROR) {
            err_display("recv()");
            break;
        }
        else if (retval == 0)
            break;

        // 받은 데이터 출력
        buf[retval] = '\0';
        DisplayText("[받은 데이터] %s\r\n", buf);

        EnableWindow(hSendButton, TRUE); // 보내기 버튼 활성화
        SetEvent(hReadEvent); // 읽기 완료 알리기
    }

    return 0;
}

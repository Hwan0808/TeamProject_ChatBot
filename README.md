# TCP-IP

인터넷 프로그래밍 8조 팀프로젝트 <br>

최창환(팀장) - 개발 , 이병준 - 발표, 이동건 - 테스트

## 프로젝트 제목

멀티스레드를 이용한 다중 클라이언트 채팅 프로그램

## 프로젝트 목표

- 서버와 클라이언트간의 1대N 통신을 가능하게 한다.
- 클라이언트간의 통신이 가능하게 한다.(필수 X)
- 현재 연결되어 있는 클라이언트의 리스트를 UI에 표시
- 서버 접속, 해제 버튼, 닉네임 설정 옵션을 추가
- 클라이언트를 서버 관리자가 지정하여 강제로 접속 해제 할 수 있는 기능 추가
- 클라이언트간의 채팅방 접속을 구현하고, 파일전송 기능 추가

![image](https://user-images.githubusercontent.com/57865037/143832904-8e0aaac2-ebc1-44fc-ab61-f13bc6b4f006.png)

![image](https://user-images.githubusercontent.com/57865037/143832926-7de60ec1-cd16-4257-a773-c607c985358a.png)

![image](https://user-images.githubusercontent.com/57865037/144167484-980a9aee-4834-4000-8b65-d5de1b7796d2.png)

## Bug Fix

- 전체적인 코드 일괄 수정
- 클라이언트간의 통신 가능
- 원격 접속 문제 해결 (서버 담당: 이동건)
- 서버 접속 폼 추가 완료 (서버 IP 변경을 위해 클라이언트쪽에서 디버깅을 수행하지 않아도 됨)

## To Do List

- 서버에서 강제 내보내기 기능 추가
- 파일 전송기능은 채팅방을 구현 하지 않으면 힘듬. 상대의 IP 주소를 알고있어야 가능함.

## 주요 알고리즘

- ThreadMain(void* arg)
  <br> socket() -> bind() -> listen() -> accept()의 기능을 수행하고 각 클라이언트와 통신이 가능하도록 스레드를 생성함.
  
- RecvMsg(void* arg), SendMsg(void* arg)
  <br> 스레드 함수를 설정하여 메시지를 받음과 동시에 전송이 가능하도록 구현하였음.
  
- Time_Hour(), Time_Min() 
  <br> C++ 라이브러리에 있는 시간 함수 구조체를 사용하여 클라이언트가 접속하는 시간을 표현하였음.

- int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
  <br> 윈속 초기화, 대화상자(다이얼로그)생성, 아이콘 추가 등의 기능을 수행함.

- BOOL CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
  <br> 사용자가 이벤트를 발생시키면 대화상자 안에 있는 함수들이 작동하며 접속,해제,보내기 등의 기능을 수행함.
  
- void DisplayText(char* fmt, ...);
  <br> 리스트 박스와 에디트 박스에 메시지 내용을 출력하는 기능을 수행함.

## 참고 자료

- https://blog.naver.com/ree31206/46430031
- https://blog.naver.com/ree31206/46430004

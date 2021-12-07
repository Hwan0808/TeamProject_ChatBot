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

![image](https://user-images.githubusercontent.com/57865037/144731564-ccd3086b-cf46-423d-8ffd-cc2c30201872.png)

![image](https://user-images.githubusercontent.com/57865037/144814780-013a07b0-d61b-44a4-87f4-c9d0d84faaf7.png)

![image](https://user-images.githubusercontent.com/57865037/144731605-41dd8eec-ab73-48b9-9aa1-32bc0edc36d4.png)

![image](https://user-images.githubusercontent.com/57865037/144815405-05d163f5-3c8e-43d2-9e7a-e0a51c31f889.png)

![image](https://user-images.githubusercontent.com/57865037/144860985-b5608cb8-cca9-42ea-9f06-b47b7b86b24e.png)

![image](https://user-images.githubusercontent.com/57865037/144861026-0ce599a2-16df-4afa-90ff-23d7f873059a.png)

## Bug Fix

- 서버 내보내기 기능 추가
- 버튼 이미지 추가
- 상단(메뉴바) 추가, 클릭 시 클라이언트 정보 출력

## To Do List

- 파일 전송기능 미구현, 테스트중

## 주요 알고리즘

- ThreadMain(void* arg)
  <br> socket() -> bind() -> listen() -> accept()의 기능을 수행하고 각 클라이언트와 통신이 가능하도록 스레드를 생성함.
  
- RecvMsg(void* arg), SendMsg(void* arg)
  <br> 스레드 함수를 설정하여 메시지를 받음과 동시에 전송이 가능하도록 구현하였음.
  
- Time_Hour(), Time_Min() 
  <br> C++ 라이브러리에 있는 시간 함수 구조체를 사용하여 클라이언트가 접속하는 시간을 표현하였음.

- WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
  <br> 윈속 초기화, 대화상자(다이얼로그)생성, 아이콘 추가 등의 기능을 수행함.

- DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
  <br> 사용자가 이벤트를 발생시키면 대화상자 안에 있는 함수들이 작동하며 접속,해제,보내기 등의 기능을 수행함.
  
- DisplayText(char* fmt, ...);
  <br> 리스트 박스와 에디트 박스에 메시지 내용을 출력하는 기능을 수행함.

## 참고 자료

- TCP/IP 윈도우 소켓 프로그래밍 (한빛 미디어) 및 수업 내용
- 윤성우의 열혈 TCP/IP 소켓 프로그래밍
- 핵심 API로 배우는 윈도우 프로그래밍
- https://blog.naver.com/ree31206/46430031
- https://blog.naver.com/ree31206/46430004
- https://blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=tipsware&logNo=221659437681
- https://blog.naver.com/ree31206/46430004
- https://blog.naver.com/makelove7942/220559541931

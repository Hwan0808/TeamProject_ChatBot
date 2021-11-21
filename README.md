# TCP-IP

인터넷 프로그래밍 8조 팀프로젝트 <br>

최창환(팀장) - 개발 , 이병준 - 발표, 이동건 - 테스트

<h3>프로젝트 제목</h3>

멀티스레드를 이용한 다중 클라이언트 채팅 프로그램

<h3>프로젝트 목표</h3> 

- 서버와 클라이언트간의 1대N 통신을 가능하게 한다.
- 클라이언트간의 통신이 가능하게 한다.(필수 X)
- 현재 연결되어 있는 클라이언트의 리스트를 UI에 표시
- 서버 접속, 해제 버튼, 닉네임 설정 옵션을 추가
- 클라이언트를 서버 관리자가 지정하여 강제로 접속 해제 할 수 있는 기능 추가
- 클라이언트간의 채팅방 접속을 구현하고, 파일전송 기능 추가

![image](https://user-images.githubusercontent.com/57865037/142768503-d1521166-d81a-4e27-a61d-b7cb49185b98.png)

![image](https://user-images.githubusercontent.com/57865037/142768573-d9a5cdf6-44d7-4ae8-a1bb-4173cde11872.png)

<h3>Bug Fix</h3> 

- 콘솔 아이콘 추가 <img src="https://user-images.githubusercontent.com/57865037/142768687-77db8cf3-7465-43f4-bdd8-90a11fccea09.png" width=20 height=20>
- 최소화/최대화 트레이 아이콘 추가
- 윈도우 화면 상에 가운데에서 콘솔 창이 열리도록 수정. (버튼 팝업 창도 가운데에서 열림)
- 정적 텍스트를 지우고 클라이언트 리스트를 추가

<h3>To Do List</h3>

- 현재 클라이언트간의 통신이 불가능 함. (개선사항)
- DMZ 설정, 포트포워딩 설정을 해도 원격에서 접속이 되지 않음. (해결방법 필요)
- 파일 전송기능은 향후 클라이언트 간의 통신이 가능한다 할지라도 채팅방기능을 구현하지 않는 이상 힘듬.

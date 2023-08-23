# Webserv

| Tag name           | Description |
|:-------------------|:------------|
|**Feat**            |새로운 기능을 추가|
|**Fix**             |버그 수정|
|**Design**          |CSS 등 사용자 UI 디자인 변경|
|**!BREAKING CHANGE**|커다란 API 변경의 경우|
|**!HOTFIX**         |급하게 치명적인 버그를 고쳐야하는 경우|
|**Style**           |코드 포맷 변경, 세미 콜론 누락, 코드 수정이 없는 경우|
|**Refactor**        |프로덕션 코드 리팩토링|
|**Comment**         |필요한 주석 추가 및 변경|
|**Docs**            |문서 수정|
|**Test**            |테스트 코드, 리펙토링 테스트 코드 추가, Production Code(실제로 사용하는 코드) 변경 없음|
|**Chore**           |빌드 업무 수정, 패키지 매니저 수정, 패키지 관리자 구성 등 업데이트, Production Code 변경 없음|
|**Rename**          |파일 혹은 폴더명을 수정하거나 옮기는 작업만인 경우|
|**Remove**          |파일을 삭제하는 작업만 수행한 경우|
>
- TODO LIST
>

> 1. event의 filter만 확인을 하는 로직으로 구성했는데 어떤 이벤트를 받더라도 항상 ident도 같이 확인을 해야한다. 
	 들어온 이벤트가 클라이언트 소켓인지, 서버 소켓인지, 파이프인지 구분하지 못하면 어떤동작을 취할지 판단 할 수 없기 때문이다. 같은 맥락에서 전체 코드에서 kevent는 단 한번만 호출되어야 한다.

> 2. python을 처리할때 굳이 파일경로와 엔진을 나누어서 처리할 필요는 없다. python의 첫째줄 주석에 #!/bin/usr/python3 로 엔진의 경로를 명시해주면 파일명으로 execve가 가능하다. 대신 python 확장자를 여러 요청에서 처리할 수 있어야 할 수도 있다. 예를들어서 사용자가 get요청으로 index.py를 요청했을때, 우리는 cgi로 처리하지 못하고 그냥 plain/text로 반환하고 있는데, post나 put에서만 python을 처리할 수 있다면 불완전한 서버일 수 있다.


> 3. 청크드 데이터를 보낼때도 이벤트가 한번 걸렸을때 전부다 보내는 것이 아니라 커널의 송신버퍼에 쓸 수 있는 만큼 보내야 한다. 
한번 썼으면 어쩔 수 없이 쓴길이와 못쓴길이를 재서 인덱스로 측정하고 남은 길이를 다음번 요청에 얹어 보내는 로직으로 수정해야 한다. 요청이 날라왔을때 다쓸 때까지 기다리는 로직 자체가 blocking에 가깝다.

> 4. 소켓 링거 옵션 추가 - 클라이언트 연결이 끊어졌을 때 버퍼가 일시적으로 유지되어 남아있는데 그런경우 빠르게 연속해서 get 요청을 보내는 테스트 ciege에 걸리기 때문에 버퍼를 바로바로 지워주는 링거옵션을 찾아서 대기 시간을 0으로 세팅해줘야 한다.

4가지 수정하고 나면 웬만한 처리는 다 끝났다고 볼 수 있음

처리의 중요도 1 -> 3 -> 4 -> 2

1번, 3번은 데이터레이스와 직결된 문제라 우선순위가 높고 4번은 평가표 충족을 위해서 필수적이다.
다만 2번은 굳이 처리할 필요까지는 없을 수 있다.

* 생각해 봐야할 것 
	1.  빠른과제통과를 위해서 완벽한 서버의 동작을 포기할 수 있는가 
		* 테스터기 1억개 통과를 하지 않고 cige테스트를 통과시킨채로 평가를 받는다. 어차피 평가 기준에 tester가 포함되어 있지 않다.
		* 코드의 중요로직은 당연히 수정해야 하지만 잔잔바리 리팩토링을 내다 버린다. 주석만 없애고 모듈 분할하지 않고 그대로 제출한다.
		* 브랜치를 더 이상 따지 않는다. 분산 협업을 위한 시스템인데 모여서 코딩하는데 사실상 필요가 없다.
	2. 코드의 로직을 어디까지 뜯어 고칠 것인가
		* write read, recv send 의 안정성을 위해서 완벽한 상태일 때만 수행 하는것이 맞다. 따라서 위 1번문제를 해결하기 위해선 operation cpp를 크게 고쳐야 한다. (그게 시간이 더 적게 걸릴것.)
		또 리스폰스와 리퀘스트의 생성 타이밍을 맞춰야 한다. 쓰지 않더라도 빈 파이프 변수를 만들어 놓긴 해야 한다는 뜻이다. 
		처음 Read 이벤트가 들어왔을 때 ident를 검사해야 할텐데, 소켓인지 파이프인지 체크하려면 결국 만들어 놓긴 해야한다.
		리퀘스트는 괜찮을 수 있지만 리스폰스는 만드는 쪽 로직을 대거 수정해야 한다. 
	



//timer 거는법

 EV_SET(&event, fd, EVFILT_TIMER, EV_ADD | EV_ENABLE, NOTE_SECONDS, 10, NULL); 

 EV_SET(&event, fd, EVFILT_TIMER, EV_ADD | EV_ENABLE, NOTE_SECONDS, 10, NULL);


// [Feat]autoindex 동작 안함. 왜? - kyeonkim
// [Done] server name과 server listen이 같으면 동작하면 안된다.- kyeonkim
	- In the configuration, try to setup the same port multiple times. It should not work. -> 완료
	 - Launch multiple servers at the same time with different configurations but with common ports. Does it work? If it does, ask why the server should work if one of the configurations isn't functional. Keep going.
	 	같은 포트로도 bind 시킬 수 있는 소켓 옵션이 있다(REUSEPORT). 해당 옵션을 주면 같은 포트로도 서버를 동작시킬 수 있지만 요청을 보내면 먼저 열은 서버만 반응을 한다. 이때 첫번째 서버를 중단하고 다시 요청을 보내면 두번째 서버가 동작을 하다. 이런 결과를 봤을 때 두번째 서버의 용도는 첫번째 서버가 중단되었을 경우, 백업의 용도로 쓰이지 않을까?하고 생각이 든다.
// [Test] Siege 테스트 시에 memory leak 이 나면 안된다.- kyeonkim 
// listen 192.0.0.1:80 이거 바인드가 안됨. 0.0.0.0, 127.0.0.1 만 바인드됨. ip 변경하는 거 필요없음. - kyeonkim
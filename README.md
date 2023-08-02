#강 Webserv

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
	- CGI
	- POST 청크
	- delete 메소드
	- 오류마다 처리
	- auto index
	- redirection

	- 테스트

location

* ./src/pages/
* ./src/pages/src
* ./src/pages/src/pages/
* /src/other

request

* ./src/pages/index.html


	- loop을 돌면서 location _path 의 길이를 세서 location _path 길이까지만 request _url과 비교를 한다.
	- if request _url와 location _path이 같은데 길이가 이전 길이보다 길다면 해당 _path의 location 구조체를 가지고 있는다.
	- location을 찾았을때 || 가지고 있을 때, 
	- location root가 있을 경우,
		request url 에서 location _path와 겹치는 부분을 root value 로 치환하여
	- location root가 없을 경우,
		
	- 파일일 경우,
		해당 파일이 있는지 찾는다.
	- 폴더일 경우,
		index 가 있다면, index 붙여서 해당 파일이 있는지 찾는다.
		index 가 없을 경우, 보여줄 게 없다는 메시지를 띄운다(state code는 200으로 처리한다. 왜냐하면 폴더는 있기 때문에).
	
request 요청이 dir 인 경우,
---------------------------
	| path | root | index |
       o      o       o         : 동일한 부분을 root로 치환한 url + index 경로에 해당 파일이 있는지 찾는다.
       o      o       x         : 동일한 부분을 root로 치환하고 autoindex 가 있으면 리스트로 보여주고 없으면 보여줄 게 없다는 메시지를 띄운다(state code는 200으로 처리한다. 왜냐하면 폴더는 있기 때문에).
	   o      x       o         : path + index 경로에 해당 파일이 있는지
	   o      x       x         : autoindex 가 있으면 리스트로 보여주고 없으면 보여줄 게 없다는 메시지를 띄운다(state code는 200으로 처리한다. 왜냐하면 폴더는 있기 때문에).
	   x      x       x         : return 404
--------------------------
auto index off && index X = 보여줄게 없다?
auto index on && index X = 목록 보여주기

index 옵션이 있고 request 요청이 dir 인 경우, 경로 끝에 index value 값을 무조건 붙여서 찾는다. - kyeonkim
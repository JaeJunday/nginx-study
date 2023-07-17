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

<!-- 
// 서버, 로케이션 스트링 넣음
// 열린괄호 넣으면 열린괄호 넣음
// 닫힌괄호 받으면 열린괄호 뺌

// 플래그
// 서버, 로케이션, 카운트
// 의사코드
// 서버를 넣을떄 : 서버를 만나면 서버플래그랑 로케이션플래그 검사한 후 서버 스트링을 스택에 넣고 카운트를 올린다. 플래그를 서버로 바꾼다
// 로케이션을 넣을때 : 로케이션을 만나면 서버플래그랑 로케이션플래그를 검사한 후 로케이션 스트링을 스택에 넣고 카운트를 올린다 플래그를 로케이션으로 바꾼다
// 열린괄호를 만나면 열린괄호를 스택에 넣는다.
// 닫힌괄호를 만나면 열린괄호 - 스트링 순서를 맞추어서 스택에서 뺀다. 카운트를 내린다. 플래그를 끈다. 
// 다 읽었을때 카운트가 남아 있으면 에러
// 카운트가 0일때 스택이 비어있지 않으면 에러

// ;룰 끝 
//find로 ;을 찾는다. 못찾으면 에러
//;뒤에 널문자랑 닫는 괄호문자가 아니면 에러

//서버 안에 서버  서버가 꺼져있는지
//로케이션 안에 서버 로케이션이 켜져있는지 

//로케이션 안에 로케이션 로케이션이 꺼져있는지 
//서버 밖에 로케이션 서버가 꺼져있는지 

// server 조건식인 경우
// if (serverFlag == on)
//  '{' 시작전에 문자열이 들어온다 -> error

// to do  ##########################################################################3

// location 조건식인 경우
// if (location == on)
// 문자열이 있어야 하나??  

// 로케이션 path 저장하는 기능 추가해야함
// 키벨류 집어넣을떄 세미콜론 처리 
// 서버 저장하는 자료구조 맵으로 변경 -> 이유 ) 어차피 ip주소로 찾아야하는데 map이 효율적임
//

--> 
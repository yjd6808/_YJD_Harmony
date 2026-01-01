원래 개별 프로젝트마다 Output 디렉토리를 만들고 있었음.
그래서 PostBuildEvent.ps1를 실행해서 솔루션 디렉토리에 Output/lib, Output/include에 라이브러리 파일을 복사해놓고 있었다.
하지만 이제 구조를 기본적으로 솔루션 Output 디렉토리에 모두 뽑도록 하고 있어서 이제는 사용하지 않는다.


# REGISTER_UI 헤더→CPP 이동 (Static Init Crash Fix)

## 작업 내용
sgcl 실행 시 REGISTER_UI 매크로가 헤더 파일에 있어 static initialization order 문제로 0xC0000005 ACCESS_VIOLATION crash 발생. 5개 헤더의 REGISTER_UI를 모두 대응하는 .cpp 파일로 이동하여 해결.

## 수정 파일
- UI_Popup.h: REGISTER_UI(Popup, UI_Popup) 제거 → UI_Popup.cpp 끝에 추가
- UI_Login.h: REGISTER_UI(Login, UI_Login) 제거 → UI_Login.cpp 끝에 추가
- UI_Inventory.h: REGISTER_UI(Inventory, UI_Inventory) 제거 → UI_Inventory.cpp 끝에 추가
- UI_ChannelSelect.h: REGISTER_UI(ChannelSelect, UI_ChannelSelect) 제거 → UI_ChannelSelect.cpp 끝에 추가
- UI_Test.h: REGISTER_UI(Test, UI_Test) 제거 → UI_Test.cpp 끝에 추가

## 결과
- 빌드 성공 (x64 Debug, Rebuild, 오류 0)
- sgcl.exe 실행 시 crash 없이 정상 종료 (exit code 1, 서버 미연결 상태)
  - Before: exit code -1073741819 (0xC0000005, ACCESS_VIOLATION at ntdll.dll+0x2f6a3)
  - After: exit code 1 (정상 종료)

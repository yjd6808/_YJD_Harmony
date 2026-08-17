# UI XML 경로 및 매핑 시스템 수정

## 작업 내용
sgcl 실행 시 UI XML 파일 로드 실패(`res_data/UI/Login.xml` not found) 문제 해결.

## 원인
1. 코드가 `resDataPath_ + "/UI/" + name + ".xml"` 경로로 XML을 찾았지만 실제 asset 구조는 `res_data/layout/` 
2. Show() 호출 시 파일명(Login)과 실제 파일명(ui_login.xml) 불일치
3. PopupManager에서 LoadFromFile("Popup.xml")로 상대경로 사용

## 수정 사항

### 1. UI XML 경로 변경 (UIManager.cpp)
- `"/UI/"` → `"/layout/"` (실제 asset 구조에 맞춤)

### 2. UIManager::Show 개선 (UIManager.cpp)
- XML 로드 후 루트 Group의 `name` 속성(`pGroupInfo->name_`)으로 factory lookup
- name이 비어있으면 오류 로그
- 요청한 이름과 XML 루트 name이 다르면 디버그 로그

### 3. REGISTER_UI 매크로 키 변경 (UIRegister.h)
- Factory key를 `#name`에서 `#className`으로 변경
- XML 루트 Group name(예: `UI_Login`)으로 클래스 매핑

### 4. Show() 호출 인자 변경
- `Scene_Login.cpp`: `Show("Login")` → `Show("ui_login")`
- `Scene_ChannelSelect.cpp`: `Show("ChannelSelect")` → `Show("ui_channel")`

### 5. PopupManager 경로 수정 (PopupManager.cpp)
- `LoadFromFile("Popup.xml")` → `g_cAppConfig.resDataPath_ + "/layout/ui_popup.xml"`
- `sg/Core/AppConfig.h` include 추가

## 결과
- 빌드 성공 (x64 Debug, Rebuild, 오류 0)
- Login.xml 로드 실패 오류 없이 정상 실행

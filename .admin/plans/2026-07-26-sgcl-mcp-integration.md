# sgcl MCP 통합

## 목표
sgcl 클라이언트에 MCPCore 싱글톤을 추가하여 MCP 서버를 내장하고, `REGISTER_MCP_TOOL`로 도구를 등록할 수 있게 한다.

## 설계

### MCPCore 싱글톤 (`sgcl/Classes/sgcl/MCP/`)
- `NetCore`와 동일한 `jc::SingletonPointer<MCPCore>` 패턴
- `g_cMCPCore` 매크로로 접근
- 여러 `MCPServer*` 멤버 보유 (첫 번째: `Main()`)
- `Initialize()`, `Finalize()`, `PollEvents()` 메서드

### 사용 예
```cpp
g_cMCPCore.Main().Start(9191);
g_cMCPCore.PollEvents();
```

## 변경 파일

### 수정
1. **Directory.Build.props** — `mcp.props` 조건에 `sgcl` 추가 (line 137-140)
2. **sgcl.vcxproj** — `MCPCore.cpp` ClCompile, `MCPCore.h` ClInclude 추가
3. **sgcl.vcxproj.filters** — `MCPCore.cpp/h` Tools 필터 추가
4. **SteinsGateApp.cpp** — `applicationDidFinishLaunching()`에서 `MCPCore::Initialize()` 호출
5. **WorldScene.cpp** — `UpdateNet()`에서 `g_cMCPCore.PollEvents()`, `onExit()`에서 `MCPCore::Finalize()` 호출

### 생성
1. **MCPCore.h** — 싱글톤 클래스 선언
2. **MCPCore.cpp** — 싱글톤 클래스 구현

## 검증
- sgcl 빌드 성공
- mcpr 테스트 6/6 유지 (회귀 없음)

# PreBuildEvent: libcocos2d.dll 존재 여부 검사

## 개요

`Harmony_Full-NoEngine.sln`은 `libcocos2d.vcxproj`를 참조하지 않기 때문에,
처음 솔루션을 빌드하면 Cocos2d-x DLL 출력 디렉터리에 `libcocos2d.dll`이 존재하지 않는다.

그러나 `sgcl`의 PostBuildEvent에서
`ThirdParty\Cocos2d-x\Output\$(Platform)\$(Configuration)\*.dll`을
`$(OutDir)`로 복사하도록 되어 있어, **빌드 링크 단계는 성공**(.lib는 존재하므로)하지만
`libcocos2d.dll`이 없어 PostBuildEvent 복사 시 빠지고, 이후 실행 단계에서 크래시가 발생한다.

## 수정 내용

**파일:** `Projects\sgcl\Project\sgcl.vcxproj`

### Edit 1 — General PreBuildEvent (기존 Condition 제거 + 체크 추가)

**기존** (984-992번 라인):
```xml
<PreBuildEvent>
  <Command Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
  </Command>
</PreBuildEvent>
```

**변경 후:**
```xml
<PreBuildEvent>
  <Command>
    if not exist "$(SolutionDir)ThirdParty\Cocos2d-x\Output\$(Platform)\$(Configuration)\libcocos2d.dll" (
      echo [ERROR] libcocos2d.dll not found.
      echo [ERROR] Build the Cocos2d-x engine first using Harmony_Full.sln.
      exit 1
    )
  </Command>
</PreBuildEvent>
```

### Edit 2 — Release|x64 PreBuildEvent (동일 체크 추가)

**기존** (1075-1078번 라인 → 현재 1080-1088):
```xml
<PreBuildEvent>
  <Command>
  </Command>
</PreBuildEvent>
```

**변경 후:** Edit 1과 동일한 명령어

### 적용 범위 (MSBuild ItemDefinitionGroup 상속)

| Configuration | PreBuildEvent 출처 |
|---|---|
| `Debug|Win32` | General group 상속 (Edit 1) |
| `Debug|x64` | General group 상속 (Edit 1) |
| `Release|Win32` | General group 상속 (Edit 1) |
| `Release|x64` | 자체 PreBuildEvent가 override (Edit 2) |

## 빌드 실패 시 표시 메시지

```
[ERROR] libcocos2d.dll not found.
[ERROR] Build the Cocos2d-x engine first using Harmony_Full.sln.
```

## 사전 빌드 방법

```powershell
cd Scripts
.\Build.ps1 -ProjectName libcocos2d -Configuration Debug -Platform x64
```

또는 Visual Studio에서 `Harmony_Full.sln`으로 libcocos2d 프로젝트 빌드.

## 확인 사항

- [ ] `Harmony_Full-NoEngine.sln` 처음 빌드 시 위 오류 메시지와 함께 빌드 실패
- [ ] libcocos2d.dll 생성 후에는 정상 빌드됨
- [ ] 4개 Configuration(Debug/Release × Win32/x64) 모두 적용되는지 확인

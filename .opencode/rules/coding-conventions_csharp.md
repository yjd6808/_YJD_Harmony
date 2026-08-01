---
paths:
  - "Projects/SteinsGate-Tools*/**/*.cs"
---

# C# 코딩 컨벤션

## 네이밍 규칙

| 대상 | 규칙 |
|------|------|
| private / protected 멤버 필드 (non-static) | suffix `_`, camelCase (예: `health_`, `commandMap_`) |
| ref / out 파라미터 | prefix `_`, camelCase (예: `_data`, `_result`) |
| 일반 파라미터 | prefix `_`, camelCase (예: `_description`, `_level`) |
| 일반 지역 변수 | camelCase (예: `playerCount`, `elementList`) |
| const 지역 변수 | UPPER_CASE (예: `MAX_COUNT`, `BUFFER_SIZE`) |
| 클래스 / 구조체 / 열거형 | PascalCase (예: `SgaElement`, `IntPoint`) |
| 인터페이스 | prefix `I` + PascalCase (예: `IDataDragReceiver`) |
| 추상 클래스 | suffix `Abstract` (예: `CommandAbstract`, `SgaSpriteAbstract`) |
| 확장 메서드 클래스 | suffix `Ex` (예: `StringEx`, `CollectionEx`) |
| Converter 클래스 | suffix `Converter` (예: `ColorSolidBrushConverter`) |
| 메서드 | PascalCase, 동사 시작 (예: `Execute()`, `LoadIndexOnly()`) |
| 프로퍼티 | PascalCase (예: `ElementCount`, `IsNotifyEnabled`) |
| 이벤트 | PascalCase (예: `PropertyChanged`, `LogClick`) |
| const / static readonly 멤버 | PascalCase (예: `MaxItemCount`, `FadeInDuration`) |
| 열거형 값 | PascalCase (예: `None`, `Wait`, `Dragging`) |
| 제네릭 타입 파라미터 | 단일 대문자 (예: `T`, `K`, `V`) |

**금지 접두사:** `dw`, `ui`, `i`, `v` — 지역변수/파라미터/멤버 모두에 사용 금지 (예: `iCount` X → `count` O)

## 포매팅

- **Allman 스타일** 중괄호 사용
- `return`, `continue`, `break` 단일 제어문은 중괄호 생략 가능

```csharp
if (condition)
{
    DoSomething();
}

if (value == 0)
    return;
```

## 가상 함수 규칙

- 가상 함수: 반드시 `virtual` 명시
- 오버라이드 함수: `override`만 명시 (C#에서 override는 암묵적으로 virtual이므로 `virtual override` 조합은 CS0113 컴파일 오류)
- 추가 override를 막으려면: `sealed override` 명시
- abstract 구현: `override`만 명시

```csharp
virtual void Update();                        // 기반 가상 함수
override void Update();                       // 오버라이드 함수 (하위 클래스에서 다시 override 가능)
sealed override void Update();                // 오버라이드 함수 (하위 클래스에서 override 불가)
override void Execute(object? parameter);     // abstract 구현
```

## 함수 시각적 구분자 (.cs 전용)

각 함수 앞에 `/` 문자로 구성된 구분자 삽입. 길이 = `90 - (들여쓰기 레벨 × 4)`

```csharp
// 들여쓰기 레벨 0 (길이 90):
//////////////////////////////////////////////////////////////////////////////////////////
void Function() { }

// 들여쓰기 레벨 1 (길이 86):
namespace NA
{
    //////////////////////////////////////////////////////////////////////////////////////
    void Function() { }
}

// 들여쓰기 레벨 2 (길이 82):
namespace NA
{
    class CA
    {
        //////////////////////////////////////////////////////////////////////////////////
        void Function() { }
    }
}
```

## 접근 제한자

- 항상 **명시적으로** 작성 (생략 금지)
- 순서: 접근 제한자 → `static` → `abstract` / `virtual` / `override` / `sealed`

```csharp
public abstract class CommandAbstract : ICommand
{
    public string Name => GetType().Name;
    protected bool initialized_;

    public abstract void Execute(object? parameter);
    protected virtual void OnDispose() { }
    public override string ToString() => Name;
}
```

## 프로퍼티

Expression-bodied 프로퍼티 선호:

```csharp
public SgaPackage Parent => parent_;
public bool DataLoaded => dataLoaded_;
public int Right => X + Width;

// 복잡한 경우 명시적 getter
public string InfoString
{
    get
    {
        if (IsLink)
            return $"[{FrameIndex}] {Width}x{Height} (링크-{TargetFrameIndex})";
        return $"[{FrameIndex}] {Width}x{Height}";
    }
}

// Bindable 기반 setter
public LogData LastLog
{
    get => lastLog_;
    set
    {
        lastLog_ = value;
        OnPropertyChanged();
    }
}
```

## 이벤트

```csharp
public event PropertyChangedEventHandler? PropertyChanged;
public event EventHandler? CanExecuteChanged;

// 커스텀 RoutedEvent
public event RoutedEventHandler LogClick
{
    add { AddHandler(LogClickEvent, value); }
    remove { RemoveHandler(LogClickEvent, value); }
}
```

## async / await

반환 타입은 `Task` / `Task<T>` 명시. 메서드명에 `Async` suffix 사용:

```csharp
public virtual async Task ExecuteAsync(object? _parameter) { }
public async Task<Exception> SaveAsync(string _path, bool _minify = false) { }
```

## using 문 순서

1. `System.*`
2. `System.Windows.*` (WPF)
3. 외부 NuGet 라이브러리 (예: `Newtonsoft.Json`)
4. 프로젝트 네임스페이스 (예: `SGToolsCommon`, `SGToolsUI`)

## 주석 스타일

파일 헤더:
```csharp
/*
 * 작성자: 윤정도
 * 생성일: 3/6/2023 4:44:35 PM
 *
 */
```

인라인 주석은 한국어 사용:
```csharp
// Xaml 바인딩용
public SgaElement() { }

// Lazy Loading 용
private List<SgaElementHeader> elementHeaderList_;
```

## C# 전용 패턴

**Nullable Reference Types:**
```csharp
// 이벤트 / 콜백
public event PropertyChangedEventHandler? PropertyChanged;
private Action<Exception>? errorHandler_;

// null이 될 수 있는 모든 참조 타입 필드 / 프로퍼티에 ? 명시
private ScrollViewer? scrollViewer_;
public Brush? Foreground { get; set; } = null;
public FontFamily? FontFamily { get; set; } = null;
public Action? Click { get; set; } = null;
public ContextMenu? LogContextMenu { get; set; }

// null이 될 수 있는 반환 타입
public RTBLogItem? Find(Func<RTBLogItem, bool> _predicate) => ...;
private ScrollViewer? GetScrollViewer() => ...;
private RTBLogItemElement? HitTestElement(...) => ...;

// null이 될 수 있는 파라미터
public void WriteLine(string _text, RTBTextOption? _option = null) { }
public void MergeFrom(RTBTextOption? _override) { }
public void Select(RTBLogItem? _item) { }
public string GetPlainText(RTBLogItem? _item) => _item?.PlainText ?? string.Empty;
```

**Nullable 적용 기준:**
- 참조 타입 필드/프로퍼티의 기본값이 `null`이면 반드시 `?` 명시
- 메서드 반환 타입이 `null`을 반환할 수 있으면 반드시 `?` 명시 (예: `FirstOrDefault`, `VisualTreeHelper 탐색` 등)
- 파라미터가 `null` 허용인 경우 `?` 명시 (특히 `= null` 기본값 파라미터)
- null이 **절대 될 수 없는** 참조 타입은 `?` 생략 (예: 생성자에서 반드시 초기화되는 필드)
- null 단언 연산자(`!`)는 `null`이 아님이 **논리적으로 보장**될 때만 사용

```csharp
// null 단언: HasClick 검사 후 Click이 null이 아님이 보장됨
if (hit.Option.HasClick && !hit.Option.IsLink)
    hit.Option.Click!();
```

**ref / out 파라미터:**
```csharp
public static void ToArray(this Bitmap _bmp, out byte[] _data) { }
private static extern int Compress(byte[] _dest, ref int _destLen, byte[] _source, int _sourceLen);
```

**패턴 매칭:**
```csharp
if (value is not Color color)
    throw new Exception("value가 Color 타입이 아닙니다.");

if (parameter is SGUIElement element)
{
    AddSingleElement(element);
    return;
}
```

**Collection 초기화:**
```csharp
private Dictionary<string, CommandAbstract> commandMap_ = new();
```

**문자열 보간:**
```csharp
throw new ArgumentNullException($"{Name} 커맨드의 파라미터는 널일 수 없습니다.");
```


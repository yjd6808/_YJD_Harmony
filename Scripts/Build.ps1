# Build.ps1
# 프로젝트 이름을 인자로 받아 C++ / C# 여부를 자동 판별하여 빌드/리빌드/클린을 수행한다.
# created by AI.
#
# 사용법:
#   .\Scripts\Build.ps1 -ProjectName <프로젝트명> [-Configuration|-C Debug|Release] [-Platform|-P x64|x86] [-Rebuild] [-Clean] [-FileLog] [-NoConsoleLogging]
#
# 예시:
#   .\Scripts\Build.ps1 -ProjectName MyProject
#   .\Scripts\Build.ps1 -ProjectName SteinsGate-Tools.UI -C Release
#   .\Scripts\Build.ps1 -ProjectName MyProject -Rebuild
#   .\Scripts\Build.ps1 -ProjectName MyProject -Clean
#   .\Scripts\Build.ps1 -ProjectName MyProject -FileLog   (빌드 로그를 Scripts\BuildProject\sgs-{년월일}-{시분초}.log 에 저장)
#   .\Scripts\Build.ps1 -ProjectName MyProject -FileLog -NoConsoleLogging   (콘솔 출력 없이 로그 파일로만 저장)

param (
    [Parameter(Mandatory = $true)]
    [string]$ProjectName,

    [Alias("C")]
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Debug",

    [Alias("P")]
    [ValidateSet("x64", "x86")]
    [string]$Platform = "x64",

    [switch]$Rebuild,

    [switch]$Clean,

    [switch]$FileLog,

    [switch]$NoConsoleLogging
)

# UTF-8 고정 (한글 출력 깨짐 방지)
# UTF-8 고정 (외부 명령어 파이프 시 한글 깨짐 방지)
$OutputEncoding = [System.Text.Encoding]::UTF8

# ---------------------------------------------------------------------
# 유틸리티 함수
# ---------------------------------------------------------------------

$script:LogWriter = $null
$script:LogFile   = $null
$script:NoConsole = $NoConsoleLogging

function Write-Log([string]$msg) {
    if ($script:LogWriter) { $script:LogWriter.WriteLine($msg) }
}

function Write-Info  ([string]$msg) { if (-not $script:NoConsole) { Write-Host "[INFO]  $msg" -ForegroundColor Cyan };   Write-Log "[INFO]  $msg" }
function Write-Ok    ([string]$msg) { if (-not $script:NoConsole) { Write-Host "[OK]    $msg" -ForegroundColor Green };  Write-Log "[OK]    $msg" }
function Write-Fail  ([string]$msg) { if (-not $script:NoConsole) { Write-Host "[FAIL]  $msg" -ForegroundColor Red };    Write-Log "[FAIL]  $msg" }
function Write-Warn  ([string]$msg) { if (-not $script:NoConsole) { Write-Host "[WARN]  $msg" -ForegroundColor Yellow };  Write-Log "[WARN]  $msg" }

function Close-Log {
    if ($script:LogWriter) {
        $script:LogWriter.Flush()
        $script:LogWriter.Dispose()
        $script:LogWriter = $null
    }
}

# ---------------------------------------------------------------------
# 파라미터 유효성 검사
# ---------------------------------------------------------------------

if ($Rebuild -and $Clean) {
    Write-Fail "-Rebuild 과 -Clean 은 동시에 사용할 수 없습니다."
    exit 1
}

# 수행할 MSBuild 타겟 결정
$action = if ($Clean) { "Clean" } elseif ($Rebuild) { "Rebuild" } else { "Build" }

# ---------------------------------------------------------------------
# SolutionDir 결정
# ---------------------------------------------------------------------

try {
    $gitRoot = & git rev-parse --show-toplevel 2>&1
    if ($LASTEXITCODE -ne 0) { throw "git 명령 실패: $gitRoot" }
    $solutionDir = $gitRoot.Replace('/', '\').TrimEnd('\') + "\"
}
catch {
    Write-Fail "SolutionDir를 결정할 수 없습니다: $_"
    exit 1
}

# ---------------------------------------------------------------------
# 파일 로그 초기화 (Scripts\BuildProject\sgs-{년월일}-{시분초}.log)
# ---------------------------------------------------------------------

if ($FileLog) {
    $logDir = Join-Path $solutionDir "Scripts\BuildProject"
    if (-not (Test-Path $logDir)) { New-Item -ItemType Directory -Path $logDir -Force | Out-Null }
    $script:LogFile = Join-Path $logDir ("sgs-{0}-{1}.log" -f (Get-Date -Format 'yyyyMMdd'), (Get-Date -Format 'HHmmss'))
    $script:LogWriter = New-Object System.IO.StreamWriter($script:LogFile, $true, (New-Object System.Text.UTF8Encoding($false)))
    $script:LogWriter.AutoFlush = $true
    Write-Info "LogFile     : $script:LogFile"
}

Write-Info "SolutionDir : $solutionDir"
Write-Info "ProjectName : $ProjectName"
Write-Info "Config      : $Configuration / $Platform"
Write-Info "Action      : $action"

# ---------------------------------------------------------------------
# msbuild 경로 확인
# ---------------------------------------------------------------------

$msbuild = & where.exe msbuild 2>&1 | Select-Object -First 1
if (-not $msbuild -or $LASTEXITCODE -ne 0) {
    # vswhere로 재탐색
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if ($vswhere -and (Test-Path $vswhere)) {
        $vsPath = & $vswhere -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe 2>&1 | Select-Object -First 1
        if ($vsPath -and (Test-Path $vsPath)) {
            $msbuild = $vsPath
        }
    }
}

if (-not $msbuild -or -not (Test-Path $msbuild -ErrorAction SilentlyContinue)) {
    Write-Fail "msbuild를 찾을 수 없습니다. Visual Studio 또는 Build Tools가 설치되어 있는지 확인하세요."
    exit 1
}

Write-Info "msbuild     : $msbuild"

# ---------------------------------------------------------------------
# 프로젝트 파일 경로 결정
# ---------------------------------------------------------------------

$projectBase  = "${solutionDir}Projects\$ProjectName\Project"
$vcxprojPath  = "$projectBase\$ProjectName.vcxproj"
$csprojPath   = "$projectBase\$ProjectName.csproj"

# ---------------------------------------------------------------------
# 실행
# ---------------------------------------------------------------------

if (Test-Path $vcxprojPath) {
    # ---- C++ 프로젝트 ----
    Write-Info "프로젝트 종류: C++ (.vcxproj)"
    Write-Info "$action 시작..."

    & $msbuild $vcxprojPath `
        /t:$action `
        /p:Configuration=$Configuration `
        /p:Platform=$Platform `
        /p:SolutionDir="$solutionDir" `
        /m 2>&1 | ForEach-Object {
            if (-not $script:NoConsole) { Write-Output $_ }
            Write-Log "$_"
        }

    if ($LASTEXITCODE -ne 0) {
        Write-Fail "C++ $action 실패 (exit code: $LASTEXITCODE)"
        Write-Fail "로그 파일: $script:LogFile"
        exit $LASTEXITCODE
    }

    Write-Ok "C++ $action 성공: $ProjectName"
}
elseif (Test-Path $csprojPath) {
    # ---- C# 프로젝트 ----
    Write-Info "프로젝트 종류: C# (.csproj)"
    Write-Info "$action 시작..."

    & $msbuild $csprojPath `
        /t:$action `
        /p:Configuration=$Configuration `
        /p:Platform=$Platform `
        /p:SolutionDir="$solutionDir" `
        /m 2>&1 | ForEach-Object {
            if (-not $script:NoConsole) { Write-Output $_ }
            Write-Log "$_"
        }

    if ($LASTEXITCODE -ne 0) {
        Write-Fail "C# $action 실패 (exit code: $LASTEXITCODE)"
        Write-Fail "로그 파일: $script:LogFile"
        exit $LASTEXITCODE
    }

    Write-Ok "C# $action 성공: $ProjectName"
}
else {
    Write-Fail "프로젝트 파일을 찾을 수 없습니다: $ProjectName"
    Write-Warn "확인한 경로:"
    Write-Warn "  .vcxproj -> $vcxprojPath"
    Write-Warn "  .csproj  -> $csprojPath"
    exit 1
}

Close-Log

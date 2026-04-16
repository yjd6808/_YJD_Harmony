# Build.ps1
# 프로젝트 이름을 인자로 받아 C++ / C# 여부를 자동 판별하여 빌드/리빌드/클린을 수행한다.
# created by AI.
#
# 사용법:
#   .\Scripts\Build.ps1 -ProjectName <프로젝트명> [-Configuration|-C Debug|Release] [-Platform|-P x64|x86] [-Rebuild] [-Clean]
#
# 예시:
#   .\Scripts\Build.ps1 -ProjectName MyProject
#   .\Scripts\Build.ps1 -ProjectName SteinsGate-Tools.UI -C Release
#   .\Scripts\Build.ps1 -ProjectName MyProject -Rebuild
#   .\Scripts\Build.ps1 -ProjectName MyProject -Clean

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

    [switch]$Clean
)

# ---------------------------------------------------------------------
# 유틸리티 함수
# ---------------------------------------------------------------------

function Write-Info  ([string]$msg) { Write-Host "[INFO]  $msg" -ForegroundColor Cyan }
function Write-Ok    ([string]$msg) { Write-Host "[OK]    $msg" -ForegroundColor Green }
function Write-Fail  ([string]$msg) { Write-Host "[FAIL]  $msg" -ForegroundColor Red }
function Write-Warn  ([string]$msg) { Write-Host "[WARN]  $msg" -ForegroundColor Yellow }

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
    if (Test-Path $vswhere) {
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
        /m

    if ($LASTEXITCODE -ne 0) {
        Write-Fail "C++ $action 실패 (exit code: $LASTEXITCODE)"
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
        /m

    if ($LASTEXITCODE -ne 0) {
        Write-Fail "C# $action 실패 (exit code: $LASTEXITCODE)"
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

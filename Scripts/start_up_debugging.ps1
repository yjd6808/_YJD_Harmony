# 시작 시점 오류 디버깅 스크립트
# 실행 -> 15초 대기 -> 종료 -> 남은 로그 확인
# 사용: .\start_up_debugging.ps1 [-ProjectName <프로젝트명>] [-Args <인자 문자열>]
#   -ProjectName  실행할 프로젝트명 (기본: sgcl) — release\{ProjectName}\{ProjectName}.exe
#   -Args         전달할 단일 인자 문자열 (기본: sgcl 표준 인자)

param(
    [string]$ProjectName = "sgcl",
    [string]$Args = ""
)

$ErrorActionPreference = "Stop"

# 출력 인코딩 UTF-8 설정 (한글 출력 깨짐 방지)
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
$OutputEncoding = [System.Text.Encoding]::UTF8

# SolutionDir = Harmony_Full.sln이 위치한 디렉토리 (이 스크립트는 Scripts\ 아래에 있음)
# DeployRoot  = $(SolutionDir)..\UltimateHarmony_Dev (저장소 외부 공용 개발 폴더)
$SolutionDir = Split-Path -Parent $PSScriptRoot
$deployRoot  = Join-Path $SolutionDir "..\UltimateHarmony_Dev" | Resolve-Path -ErrorAction SilentlyContinue
if (-not $deployRoot) {
    Write-Error "배포 폴더를 찾을 수 없습니다: $SolutionDir\..\UltimateHarmony_Dev (UltimateHarmony_Dev 폴더 존재 확인)"
}

$rel    = Join-Path $deployRoot "release\$ProjectName"
$logDir = Join-Path $rel "logs\default"
$assets = Join-Path $deployRoot "assets"
$exe    = Join-Path $rel "$ProjectName.exe"

# 경로 검증
if (-not (Test-Path $rel)) {
    Write-Error "실행 폴더를 찾을 수 없습니다: $rel"
}
if (-not (Test-Path $exe)) {
    Write-Error "exe를 찾을 수 없습니다: $exe (빌드 후 release 폴더에 복사 필요)"
}
if (-not (Test-Path $assets)) {
    Write-Error "assets 폴더를 찾을 수 없습니다: $assets"
}

if (-not $Args) {
    # sgcl은 argv[1] 하나만 파싱하므로 인자를 단일 따옴표 문자열로 전달
    $Args = '"assets=' + $assets + ' mode=1 auth_ep=127.0.0.1:10110"'
}

# 실행 전 로그 스냅샷 (이전 실행 로그와 구분)
$before = (Get-ChildItem $logDir -Filter *.log -ErrorAction SilentlyContinue |
    Sort-Object LastWriteTime -Descending | Select-Object -First 1).LastWriteTime

# 기존 프로세스 정리 후 실행
Get-Process $ProjectName -ErrorAction SilentlyContinue | Stop-Process -Force
Start-Process -FilePath $exe -WorkingDirectory $rel -ArgumentList $Args
Write-Output "[1/3] $ProjectName 실행됨 - 15초 대기 중..."

# 15초 대기 후 프로그램 종료
Start-Sleep -Seconds 15
Get-Process $ProjectName -ErrorAction SilentlyContinue | Stop-Process -Force
Write-Output "[2/3] 15초 경과 - 프로세스 종료"

# 남은 로그 확인 (이번 실행 파일)
$log = Get-ChildItem $logDir -Filter *.log -ErrorAction SilentlyContinue |
    Where-Object { $_.LastWriteTime -gt $before } |
    Sort-Object LastWriteTime -Descending | Select-Object -First 1

if ($log) {
    Write-Output "[3/3] 이번 실행 로그: $($log.FullName)"
    Write-Output "========================================"
    Get-Content $log.FullName -Tail 60
} else {
    Write-Output "[3/3] 새 로그가 생성되지 않음 - 실행 실패 가능성 (assets 경로 / DLL 유무 확인)"
}

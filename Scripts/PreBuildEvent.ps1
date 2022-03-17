# 작성자 : 윤정도
#   목적 : 라이브러리 프로젝트의 포스트 빌드 이벤트를 수행합니다.
#          헤더 파일들과 출력된 라이브러리 파일을 복사합니다.

using module '.\FileUtil.psm1'

using namespace System;
using namespace System.IO;
using namespace System.Diagnostics;


[CmdletBinding(PositionalBinding=$false)]
 Param(
    [string][Alias('p')]$ProjectName,
    [string][Alias('c')]$Configuration
)

 # $ProjectName = 'JCore'
 # $Configuration = 'Debug'


if (($Configuration -ne 'Debug') -and ($Configuration -ne 'Release')) {
    Write-Host '-c Debug 또는 -c Release 옵션이 필요합니다.'
}

$SolutionPath = Resolve-Path (Join-Path $PSScriptRoot '..\')
$SolutionOutputPath = Join-Path $SolutionPath 'Output'
$OutputIncludePath = [Path]::Combine($SolutionOutputPath, 'include', $ProjectName, $ProjectName)
$OutputLibraryPath = Join-Path $SolutionOutputPath 'lib'
$OutputBinPath = Join-Path $SolutionOutputPath 'bin'

$ProjectPath = Join-Path $SolutionPath 'Projects' $ProjectName
$SourcesPath = [Path]::Combine($ProjectPath, 'Sources', $ProjectName)
$LibraryPath = [Path]::Combine($ProjectPath, 'Output', $Configuration) 
$LibraryFilePath = Join-Path $LibraryPath ([String]::Format("{0}.lib", $ProjectName))

if (!(Test-Path $SourcesPath)) {
    Write-Host $SourcesPath 
    Write-Host '소스 파일 경로가 존재하지 않습니다.'
    exit 1
}

if (!(Test-Path $LibraryFilePath)) {
    Write-Host $LibraryFilePath
    Write-Host '출력된 라이브러리 파일이 존재하지 않습니다.'
    exit 1
}

Write-Host 'Post Build Event를 시작합니다.'
Write-Host '헤더파일들을 복사합니다.'

foreach ($HeaderFile in [FileUtil]::GetChildrenInDirectory($SourcesPath, $True, $True)) {

    if ([Path]::GetExtension($HeaderFile) -ne '.h') {
        continue
    }

    $HeaderFileName = [Path]::GetFileName($HeaderFile)

    $SubPath = $HeaderFile.Replace($SourcesPath, [String]::Empty)
    $SubPath = $SubPath.Replace($HeaderFileName, [String]::Empty)
    
    $DetinationDirectoryPath = Join-Path $OutputIncludePath $SubPath
    $DetinationFilePath = Join-Path $DetinationDirectoryPath $HeaderFileName

    # 중간 경로 없을 경우 생성
    if ((Test-Path $DetinationDirectoryPath) -eq $False) {
        New-Item -Type Directory $DetinationDirectoryPath
    }

    Copy-Item $HeaderFile -Destination $DetinationFilePath
    Write-Host $DetinationFilePath 복사완료
}


Write-Host '라이브러리 파일을 복사합니다.'
$LibraryFileName = [Path]::GetFileName($LibraryFilePath)
Copy-Item $LibraryFilePath -Destination $OutputLibraryPath
Write-Host (Join-Path $OutputLibraryPath $LibraryFileName) 복사완료


Write-Host 'Post Build Event를 완료하였습니다.'



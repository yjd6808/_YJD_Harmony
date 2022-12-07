# �ۼ��� : ������
#   ���� : ���̺귯�� ������Ʈ�� ����Ʈ ���� �̺�Ʈ�� �����մϴ�.
#          ��� ���ϵ�� ��µ� ���̺귯�� ������ �����մϴ�.

using module '.\FileUtil.psm1'

using namespace System;
using namespace System.IO;
using namespace System.Diagnostics;


[CmdletBinding(PositionalBinding=$false)]
 Param(
    [string][Alias('p')]$ProjectName,
    [string][Alias('f')]$Platform,
    [string][Alias('c')]$Configuration
)

# �׽�Ʈ
# $Configuration = 'Debug'
# $Platform = 'Win32'
# $ProjectName = 'JCore'

if (($Platform -ne 'x64') -and ($Platform -ne 'Win32')) {
    Write-Host '-f Win32 �Ǵ� -f x64 �ɼ��� �ʿ��մϴ�.'
    exit 3
}

if (($Configuration -ne 'Debug') -and ($Configuration -ne 'Release')) {
    Write-Host '-c Debug �Ǵ� -c Release �ɼ��� �ʿ��մϴ�.'
    exit 2
}

$SolutionPath = Resolve-Path (Join-Path $PSScriptRoot '..\..\')
$SolutionOutputPath = Join-Path $SolutionPath 'Output'
$OutputIncludePath = [Path]::Combine($SolutionOutputPath, 'include', $ProjectName, $ProjectName)
$OutputLibraryPath = [Path]::Combine($SolutionOutputPath, 'lib', $Platform, $Configuration)
$OutputBinPath = [Path]::Combine($SolutionOutputPath, 'bin', $Platform, $Configuration)

$ProjectPath = [Path]::Combine($SolutionPath, 'Projects', $ProjectName)
$SourcesPath = [Path]::Combine($ProjectPath, 'Sources', $ProjectName)
$LibraryPath = [Path]::Combine($ProjectPath, 'Output', $Platform, $Configuration) 
$LibraryFilePath = Join-Path $LibraryPath ([String]::Format("{0}.lib", $ProjectName))
$LibraryPdbPath = Join-Path $LibraryPath ([String]::Format("{0}.pdb", $ProjectName))

Write-Host '[Post-Build Event ��� ����]'
Write-Host 'SolutionPath : ' $SolutionPath
Write-Host 'SolutionOutputPath : ' $SolutionOutputPath
Write-Host 'OutputIncludePath : ' $OutputIncludePath
Write-Host 'OutputLibraryPath : ' $OutputLibraryPath
Write-Host 'OutputBinPath : ' $OutputBinPath

Write-Host 'ProjectPath : ' $ProjectPath
Write-Host 'SourcesPath : ' $SourcesPath
Write-Host 'LibraryPath : ' $LibraryPath
Write-Host 'LibraryFilePath : ' $LibraryFilePath

if (!(Test-Path $SourcesPath)) {
    Write-Host $SourcesPath 
    Write-Host '�ҽ� ���� ��ΰ� �������� �ʽ��ϴ�.'
    exit 1
}

if (!(Test-Path $LibraryFilePath)) {
    Write-Host $LibraryFilePath
    Write-Host '��µ� ���̺귯�� ������ �������� �ʽ��ϴ�.'
    exit 1
}

Write-Host 'Post Build Event�� �����մϴ�.'
Write-Host $SourcesPath '������ ���ϴ�.'
[Directory]::Delete($OutputIncludePath, $True);

Write-Host '������ϵ��� �����մϴ�.'

foreach ($HeaderFile in [FileUtil]::GetChildrenInDirectory($SourcesPath, $True, $True)) {

    if ([Path]::GetExtension($HeaderFile) -ne '.h') {
        continue
    }

    $HeaderFileName = [Path]::GetFileName($HeaderFile)

    $SubPath = $HeaderFile.Replace($SourcesPath, [String]::Empty)
    $SubPath = $SubPath.Replace($HeaderFileName, [String]::Empty)
    
    $DetinationDirectoryPath = Join-Path $OutputIncludePath $SubPath
    $DetinationFilePath = Join-Path $DetinationDirectoryPath $HeaderFileName

    # �߰� ��� ���� ��� ����
    if ((Test-Path $DetinationDirectoryPath) -eq $False) {
        New-Item -Type Directory $DetinationDirectoryPath
    }

    Copy-Item $HeaderFile -Destination $DetinationFilePath
    Write-Host $DetinationFilePath ����Ϸ�
}


Write-Host '���̺귯�� ������ �����մϴ�.'
$LibraryFileName = [Path]::GetFileName($LibraryFilePath)

# lib/$Configuraton ��� ���� �� ������ ����
if ((Test-Path $OutputLibraryPath) -eq $False) {
    New-Item -Type Directory $OutputLibraryPath
}

Copy-Item $LibraryFilePath -Destination $OutputLibraryPath
Write-Host (Join-Path $OutputLibraryPath $LibraryFileName) ����Ϸ�


Write-Host 'Post Build Event�� �Ϸ��Ͽ����ϴ�.'



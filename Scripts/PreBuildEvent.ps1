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
    [string][Alias('c')]$Configuration
)

 # $ProjectName = 'JCore'
 # $Configuration = 'Debug'


if (($Configuration -ne 'Debug') -and ($Configuration -ne 'Release')) {
    Write-Host '-c Debug �Ǵ� -c Release �ɼ��� �ʿ��մϴ�.'
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
    Write-Host '�ҽ� ���� ��ΰ� �������� �ʽ��ϴ�.'
    exit 1
}

if (!(Test-Path $LibraryFilePath)) {
    Write-Host $LibraryFilePath
    Write-Host '��µ� ���̺귯�� ������ �������� �ʽ��ϴ�.'
    exit 1
}

Write-Host 'Post Build Event�� �����մϴ�.'
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
Copy-Item $LibraryFilePath -Destination $OutputLibraryPath
Write-Host (Join-Path $OutputLibraryPath $LibraryFileName) ����Ϸ�


Write-Host 'Post Build Event�� �Ϸ��Ͽ����ϴ�.'



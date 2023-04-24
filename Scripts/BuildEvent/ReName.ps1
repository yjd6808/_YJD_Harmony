# 작성자: 윤정도
# 작성일: 2023년 4월 24일 밤 11시 09분
# 파일이름 한번에 변경하기 위해서

using module '.\FileUtil.psm1'


$Ini = [FileUtil]::ReadInifile($PSScriptRoot + "/ReName.ini")
$FileMap = @{}
$ReplacableFileCount = 0

foreach ($File in [FileUtil]::GetChildrenInDirectory($ini.Setting.Directory, $True, $True)) {
    $File = [System.IO.Path]::GetFileName($File)
    [System.String]$ReplaceFile = [System.Text.RegularExpressions.Regex]::Replace($File, $Ini.Setting.RegexSearch, $Ini.Setting.RegexReplace)
    $FileMap.Add($File, $ReplaceFile)
}

# 해쉬맵 사용법
# https://learn.microsoft.com/en-us/powershell/module/microsoft.powershell.core/about/about_hash_tables?view=powershell-7.3
$FileMap.GetEnumerator() | ForEach-Object {
    if ($_.Key -ne $_.Value) { 
        ++$ReplacableFileCount 
        $PaddedKey   = [System.String]::Format("{0,-30}", $_.Key)
        $PaddedValue = [System.String]::Format("{0,-30}", $_.Value)
        [System.Console]::WriteLine($PaddedKey + " ==> " + $PaddedValue)
    }
}

if ($ReplacableFileCount -eq 0) {
    [System.Console]::WriteLine("변경가능한 파일이 없습니다.");
    exit
}

[System.Console]::Write("'$ReplacableFileCount'개의 파일명을 변경하시겠습니까? [Y|y]es 입력시 변경함 >> ")
$UserInput = Read-Host

if (($UserInput -ne "Yes") -and ($UserInput -ne "yes")) {
    exit
}

$FileMap.GetEnumerator() | ForEach-Object {
    $Source = Join-Path $ini.Setting.Directory $_.Key
    $Destination = Join-Path $ini.Setting.Directory $_.Value
    [System.IO.File]::Move($Source, $Destination)
}
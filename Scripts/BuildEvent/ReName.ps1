# �ۼ���: ������
# �ۼ���: 2023�� 4�� 24�� �� 11�� 09��
# �����̸� �ѹ��� �����ϱ� ���ؼ�

using module '.\FileUtil.psm1'


$Ini = [FileUtil]::ReadInifile($PSScriptRoot + "/ReName.ini")
$FileMap = @{}
$ReplacableFileCount = 0

foreach ($File in [FileUtil]::GetChildrenInDirectory($ini.Setting.Directory, $True, $True)) {
    $File = [System.IO.Path]::GetFileName($File)
    [System.String]$ReplaceFile = [System.Text.RegularExpressions.Regex]::Replace($File, $Ini.Setting.RegexSearch, $Ini.Setting.RegexReplace)
    $FileMap.Add($File, $ReplaceFile)
}

# �ؽ��� ����
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
    [System.Console]::WriteLine("���氡���� ������ �����ϴ�.");
    exit
}

[System.Console]::Write("'$ReplacableFileCount'���� ���ϸ��� �����Ͻðڽ��ϱ�? [Y|y]es �Է½� ������ >> ")
$UserInput = Read-Host

if (($UserInput -ne "Yes") -and ($UserInput -ne "yes")) {
    exit
}

$FileMap.GetEnumerator() | ForEach-Object {
    $Source = Join-Path $ini.Setting.Directory $_.Key
    $Destination = Join-Path $ini.Setting.Directory $_.Value
    [System.IO.File]::Move($Source, $Destination)
}
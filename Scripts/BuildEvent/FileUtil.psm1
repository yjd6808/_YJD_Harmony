##�ۼ��� : ������
##�ۼ��� : 20-02-25

using module ".\Logger.psm1"

using namespace System
using namespace System.IO
using namespace System.Collections.Generic


class FileUtil
{
    <#########################################################################
                          �����ȿ� ���ϵ� ��������
     recursive - �������� ���ϵ� �˻��Ұ���
     exceptFolder - ������ ��������
    ##########################################################################>
    [Array] static GetChildrenInDirectory([string]$folderPath, [bool] $recursive, [bool] $exceptFolder)
    {
        $children = @()

        if ([Directory]::Exists( $folderPath ) -eq $false)
        {
            [Logger]::WriteLineNotice($folderPath + " ��ΰ� �������� �ʽ��ϴ�.")
            return 0
        }
        
        if ([FileUtil]::IsDirectoryPath($folderPath) -eq $false)
        {
            [Logger]::WriteLineError($folderPath + " �� ������ΰ� �ƴմϴ�")
            exit -1
        }

        [IEnumerable[string]]$folderEnumerator =  [Directory]::EnumerateFileSystemEntries($folderPath)
         foreach ($fileOrFolder in $folderEnumerator)
         {
            
            if ( [FileUtil]::IsDirectoryPath($fileOrFolder.ToString()))
            {
                
                if ($recursive)
                {
                    $children += [FileUtil]::GetChildrenInDirectory($fileOrFolder, $recursive, $exceptFolder)
                }

                if ($exceptFolder -eq $false)
                {
                    $children += $fileOrFolder
                }
            }
            else
            {
                $children += $fileOrFolder
            }
         }
         return $children
    }

    <#########################################################################
                          �����ȿ� ���� ���� �Ǵ� �������� ���� ���� ��������
     recursive - �������� ���ϵ� �˻��Ұ���
     exceptFolder - ������ �������� �����Ұ���
    ##########################################################################>
    [int] static GetChildrenCountInDirectory([string]$folderPath, [bool] $recursive, [bool] $exceptFolder)
    {
        if ([Directory]::Exists( $folderPath ) -eq $false)
        {
            [Logger]::WriteLineNotice($folderPath + " ��ΰ� �������� �ʽ��ϴ�.")
            return 0
        }
        
        if ([FileUtil]::IsDirectoryPath($folderPath) -eq $false)
        {
            [Logger]::WriteLineError($folderPath + " �� ������ΰ� �ƴմϴ�")
            exit -1
        }

        [IEnumerable[string]]$folderEnumerator =  [Directory]::EnumerateFileSystemEntries($folderPath)
         $count = 0
         foreach ($fileOrFolder in $folderEnumerator)
         {
            
            if ( [FileUtil]::IsDirectoryPath($fileOrFolder.ToString()))
            {
                if ($recursive)
                {
                    $count += [FileUtil]::GetChildrenCountInDirectory($fileOrFolder, $recursive, $exceptFolder)
                }

                if ($exceptFolder -eq $false)
                {
                    $count++
                }
            }
            else
            {
                $count++
            }
         }
         return $count
    }

    <#########################################################################
                            ���� �Ǵ� ���� ����
    ##########################################################################>
    [void] static RemoveFileOrDirectory([string] $item)
    {

        if( (Test-Path $item) -eq $true )
        {
            Remove-Item $item -Recurse -Force
            [Logger]::WriteLineNotice($item + "�׸��� �����߽��ϴ�.")
        }
    }

    <#########################################################################
                            XML ���� �б�
    ##########################################################################>
    [xml] static ReadXmlFile([string]$xmlFilePath)
    {
        
        if( (Test-Path $xmlFilePath) -eq $FALSE )
        {
           [Logger]::WriteLineErrorCovered([string]::Format("{0} ������ �������� �ʽ��ϴ�. ������ �����ϰ� �������� �־��ּ���.", $xmlFilePath)) 
            exit -1
        }

        [xml]$xmlData = Get-content $xmlFilePath

        if( ($xmlData -eq $NULL) )
        {
            [Logger]::WriteLineErrorCovered([string]::Format("{0} ������ �������� Ȯ���ϼ���.", $xmlFilePath)) 
            exit -1
        }

        return $xmlData
    }

    <#########################################################################
                            XML ���� ����
    ##########################################################################>
    [void] static SaveXmlFile([xml] $xmlData, [string] $filePath)
    {
        [string]$directoryName = [Path]::GetDirectoryName($filePath)
        if ([Directory]::Exists( $directoryName ) -eq $false)
        {
            [Logger]::WriteLineNotice($directoryName + " ��ΰ� �������� �ʾƼ� �����߽��ϴ�.")
            New-Item $directoryName -Type Directory
        }

        # Patch Version ������ ����
        $xmlData.save($filePath)
        [Logger]::WriteLineNotice($filePath + " �����͸� �����Ͽ����ϴ�")
    }
    <#########################################################################
                            7z ���Ϸ� �����ϱ�
    ##########################################################################>
    [void] static MakeDirectoryTo7zFile( [string] $srcDirectory, [string]$saveFilePath)
    {
        & 7z a $saveFilePath $srcDirectory | Out-Host
        [Logger]::WriteLineNotice( $srcDirectory + " ��θ� " + [Path]::GetFileName($saveFilePath) + " �̸����� �����Ͽ����ϴ�.")
    }

    <#########################################################################
                              ���� ����
    ##########################################################################>
    [void] static FileCopy( [string] $srcPath, [string] $dstPath )
    {
        $srcDirectoryPath = [Path]::GetDirectoryName($srcPath)
        if ([Directory]::Exists($srcDirectoryPath) -eq $false)
        {
            [Logger]::WriteLineErrorCovered($srcDirectoryPath + " ��ΰ� �������� �ʽ��ϴ�. ���翡 �����߽��ϴ�.", "")
            return
        }

        if( (Test-Path $srcPath) -eq $FALSE )
        {[Logger]::WriteLineErrorCovered($srcPath + "������ �������� �ʽ��ϴ�. ���翡 �����߽��ϴ�.", "")
            return
        }

        $dstDirectoryPath = [Path]::GetDirectoryName($dstPath)
        if( [Directory]::Exists($dstDirectoryPath) -eq $false )
        {
            [Logger]::WriteLineNotice($dstDirectoryPath + " ��ΰ� �������� �ʾƼ� �����߽��ϴ�.")
            New-Item $dstDirectoryPath -Type Directory
        }
        
        Copy-Item -Path $srcPath -Destination $dstPath -Recurse -Force
        [Logger]::WriteLineNotice($dstPath + "�� ���� ���簡 �����Ͽ����ϴ�.")
    }

    <#########################################################################
                              INI ���� �б�
                              https://stackoverflow.com/questions/43690336/powershell-to-read-single-value-from-simple-ini-file
    ##########################################################################>
    [Array] static ReadInifile([string] $iniFilePath)
    {
        $anonymous = "NoSection"

        $ini = @{}  
        $section = $null
        $CommentCount = 0


        switch -regex -file $iniFilePath  
        {  
            "^\[(.+)\]$" # ���� �Ľ�
            {  
                $section = $matches[1]  
                $ini[$section] = @{}  
                $CommentCount = 0  
            }  

            "^(#.*)$" # �ּ� �Ľ�
            {  
                if ($section -eq $null)  
                {  
                    $section = $anonymous  
                    $ini[$section] = @{}  
                }  
                $value = $matches[1]  
                $CommentCount = $CommentCount + 1  
                $name = "Comment" + $CommentCount  
                $ini[$section][$name] = $value  
            }   

            "(.+?)\s*=\s*(.*)" # Ű�� �Ľ�  
            {  
                if ($section -eq $null)  
                {  
                    $section = $anonymous  
                    $ini[$section] = @{}  
                }  
                $name,$value = $matches[1..2]  
                $ini[$section][$name] = $value  
            }  
        }  

        return $ini  

    }



    <#########################################################################
                              ���� ����
    ##########################################################################>
    [void] static Savefile([string] $path, [string] $content)
    {
        if ( (Test-Path $path) -and [FileUtil]::IsDirectoryPath($path) -eq $true)
        {
            [Logger]::WriteLineNotice([string]::Format("�Է��� {0} ��δ� ���� ����Դϴ�. �ùٸ� ���ϸ��� �Է����ּ���", $path))
            exit -1
        }

        ([Directory]::Exists( [Path]::GetDirectoryName($path)) -eq $false)
        {
            [Directory]::CreateDirectory([Path]::GetDirectoryName($path))
            [Logger]::WriteLineNotice( "�ش� ��ΰ� �������� �ʾƼ� �����߽��ϴ�.")
        }

        [File]::WriteAllText($path, $content)
    }

    <#########################################################################
                      �Է��� ��ΰ� �������� �������� Ȯ��
    ##########################################################################>
    [bool] static IsDirectoryPath([string]$path)
    {
        [FileAttributes] $attr = [File]::GetAttributes($path);

        if ($attr.HasFlag([FileAttributes]::Directory) -eq $true)
        {
            return $true
        }

        return $false
    }

    <#########################################################################
                              ���̳ʸ� ��������
    ##########################################################################>
    static [bool] IsBinaryFile([string]$path)
    {
        [long] $length = (New-Object FileInfo($path)).Length

        if ($length -eq 0) 
        {
            return $false
        }

        [StreamReader]$stream = New-Object StreamReader($path)
        [int] $ch = 0
        while (($ch = $stream.Read()) -ne -1)
        {
            if ( ($ch -gt [char]0 -and $ch -lt [char]8) -or ($ch -gt [char]13 -and $ch -lt [char]26))
            {
                return $true;
            }
        }
        return $false;
    }

    <#########################################################################
                          ���� ��θ� ������ ��Ÿ�Ϸ� ��ȯ

    ������� ����� �����
    ##########################################################################>
    static [string] ChangePathStyleToLinux([string]$path)
    {
        [string]$rootPath = [Path]::GetPathRoot($path)
        $rootPath = $rootPath.Replace( '\', '/' )
        $path = $path.Replace( '\', '/' )

        if ($rootPath.Length -gt 1)
        {
            $path = $path.Replace( $rootPath, "/mnt/c/" )
            
        }
        else
        {
            if ($path.StartsWith("/"))
            {
                $path = $path.Substring(1)
            }
        }

        return $path
    }


    <#########################################################################
     Json ���� �б� (������� ���� �� ����ؾ���)
    ##########################################################################>
#    [JObject] static ReadJsonfile([string] $jsonfilepath)
#    {
#        if ((Test-Path $jsonfilepath) -eq $false)
#        {
#            [Logger]::WriteLineNoticeError($jsonfilepath + " ������ �������� �ʽ��ϴ�", "")
#            exit -1
#        }

#        return [JObject]::Parse($jsonfilepath)
#    }
}



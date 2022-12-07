##작성자 : 윤정도
##작성일 : 20-02-25

using module ".\Logger.psm1"

using namespace System
using namespace System.IO
using namespace System.Collections.Generic


class FileUtil
{
    <#########################################################################
                          폴더안에 파일들 가져오기
     recursive - 폴더안의 파일도 검사할건지
     exceptFolder - 폴더는 제외할지
    ##########################################################################>
    [Array] static GetChildrenInDirectory([string]$folderPath, [bool] $recursive, [bool] $exceptFolder)
    {
        $children = @()

        if ([Directory]::Exists( $folderPath ) -eq $false)
        {
            [Logger]::WriteLineNotice($folderPath + " 경로가 존재하지 않습니다.")
            return 0
        }
        
        if ([FileUtil]::IsDirectoryPath($folderPath) -eq $false)
        {
            [Logger]::WriteLineError($folderPath + " 는 폴더경로가 아닙니다")
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
                          폴더안에 파일 갯수 또는 폴더포함 파일 갯수 가져오기
     recursive - 폴더안의 파일도 검사할건지
     exceptFolder - 폴더는 갯수에서 제외할건지
    ##########################################################################>
    [int] static GetChildrenCountInDirectory([string]$folderPath, [bool] $recursive, [bool] $exceptFolder)
    {
        if ([Directory]::Exists( $folderPath ) -eq $false)
        {
            [Logger]::WriteLineNotice($folderPath + " 경로가 존재하지 않습니다.")
            return 0
        }
        
        if ([FileUtil]::IsDirectoryPath($folderPath) -eq $false)
        {
            [Logger]::WriteLineError($folderPath + " 는 폴더경로가 아닙니다")
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
                            파일 또는 폴더 삭제
    ##########################################################################>
    [void] static RemoveFileOrDirectory([string] $item)
    {

        if( (Test-Path $item) -eq $true )
        {
            Remove-Item $item -Recurse -Force
            [Logger]::WriteLineNotice($item + "항목을 삭제했습니다.")
        }
    }

    <#########################################################################
                            XML 파일 읽기
    ##########################################################################>
    [xml] static ReadXmlFile([string]$xmlFilePath)
    {
        
        if( (Test-Path $xmlFilePath) -eq $FALSE )
        {
           [Logger]::WriteLineErrorCovered([string]::Format("{0} 파일이 존재하지 않습니다. 파일을 생성하고 설정값을 넣어주세요.", $xmlFilePath)) 
            exit -1
        }

        [xml]$xmlData = Get-content $xmlFilePath

        if( ($xmlData -eq $NULL) )
        {
            [Logger]::WriteLineErrorCovered([string]::Format("{0} 파일의 설정값을 확인하세요.", $xmlFilePath)) 
            exit -1
        }

        return $xmlData
    }

    <#########################################################################
                            XML 파일 저장
    ##########################################################################>
    [void] static SaveXmlFile([xml] $xmlData, [string] $filePath)
    {
        [string]$directoryName = [Path]::GetDirectoryName($filePath)
        if ([Directory]::Exists( $directoryName ) -eq $false)
        {
            [Logger]::WriteLineNotice($directoryName + " 경로가 존재하지 않아서 생성했습니다.")
            New-Item $directoryName -Type Directory
        }

        # Patch Version 설정값 저장
        $xmlData.save($filePath)
        [Logger]::WriteLineNotice($filePath + " 데이터를 저장하였습니다")
    }
    <#########################################################################
                            7z 파일로 압축하기
    ##########################################################################>
    [void] static MakeDirectoryTo7zFile( [string] $srcDirectory, [string]$saveFilePath)
    {
        & 7z a $saveFilePath $srcDirectory | Out-Host
        [Logger]::WriteLineNotice( $srcDirectory + " 경로를 " + [Path]::GetFileName($saveFilePath) + " 이름으로 압축하였습니다.")
    }

    <#########################################################################
                              파일 복사
    ##########################################################################>
    [void] static FileCopy( [string] $srcPath, [string] $dstPath )
    {
        $srcDirectoryPath = [Path]::GetDirectoryName($srcPath)
        if ([Directory]::Exists($srcDirectoryPath) -eq $false)
        {
            [Logger]::WriteLineErrorCovered($srcDirectoryPath + " 경로가 존재하지 않습니다. 복사에 실패했습니다.", "")
            return
        }

        if( (Test-Path $srcPath) -eq $FALSE )
        {[Logger]::WriteLineErrorCovered($srcPath + "파일이 존재하지 않습니다. 복사에 실패했습니다.", "")
            return
        }

        $dstDirectoryPath = [Path]::GetDirectoryName($dstPath)
        if( [Directory]::Exists($dstDirectoryPath) -eq $false )
        {
            [Logger]::WriteLineNotice($dstDirectoryPath + " 경로가 존재하지 않아서 생성했습니다.")
            New-Item $dstDirectoryPath -Type Directory
        }
        
        Copy-Item -Path $srcPath -Destination $dstPath -Recurse -Force
        [Logger]::WriteLineNotice($dstPath + "로 파일 복사가 성공하였습니다.")
    }

    <#########################################################################
                              INI 파일 읽기
    ##########################################################################>
    [Array] static ReadInifile([string] $iniFilePath)
    {
        $anonymous = "NoSection"

        $ini = @{}  
        $section = $null
        $CommentCount = 0


        switch -regex -file $iniFilePath  
        {  
            "^\[(.+)\]$" # 구분 파싱
            {  
                $section = $matches[1]  
                $ini[$section] = @{}  
                $CommentCount = 0  
            }  

            "^(#.*)$" # 주석 파싱
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

            "(.+?)\s*=\s*(.*)" # 키값 파싱  
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
                              파일 저장
    ##########################################################################>
    [void] static Savefile([string] $path, [string] $content)
    {
        if ( (Test-Path $path) -and [FileUtil]::IsDirectoryPath($path) -eq $true)
        {
            [Logger]::WriteLineNotice([string]::Format("입력한 {0} 경로는 폴더 경로입니다. 올바른 파일명을 입력해주세요", $path))
            exit -1
        }

        ([Directory]::Exists( [Path]::GetDirectoryName($path)) -eq $false)
        {
            [Directory]::CreateDirectory([Path]::GetDirectoryName($path))
            [Logger]::WriteLineNotice( "해당 경로가 존재하지 않아서 생성했습니다.")
        }

        [File]::WriteAllText($path, $content)
    }

    <#########################################################################
                      입력한 경로가 파일인지 폴더인지 확인
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
                              바이너리 파일인지
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
                          파일 경로를 리눅스 스타일로 변환

    윈도우상에 우분투 경로임
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
     Json 파일 읽기 (어셈블리 참조 후 사용해야함)
    ##########################################################################>
#    [JObject] static ReadJsonfile([string] $jsonfilepath)
#    {
#        if ((Test-Path $jsonfilepath) -eq $false)
#        {
#            [Logger]::WriteLineNoticeError($jsonfilepath + " 파일이 존재하지 않습니다", "")
#            exit -1
#        }

#        return [JObject]::Parse($jsonfilepath)
#    }
}



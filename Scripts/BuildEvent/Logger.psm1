##작성자 : 윤정도
##작성일 : 20-02-25

class Logger
{
    static [bool]$NoLog = $false

    [void] static WriteLineNotice([string]$message)
    {
        if ([Logger]::NoLog -eq $false)
        {
            Write-Host "[알림]" $message 
        }
    }

    [void] static WriteLineNoticeCovered([string]$message)
    {
        if ([Logger]::NoLog -eq $false)
        {
            Write-Host "[알림] ====================================================================================" 
            Write-Host "[알림]" $message 
            Write-Host "[알림] ====================================================================================" 
            Write-Host ""
        }
    }

    [void] static WriteLineError([string]$message, [string]$content)
    {
        if ([Logger]::NoLog -eq $false)
        {
            Write-Host "[오류]" $message 
            if ($content -ne [String]::Empty)
            {
                Write-Host "[오류]" $content
            }
        }
    }

    [void] static WriteLineErrorCovered([string]$message, [string]$content)
    {
        if ([Logger]::NoLog -eq $false)
        {
            Write-Host "[오류] ====================================================================================" 
            Write-Host "[오류]" $message
            if ($content -ne [String]::Empty)
            {
                Write-Host "[오류]" $content
            }
            Write-Host "[오류] ====================================================================================" 
            Write-Host ""
        }
    }
}
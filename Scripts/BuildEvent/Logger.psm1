##�ۼ��� : ������
##�ۼ��� : 20-02-25

class Logger
{
    static [bool]$NoLog = $false

    [void] static WriteLineNotice([string]$message)
    {
        if ([Logger]::NoLog -eq $false)
        {
            Write-Host "[�˸�]" $message 
        }
    }

    [void] static WriteLineNoticeCovered([string]$message)
    {
        if ([Logger]::NoLog -eq $false)
        {
            Write-Host "[�˸�] ====================================================================================" 
            Write-Host "[�˸�]" $message 
            Write-Host "[�˸�] ====================================================================================" 
            Write-Host ""
        }
    }

    [void] static WriteLineError([string]$message, [string]$content)
    {
        if ([Logger]::NoLog -eq $false)
        {
            Write-Host "[����]" $message 
            if ($content -ne [String]::Empty)
            {
                Write-Host "[����]" $content
            }
        }
    }

    [void] static WriteLineErrorCovered([string]$message, [string]$content)
    {
        if ([Logger]::NoLog -eq $false)
        {
            Write-Host "[����] ====================================================================================" 
            Write-Host "[����]" $message
            if ($content -ne [String]::Empty)
            {
                Write-Host "[����]" $content
            }
            Write-Host "[����] ====================================================================================" 
            Write-Host ""
        }
    }
}
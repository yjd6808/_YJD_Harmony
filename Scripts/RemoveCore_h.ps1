# 해당 프로젝트 cpp 파일들의 #include "Core.h"와 #include "$(ProjectName)/Core.h" 문자열을 제거한다.
# ---------------------------------------------------------------------
# $ProjectNames = @('sgs_auth')

$ProjectNames = @('sgs_auth', 'sgs_center', 'sgs_lobby')

# $ProjectNames = @(
#     'jc_gbench', 
#     'jc_gtest', 
#     'jdb', 
#     'jnet', 
#     'jnet_gbench', 
#     'jnet_gtest',
#     'jnetr_common')

$root = (Get-Location).Path

foreach ($project in $ProjectNames)
{
    $projectPath = Join-Path $root "../Projects/$project/Sources/$project"

    $cppFiles = Get-ChildItem -Path $projectPath -Recurse -Filter *.cpp

    $includeLine1 = "#include `"$project/Core.h`""
    $includeLine2 = "#include `"Core.h`""

    foreach ($file in $cppFiles)
    {
        $content = Get-Content $file.FullName

        $newContent = @()
        $removed = $false

        foreach ($line in $content)
        {
            if (-not $removed)
            {
                $trim = $line.Trim()

                if ($trim -eq $includeLine1 -or $trim -eq $includeLine2)
                {
                    # 👉 최초 1회만 삭제
                    $removed = $true
                    continue
                }
            }

            $newContent += $line
        }

        # 변경된 경우만 저장 (선택사항)
        if ($removed)
        {
            Set-Content -Path $file.FullName -Value $newContent -Encoding UTF8
        }
    }
}
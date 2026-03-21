$ProjectNames = @('sg')

foreach ($project in $ProjectNames)
{
    # 프로젝트 루트 경로 (필요하면 수정)
    $projectPath = Join-Path -Path (Get-Location) -ChildPath $project

    # 모든 cpp 파일 탐색
    $cppFiles = Get-ChildItem -Path $projectPath -Recurse -Filter *.cpp

    foreach ($file in $cppFiles)
    {
        $content = Get-Content $file.FullName

        # 이미 include 되어 있으면 skip
        $includeLine = "#include `"$project/Core.h`""
        if ($content -contains $includeLine)
        {
            continue
        }

        $newContent = @()
        $inserted = $false

        for ($i = 0; $i -lt $content.Length; $i++)
        {
            $line = $content[$i]

            # 첫 #include 발견 시 그 위에 삽입
            if (-not $inserted -and $line -match '^\s*#include')
            {
                $newContent += $includeLine
                $inserted = $true
            }

            $newContent += $line
        }

        # include가 하나도 없는 경우 맨 위에 추가
        if (-not $inserted)
        {
            $newContent = @($includeLine) + $content
        }

        # 파일 덮어쓰기
        Set-Content -Path $file.FullName -Value $newContent -Encoding UTF8
    }
}
$directory = Resolve-Path -Path "$PSScriptRoot/../../Projects" 
$extensions = "*.cpp;*.h;*.inl;*.asm;*.xaml;*.cs;"
$extensionArray = $extensions -split ';'| Where-Object { $_ -ne "" }
$excludedPatterns = @("AssemblyAttributes.cs", "g.cs", "g.i.cs", "AssemblyInfo.cs")
$totalCount = 0
$filesWithExtension = @{}

foreach ($extension in $extensionArray) {
    $files = Get-ChildItem -Path $directory -Recurse -Filter $extension -File |
             Sort-Object FullName |
             Select-Object -ExpandProperty FullName
    $newFiles = @()
	foreach ($file in $files) {
		$exclude = $false
		foreach ($pattern in $excludedPatterns) {
			if ($file.EndsWith($pattern)) {
				$exclude = $true
				break
			}
		}

		if (-not $exclude) {
			$newFiles += $file
		}
	}
    $files = $newFiles
	$count = $files.Count 
    $totalCount += $count
	$filesWithExtension[$extension] = $files
    Write-Output "$extension : $count"
}

Write-Output "=============================="
Write-Output "total : $totalCount"
Write-Output ""
Write-Output ""

$sortedExtensions = $filesWithExtension.GetEnumerator() | Sort-Object { $_.Value.Count }

foreach ($extension in $sortedExtensions) {
    $key = $extension.Key
    Write-Output "$key"
    foreach ($file in $filesWithExtension[$key]) {
        Write-Output "  - $file"
    }
}
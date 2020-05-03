Get-FileHash "D:\Code\dazaamb16e0 rev e.bin" -Algorithm MD5
$port = new-Object System.IO.Ports.SerialPort COM3,115200,None,8,one #Even
$port.Open()
$HOST.UI.RawUI.Flushinputbuffer()
Write-Host "t - test" -ForegroundColor "Green"
Write-Host "q - quit" -ForegroundColor "Green"
Write-Host "i - info" -ForegroundColor "Green"
Write-Host "r - read" -ForegroundColor "Green"
Write-Host "e - erase" -ForegroundColor "Green"
Write-Host "w - write" -ForegroundColor "Green"
Write-Host "v - test write" -ForegroundColor "Green"
Write-Host "c - read 2048 page" -ForegroundColor "Green"
Start-Sleep 2
$ps = 256
while ($port.IsOpen) {
    $port.ReadExisting() | Out-Null
    do {
        if ($host.ui.RawUi.KeyAvailable) {
            $key = $host.ui.RawUi.ReadKey("NoEcho,IncludeKeyUp,IncludeKeyDown")
            $HOST.UI.RawUI.Flushinputbuffer()
            $port.Write($key.Character)
            switch ($key.Character) {
                't' {
                    $np = 8
                    $bs = $ps*$np
                    $bytes  = [System.IO.File]::ReadAllBytes("D:\Code\dazaamb16e0 rev e.bin")
                    $bytes.Count
                    "First 400th"
                    $bytes[0..$($bs-1)] | ForEach-Object -Process { "{0:X} " -f $_ | Write-Host -NoNewline }
                    "`n"
                    "Last 400th"
                    $bytes[$(-$bs)..-1] | ForEach-Object -Process { "{0:X} " -f $_ | Write-Host -NoNewline }
                    "`nWrite {0} bites" -f $bs
                    $port.Write($bytes, 0, $bs)
                    while ($port.BytesToRead -eq 0) { }
                    $port.ReadLine()
                    #Start-Sleep 10
                    $buff = [System.Byte[]]::new($bs)
                    while ($port.BytesToRead -lt $bs) {
                        Start-Sleep -Milliseconds 5
                        "{0} " -f $port.BytesToRead | Write-Host -NoNewline
                    }
                    "`n"
                    $port.Read($buff, 0, $bs)
                    $buff | ForEach-Object -Process { "{0:X} " -f $_ | Write-Host -NoNewline }
                    "`n"
                    #Compare-Object -ReferenceObject $bytes[0..$($bs-1)] -DifferenceObject $buff[0..$($bs-1)]
                    break              
                }
                'q' { 
                    $port.Close()
                    exit 
                }
                'i' {
                    $port.ReadLine()
                    $port.ReadLine()
                    $port.ReadLine()
                    break
                }
                'e' {
                    while ($port.BytesToRead -eq 0) { }
                    $port.ReadLine()
                }
                'v' {
                    $np = 4
                    $bs = $ps*$np
                    $maxP = $port.ReadLine()
                    $maxPage = [Convert]::ToUInt32($maxP)
                    "Max Page: {0}" -f $maxPage 
                    $cap = $port.ReadLine()
                    $capacity = [Convert]::ToUInt32($cap)
                    "Capasity: {0}" -f $capacity
                    $maxPage = $maxPage/$np
                    "{0} bytes Max Page {1}" -f $bs, $maxPage
                    $bytes  = [System.IO.File]::ReadAllBytes("D:\Code\dazaamb16e0 rev e.bin")
                    $i = Read-Host "Page"
                    $port.WriteLine($i)
                    while ($port.BytesToRead -eq 0) { }
                    $port.ReadLine()
                    [int32]$ii = $i
                    [int32]$addr = $ii*$bs
                    Write-Host $addr, $bs -Separator " "
                    $port.Write($bytes, $addr, $bs)
                    $port.ReadLine()
                    $port.ReadLine()
                    $port.ReadLine()
                }
                'w' {
                    $np = 1
                    $bs = $ps*$np
                    $maxP = $port.ReadLine()
                    $maxPage = [Convert]::ToUInt32($maxP)
                    "Max Page: {0}" -f $maxPage 
                    $cap = $port.ReadLine()
                    $capacity = [Convert]::ToUInt32($cap)
                    "Capasity: {0}" -f $capacity
                    $maxPage = $maxPage/$np
                    "{0} bytes Max Page {1}" -f $bs, $maxPage

                    Add-Type -AssemblyName System.Windows.Forms
                    $f = new-object Windows.Forms.OpenFileDialog
                    $f.InitialDirectory = "D:\Code\"
                    $f.Filter = "Bin Files (*.bin)|*.bin"
                    $f.ShowHelp = $true
                    $f.Multiselect = $false
                    $f.ShowDialog() |Out-Null
                    if (-Not $f.FileName) {
                        "File not chosen"
                        return
                    }
                    $f.FileName
                    
                    $bytes  = [System.IO.File]::ReadAllBytes($f.FileName)
                    Get-Date -Format "HH:mm dd/MM/yyyy"
                    $watch = [System.Diagnostics.Stopwatch]::StartNew()
                    $watch.Start()
                    for ($i = 0; $i -lt $maxPage; $i++) {
                        [int32]$addr = $i*$bs
                        $port.Write($bytes, $addr, $bs)
                        if (($i -ge $ps) -and ($($i%$ps)-eq 0)) {
                            "{0} " -f $i | Write-Host -NoNewline
                        }
                        if ($port.BytesToRead -ne 0) {
                            $port.ReadLine() 
                            break
                        }
                    }
                    while ($port.BytesToRead -eq 0) { }
                    $port.ReadLine()    
                    $watch.Stop()
                    "Done. Write time: {0}" -f $watch.Elapsed
                    break
                }
                'c' {
                    $addr = Read-Host "Page (0-4046)"
                    $port.WriteLine($addr)
                    while ($port.BytesToRead -eq 0) { }
                    $port.ReadLine()
                    $np = 8
                    $bs = $ps*$np
                    $buff = [System.Byte[]]::new($bs)
                    $buff1 = [System.Byte[]]::new($bs)
                    while ($port.BytesToRead -lt $bs) { }
                    $port.Read($buff, 0, $bs)
                    while ($port.BytesToRead -lt $bs) { }
                    $port.Read($buff1, 0, $bs)
                    $buff | ForEach-Object -Process { "{0:X} " -f $_ | Write-Host -NoNewline }
                    "`n"
                    $buff1 | ForEach-Object -Process { "{0:X} " -f $_ | Write-Host -NoNewline }
                    "`n"
                    break
                }
                'r' {
                    $tmot = 450
                    $np = 16
                    $bs = $ps*$np
                    $maxP = $port.ReadLine()
                    $maxPage = [Convert]::ToUInt32($maxP)
                    "Max Page: {0}" -f $maxPage 
                    $cap = $port.ReadLine()
                    $capacity = [Convert]::ToUInt32($cap)
                    "Capasity: {0}" -f $capacity
                    $maxPage = $maxPage/$np
                    "{0} bytes Max Page {1}" -f $bs, $maxPage
                    $bff = [System.Byte[]]::new($bs)
                    Get-Date -Format "HH:mm dd/MM/yyyy"
                    $dt = Get-Date -Format "_MMdd_HHmm"
                    $path = "D:\Code\temp" + $dt + ".bin"
                    New-Item -Path $path
                    $watch = [System.Diagnostics.Stopwatch]::StartNew()
                    $watch.Start()
                    for ($i = 0; $i -lt $maxPage; $i++) {
                        while ($port.BytesToRead -lt $bs) {
                        }
                        $rl = $port.Read($bff, 0, $bs)
                        Add-Content -Path $path -Value $bff -Encoding Byte
                        if (($i -ge $ps) -and ($($i%$ps)-eq 0)) {
                            "{0} " -f $i | Write-Host -NoNewline
                        }
                    }
                    $watch.Stop()
                    "`nDone. Read time: {0}" -f $watch.Elapsed
                    break
                }
                default {"Invalid key"}
            } 
            $port.ReadLine()
            $port.ReadExisting()
        }
        Start-Sleep 2
    } until ($false)
}
"Port closed"
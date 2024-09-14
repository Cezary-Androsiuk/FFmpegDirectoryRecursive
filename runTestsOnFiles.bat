@echo off




IF EXIST ".\tests\test-test" (
    rd /S /Q .\tests\test-test

    xcopy .\tests\backupenv\test\ .\tests\test-test\ /E /H /Q
    
) ELSE (
    xcopy .\tests\backupenv\test\ .\tests\test-test\ /E /H /Q
)




IF EXIST ".\tests\test-skip" (
    rd /S /Q .\tests\test-skip

    xcopy .\tests\backupenv\test\ .\tests\test-skip\ /E /H /Q
    
    IF EXIST ".\tests\test-skip-ffmpeg-h.265" (
        rd /S /Q .\tests\test-skip-ffmpeg-h.265
    )
    
    IF EXIST ".\tests\test-skip-finished_source_files" (
        rd /S /Q .\tests\test-skip-finished_source_files
    )
) ELSE (
    xcopy .\tests\backupenv\test\ .\tests\test-skip\ /E /H /Q
)




IF EXIST ".\tests\test-copy" (
    rd /S /Q .\tests\test-copy

    xcopy .\tests\backupenv\test\ .\tests\test-copy\ /E /H /Q
    
    IF EXIST ".\tests\test-copy-ffmpeg-h.265" (
        rd /S /Q .\tests\test-copy-ffmpeg-h.265
    )
    
    IF EXIST ".\tests\test-copy-finished_source_files" (
        rd /S /Q .\tests\test-copy-finished_source_files
    )
) ELSE (
    xcopy .\tests\backupenv\test\ .\tests\test-copy\ /E /H /Q
)




IF EXIST ".\tests\test-move" (
    rd /S /Q .\tests\test-move

    xcopy .\tests\backupenv\test\ .\tests\test-move\ /E /H /Q
    
    IF EXIST ".\tests\test-move-ffmpeg-h.265" (
        rd /S /Q .\tests\test-move-ffmpeg-h.265
    )
    
    IF EXIST ".\tests\test-move-finished_source_files" (
        rd /S /Q .\tests\test-move-finished_source_files
    )
) ELSE (
    xcopy .\tests\backupenv\test\ .\tests\test-move\ /E /H /Q
)




IF EXIST ".\tests\test-force" (
    rd /S /Q .\tests\test-force

    xcopy .\tests\backupenv\test\ .\tests\test-force\ /E /H /Q
    
    IF EXIST ".\tests\test-force-ffmpeg-h.265" (
        rd /S /Q .\tests\test-force-ffmpeg-h.265
    )
    
    IF EXIST ".\tests\test-force-finished_source_files" (
        rd /S /Q .\tests\test-force-finished_source_files
    )
) ELSE (
    xcopy .\tests\backupenv\test\ .\tests\test-force\ /E /H /Q
)


for /f "usebackq" %%i in (`powershell -command "Get-Date -Format 'yyyy-MM-dd_HH-mm-ss-fff'"`) do set timestamp=%%i

echo -----------------
echo -----------------
echo ALL OUTPUT IS IN '%timestamp%-[...].txt' FILES!
echo -----------------
echo -----------------


.\bin\ffmpegRecTest .\tests\test-test avi+mkv+mp4+ test > .\tests\%timestamp%-test.txt 2>&1
echo test test finished

.\bin\ffmpegRecTest .\tests\test-skip avi+mkv+mp4+ skip > .\tests\%timestamp%-skip.txt 2>&1
echo test skip finished

.\bin\ffmpegRecTest .\tests\test-copy avi+mkv+mp4+ copy > .\tests\%timestamp%-copy.txt 2>&1
echo test copy finished

.\bin\ffmpegRecTest .\tests\test-move avi+mkv+mp4+ move > .\tests\%timestamp%-move.txt 2>&1
echo test move finished

.\bin\ffmpegRecTest .\tests\test-force avi+mkv+mp4+ force > .\tests\%timestamp%-force.txt 2>&1
echo test force finished


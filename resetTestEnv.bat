@echo off
IF EXIST ".\tests\test" (
    rd /S /Q .\tests\test

    xcopy .\tests\backupenv\test\ .\tests\test\ /E /H /Q
    
    IF EXIST ".\tests\test-ffmpeg-h.265" (
        rd /S /Q .\tests\test-ffmpeg-h.265
    )
    
    IF EXIST ".\tests\test-finished_source_files" (
        rd /S /Q .\tests\test-finished_source_files
    )
) ELSE (
    xcopy .\tests\backupenv\test\ .\tests\test\ /E /H /Q
)
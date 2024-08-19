# FFmpegDirectoryRecursive

## About
It is a twin project to [FFmpegDirectory](https://github.com/Cezary-Androsiuk/FFmpegDirectory), but upgraded to handle recursive directories.
Installation instructions are in ```main.cpp```, but it requires FFmpeg to be already installed. 
Check out [FFmpegDirectory](https://github.com/Cezary-Androsiuk/FFmpegDirectory) for more screenshots!

Unlike FFmpegDirectory, this algorithm creates a parallel directory that will contain an identical structure to the one specified by the user but with compressed recordings.
I chose a fixed number of arguments (instead of variable and more flexible) to force the user to actively decide what to do with files that already have H.265 encryption and what file extensions to consider.

## Input Files:
![inFiles](https://github.com/Cezary-Androsiuk/FFmpegDirectoryRecursive/blob/master/images/inFiles.png "inFiles") 

## Command Prompt Output
![display1](https://github.com/Cezary-Androsiuk/FFmpegDirectoryRecursive/blob/master/images/display1.png "display1") 

## Output Files:
![outFiles](https://github.com/Cezary-Androsiuk/FFmpegDirectoryRecursive/blob/master/images/outFiles.png "outFiles")


## Invalid arguments
You can also change the value of the definition of ```FLEXIBLE_ARGUMENTS``` to ```true``` to get a floating number of arguments. In that case, the default values will be used.
![display2](https://github.com/Cezary-Androsiuk/FFmpegDirectoryRecursive/blob/master/images/display2.png "display2") 

## Catalog Occupied
![display3](https://github.com/Cezary-Androsiuk/FFmpegDirectoryRecursive/blob/master/images/display3.png "display3") 

The algorithm is armed with many more tools to protect and facilitate file compression. As soon as I find time to do so, I will describe the operation of both algorithms.
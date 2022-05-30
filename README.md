# PacViewer
 Advanced tool for viewing and editing Patapon PAC files.
 
 To make it work properly after compilation, place all files from /res/ folder to the place of compiled .exe file. /res/ folder contains the necessary instruction set for the tool and guide on how to write your own instructions to the set. (+ a font file to make it show text)
 
 Made by Owocek, thanks to Madwig and Shockturtle for helping with PAC
 
### Required Libraries and Tools

 * [CMake](https://cmake.org/download/) >= 3.19
 * [SFML](https://www.sfml-dev.org/download.php) >= 2.5
 * [Git](https://git-scm.com/downloads)

### Note for Linux users

Idk if it works for linux feel free to test it out

### Note for Windows users

You should enable [Developer Mode](https://docs.microsoft.com/en-us/windows/apps/get-started/enable-your-device-for-development) in order to successfully configure the project.

Please also make sure that the environment variable `SFML_ROOT` points to a valid SFML installation and that the `/bin` folder of the SFML installation is also added to `PATH`.

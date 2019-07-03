# Build Instructions

## Windows
These setup instructions are adapted from [this article](https://code.visualstudio.com/docs/cpp/config-mingw). Several of the steps should be preconfigured.  
### Installing mingw-w64
Download the mingw-w64 installer from [here](https://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/installer/mingw-w64-install.exe/download).  
On the first page of the installer, choose the following settings:  
![](https://github.com/wormyrocks/jcl2.0/blob/master/docs/setup1.png)  
On the second page, make sure to change the install directory to `C:/` rather than `C:/Program Files`.  
![](https://github.com/wormyrocks/jcl2.0/blob/master/docs/setup2.png)  
Proceed with the install.  

### Installing git bash
If you haven't installed git bash yet, [do so](https://gitforwindows.org). The VSCode build environment is configured to use it as a default shell.  

### Building in WSL
Run `source wsl_env_setup.sh`, then type 'make' as expected. This will alias 'make' to a gitbash subshell that runs mingw32-make. 

## Mac
No setup required. :p

## Linux
Haven't gotten around to setting up the makefile but it should just work if you wanna do it for me.

## Build and Run
### Required Extensions
Install the [C/C++ extensions for VSCode](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools) and open the repository.  

### Terminal (recommended)
Open Git Bash terminal with ``Ctrl+` ``.  
Type `make` or `make clean` to compile.  
Type `./run` or `run.exe` to run.

### Keyboard Shortcuts (not recommended)
Hit `F5` to build and debug.  
Hit `Shift+F5` to end debugging.  
Hit `Ctrl+F5` to build and run.  
Hit `Ctrl+Shift+B` to build without running.  

If editing a Markdown file: hit `Ctrl+Shift+V` to open a live preview.  

## Optional Stuff
### VSCode Extensions
I am using [Todo Tree](https://marketplace.visualstudio.com/items?itemName=Gruntfuggly.todo-tree) to keep track of smallish incomplete tasks - it keeps track of "TODO" in comments and formats them as a list.

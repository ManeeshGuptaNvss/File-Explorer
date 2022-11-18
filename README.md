# Linux based File Explorer
A file explorer for linux systems that runs on terminal.

This application will work in two modes :

1. `Normal Mode` : used to explore the current directory and navigate the filesystem.

2. `Command Mode` : used to enter and execute shell commands.

The root of the application is the directory where the application will start.

## Prerequisites

**1. Platform:** Linux 

**2. Software Requirement:**
* G++ compiler
   * **To install G++ :** ```sudo apt-get install g++```
## How to execute

Open terminal and enter following commands :

1. For Compilation : ```g++ main.cpp -o main```
2. For Execution : ```./main```

## How to switch between Two Modes

Press `:` to switch from Normal Mode to Command Mode. Press `Esc` to switch from Command Mode to Normal Mode

## Command Support

1. `Copy` - copy <source_file(s)> <destination_directory>

2. `Move` - move <source_file(s)> <destination_directory>

3. `Rename` - rename <old_filename> <new_filename>

4. `Create File` - create_file <file_name> <destination_path>

5. `Create Directory` - create_dir <dir_name> <destination_path>

6. `Delete File` - delete_file <file_path>

7. `Delete Directory` - delete_dir <dir_path>

8. `Goto` - goto <location>

9. `Search` - search <file_name> or search <directory_name>


## Assumptions
* Default mode is Normal mode and press **:** to switch to Command mode.
* Press **ESC** key to go back to normal mode from command mode.
* Press **q** key from normal mode to exit from program.
* In Normal Mode, assumed the max window size as 10.
* In Command mode, to edit the command use backspace only.
* For files/directories which contain space in name, backslash (\\) is used as delimeter. 
   * For Example :- If file name is "os assignment" (without double quote) then user should enter "os\ assignment" (without double quote).
* Preferable to open terminal in full size. if one record spans over more than one line then navigation won't work correctly.
* if **Enter** is pressed on some file and if there is no default application found then there will be no movement.

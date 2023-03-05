# TBScroll

### What's this?
TBScroll is an alternative scrolling solution for pointing devices without a dedicated scroll input.  
It allows you to scroll by clicking and holding the "middle" button and moving the cursor along the X or Y axis.  
If you click without moving the cursor, the default middle click action is performed instead.

### Features
- Wheel-like scrolling in all applications
- Customizable sensitivity options
- "Natural" scrolling (reverse scroll direction)
- Minimal CPU and memory usage
- No external libraries, dependencies etc. required

### Usage
Download and run TBScrl32.exe. The program starts silently and creates a system tray icon.  
To open the settings menu, right-click the icon.

### Known bugs
- Glitchy behavior in games/programs using raw mouse input

### To-do
- Customizable click action
- Quick disable hotkey/modifier

### Download
Latest release: [EXE](https://github.com/spitfirex86/TBScroll/releases/latest/download/TBScrl32.exe)
/ [noadmin EXE](https://github.com/spitfirex86/TBScroll/releases/latest/download/TBScrl32-noadmin.exe)

###### What's the difference?
By default, TBScroll requires admin permissions to simulate scroll input in some windows (like the Task Manager) or other programs running with admin privileges.
You can use the "noadmin" version if you do not have access to an administrator account. However, keep in mind it may not work in some applications.

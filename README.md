# Digital Clock System

## Project Overview
This project implements a console-based digital clock system written in C.
It integrates multiple functional modules to provide a complete clock application,
including real-time display, alarm, timer, UI settings, and time zone adjustment.

The project emphasizes system integration, modular design, and real-time logic handling.

## Features
- Real-time clock and date display
- Alarm with trigger detection and sound notification
- Countdown timer functionality
- Time zone adjustment and manual time setting
- UI settings (12/24-hour format, language, theme)
- Modular architecture with separated components

## Project Structure
- Alarm.wav
- Clock/
- main.c
- main.h
- timer_alarm.c
- timer_alarm.h
- ui_profile.c
- ui_profile.h
- utc_adjust.h

### Alarm.wav
Sound file used for alarm and timer notifications.

### Clock/
Contains project resources and additional files.

### main.c
Main program responsible for overall control flow, UI rendering,
time and date display, and integration of all modules.

### main.h
Defines shared global variables, structures, and common configurations.

### timer_alarm.c / timer_alarm.h
Handles alarm and countdown timer functionalities, including time comparison,
triggering logic, and display control.

### ui_profile.c / ui_profile.h
- Time format (12/24-hour)  
- Language selection  
- Theme (Dark: black background with white text / Light: white background with black text)

### utc_adjust.h
Handles time zone adjustment and manual time configuration.

## How to Run (Windows GCC)
1. Open a command prompt and navigate to the project directory.
2. Compile the program using:
       gcc main.c timer_alarm.c ui_profile.c -o clock -lwinmm -limm32
3. Run the executable:
       ./clock.exe
4. Make sure Alarm.wav is in the same directory, otherwise the sound will not play.
Note: The sound functionality is currently only supported on Windows.

## Author
Yi-An, Yang

## Contributions

- **Integration & Main Program (main.c)**  
  I implemented the main program flow, including:
  - Panel/UI integration
  - Alarm and timer functionalities
  - Manual time and timezone handling (integration only)

- **Provided Modules (developed by other team members)**  
  - UI settings (ui_profile.c / ui_profile.h) – handles time format, language, and theme  
  - Timezone adjustments (utc_adjust.h) – handles system timezone conversion and manual time setting

## Acknowledgements
- UI assets (clock graphics) were not created by the author
- Some modules (such as UI settings and time zone adjustment) were provided or pre-developed

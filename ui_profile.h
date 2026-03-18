#ifndef TIME_SYSTEM_H
#define TIME_SYSTEM_H

#include <windows.h>

// ===============================
// 時間模式 12/24
// ===============================
#define MODE_12 12
#define MODE_24 24
extern int mode12_24;
extern int confirmStatus;
int getTimeMode(void);
void toggleTimeMode(void);
void setTimeMode(int mode);
int convertHour(int hour24, const char** ampm);

// ===============================
// 語言
// ===============================
extern int language;   // 0=中文,1=English
extern int tempLang;   // 暫存

extern const char* TXT_DATE[2];
extern const char* TXT_WEEK[2][7];
extern const char* TXT_12H[2];
extern const char* TXT_24H[2];
extern const char* TXT_DA_HINT[2];

extern const char* BTN_THEME[2];
extern const char* BTN_12_24[2];
extern const char* BTN_TIMER[2];
extern const char* BTN_ALARM[2];
extern const char* BTN_LANGUAGE[2];
extern const char* BTN_TIMEZONE[2];
extern const char* BTN_MANUAL_TIME[2];

void handleLanguageInput(char c);
const char* getDaySuffix(int day);

// ===============================
// 背景模式
// ===============================
extern int bgMode; // 0=深色, 1=淺色
extern int BG_X;
extern int BG_Y;

void BG_SetColor();
void BG_ClearScreen();
void BG_Select();

// ===============================
// Console 工具
// ===============================
void gotoxy(int x,int y);
void setColor(int c);

#endif

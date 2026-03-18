#pragma once

#include <windows.h>


extern const char *NUM[10][5]; // 數字陣列 
extern COORD time_pos[3]; // 時鐘陣列 

// ===== Console context =====
typedef struct {
    HANDLE hOut;                     // Console handle
    int width;                       // Console 寬度
    int height;                      // Console 高度
    WORD Attr;                   // 保存背景及文字顏色
    CONSOLE_CURSOR_INFO cursorInfo;  // 保存原本游標資訊
} ConsoleCtx;

// 全域 context
extern ConsoleCtx gConsole;

// 初始化函式
void init_console_context(void);

// 啟用 12 小時制 
extern int isAmPm; 

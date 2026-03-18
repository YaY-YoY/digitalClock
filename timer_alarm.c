#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>
#include <mmsystem.h> // 鈴聲 
#include "timer_alarm.h"
#include "main.h"
 
// 跟 main 對齊 
#define button_center 18
#define BTN_H 3
#define center_width (gConsole.width - 2 * button_center)

// 畫數字 
void draw_num(COORD pos, int num, int isHour) {
    if(isAmPm && isHour) {
        if(num == 0) num = 12;        // 0點 → 12AM
        else if(num > 12) num -= 12;  // 13~23 → 1~11PM
    }
    char buf[2];
    sprintf(buf, "%02d", num); // 轉成字串
    
    for (int r = 0; r < 5; r++) {
        int x = pos.X;
        for (int i = 0; i < 2; i++) {
            int d = buf[i]-'0';
            COORD p = {x, pos.Y + r};
            SetConsoleCursorPosition(gConsole.hOut, p);
            printf("%s", NUM[d][r]);
            x += 8;
        }
    }
    Sleep(50);
}

// 畫 AM/PM
void draw_ampm(const char* ampm) {
    COORD p = {button_center + (center_width - 60) / 2 - 6, (gConsole.height - 5) / 2 + 2}; // 右側一點偏移
    SetConsoleCursorPosition(gConsole.hOut, p);
    printf("%s", ampm);
    Sleep(50);
}

typedef struct {
    int visible;    // 用於閃爍
    COORD pos;      // 座標 
    int time;       // 暫存數值 
} FunctionBox;

int setAlarm(int hour, int min) {
    draw_num(time_pos[2], 0, 0);
    
	// 初始值設定 
    FunctionBox alarmFields[2];
    alarmFields[0].time = hour;
    alarmFields[1].time = min;
    // 跟 main 同步 
    alarmFields[0].pos = time_pos[0];
    alarmFields[1].pos = time_pos[1];
    HANDLE hConsole = gConsole.hOut;
    WORD origAttr = gConsole.Attr;
    // 跟時制同步
    int isPm = 0;
    if (isAmPm && hour >= 12 && hour != 24) isPm = 1;
    
    int i;
    for(i=0; i<2; i++) {
        alarmFields[i].visible = 1;
	    DWORD lastBlink = GetTickCount();  // 記錄前一次閃爍時間
		
        while(1) {
        	// 閃爍 
        	DWORD nowTick = GetTickCount();
            if (nowTick - lastBlink >= 300) {
                alarmFields[i].visible = !alarmFields[i].visible;
                lastBlink = nowTick;
                if(alarmFields[i].visible)
                    SetConsoleTextAttribute(hConsole, origAttr); // 正常顯示 
                else
                    SetConsoleTextAttribute(hConsole, (origAttr & 0xF0) | 8); // 灰色文字
				draw_num(alarmFields[i].pos, alarmFields[i].time, !i);
            }
            
            // 更新數值 
        	if(_kbhit()) {
        		char ch = _getch();
        		if(ch == '\033') exit(0);
                // S：當前欄位數值++
                else if(toupper(ch) == 'S') {
                	if(i == 0) { // 小時
                		alarmFields[i].time = (alarmFields[i].time + 1) % 24;
                	    if(isAmPm) {
                	        // 每次時間變成 12，就切換 AM/PM
                	        if(alarmFields[i].time == 12 || alarmFields[i].time == 0)
                	            isPm = !isPm;
                	        SetConsoleTextAttribute(hConsole, origAttr);
                            draw_ampm(isPm ? "PM" : "AM"); // 更新顯示
                        }
					}
                    else alarmFields[i].time = (alarmFields[i].time + 1) % 60; // 分鐘
                    
        			Sleep(200);
                } 
            	// D：切換到下一個欄位
                if(toupper(ch) == 'D') { 
                    alarmFields[i].visible = 1;
                    SetConsoleTextAttribute(hConsole, origAttr);
                    draw_num(alarmFields[i].pos, alarmFields[i].time, !i);
                    break;
                } 
	    	}
        }
    }
    
    Sleep(500); system("cls");
    return alarmFields[0].time * 100 + alarmFields[1].time;
}

void alarmTick(int alarmTime, int now_hour, int now_min) {
    static int ringing = 0; // 判斷是否響鈴中
    static int alarmState = 0;
    // 鬧鐘響鈴 
    if (!alarmState && alarmTime/100 == now_hour && alarmTime%100 == now_min) {
        ringing = 1;
        alarmState = 1;
		PlaySound(TEXT("Alarm.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	}
	// 關掉鬧鐘 
	if (alarmState == 1 && ringing && _kbhit()) {
		char ch = _getch();
		if (ch == '\033') exit(0);
		else if (toupper(ch) == 'D') {
	    	ringing = 0;
	    	PlaySound(NULL, 0, 0);
		}
	}
	// 天天響鈴 
    if (alarmTime%100 == now_min - 1)
        alarmState = 0;
}

void timer() {
    // 跟 main 同步
    FunctionBox timerFields[3];
    timerFields[0].pos = time_pos[0];
    timerFields[1].pos = time_pos[1];
    timerFields[2].pos = time_pos[2];
    HANDLE hConsole = gConsole.hOut;
    WORD origAttr = gConsole.Attr;
    
    int i;
    for(i=0; i<3; i++) 
        draw_num(timerFields[i].pos, 0, 0);
    if (isAmPm) draw_ampm("  ");
        
    // 設定倒數計時器 
    for(i=0; i<3; i++) {
    	// 初始值設定 
        timerFields[i].time = 0;
        timerFields[i].visible = 1;
        DWORD lastBlink = GetTickCount();   // 記錄前一次閃爍時間
        
        while(1) {
        	// 閃爍
        	DWORD nowTick = GetTickCount();
            if (nowTick - lastBlink >= 300) {
                timerFields[i].visible = !timerFields[i].visible;
                lastBlink = nowTick;
                if(timerFields[i].visible)
                    SetConsoleTextAttribute(hConsole, origAttr); // 正常顯示
                else
                    SetConsoleTextAttribute(hConsole, (origAttr & 0xF0) | 8); // 灰色文字
                draw_num(timerFields[i].pos, timerFields[i].time, 0);
            }
            
        	if(_kbhit()) {
        		char ch = _getch();
        		if(ch == '\033') exit(0);
                // S：當前欄位數值++
                if(toupper(ch) == 'S') {
                	if(i == 0) timerFields[i].time = (timerFields[i].time + 1) % 24;
                    else timerFields[i].time = (timerFields[i].time + 1) % 60;
        			Sleep(200);
                } 
            	// D：切換到下一個欄位
                if(toupper(ch) == 'D') { 
                    timerFields[i].visible = 1;
                    SetConsoleTextAttribute(hConsole, origAttr);
                    draw_num(timerFields[i].pos, timerFields[i].time, 0);
                    break;
                } 
	    	}
        }
    }
    
    // 開始倒數的畫面 
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE)); // 清空鍵盤緩衝
    int totalSec = (timerFields[0].time * 60 + timerFields[1].time) * 60 + timerFields[2].time;
    DWORD lastTick = GetTickCount();   // 記錄前一次閃爍時間
    while(totalSec) {
    	if(_kbhit()) {
        	char ch = _getch();
        	if (ch == '\033') exit(0);
        	else if(toupper(ch) == 'D') {
    	        system("cls");
	            return;
			}
		}
		
        DWORD now = GetTickCount();
        if(now - lastTick >= 1000) {
            totalSec--;
            lastTick = now;
		    draw_num(timerFields[0].pos, totalSec / 3600, 0);
		    draw_num(timerFields[1].pos, (totalSec % 3600) / 60, 0);
		    draw_num(timerFields[2].pos, totalSec % 60, 0);
        }
    	Sleep(20);
	}
           
    // 響鈴
    PlaySound(TEXT("Alarm.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
    DWORD lastBlink = GetTickCount();   // 記錄前一次閃爍時間
    int visible = 0;
    while(1) {
    	// 響鈴閃爍
        DWORD nowTick = GetTickCount();
        if (nowTick - lastBlink >= 500) {
            visible = !visible;
            lastBlink = nowTick;
            if(visible)
                SetConsoleTextAttribute(hConsole, origAttr); // 正常顯示 
            else
                SetConsoleTextAttribute(hConsole, origAttr ^ 0x0F); // 將文字顏色反轉，保留背景 
            int j;
            for(j=0; j<3; j++)
                draw_num(timerFields[j].pos, 0, 0);
        }
        if(_kbhit()) {
        	char ch = _getch();
        	if (ch == '\033') exit(0);
        	else if(toupper(ch) == 'D') {
                PlaySound(NULL, 0, 0);
    	        system("cls");
	            return;
			}
		}
	}	
}

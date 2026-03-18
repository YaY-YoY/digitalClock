#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <conio.h>
#include <ctype.h>
#include "main.h"
#include "timer_alarm.h"
#include "ui_profile.h"
#include "utc_adjust.h"
#include <imm.h>
#pragma comment(lib, "imm32.lib")

#define button_center 18
#define BTN_H 3
#define center_width (gConsole.width - 2 * button_center)

/* ===== 按鈕順序 ===== */
#define THEME 0
#define TIMEFORMAT 1
#define TIMER 2
#define ALARM 3
#define LANGUAGE 4
#define TIMEZONE 5
#define MANUAL 6

/* ===== 數字 ===== */
const char *NUM[10][5] = {
{" ◇◇◇◇◇ ","◇◇   ◇◇","◇◇   ◇◇","◇◇   ◇◇"," ◇◇◇◇◇ "},
{"   ◇◇  "," ◇◇◇◇  ","   ◇◇  ","   ◇◇  "," ◇◇◇◇◇◇"},
{" ◇◇◇◇◇ ","◇◇   ◇◇","    ◇◇ ","  ◇◇   ","◇◇◇◇◇◇◇"},
{" ◇◇◇◇◇ ","     ◇◇"," ◇◇◇◇◇ ","     ◇◇"," ◇◇◇◇◇ "},
{"◇◇   ◇◇","◇◇   ◇◇","◇◇◇◇◇◇◇","     ◇◇","     ◇◇"},
{"◇◇◇◇◇◇◇","◇◇     ","◇◇◇◇◇◇ ","     ◇◇","◇◇◇◇◇◇ "},
{" ◇◇◇◇◇ ","◇◇     ","◇◇◇◇◇◇ ","◇◇   ◇◇"," ◇◇◇◇◇ "},
{"◇◇◇◇◇◇◇","     ◇◇","    ◇◇  ","   ◇◇  ","   ◇◇  "},
{" ◇◇◇◇◇ ","◇◇   ◇◇"," ◇◇◇◇◇ ","◇◇   ◇◇"," ◇◇◇◇◇ "},
{" ◇◇◇◇◇ ","◇◇   ◇◇"," ◇◇◇◇◇◇","     ◇◇"," ◇◇◇◇◇ "}
};
const char *COLON[5] = {" "," ◇◇"," "," ◇◇"," "}; // 冒號

/* ===== 初始化全域 console context ===== */
ConsoleCtx gConsole;
COORD time_pos[3];
void init_console_context(void){
    gConsole.hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    
	// 取得 console 寬高
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(gConsole.hOut, &csbi)){
        gConsole.width  = csbi.dwSize.X;
        gConsole.height = csbi.dwSize.Y;
    } 
	else {
        gConsole.width  = 80;   // 預設值
        gConsole.height = 25;   // 預設值
        
    }
    GetConsoleCursorInfo(gConsole.hOut, &gConsole.cursorInfo); // 取得游標資訊
    gConsole.cursorInfo.bVisible = FALSE; // 隱藏游標
    SetConsoleCursorInfo(gConsole.hOut, &gConsole.cursorInfo);
}

/* ===== 全螢幕 ===== */
void full_screen() {
    HWND hWnd = GetForegroundWindow(); // 取得目前視窗
	
    int cx = GetSystemMetrics(SM_CXSCREEN);
    int cy = GetSystemMetrics(SM_CYSCREEN);
	
    LONG l_WinStyle = GetWindowLong(hWnd, GWL_STYLE);
	
	// 視窗最大化 取消標題及邊框 
	int offsetY = -43;
    SetWindowLong(hWnd, GWL_STYLE, (l_WinStyle | WS_POPUP) & ~(WS_CAPTION | WS_THICKFRAME | WS_BORDER));
    SetWindowPos(hWnd, HWND_TOP, 0, offsetY, cx, cy - offsetY, SWP_FRAMECHANGED | SWP_NOZORDER | SWP_SHOWWINDOW);  
} 

void print_xy(int x,int y,const char *s){
    COORD p={x,y};
    SetConsoleCursorPosition(gConsole.hOut, p);
    printf("%s",s);
}

/* ===== 狀態 ===== */
int selected = -1; // 選單
int returnFromElsewhere = 0;
int flag[7] = {0}; // 功能啟用與否 
int timezone_hour = 8; // 預設在台灣
DWORD last_tick; // 更新
int isAmPm; // 時制 

/* ===== 畫面 ===== */
void draw_panel(void) {
    /* ===== 署名 ===== */
    char p_names[64], g_names[42];
    if (language) {
    	strcpy(p_names, "(19)Kuo En Yo (26)Chung Hui Xin (27)Yang Yi An (30)CHIU HSU TING"); // 64 個字 
    	strcpy(g_names, "Group 6 (members are sorted by student ID)"); // 42 個字 
	}
    else {
    	strcpy(p_names, "          (19)郭恩佑 (26)鍾慧欣 (27)楊貽安 (30)邱旭廷           ");
    	strcpy(g_names, "         第六組  (組員依座號排序)         ");
    } 
	int p_x = (gConsole.width - 64) / 2;
	int g_x = (gConsole.width - 42) / 2;
	int y = gConsole.height - 1;  // 最下面一行
	print_xy(p_x, y, p_names);
	print_xy(g_x, y-1, g_names);

    /* ===== 左 panel ===== */
    int left_gap = (gConsole.height - 3 * BTN_H) / 4;
    for(int i = 0; i < 3; i++) {
        const char* label = (i==0? BTN_THEME[language] : i==1? BTN_12_24[language] : BTN_TIMER[language]);
        int y = left_gap * (i + 1) + BTN_H * i;
        int bx = (button_center - 14) / 2; // 按鈕 X 起點
        int text_x = bx + (15 - strlen(label)) / 2;
        int isSelected = (i == selected);
        int isFlagged  = flag[i];

        int fg, bg;
        if(isSelected) {
            fg = (bgMode==0 ? 14 : 3); // 選中顏色
            bg = (bgMode==0 ? 0 : 15);
        } else if(isFlagged) {
            fg = (bgMode==0 ? 6 : 4); // 已啟用顏色
            bg = (bgMode==0 ? 0 : 15);
        } else {
            fg = (bgMode==0 ? 15 : 0);
            bg = (bgMode==0 ? 0 : 15);
        }
        SetConsoleTextAttribute(gConsole.hOut, (bg<<4)|fg);

        print_xy(bx, y,     "+-------------+");
        print_xy(bx, y + 1, "|             |");
        print_xy(text_x, y + 1, label);
        print_xy(bx, y + 2, "+-------------+");
    }

    /* ===== 右 panel ===== */
    int right_x = gConsole.width - button_center;
    int right_gap = (gConsole.height - 4 * BTN_H) / 5;
    for(int i = 0; i < 4; i++) {
        const char* label = (i==0? BTN_ALARM[language] : i==1? BTN_LANGUAGE[language] :
                             i==2? BTN_TIMEZONE[language] : BTN_MANUAL_TIME[language]);
        int y = right_gap * (i + 1) + BTN_H * i;
        int bx = right_x + (button_center - 14) / 2;
        int text_x = bx + (15 - strlen(label)) / 2;
        int idx = i + 3;
        int isSelected = (idx == selected);
        int isFlagged  = flag[idx];

        int fg, bg;
        if(isSelected) {
            fg = (bgMode==0 ? 14 : 3);
            bg = (bgMode==0 ? 0 : 15);
        } else if(isFlagged) {
            fg = (bgMode==0 ? 4 : 4);
            bg = (bgMode==0 ? 0 : 15);
        } else {
            fg = (bgMode==0 ? 15 : 0);
            bg = (bgMode==0 ? 0 : 15);
        }
        SetConsoleTextAttribute(gConsole.hOut, (bg<<4)|fg);

        print_xy(bx, y,     "+-------------+");
        print_xy(bx, y + 1, "|             |");
        print_xy(text_x, y + 1, label);
        print_xy(bx, y + 2, "+-------------+");
    }

    SetConsoleTextAttribute(gConsole.hOut, (bgMode==0 ? 0x0F : 0xF0));
}

/* ===== 時鐘 ===== */
// 畫時分秒
void draw_time(int hour, int min, int sec, const char* ampm) {
    int displayHour = hour;

    if (strcmp(ampm, "AM") == 0) {
        if (hour == 0) displayHour = 12;  // 00AM → 12AM
        else displayHour = hour;          // 1~11AM → 不變
    } 
	else if (strcmp(ampm, "PM") == 0) {
        if (hour == 12) displayHour = 12; // 12PM → 12
        else displayHour = hour - 12;     // 13~23 → 1~11
    }
	
    // 將時間轉成字串
    char buf[9];
    sprintf(buf, "%02d:%02d:%02d", displayHour, min, sec);
    
    int date_x = button_center + (center_width - strlen(buf)) / 2;
    int start_y = (gConsole.height - 5) / 2;

    // 畫數字
    int x0 = button_center + (center_width - 60) / 2;
    for (int r = 0; r < 5; r++) {
        int x = x0;
        for (int i = 0; i < 8; i++) {
            if (buf[i]==':') print_xy(x, start_y+r, COLON[r]);
            else {
                int d = buf[i]-'0';
                print_xy(x, start_y+r, NUM[d][r]);
            }
            x += 8;
        }
    }
    
    // 畫 AM/PM
    print_xy(x0 - 6, start_y + 2, ampm); // 右側一點偏移
}

// 畫日期（依使用者設定時區計算）
void clock_get_local_date(int *year, int *month, int *day, int *wday);
void draw_date(void) {
    int year, month, day, wday;
    clock_get_local_date(&year, &month, &day, &wday);

    char date_buf[64];
    if(!flag[MANUAL]) {
    	if (timezone_hour >= 0)
        	sprintf(date_buf, "%04d/%02d/%02d  %s  UTC+%02d",
            	year, month, day, TXT_WEEK[language][wday], timezone_hour);
        else
        	sprintf(date_buf, "%04d/%02d/%02d  %s  UTC%03d",
            	year, month, day, TXT_WEEK[language][wday], timezone_hour);
    } 
	else {
        sprintf(date_buf, "    %04d/%02d/%02d  %s    ",
            year, month, day, TXT_WEEK[language][wday]);
    }

    int date_x = button_center + (center_width - strlen(date_buf)) / 2;
    int start_y = (gConsole.height - 5) / 2;
    print_xy(date_x, start_y - 2, date_buf);
}

/* ==== 時區/手調 ==== */ 
void Clock_UserInit(void) // 系統初始化時呼叫一次
{
    // 初始時間為台灣時區
    time_t t = time(NULL);
	struct tm *lt = gmtime(&t);  // UTC
	clock_init(8, 0, lt->tm_hour, lt->tm_min, lt->tm_sec);

}
void clock_get_date_with_tz(int tz_hour, int *year, int *month, int *day, int *wday) {
    time_t t = time(NULL);          // UTC
    t += tz_hour * 3600;            // 套用指定時區（小時 → 秒）

    struct tm lt = *gmtime(&t);

    *year  = lt.tm_year + 1900;
    *month = lt.tm_mon + 1;
    *day   = lt.tm_mday;
    *wday  = lt.tm_wday;
}
void clock_get_local_date(int *year, int *month, int *day, int *wday) {
    clock_get_date_with_tz(timezone_hour, year, month, day, wday);
}


void UpdateClockLED(void) // 這個是取得目前時間 放在更新LED的地方
{
    const Time *t = clock_get_time();  // 取得顯示用時間
    const char *ampm;
    int hour   = t->hour; // 取得 12/24 小時顯示與 AM/PM
    int minute = t->minute;
    int second = t->second;
    if(isAmPm) // 12 小時制
        ampm = (t->hour >= 12) ? "PM" : "AM";
    else       // 24 小時制
        ampm = "  ";

    /* 把時間丟給你原本的 LED 顯示函式 */
    draw_time(hour, minute, second, ampm);
    draw_date();
}
void timeZoneSetting(void);
void manualTimeSetting(void) {
    // 第一次進入 → 啟用手動時間設定
    if(!flag[MANUAL]) {
    	// 閃爍用的
    	DWORD lastBlink = GetTickCount();
        int visible = 1;
        draw_date();
        if(flag[TIMEZONE]) {
			timeZoneSetting(); // 啟用手動時間 → 取消時區
	        draw_panel();
		}
        flag[MANUAL] = 1;   // 標記已啟用
        
        g_mode = MODE_SET_H;
        ClockMode oldMode = g_mode;
        while(g_mode != MODE_RUN) {
            if(_kbhit()) {
                char ch = _getch();
                if (ch == '\033') exit(0); // 結束整個程式
                clock_key(ch);
            }
            
            DWORD now = GetTickCount();
			if (now - lastBlink >= 300) {
				visible = !visible;
				lastBlink = now;
				if (visible)
				    SetConsoleTextAttribute(gConsole.hOut, gConsole.Attr);
				else
				    SetConsoleTextAttribute(gConsole.hOut, (gConsole.Attr & 0xF0) | 8); // 灰色文字
			}
			
	        const Time *t = clock_get_time();
	        if (g_mode != oldMode) SetConsoleTextAttribute(gConsole.hOut, gConsole.Attr);
			switch (oldMode) {
				case MODE_SET_H: draw_num(time_pos[0], t->hour, 1); break;
				case MODE_SET_M: draw_num(time_pos[1], t->minute, 0); break;
				case MODE_SET_S: draw_num(time_pos[2], t->second, 0); break;
			}
			if (g_mode != oldMode) oldMode = g_mode;
			
			if (isAmPm) {
			    const char* ampm_str;
			    if(t->hour == 0) ampm_str = "AM";       // 0 -> 12AM
			    else if(t->hour < 12) ampm_str = "AM";  // 1~11
			    else if(t->hour == 12) ampm_str = "PM"; // 12
			    else ampm_str = "PM";                    // 13~23
			    if (g_mode != MODE_SET_H) SetConsoleTextAttribute(gConsole.hOut, gConsole.Attr); 
			    draw_ampm(ampm_str);
			}
			
            Sleep(50);
        }
        last_tick = GetTickCount();
    } 
    // 已經啟用 → 取消手動時間，回到系統時間
	else {
	    time_t t = time(NULL);
		struct tm *lt = gmtime(&t);  // UTC
		timezone_hour = 8;
		clock_init(8, 0, lt->tm_hour, lt->tm_min, lt->tm_sec);
	    flag[MANUAL] = 0;
	    UpdateClockLED();
	    last_tick = GetTickCount();
	}
}
void timeZoneSetting(void) {
    // 第一次選到 → 進入時區設定模式
    if(!flag[TIMEZONE]) {
        // 閃爍用的 
        DWORD lastBlink = GetTickCount();
        int visible = 1;

        int now_time_zone = timezone_hour;  // 系統目前時區
        int temp_tz = timezone_hour;        // 設定用暫存

        if(flag[MANUAL]) {
            manualTimeSetting(); // 啟用時區 → 取消手動
            draw_panel();
        }

        int flag_timezone_setting = 1; // 正在設定模式
        flag[TIMEZONE] = 1;

        // 只調整小時
        while(1) {
            if(_kbhit()) {
                char ch = _getch();
                // S 鍵小時+1
                if(ch == '\033') exit(0);
                else if(toupper(ch) == 'S') {
                    temp_tz++;
                    if (temp_tz > 14)
                        temp_tz = -12;
                }
                else if(toupper(ch) == 'D'){
                    if (temp_tz != now_time_zone) {
                        clock_set_timezone(temp_tz, 0);
                        timezone_hour = temp_tz;
                        flag[TIMEZONE] = (timezone_hour != 8); // 語意正確
                    }
                    flag_timezone_setting = 0; // 離開設定模式
                    break;
                }
            }

            DWORD now = GetTickCount();
            if (now - lastBlink >= 300) {
                visible = !visible;
                lastBlink = now;
                if (visible)
                    SetConsoleTextAttribute(gConsole.hOut, gConsole.Attr);
                else
                    SetConsoleTextAttribute(gConsole.hOut, (gConsole.Attr & 0xF0) | 8); // 灰色文字
            }

            const Time *t = clock_get_time();
            // 用暫存時區計算顯示用小時，處理跨日
            int display_hour = (t->hour + (temp_tz - now_time_zone) + 24) % 24;
            draw_num(time_pos[0], display_hour, 1);

            if (isAmPm) {
                const char* ampm_str;
                if(display_hour == 0) ampm_str = "AM";       // 0 -> 12AM
                else if(display_hour < 12) ampm_str = "AM";  // 1~11
                else if(display_hour == 12) ampm_str = "PM"; // 12
                else ampm_str = "PM";                        // 13~23
                if (g_mode != MODE_SET_H) SetConsoleTextAttribute(gConsole.hOut, gConsole.Attr); 
                draw_ampm(ampm_str);
            }

            int year, month, day, wday;
            clock_get_date_with_tz(temp_tz, &year, &month, &day, &wday);

            char date_buf[64];
            if(!flag[MANUAL]) {
                if (temp_tz >= 0)
                    sprintf(date_buf, "%04d/%02d/%02d  %s  UTC+%02d",
                        year, month, day, TXT_WEEK[language][wday], temp_tz);
                else
                    sprintf(date_buf, "%04d/%02d/%02d  %s  UTC%03d",
                        year, month, day, TXT_WEEK[language][wday], temp_tz);
            } 
            else {
                sprintf(date_buf, "    %04d/%02d/%02d  %s    ",
                    year, month, day, TXT_WEEK[language][wday]);
            }

            int date_x = button_center + (center_width - strlen(date_buf)) / 2;
            int start_y = (gConsole.height - 5) / 2;
            print_xy(date_x, start_y - 2, date_buf);
        }
    } 
    // 已經調整過 → 取消時區設定，回到系統預設
    else {
        flag[TIMEZONE] = 0;
        timezone_hour = 8;
        clock_set_timezone(8, 0); // 系統預設 UTC+8
        UpdateClockLED();
    }
}

/* ===== 主程式 ===== */
int main() {
	
	// ----------------------------
    // Windows 專用：切換終端機
    // ----------------------------
	#ifdef _WIN32
	    system("chcp 950 > nul");  // > nul 把輸出隱藏
	#endif
	
    full_screen();
    Sleep(100);  // 等 Windows 更新 Console 緩衝區
    init_console_context();
    gConsole.Attr = (0 << 4) | 15;
    SetConsoleTextAttribute(gConsole.hOut, gConsole.Attr); // 預設白字黑底
    
	draw_panel(); // UI
    
    // 初值設定
	const char** ampm; // 時制 
	int alarmTime = -1; // 鬧鐘
	time_t t = time(NULL);
	struct tm *lt = gmtime(&t);
	clock_init(8, 0, lt->tm_hour, lt->tm_min, lt->tm_sec); // 使用系統本地時間
    last_tick = GetTickCount(); // 紀錄上一次更新時間 
    DWORD lastKeyTime = GetTickCount();  // 記錄上一次按鍵時間
    
	time_pos[0] = (COORD) { button_center + (center_width - 60) / 2, (gConsole.height - 5) / 2  };       // Hour
	time_pos[1] = (COORD) { button_center + (center_width - 60) / 2 + 24, (gConsole.height - 5) / 2  };  // Minute
	time_pos[2] = (COORD) { button_center + (center_width - 60) / 2 + 48, (gConsole.height - 5) / 2  };  // Second

	while(1){
	    DWORD now = GetTickCount();
	
	    // 每秒更新一次時鐘
	    if(now - last_tick >= 1000){
	        last_tick += 1000;
			clock_tick_1s();
	        const Time *t = clock_get_time();
	        UpdateClockLED();
	    }
	    const Time *t = clock_get_time();
	    alarmTick(alarmTime, t->hour, t->minute);
	
	    // 按鍵偵測
	    if(_kbhit()){
	        char ch = _getch();
	        if(ch == '\033') exit(0);
	        else if(ch=='s'||ch=='S') { 
				selected=(selected+1)%7; 
				draw_panel();
				lastKeyTime = GetTickCount();
			}
	        else if(ch=='d'||ch=='D'){ // D 確認
	            const Time *t = clock_get_time(); // 重新抓時間
	            switch (selected){
	                case 0: 
                        BG_Select(); 
                        CONSOLE_SCREEN_BUFFER_INFO csbi;
                        GetConsoleScreenBufferInfo(gConsole.hOut, &csbi);
                        gConsole.Attr = csbi.wAttributes;
                        selected = -1;
                        returnFromElsewhere = 1;
                        break;
	                case 1: toggleTimeMode(); isAmPm = !isAmPm; selected = -1; draw_panel(); break;
	                case 2: timer(); selected = -1; returnFromElsewhere = 1; break;
	                case 3: 
	                    // 第一次選到 → 進入鬧鐘設定模式
	                	if (!flag[ALARM]) {
							flag[ALARM] = 1;
							alarmTime = setAlarm(t->hour, t->minute);
							returnFromElsewhere = 1;
						} 
						// 已經啟用 → 取消鬧鐘設定
						else {
							flag[ALARM] = 0;
							alarmTime = -1;
							selected = -1;
							draw_panel();
						}
						selected=-1;
						break;
	                case 4: language = !language; selected=-1; draw_panel(); break;
	                case 5: timeZoneSetting(); selected = -1; draw_panel(); break;
	                case 6: manualTimeSetting(); selected = -1; draw_panel(); break;
	            }
	        }
	    }
	    
	    // 如果 5 秒沒按鍵，取消選取
	    if(selected != -1 && GetTickCount() - lastKeyTime >= 5000){
	        selected = -1;  // 自動取消選取
	        draw_panel();  // 更新 UI
	    }
	
	    // 重新畫 panel
	    if(returnFromElsewhere){
	        returnFromElsewhere = 0;
	        system("cls");
	        const Time *t = clock_get_time();
	        UpdateClockLED();
	        draw_panel();
	    }
	
	    Sleep(10); // 小延遲，避免 CPU 100%
	}

}

#include "ui_profile.h"
#include <stdio.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

/* ================= 12/24 時制 ================= */
int mode12_24 = MODE_24;
int confirmStatus = 0;

int getTimeMode(void){
    return mode12_24;
}

void toggleTimeMode(void){
    mode12_24 = (mode12_24==MODE_12)?MODE_24:MODE_12;
    confirmStatus = 0;
}

void setTimeMode(int mode){
    if(mode==MODE_12 || mode==MODE_24) mode12_24=mode;
    confirmStatus = 0;
}

int convertHour(int hour24,const char** ampm){
    *ampm = "  ";
    if(mode12_24==MODE_24) return hour24;
    if(hour24==0){ *ampm="AM"; return 12; }
    if(hour24<12){ *ampm="AM"; return hour24; }
    if(hour24==12){ *ampm="PM"; return 12; }
    *ampm="PM";
    return hour24-12;
}

/* ================= 語言 ================= */
int language=0;
int tempLang=0;

const char* TXT_DATE[2] = {"日期","Date"};
const char* TXT_WEEK[2][7] = {
    {"週日","週一","週二","週三","週四","週五","週六"},
    {"Sun ","Mon ","Tues","Wed ","Thur","Fri ","Sat "}
};
const char* TXT_12H[2] = {"12小時制","12h"};
const char* TXT_24H[2] = {"24小時制","24h"};

const char* BTN_LANGUAGE[2]    = {"語 言", "Language"};
const char* BTN_THEME[2]       = {"主 題", "Theme"};
const char* BTN_12_24[2]       = {"時 制", "Time Format"};
const char* BTN_TIMEZONE[2]    = {"時 區", "Time Zone"};
const char* BTN_ALARM[2]       = {"鬧 鐘", "Alarm"};
const char* BTN_TIMER[2]       = {"計 時", "Timer"};
const char* BTN_MANUAL_TIME[2] = {"手 動", "Manual Time"};

void handleLanguageInput(char c){
    if(c=='d'||c=='D') tempLang = 1-tempLang;
    if(c=='s'||c=='S') language = tempLang;
}

const char* getDaySuffix(int day){
    if(day%100>=11 && day%100<=13) return "th";
    switch(day%10){
        case 1:return "st";
        case 2:return "nd";
        case 3:return "rd";
        default:return "th";
    }
}

/* ================= 背景模式 ================= */
int bgMode=0;
int BG_X=10;
int BG_Y=5;

void BG_SetColor(){
    int fg = (bgMode==0)?15:0;
    int bg = (bgMode==0)?0:15;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),(bg<<4)|fg);
}

void BG_ClearScreen(){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD written;
    COORD home={0,0};
    GetConsoleScreenBufferInfo(hOut,&csbi);
    FillConsoleOutputCharacter(hOut,' ',csbi.dwSize.X*csbi.dwSize.Y,home,&written);
    FillConsoleOutputAttribute(hOut,(bgMode==0?0x0F:0xF0),csbi.dwSize.X*csbi.dwSize.Y,home,&written);
    SetConsoleCursorPosition(hOut,home);
}

void BG_Select(){
    int select = bgMode;
    while(1){
        bgMode=select;
        BG_ClearScreen();
        BG_SetColor();
        gotoxy(BG_X,BG_Y);
        printf("%s 深色模式\n",(select==0)?">":" ");
        gotoxy(BG_X,BG_Y+2);
        printf("%s 淺色模式\n",(select==1)?">":" ");
        gotoxy(BG_X,BG_Y+4);
        if(kbhit()){
            char c=getch();
        	if(c == '\033') exit(0);
            if(c=='S'||c=='s') select=!select;
            if(c=='d'||c=='D'){ bgMode=select; BG_ClearScreen(); BG_SetColor(); return; }
        }
        Sleep(150);
    }
}

/* ================= Console 工具 ================= */
void gotoxy(int x,int y){
    COORD pos={x,y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),pos);
}

void setColor(int c){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),c);
}


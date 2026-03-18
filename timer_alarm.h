#pragma once

// 鬧鐘設定（進入此狀態直到設定完成）
int setAlarm(int hour, int min);
/*
    int 響鈴秒數 = setAlarm(傳入現在小時, 傳入現在分鐘);
    且在設定完成後，主介面的 ALARM 需要設定成亮起狀態 
*/ 

// 倒數計時器（進入此狀態直到設定完成或取消）
void timer(void);

// 更新即呼叫，用來檢查是否該響鈴
void alarmTick(int alarmTime, int hour, int min);

// 畫時鐘 
void draw_num(COORD pos, int num, int isHour);
void draw_ampm(const char* ampm);



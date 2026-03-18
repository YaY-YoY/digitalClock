#ifndef CLOCK_UTC_ADJUST_H
#define CLOCK_UTC_ADJUST_H

/* ===== 型別定義 ===== */

typedef struct {
    int hour;   /* 0~23 */
    int minute; /* 0~59 */
    int second; /* 0~59 */
} Time;

typedef enum {
    MODE_RUN = 0,  /* 主畫面：正常走時 */
    MODE_SET_H,    /* 設定時 */
    MODE_SET_M,    /* 設定分 */
    MODE_SET_S     /* 設定秒 */
} ClockMode;

/* ===== 內部狀態（這些變數隸屬「這個 .c」） ===== */

static Time      g_utc_time       = {0, 0, 0};  /* 內部 UTC 時間 */
static Time      g_local_time     = {0, 0, 0};  /* 顯示用：套用時區後的時間 */
static ClockMode g_mode           = MODE_RUN;   /* 目前模式 */
static int       g_utc_offset_min = 0;          /* 相對 UTC 的分鐘偏移 */

/* ===== 內部工具：UTC ? Local ===== */

static inline void clock_utc_to_local(void)
{
    int total_min = g_utc_time.hour * 60 + g_utc_time.minute + g_utc_offset_min;
    int day_min   = 24 * 60;

    total_min %= day_min;
    if (total_min < 0)
        total_min += day_min;

    g_local_time.hour   = total_min / 60;
    g_local_time.minute = total_min % 60;
    g_local_time.second = g_utc_time.second;
}

static inline void clock_local_to_utc(void)
{
    int total_min = g_local_time.hour * 60 + g_local_time.minute - g_utc_offset_min;
    int day_min   = 24 * 60;

    total_min %= day_min;
    if (total_min < 0)
        total_min += day_min;

    g_utc_time.hour   = total_min / 60;
    g_utc_time.minute = total_min % 60;
    g_utc_time.second = g_local_time.second;
}

/* ===== 對外：初始化 / 時區設定 ===== */

/* 在 main 開頭或系統初始化呼叫：
 *   tz_hour, tz_min  : 時區（例：UTC+8 → 8, 0）
 *   utc_h, utc_m, utc_s : 起始 UTC 時間
 */
static inline void clock_init(int tz_hour, int tz_min,
                              int utc_h, int utc_m, int utc_s)
{
    if (tz_hour >= 0)
        g_utc_offset_min = tz_hour * 60 + tz_min;
    else
        g_utc_offset_min = tz_hour * 60 - tz_min;

    if (utc_h < 0) utc_h = 0; if (utc_h > 23) utc_h = 23;
    if (utc_m < 0) utc_m = 0; if (utc_m > 59) utc_m = 59;
    if (utc_s < 0) utc_s = 0; if (utc_s > 59) utc_s = 59;

    g_utc_time.hour   = utc_h;
    g_utc_time.minute = utc_m;
    g_utc_time.second = utc_s;

    clock_utc_to_local();
}

/* 日後若要改時區（例如做「時區設定模式」時）呼叫 */
static inline void clock_set_timezone(int tz_hour, int tz_min)
{
    if (tz_hour >= 0)
        g_utc_offset_min = tz_hour * 60 + tz_min;
    else
        g_utc_offset_min = tz_hour * 60 - tz_min;

    clock_utc_to_local();
}

/* ===== 對外：走時 / 手動調時 ===== */

/* 每秒呼叫一次（放 Timer 中斷）：主畫面時讓時間 +1 秒 */
static inline void clock_tick_1s(void)
{
    if (g_mode != MODE_RUN)
        return;

    g_utc_time.second++;
    if (g_utc_time.second >= 60) {
        g_utc_time.second = 0;
        g_utc_time.minute++;
        if (g_utc_time.minute >= 60) {
            g_utc_time.minute = 0;
            g_utc_time.hour++;
            if (g_utc_time.hour >= 24) {
                g_utc_time.hour = 0;
            }
        }
    }
    clock_utc_to_local();
}

/* 按鍵處理：
 *   D：模式切換（RUN → 設時 → 設分 → 設秒 → RUN）
 *   S：在設定模式下，使目前欄位 +1（24→0、60→0）
 */
static inline void clock_key(char key)
{
    switch (key) {
    case 'S':
    case 's':
        if (g_mode == MODE_SET_H) {
            g_local_time.hour++;
            if (g_local_time.hour >= 24)
                g_local_time.hour = 0;
        } else if (g_mode == MODE_SET_M) {
            g_local_time.minute++;
            if (g_local_time.minute >= 60)
                g_local_time.minute = 0;
        } else if (g_mode == MODE_SET_S) {
            g_local_time.second++;
            if (g_local_time.second >= 60)
                g_local_time.second = 0;
        }
        if (g_mode == MODE_SET_H || g_mode == MODE_SET_M || g_mode == MODE_SET_S)
            clock_local_to_utc();
        break;

    case 'D':
    case 'd':
        if (g_mode == MODE_RUN) {
            clock_utc_to_local();
            g_mode = MODE_SET_H;
        } else if (g_mode == MODE_SET_H) {
            g_mode = MODE_SET_M;
        } else if (g_mode == MODE_SET_M) {
            g_mode = MODE_SET_S;
        } else if (g_mode == MODE_SET_S) {
            g_mode = MODE_RUN;
            clock_utc_to_local();
        }
        break;

    default:
        break;
    }
}

/* ===== 對外：給顯示用 ===== */

/* 拿現在「顯示用」的時間（已套用時區＋手動調整） */
static inline const Time *clock_get_time(void)
{
    return &g_local_time;
}

/* 拿目前模式：可用來決定哪一欄要閃爍 */
static inline ClockMode clock_get_mode(void)
{
    return g_mode;
}

#endif /* CLOCK_UTC_ADJUST_H */


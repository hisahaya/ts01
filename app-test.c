# include "ev3api.h"
# include "app.h"
//# include "parameters.h"

/* ---- 前提 ---- */
// 1. モーターの向き的に、負の値で前進、正の値で後退する


/* ---- 制御関数 ---- */
void R_MOTOR(int power) {   // 右モーターの関数
    ev3_motor_set_power(EV3_PORT_A, power);
}

void L_MOTOR(int power) {   // 左モーターの関数
    ev3_motor_set_power(EV3_PORT_B, power);
}

void TURN_MOTOR(int powerL, int powerR) {   // 両モーターの関数 (旋回・連続運転用)
    ev3_motor_set_power(EV3_PORT_A, powerR);
    ev3_motor_set_power(EV3_PORT_B, powerL);
}

void DS_MOTOR(int power, int time) {    // 両モーターの関数 (Dual Sync Motor)
    ev3_motor_set_power(EV3_PORT_A, power);
    ev3_motor_set_power(EV3_PORT_B, power);
    tslp_tsk(time);
    ev3_motor_stop(EV3_PORT_A, true);
    ev3_motor_stop(EV3_PORT_B, true);
}

void DSU_MOTOR(int power) {     // 両モーターの関数（Dual Sync Unlimited Motor）
    ev3_motor_set_power(EV3_PORT_A, power);
    ev3_motor_set_power(EV3_PORT_B, power);
}

void DAS_MOTOR(int powerL, int powerR, int time) {  // 両モーターの関数 (Dual Async Motor)
    ev3_motor_set_power(EV3_PORT_A, powerR);
    ev3_motor_set_power(EV3_PORT_B, powerL);
    tslp_tsk(time);
    ev3_motor_stop(EV3_PORT_A, true);
    ev3_motor_stop(EV3_PORT_B, true);
}

void STOP_MOTOR() {   // 両モーターの停止関数
    ev3_motor_stop(EV3_PORT_A, true);
    ev3_motor_stop(EV3_PORT_B, true);
    tslp_tsk(100);
}

void GoToWall() {   // 壁に向かって後進する関数
    DS_MOTOR(50, 1500); // 1.5秒後退する
    GyroReset();        // ジャイロリセット
}

void GyroReset() {  // ジャイロセンサーのリセット関数
    ev3_gyro_sensor_reset(EV3_PORT_4);
    tslp_tsk(100);
}

int GyroAngle() {   // ジャイロセンサーの角度取得関数
    return ev3_gyro_sensor_get_angle(EV3_PORT_4);
}

int Touch() {   // タッチセンサーの状態取得関数
    return ev3_touch_sensor_is_pressed(EV3_PORT_3);
}

int GetColor(int port) {   // カラーセンサーの色取得関数
    return ev3_color_sensor_get_color(port);
}

void SOUND(int freq, int time) {    // 音を鳴らす関数
    ev3_speaker_play_tone(freq, time);
}


/* ---------------- メインプログラム ---------------- */
void main_task(intptr_t unused){
    /* ---- 定義 ---- */
    // カラーセンサー　1,2 1=Left 2=Right
    ev3_sensor_config(EV3_PORT_1, COLOR_SENSOR);
    ev3_sensor_config(EV3_PORT_2, COLOR_SENSOR);

    // タッチセンサー
    ev3_sensor_config(EV3_PORT_3, TOUCH_SENSOR);

    // ジャイロセンサー
    ev3_sensor_config(EV3_PORT_4, GYRO_SENSOR);

    // モーター　A=Right B=Left
    ev3_motor_config(EV3_PORT_A, LARGE_MOTOR);
    ev3_motor_config(EV3_PORT_B, LARGE_MOTOR);

    // スピーカー音量
    ev3_speaker_set_volume(10);

    /* ---------------- スタート ---------------- */
    // スタートし、ジャイロリセット
    GyroReset();


    /* ---------------- 黒線トレース ---------------- */
    //壁タッチまで黒線をトレース
    while(Touch() == 0) {
        // 左センサーが黒検知したら左Speed=(-20)
        if (GetColor(EV3_PORT_1) == 1) {
            L_MOTOR(-20);
            SOUND(NOTE_C5, 100);
            tslp_tsk(50);
        }else {
            L_MOTOR(-30);
        }
        // 右センサーが黒検知したら右Speed=(-20)
        if (GetColor(EV3_PORT_2) == 1) {
            R_MOTOR(-20);
            SOUND(NOTE_C5, 100);
            tslp_tsk(50);
        }else {
            R_MOTOR(-30);
        }
    }
    // 壁にタッチしたら少し下がる
    DS_MOTOR(30, 250);

    // 左に90度回転
    while(GyroAngle() > -83) {
        TURN_MOTOR(30, -30);
        tslp_tsk(10);
    }
    // モーターストップ
    STOP_MOTOR();

    // ジャイロリセット
    GyroReset();


    /* ---------------- 黄線トレース ---------------- */
    // 壁タッチまで黄線をトレース
    while(Touch() == 0) {
        // 左センサーが黄検知したら左Speed=(-20)
        if (GetColor(EV3_PORT_1) == 4) {
            L_MOTOR(-20);
            SOUND(NOTE_C5, 100);
            tslp_tsk(50);
        }else {
            L_MOTOR(-30);
        }
        // 右センサーが黄検知したら右Speed=(-20)
        if (GetColor(EV3_PORT_2) == 4) {
            R_MOTOR(-20);
            SOUND(NOTE_C5, 100);
            tslp_tsk(50);
        }else {
            R_MOTOR(-30);
        }
    }
    // 壁にタッチしたら少し下がる
    DS_MOTOR(30, 300);

    // 左に90度回転
    while(GyroAngle() > -85) {
        TURN_MOTOR(30, -30);
        tslp_tsk(10);
    }
    // モーターストップ
    STOP_MOTOR();

    // 後ろの壁に向かって後退する
    GoToWall();


    /* ---------------- 緑線まで直進 ---------------- */
    // 緑線を検知するまで直進
    while(GetColor(EV3_PORT_1) != 3 && GetColor(EV3_PORT_2) != 3) {
        DSU_MOTOR(-30);
        tslp_tsk(10);
    }
    // 少し下がる
    DS_MOTOR(50, 30);

    // 左に90度回転
    while(GyroAngle() > -85) {
        TURN_MOTOR(30, -30);
        tslp_tsk(10);
    }
    // モーターストップ
    STOP_MOTOR();

    // 後ろの壁に向かって後退する
    GoToWall();


    /* ---------------- 緑線トレース ---------------- */
    // 壁タッチまで緑線をトレース
    while(Touch() == 0) {
        // 左センサーが緑検知したら左Speed=(-20)
        if (GetColor(EV3_PORT_1) == 3) {
            L_MOTOR(-20);
            SOUND(NOTE_C5, 100);
            tslp_tsk(50);
        }else {
            L_MOTOR(-30);
        }
        // 右センサーが緑検知したら右Speed=(-20)
        if (GetColor(EV3_PORT_2) == 3) {
            R_MOTOR(-20);
            SOUND(NOTE_C5, 100);
            tslp_tsk(50);
        }else {
            R_MOTOR(-30);
        }
    }
    // 壁にタッチしたら少し下がる
    DS_MOTOR(30, 300);

    // 左に90度回転
    while(GyroAngle() > -85) {
        TURN_MOTOR(30, -30);
        tslp_tsk(10);
    }
    // モーターストップ
    STOP_MOTOR();

    // ジャイロリセット
    GyroReset();


    /* ---------------- 黄線まで直進 ---------------- */
    // 黄線の壁まで直進
    while(Touch() == 0) {
        DSU_MOTOR(-30);
        tslp_tsk(10);
    }
    // 壁にタッチしたら少し下がる
    DS_MOTOR(30, 200);

    // 左に90度回転
    while(GyroAngle() > -85) {
        TURN_MOTOR(30, -30);
        tslp_tsk(10);
    }
    // モーターストップ
    STOP_MOTOR();

    // 後ろの壁に向かって後退する
    GoToWall();


    /* ---------------- 黒線まで黄線トレース ---------------- */
    // 黒線を検知するまで黄線をトレース
    while(GetColor(EV3_PORT_1) != 1 && GetColor(EV3_PORT_2) != 1) {
        // 左センサーが黄検知したら左Speed=(-20)
        if (GetColor(EV3_PORT_1) == 4) {
            L_MOTOR(-20);
            SOUND(NOTE_C5, 100);
            tslp_tsk(50);
        }else {
            L_MOTOR(-30);
        }
        // 右センサーが黄検知したら右Speed=(-20)
        if (GetColor(EV3_PORT_2) == 4) {
            R_MOTOR(-20);
            SOUND(NOTE_C5, 100);
            tslp_tsk(50);
        }else {
            R_MOTOR(-30);
        }
    }
    //少し下がる
    DS_MOTOR(50, 30);

    // 左に90度回転
    while(GyroAngle() > -85) {
        TURN_MOTOR(30, -30);
        tslp_tsk(10);
    }
    // モーターストップ
    STOP_MOTOR();

    // 後ろの壁に向かって後退する
    GoToWall();


    /* -------- 赤線まで黒線トレース -------- */
    // 赤線を検知するまで黒線をトレース
    while(GetColor(EV3_PORT_1) != 5 && GetColor(EV3_PORT_2) != 5) {
        // 左センサーが黒検知したら左Speed=(-20)
        if (GetColor(EV3_PORT_1) == 1) {
            L_MOTOR(-20);
            SOUND(NOTE_C5, 100);
            tslp_tsk(50);
        }else {
            L_MOTOR(-30);
        }
        // 右センサーが黒検知したら右Speed=(-20)
        if (GetColor(EV3_PORT_2) == 1) {
            R_MOTOR(-20);
            SOUND(NOTE_C5, 100);
            tslp_tsk(50);
        }else {
            R_MOTOR(-30);
        }
    }
    //少し下がる
    DS_MOTOR(50, 30);

    // 左に90度回転
    while(GyroAngle() > -85) {
        TURN_MOTOR(30, -30);
        tslp_tsk(10);
    }
    // モーターストップ
    STOP_MOTOR();


    /* ---------------- 赤線トレース ---------------- */
    // 壁タッチまで赤線をトレース
    while(Touch() == 0) {
        // 左センサーが赤検知したら左Speed=(-20)
        if (GetColor(EV3_PORT_1) == 5) {
            L_MOTOR(-20);
            SOUND(NOTE_C5, 100);
            tslp_tsk(50);
        }else {
            L_MOTOR(-30);
        }
        // 右センサーが赤検知したら右Speed=(-20)
        if (GetColor(EV3_PORT_2) == 5) {
            R_MOTOR(-20);
            SOUND(NOTE_C5, 100);
            tslp_tsk(50);
        }else {
            R_MOTOR(-30);
        }
    }
    // 壁にタッチしたら少し下がる
    DS_MOTOR(30, 300);

    // 左に90度回転
    while(GyroAngle() > -85) {
        TURN_MOTOR(30, -30);
        tslp_tsk(10);
    }
    // モーターストップ
    STOP_MOTOR();

    // 後ろの壁に向かって後退する
    GoToWall();
    
    
    /* ---------------- 緑線まで直進 ---------------- */
    // 緑線を検知するまで直進
    while(GetColor(EV3_PORT_1) != 3 && GetColor(EV3_PORT_2) != 3) {
        DSU_MOTOR(-30);
        tslp_tsk(10);
    }
    // 少し下がる
    DS_MOTOR(50, 30);

    // 左に90度回転
    while(GyroAngle() > -85) {
        TURN_MOTOR(30, -30);
        tslp_tsk(10);
    }
    // モーターストップ
    STOP_MOTOR();

    // 後ろの壁に向かって後退する
    GoToWall();


    /* ---------------- 緑線トレース ---------------- */
    // 壁タッチまで緑線をトレース
    while(Touch() == 0) {
        // 左センサーが緑検知したら左Speed=(-20)
        if (GetColor(EV3_PORT_1) == 3) {
            L_MOTOR(-20);
            SOUND(NOTE_C5, 100);
            tslp_tsk(50);
        }else {
            L_MOTOR(-30);
        }
        // 右センサーが緑検知したら右Speed=(-20)
        if (GetColor(EV3_PORT_2) == 3) {
            R_MOTOR(-20);
            SOUND(NOTE_C5, 100);
            tslp_tsk(50);
        }else {
            R_MOTOR(-30);
        }
    }
    // 壁にタッチしたら少し下がる
    DS_MOTOR(30, 300);

    // 左に90度回転
    while(GyroAngle() > -85) {
        TURN_MOTOR(30, -30);
        tslp_tsk(10);
    }
    // モーターストップ
    STOP_MOTOR();


    /* ---------------- 赤線まで直進 ---------------- */
    // 赤線を検知するまで直進
    while(GetColor(EV3_PORT_1) != 5 && GetColor(EV3_PORT_2) != 5) {
        DSU_MOTOR(-30);
        tslp_tsk(10);
    }
    // 少し下がる
    DS_MOTOR(50, 30);

    // 左に90度回転
    while(GyroAngle() > -85) {
        TURN_MOTOR(30, -30);
        tslp_tsk(10);
    }
    // モーターストップ
    STOP_MOTOR();

    // 後ろの壁に向かって後退する
    GoToWall();


    /* ---------------- 黒線まで赤線トレース ---------------- */
    // 黒線を検知するまで赤線をトレース
    while(GetColor(EV3_PORT_1) != 1 && GetColor(EV3_PORT_2) != 1) {
        // 左センサーが赤検知したら左Speed=(-20)
        if (GetColor(EV3_PORT_1) == 5) {
            L_MOTOR(-20);
            SOUND(NOTE_C5, 100);
            tslp_tsk(50);
        }else {
            L_MOTOR(-30);
        }
        // 右センサーが赤検知したら右Speed=(-20)
        if (GetColor(EV3_PORT_2) == 5) {
            R_MOTOR(-20);
            SOUND(NOTE_C5, 100);
            tslp_tsk(50);
        }else {
            R_MOTOR(-30);
        }
    }
    //少し下がる
    DS_MOTOR(50, 30);
    
    // 右に90度回転
    while(GyroAngle() < 85) {
        TURN_MOTOR(-30, 30);
        tslp_tsk(10);
    }
    // モーターストップ
    STOP_MOTOR();


    /* ---------------- 黒線トレース ---------------- */
    // 壁タッチまで黒線をトレース
    while(Touch() == 0) {
        // 左センサーが黒検知したら左Speed=(-20)
        if (GetColor(EV3_PORT_1) == 1) {
            L_MOTOR(-20);
            SOUND(NOTE_C5, 100);
            tslp_tsk(50);
        }else {
            L_MOTOR(-30);
        }
        // 右センサーが黒検知したら右Speed=(-20)
        if (GetColor(EV3_PORT_2) == 1) {
            R_MOTOR(-20);
            SOUND(NOTE_C5, 100);
            tslp_tsk(50);
        }else {
            R_MOTOR(-30);
        }
    }
    // 壁にタッチしたら少し下がる
    DS_MOTOR(30, 300);
}


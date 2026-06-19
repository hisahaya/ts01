# include "ev3api.h"
# include "app.h"
//# include "parameters.h"

void main_task(intptr_t unused)
{
    /* ---- 定義 ---- */
    // カラーセンサー　1,2 1=Left 2=Right
    ev3_sensor_config(EV3_PORT_1, COLOR_SENSOR);
    ev3_sensor_config(EV3_PORT_2, COLOR_SENSOR);

    // タッチセンサー
    ev3_sensor_config(EV3_PORT_3, TOUCH_SENSOR);

    // ジャイロセンサー
    ev3_sensor_config(EV3_PORT_4, GYRO_SENSOR);

    // モーター　A=Right D=Left
    ev3_motor_config(EV3_PORT_A, LARGE_MOTOR);
    ev3_motor_config(EV3_PORT_B, LARGE_MOTOR);

    // スピーカー音量
    ev3_speaker_set_volume(10);


    /* ---- メインプログラム ---- */
    // スタートし、ジャイロリセット
    ev3_gyro_sensor_reset(EV3_PORT_4);

    //壁タッチまで黒線をトレース
    while(ev3_touch_sensor_is_pressed(EV3_PORT_3) == 0) {
        // 左センサーが黒検知したら左Speed=(-20)
        if (ev3_color_sensor_get_color(EV3_PORT_1) == 1) {
            ev3_motor_set_power(EV3_PORT_A, -20);
            ev3_speaker_play_tone(NOTE_C5, 100);
            tslp_tsk(50);
        }else {
            ev3_motor_set_power(EV3_PORT_A, -30);
        }
        // 右センサーが黒検知したら右Speed=(-20)
        if (ev3_color_sensor_get_color(EV3_PORT_2) == 1) {
            ev3_motor_set_power(EV3_PORT_B, -20);
            ev3_speaker_play_tone(NOTE_C5, 100);
            tslp_tsk(50);
        }else {
            ev3_motor_set_power(EV3_PORT_B, -30);
        }
    }

    

    // ループを抜けたら（壁にタッチしたら）安全のため両モーターを停止
    ev3_motor_stop(EV3_PORT_A, true);
    ev3_motor_stop(EV3_PORT_B, true);
}
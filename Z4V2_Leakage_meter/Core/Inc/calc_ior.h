/// @file    calc_ior.h
/// @brief   3600SPS の電圧・漏電流サンプルから I0r（抵抗分漏れ電流）を求める
/// @author  Y.Sugawara
/// @date    2026/09/28
/// @version 1.0

#ifndef INC_CALC_IOR_H_
#define INC_CALC_IOR_H_

#include <stdint.h>

/*
 * ADC2 のスキャン順（= sample_buf_t[].buf[] の並び）
 *   Rank1 VAC1 (IN1), Rank2 VAC2 (IN2),
 *   Rank3 LAC1 (IN13), Rank4 LAC2 (IN3), Rank5 LAC3 (IN4), Rank6 LAC4 (IN12)
 *   ※ QSEL_IN3_CHANNEL などの名前はピン名と一致していない（buf[2] は IN13 = LAC1）
 */
#define IOR_SIG_VAC1   0
#define IOR_SIG_VAC2   1
#define IOR_SIG_LAC1   2
#define IOR_SIG_NUM    6
#define IOR_CH_NUM     4

/* 100ms 窓 = 360 点。50Hz なら 5 周期、60Hz なら 6 周期ちょうどなので DC・高調波が消える */
#define IOR_WIN_SAMPLES  360

/*
 * ハードウェア遅延 [µs]。正 = 実信号より遅れて ADC に入る（CT のように位相が進む場合は負）。
 *   θ_true = θ_raw + 2πf(τ_I0 - τ_V)
 * キャリブレーション: 純抵抗の漏電を流し、`ior` コマンドで raw 位相 θ_raw[deg] を読む。
 *   τ_I0 - τ_V = -θ_raw / 360 / f × 1e6 [µs]
 *   例) 50Hz で raw = +3.6°（I0 が進んで見える）→ τ_I0 - τ_V = -200µs
 * I0 側は CT 種別ごとに calc_ior.c の ior_i0_delay_us[] に設定する。
 */
#define IOR_V_DELAY_US   (0.0f)   // VAC1/VAC2 の入力回路遅延

/* 基準電圧の DFT 振幅 [count] がこれ未満なら I0r は無効（100V 入力でおよそ 28000） */
#define IOR_V_MIN_DFT    (2000.0f)

/* 位相平滑化の EWMA 係数（100ms ごと）。I0 側 (tsk_calc.c ALPHA) と同じ時定数 */
#define IOR_EWMA_ALPHA   (0.05f)

typedef struct {
  float re_f;          // EWMA 済み I0·conj(Vref単位ベクトル) 実部 [count]
  float im_f;          // 同 虚部
  float phase_raw_deg; // 遅延補正前の位相 I0 - Vref [deg]（キャリブレーション用）
  float phase_deg;     // 遅延補正後の位相 [deg]（+ で I0 が進み）
  float i0r;           // 抵抗分漏れ電流 [mA]（校正済み I0 × 抵抗分比率）
  uint8_t valid;       // 1: 電圧あり・計算有効
} st_ior_ch;

typedef struct {
  /* 窓内の積算 */
  uint16_t n;
  float dc[IOR_SIG_NUM];      // 前窓の平均（DC 除去用）
  float sum[IOR_SIG_NUM];
  float re50[IOR_SIG_NUM];
  float im50[IOR_SIG_NUM];
  float re60[IOR_SIG_NUM];
  float im60[IOR_SIG_NUM];

  /* 窓ごとの結果 */
  uint8_t use60;              // 0:50Hz 1:60Hz（VAC1 の振幅で判定）
  float v_mag;                // 基準電圧の DFT 振幅 [count]
  float v12_ratio;            // |V1+V2|/|V1|（配線確認用: 1P3W≈0, 3P3W≈1.73）
  uint32_t win_count;
  st_ior_ch ch[IOR_CH_NUM];
} st_ior;

extern st_ior ior_t;

void Ior_Init( void );
void Ior_PushSample( const int16_t *buf );
float Ior_GetI0r( int ch );
float Ior_GetPhaseDeg( int ch );
int Ior_IsValid( int ch );

#endif /* INC_CALC_IOR_H_ */

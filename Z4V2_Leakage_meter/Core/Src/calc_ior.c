/// @file   calc_ior.c
/// @brief   3600SPS の電圧・漏電流サンプルから I0r（抵抗分漏れ電流）を求める
/// @author  y.sugawara
/// @date    2026/09/28
/// @version 1.0
///
/// 100ms（360 点）ごとに VAC1/VAC2/LAC1-4 の 50Hz・60Hz 成分を DFT で求め、
/// 基準電圧に対する I0 の位相から抵抗分の比率を出す。
/// 振幅は実績のある既存の I0 値（GetLInstValue：校正・平均化済み）を使い、ここでは角度だけを求める。
///
///   1P2W / 3P4W : Vref = V1                       I0r = I0 × |cosθ|
///   1P3W        : Vref = V1 (L1-N)                I0r = I0 × |cosθ|   （L2 側の漏電は 180° になる）
///   3P3W(S接地) : Vref = V1 + V2 (= V_RS + V_TS)  I0r = I0 × |cosθ| / cos30°
///                 V_RS と V_TS の二等分線に射影すると、平衡した対地静電容量分が消える

#include "prj.h"

#define COS30 0.866025404f
#define RAD2DEG (180.0f / (float)M_PI)

/* ADC2: ADC クロック 144MHz/4 = 36MHz、(47.5 + 12.5) サイクル / ch */
#define ADC2_CONV_US ((47.5f + 12.5f) / 36.0f)

/*
 * I0 側のハードウェア遅延 [µs]（CT 種別 g_setup.ct_type[] ごと）。キャリブレーションして設定する。
 * 並びは PRM_LEAKAGE_CT_OTG_LA21, MZ1H, OTG_LA21x10, MZ1Hx10
 */
static const float ior_i0_delay_us[4] = {
  0.0f,  // PRM_LEAKAGE_CT_OTG_LA21
  0.0f,  // PRM_LEAKAGE_CT_MZ1H
  0.0f,  // PRM_LEAKAGE_CT_OTG_LA21x10
  0.0f,  // PRM_LEAKAGE_CT_MZ1Hx10
};

st_ior ior_t;

static float cos50[72]; // cos(2π·k/72) : 50Hz @3600SPS の 1 周期
static float cos60[60]; // cos(2π·k/60) : 60Hz @3600SPS の 1 周期

static void ior_window_end( void );

/// @brief 初期化（計測モード移行時に呼ぶ）
void Ior_Init( void )
{
  for( int k = 0; k < 72; k++ ){
    cos50[k] = cosf(2.0f * (float)M_PI * (float)k / 72.0f);
  }
  for( int k = 0; k < 60; k++ ){
    cos60[k] = cosf(2.0f * (float)M_PI * (float)k / 60.0f);
  }
  memset( &ior_t, 0, sizeof(ior_t) );
  for( int k = 0; k < IOR_SIG_NUM; k++ ){
    ior_t.dc[k] = 4096 / 2; // ADCの中点
  }
}

/// @brief 1 サンプル（ADC2 の 6ch 分）を入れる。360 点ごとに I0r を更新する
/// @param buf ADC2 生値 buf[0..5]（並びは calc_ior.h 参照）
void Ior_PushSample( const int16_t *buf )
{
  uint16_t n = ior_t.n;
  uint16_t i50 = n % 72;
  uint16_t i60 = n % 60;
  float c50 = cos50[i50];
  float s50 = cos50[(i50 + 54) % 72]; // sin(x) = cos(x - π/2) : 72/4 = 18 戻す
  float c60 = cos60[i60];
  float s60 = cos60[(i60 + 45) % 60]; // 60/4 = 15 戻す

  for( int k = 0; k < IOR_SIG_NUM; k++ ){
    float x = (float)buf[k] - ior_t.dc[k];
    ior_t.sum[k] += (float)buf[k];
    // X = Σ x·e^{-jωn}
    ior_t.re50[k] += x * c50;
    ior_t.im50[k] -= x * s50;
    ior_t.re60[k] += x * c60;
    ior_t.im60[k] -= x * s60;
  }

  ior_t.n++;
  if( ior_t.n >= IOR_WIN_SAMPLES ){
    ior_window_end();
  }
}

/// @brief 複素数を角度 rad だけ回す
static inline void rotate( float *re, float *im, float rad )
{
  float c = cosf(rad);
  float s = sinf(rad);
  float r = *re * c - *im * s;
  float i = *re * s + *im * c;
  *re = r;
  *im = i;
}

static float wrap_deg( float deg )
{
  while( deg > 180.0f ) deg -= 360.0f;
  while( deg <= -180.0f ) deg += 360.0f;
  return deg;
}

/// @brief 360 点そろったら位相を計算して I0r を更新する
static void ior_window_end( void )
{
  /* ---- 周波数判定（VAC1 の 50/60Hz 成分の大きさ、ヒステリシス付き） ---- */
  float p50 = ior_t.re50[IOR_SIG_VAC1] * ior_t.re50[IOR_SIG_VAC1] + ior_t.im50[IOR_SIG_VAC1] * ior_t.im50[IOR_SIG_VAC1];
  float p60 = ior_t.re60[IOR_SIG_VAC1] * ior_t.re60[IOR_SIG_VAC1] + ior_t.im60[IOR_SIG_VAC1] * ior_t.im60[IOR_SIG_VAC1];
  if( p60 > 1.3f * p50 ) ior_t.use60 = 1;
  else if( p50 > 1.3f * p60 ) ior_t.use60 = 0;

  const float *re = ior_t.use60 ? ior_t.re60 : ior_t.re50;
  const float *im = ior_t.use60 ? ior_t.im60 : ior_t.im50;
  float omega_us = 2.0f * (float)M_PI * (ior_t.use60 ? 60.0f : 50.0f) * 1.0e-6f; // rad/µs

  /* ---- 各信号のフェーザ。ADC スキャン順による時間差だけ補正する ----
   * Rank k は先頭より k×ADC2_CONV_US 遅くサンプルされる = 位相が ωδ 進んで見えるので戻す */
  float xr[IOR_SIG_NUM], xi[IOR_SIG_NUM];
  for( int k = 0; k < IOR_SIG_NUM; k++ ){
    xr[k] = re[k];
    xi[k] = im[k];
    rotate( &xr[k], &xi[k], -omega_us * ADC2_CONV_US * (float)k );
  }

  /* ---- 基準電圧 ---- */
  float vr = xr[IOR_SIG_VAC1];
  float vi = xi[IOR_SIG_VAC1];
  float v1_mag = sqrtf(vr * vr + vi * vi);
  float sr = xr[IOR_SIG_VAC1] + xr[IOR_SIG_VAC2];
  float si = xi[IOR_SIG_VAC1] + xi[IOR_SIG_VAC2];
  ior_t.v12_ratio = (v1_mag > 0.0f) ? sqrtf(sr * sr + si * si) / v1_mag : 0.0f;

  float k_wire = 1.0f;
  if( g_setup.ac_phase_wire == PRM_PHASE_WIRE_3P3W ){
    vr = sr;
    vi = si;
    k_wire = 1.0f / COS30;
  }
  ior_t.v_mag = sqrtf(vr * vr + vi * vi);

  if( ior_t.v_mag >= IOR_V_MIN_DFT ){
    float ur = vr / ior_t.v_mag;
    float ui = vi / ior_t.v_mag;

    for( int ch = 0; ch < IOR_CH_NUM; ch++ ){
      st_ior_ch *p = &ior_t.ch[ch];
      int k = IOR_SIG_LAC1 + ch;

      // c = I0 · conj(Vref/|Vref|) : Vref を 0° としたときの I0
      float cr = xr[k] * ur + xi[k] * ui;
      float ci = xi[k] * ur - xr[k] * ui;
      p->re_f += IOR_EWMA_ALPHA * (cr - p->re_f);
      p->im_f += IOR_EWMA_ALPHA * (ci - p->im_f);

      uint8_t ct = g_setup.ct_type[ch];
      float tau_i = (ct < 4) ? ior_i0_delay_us[ct] : 0.0f;
      p->phase_raw_deg = atan2f(p->im_f, p->re_f) * RAD2DEG;
      p->phase_deg = wrap_deg( p->phase_raw_deg + omega_us * (tau_i - IOR_V_DELAY_US) * RAD2DEG );

      float ratio = fabsf(cosf(p->phase_deg / RAD2DEG)) * k_wire;
      p->i0r = GetLInstValue(ch) * ratio;
      p->valid = 1;
    }
  }else{
    for( int ch = 0; ch < IOR_CH_NUM; ch++ ){
      ior_t.ch[ch].i0r = 0.0f;
      ior_t.ch[ch].valid = 0;
    }
  }

  /* ---- 次の窓へ ---- */
  for( int k = 0; k < IOR_SIG_NUM; k++ ){
    ior_t.dc[k] = ior_t.sum[k] / (float)IOR_WIN_SAMPLES;
    ior_t.sum[k] = 0.0f;
    ior_t.re50[k] = 0.0f;
    ior_t.im50[k] = 0.0f;
    ior_t.re60[k] = 0.0f;
    ior_t.im60[k] = 0.0f;
  }
  ior_t.n = 0;
  ior_t.win_count++;
}

/// @brief I0r [mA]
float Ior_GetI0r( int ch )
{
  if( ch < 0 || ch >= IOR_CH_NUM ) return 0.0f;
  return ior_t.ch[ch].i0r;
}

/// @brief 補正後の位相 [deg]（+ で I0 が進み）
float Ior_GetPhaseDeg( int ch )
{
  if( ch < 0 || ch >= IOR_CH_NUM ) return 0.0f;
  return ior_t.ch[ch].phase_deg;
}

/// @brief 1: 電圧があり I0r が有効
int Ior_IsValid( int ch )
{
  if( ch < 0 || ch >= IOR_CH_NUM ) return 0;
  return ior_t.ch[ch].valid;
}

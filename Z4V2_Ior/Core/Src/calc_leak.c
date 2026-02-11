/*
 * calc_leak.c
 *
 *  Created on: 2026/02/11
 *      Author: ysuga
 */

#include "prj.h"

/* leak_1hz.h / leak_1hz.c 相当：1ファイルで完結する例 */
#include <stdint.h>
#include <math.h>



/* --------- internal helpers --------- */
static inline void leak_reset_goertzel(Leak1Hz *st)
{
  st->s1_50 = 0.0f;
  st->s2_50 = 0.0f;
}

static inline float leak_hpf_update(Leak1Hz *st, float x)
{
  float y = (x - st->x_prev) + st->hpf_a * st->y_prev;
  st->x_prev = x;
  st->y_prev = y;
  return y;
}

static inline void leak_goertzel_update_50(Leak1Hz *st, float x)
{
  float s0 = x + st->coeff50 * st->s1_50 - st->s2_50;
  st->s2_50 = st->s1_50;
  st->s1_50 = s0;
}

static inline float leak_goertzel_power_50(const Leak1Hz *st)
{
  return st->s1_50*st->s1_50 + st->s2_50*st->s2_50 - st->coeff50*st->s1_50*st->s2_50;
}

/* --------- public API --------- */

/**
 * 初期化
 * - fs_hz: ADCサンプリング周波数（例: 3600）
 * - K_mA : leak_raw[mA] = K_mA * sqrt(power) の換算係数（既存のK）
 * - alpha: EWMA係数（例: 0.30f だと体感速い）
 * - deadband_mA: 0付近を0表示にする閾値（例: 0.10f）
 * - hpf_a: DC除去係数（例: 0.995f）
 */
void Leak1Hz_Init(Leak1Hz *st, uint32_t fs_hz, float K_mA, float alpha, float deadband_mA, float hpf_a)
{
  st->fs_hz = fs_hz;
  st->nwin  = fs_hz;         // 1秒窓
  st->alpha = alpha;
  st->deadband_mA = deadband_mA;
  st->K_mA  = K_mA;

  st->hpf_a = hpf_a;
  st->x_prev = 0.0f;
  st->y_prev = 0.0f;

  st->coeff50 = 2.0f * cosf(2.0f * (float)M_PI * 50.0f / (float)fs_hz);
  leak_reset_goertzel(st);

  st->sample_count = 0;

  st->offset_mA = 0.0f;
  st->offset_valid = 0;

  st->filt_mA = 0.0f;
  st->last_mA = 0.0f;
}

/**
 * 0mA状態で数秒回して offset を決めたい場合用（任意）
 * - 0mAで安定しているとき、表示値（生のleak_rawでもOK）を10秒程度平均して設定するのが簡単
 */
void Leak1Hz_SetOffset_mA(Leak1Hz *st, float offset_mA)
{
  st->offset_mA = offset_mA;
  st->offset_valid = 1;
}

/**
 * ADCのサンプル（AD値）を渡す。
 * - samples: ADCの生データ配列（例: int16_t）
 * - n      : サンプル数
 * - out_mA : 1秒窓が終わったタイミングで最新漏電[mA]が入る
 *
 * return:
 *   1秒に1回だけ 1 を返す（out_mAが有効）
 *   それ以外は 0
 */
int Leak1Hz_PushSamples(Leak1Hz *st, const int16_t *samples, uint32_t n, float *out_mA)
{
  for (uint32_t i = 0; i < n; i++)
  {
    float x = (float)samples[i];

    /* DC除去（軽量HPF） */
    float y = leak_hpf_update(st, x);

    /* 50Hz Goertzel 更新 */
    leak_goertzel_update_50(st, y);

    st->sample_count++;

    if (st->sample_count >= st->nwin)
    {
      /* ---- 1秒ぶん確定 ---- */
      float p50 = leak_goertzel_power_50(st);
      if (p50 < 0.0f) p50 = 0.0f;

      /* mA換算（既存方式） */
      float leak_raw_mA = st->K_mA * sqrtf(p50);

      /* オフセット補正 */
      float x_mA = leak_raw_mA;
      if (st->offset_valid) {
        x_mA -= st->offset_mA;
      }
      if (x_mA < 0.0f) x_mA = 0.0f;

      /* EWMA（表示安定化） */
      st->filt_mA = st->filt_mA + st->alpha * (x_mA - st->filt_mA);

      /* デッドバンド（0付近を0表示） */
      if (st->filt_mA < st->deadband_mA) st->filt_mA = 0.0f;

      st->last_mA = st->filt_mA;
      if (out_mA) *out_mA = st->last_mA;

      /* 次の1秒へ */
      leak_reset_goertzel(st);
      st->sample_count = 0;

      return 1; // 1秒に1回だけここに来る
    }
  }
  return 0;
}

/* いつでも最新値が欲しい場合 */
float Leak1Hz_GetLast_mA(const Leak1Hz *st)
{
  return st->last_mA;
}

/// @file   calc_leak.c
/// @brief   ADCデータから漏電計算
/// @author  y.sugawara
/// @date    2026/04/19
/// @version 1.0 

#include "prj.h"

/* leak_1hz.h / leak_1hz.c 相当：1ファイルで完結する例 */
#include <stdint.h>
#include <math.h>
#include <stdint.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


/* -------- internal helpers -------- */
static inline void reset_goertzel_50(Leak100ms_5060 *st){ st->s1_50 = 0.0f; st->s2_50 = 0.0f; }
static inline void reset_goertzel_60(Leak100ms_5060 *st){ st->s1_60 = 0.0f; st->s2_60 = 0.0f; }

static inline float hpf_update(Leak100ms_5060 *st, float x)
{
  float y = (x - st->x_prev) + st->hpf_a * st->y_prev;
  st->x_prev = x;
  st->y_prev = y;
  return y;
}

static inline void goertzel_update_50(Leak100ms_5060 *st, float x)
{
  float s0 = x + st->coeff50 * st->s1_50 - st->s2_50;
  st->s2_50 = st->s1_50;
  st->s1_50 = s0;
}
static inline void goertzel_update_60(Leak100ms_5060 *st, float x)
{
  float s0 = x + st->coeff60 * st->s1_60 - st->s2_60;
  st->s2_60 = st->s1_60;
  st->s1_60 = s0;
}

static inline float goertzel_power_50(const Leak100ms_5060 *st)
{
  return st->s1_50*st->s1_50 + st->s2_50*st->s2_50 - st->coeff50*st->s1_50*st->s2_50;
}
static inline float goertzel_power_60(const Leak100ms_5060 *st)
{
  return st->s1_60*st->s1_60 + st->s2_60*st->s2_60 - st->coeff60*st->s1_60*st->s2_60;
}

/* -------- public API -------- */

void Leak100ms_5060_Init(Leak100ms_5060 *st,
                       uint32_t fs_hz,
                       float K_mA,
                       float alpha,
                       float deadband_mA,
                       float hpf_a,
                       float fsel_ratio /* e.g. 1.30f */)
{
  st->fs_hz = fs_hz;
  st->nwin  = fs_hz / 10;      // 0.1 second
  st->K_mA  = K_mA;
  st->alpha = alpha;
  st->deadband_mA = deadband_mA;

  st->hpf_a = hpf_a;
  st->x_prev = 4096/2; //ADCの中点を指す
  st->y_prev = 4096/2; //ADCの中点を指す

  st->coeff50 = 2.0f * cosf(2.0f * (float)M_PI * 50.0f / (float)fs_hz);
  st->coeff60 = 2.0f * cosf(2.0f * (float)M_PI * 60.0f / (float)fs_hz);
  reset_goertzel_50(st);
  reset_goertzel_60(st);

  st->use60 = 0;
  st->use60_prev = 0;
  st->fsel_ratio = fsel_ratio;

  st->sample_count = 0;

  st->offset_mA = 0.0f;
  st->offset_valid = 0;

  st->filt_mA = 0.0f;
  st->last_mA = 0.0f;
}

void Leak100ms_5060_SetOffset_mA(Leak100ms_5060 *st, float offset_mA)
{
  st->offset_mA = offset_mA;
  st->offset_valid = 1;
}

/**
 * Push ADC samples. Return 1 once per 0.1 second, with out_mA updated.
 * Otherwise return 0.
 */
int Leak100ms_5060_PushSamples(Leak100ms_5060 *st, const int16_t *samples, uint32_t n, float *out_mA)
{
  for (uint32_t i = 0; i < n; i++)
  {
    float x = (float)samples[i];

    /* DC removal */
    float y = hpf_update(st, x);

    /* Update both 50 and 60 */
    goertzel_update_50(st, y);
    goertzel_update_60(st, y);

    st->sample_count++;

    if (st->sample_count >= st->nwin)
    {
      float p50 = goertzel_power_50(st);
      float p60 = goertzel_power_60(st);
      if (p50 < 0.0f) p50 = 0.0f;
      if (p60 < 0.0f) p60 = 0.0f;

      /* Frequency select with hysteresis */
      int use60;
      if (p60 > st->fsel_ratio * p50)      use60 = 1;
      else if (p50 > st->fsel_ratio * p60) use60 = 0;
      else   use60 = st->use60_prev;

      st->use60 = use60;
      st->use60_prev = use60;

      float power = (st->use60 ? p60 : p50);

      /* Convert power -> mA (your existing scaling) */
      /* 10x correction for 0.1sec window (vs original 1sec) */
      float leak_raw_mA = st->K_mA * sqrtf(power) * 10.0f;

      /* Offset correction */
      float x_mA = leak_raw_mA;
      if (st->offset_valid) x_mA -= st->offset_mA;
      if (x_mA < 0.0f) x_mA = 0.0f;

      /* EWMA */
      st->filt_mA = st->filt_mA + st->alpha * (x_mA - st->filt_mA);

      /* Deadband */
      if (st->filt_mA < st->deadband_mA) st->filt_mA = 0.0f;

      st->last_mA = st->filt_mA;
      if (out_mA) *out_mA = st->last_mA;

      /* Next 0.1 second */
      reset_goertzel_50(st);
      reset_goertzel_60(st);
      st->sample_count = 0;

      return 1;
    }
  }
  return 0;
}

float Leak100ms_5060_GetLast_mA(const Leak100ms_5060 *st)
{
  return st->last_mA;
}

int Leak100ms_5060_Is60Hz(const Leak100ms_5060 *st)
{
  return st->use60;
}

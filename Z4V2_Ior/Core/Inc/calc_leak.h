/*
 * calc_leak.h
 *
 *  Created on: 2026/02/11
 *      Author: ysuga
 */

#ifndef INC_CALC_LEAK_H_
#define INC_CALC_LEAK_H_

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct
{
  /* ---- Config ---- */
  uint32_t fs_hz;        // sampling rate (e.g. 1800)
  uint32_t nwin;         // samples per 1-second window (=fs_hz)
  float    alpha;        // EWMA alpha (e.g. 0.30f)
  float    deadband_mA;  // e.g. 0.10f
  float    K_mA;         // mA per sqrt(power): your existing scaling

  /* DC removal (1st-order HPF): y = x - x_prev + a*y_prev */
  float hpf_a;           // e.g. 0.995f
  float x_prev;
  float y_prev;

  /* Goertzel for 50Hz */
  float coeff50;
  float s1_50;
  float s2_50;

  /* 1Hz output */
  uint32_t sample_count; // 0..nwin-1
  float offset_mA;       // zero offset to subtract (optional)
  uint8_t offset_valid;

  float filt_mA;         // EWMA state
  float last_mA;         // latest output (after offset+EWMA)
} Leak1Hz;

void Leak1Hz_Init(Leak1Hz *st, uint32_t fs_hz, float K_mA, float alpha, float deadband_mA, float hpf_a);
int Leak1Hz_PushSamples(Leak1Hz *st, const int16_t *samples, uint32_t n, float *out_mA);

float Leak1Hz_GetLast_mA(const Leak1Hz *st);
#endif /* INC_CALC_LEAK_H_ */

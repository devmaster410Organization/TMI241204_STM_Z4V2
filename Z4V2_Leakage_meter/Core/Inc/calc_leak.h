/// @file    calc_leak.h
/// @brief   漏れ電流計算ヘッダ
/// @author  Y.Sugawara
/// @date    2026/2/11
/// @version 1.0

#ifndef INC_CALC_LEAK_H_
#define INC_CALC_LEAK_H_


#define ADC_HZ	(3600.0)
#define I32_ADC_HZ ((int32_t)ADC_HZ)
#define ADC_CYC	(1.0/ADC_HZ)

#define V_ADC_HZ ((int32_t)ADC_HZ/10)
#define V_ADC_CYC	(1.0/ADC_HZ)



#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct
{
  /* ---- Config ---- */
  uint32_t fs_hz;          // sampling rate (e.g. 1800)
  uint32_t nwin;           // samples per 1-second window (=fs_hz)
  float    alpha;          // EWMA alpha (e.g. 0.30f)
  float    deadband_mA;    // e.g. 0.10f
  float    K_mA;           // mA per sqrt(power)
  float    hpf_a;          // DC removal: y = x - x_prev + a*y_prev

  /* DC removal state */
  float x_prev;
  float y_prev;

  /* Goertzel 50Hz */
  float coeff50;
  float s1_50;
  float s2_50;

  /* Goertzel 60Hz */
  float coeff60;
  float s1_60;
  float s2_60;

  /* Frequency decision */
  int   use60;             // 0: use 50Hz, 1: use 60Hz
  int   use60_prev;
  float fsel_ratio;        // hysteresis ratio, e.g. 1.30

  /* 1Hz output */
  uint32_t sample_count;   // 0..nwin-1
  float offset_mA;         // zero offset to subtract (optional)
  uint8_t offset_valid;

  float filt_mA;           // EWMA state
  float last_mA;           // latest output
} Leak1Hz_5060;

void Leak1Hz_5060_Init(Leak1Hz_5060 *st,
                       uint32_t fs_hz,
                       float K_mA,
                       float alpha,
                       float deadband_mA,
                       float hpf_a,
                       float fsel_ratio /* e.g. 1.30f */);
void Leak1Hz_5060_SetOffset_mA(Leak1Hz_5060 *st, float offset_mA);
int Leak1Hz_5060_PushSamples(Leak1Hz_5060 *st, const int16_t *samples, uint32_t n, float *out_mA);
float Leak1Hz_5060_GetLast_mA(const Leak1Hz_5060 *st);
int Leak1Hz_5060_Is60Hz(const Leak1Hz_5060 *st);

#endif
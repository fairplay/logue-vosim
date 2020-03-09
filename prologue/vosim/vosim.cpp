/*
 * File: vosim.cpp
 *
 * VOSIM oscillator
 * https://kaegi.nl/werner/userfiles/downloads/vosim-system.pdf
 *
 */

#include "userosc.h"

// I prefer to stick to original Kaegi's variables here, so one letter names are kept
typedef struct State {
  float w0;
  float phase;
  float M; // M
  float b; // Attenuation constant
  uint16_t NP; // Number of periods
  uint16_t Freq; // Freq is proportional to 1/T (where T is pulsewidth it terms of Kaegi's paper)
  uint16_t lfo_target;
  float lfo, lfoz;
  uint16_t flags;
} State;

static State s;

enum {
  k_flags_none = 0,
  k_flag_reset = 1<<0,
};

enum {
    lfo_target_M,
    lfo_target_b
};

void OSC_INIT(uint32_t platform, uint32_t api)
{
  s.w0 = 0.f;
  s.phase = 0.f;
  s.M = 0.2f;
  s.b = 0.8f;
  s.Freq = 8;
  s.NP = 6;
  s.lfo = s.lfoz = 0.f;
  s.flags = k_flags_none;
}

void OSC_CYCLE(const user_osc_param_t * const params,
               int32_t *yn,
               const uint32_t frames)
{
  const uint16_t flags = s.flags;
  s.flags = k_flags_none;

  const float w0 = s.w0 = osc_w0f_for_note((params->pitch)>>8, params->pitch & 0xFF);
  float phase = (flags & k_flag_reset) ? 0.f : s.phase;

  const float M = s.M;
  const float b = s.b;
  const uint16_t NP = s.NP;
  const uint16_t Freq = s.Freq;

  const float lfo = s.lfo = q31_to_f32(params->shape_lfo);
  float lfoz = (flags & k_flag_reset) ? lfo : s.lfoz;
  const float lfo_inc = (lfo - lfoz) / frames;

  q31_t * __restrict y = (q31_t *)yn;
  const q31_t * y_e = y + frames;

  for (; y != y_e; ) {
    float tri_lfo = lfoz > 0.5f ? 1.f - lfoz : lfoz;
    tri_lfo = 2.f * tri_lfo - 0.5f;

    float delay, atten;

    if (s.lfo_target == lfo_target_M) {
        delay = clipminmaxf(0.1f, M + tri_lfo, 0.8f);
        atten = b;
    } else {
        delay = M;
        atten = clipminmaxf(-1.f, b + tri_lfo, 1.f);
    }

    float sine_zone = 1.f - delay;
    float period_width = sine_zone / Freq;
    if (period_width < w0) {
        period_width = w0;
        sine_zone = w0 * Freq;
    }
    
    float out = 0.f;

    if (phase <= sine_zone) {
        float period = phase / period_width;
        uint16_t period_number = (uint16_t)period;
        float period_shift = period - period_number;

        out = osc_sinf(period_shift / 2.f);
        out *= out;

        for (uint16_t i = 0; i < period_number; i++) {
            out = i < NP - 1 ? out * atten : 0;
        }
    }

    const float sig = osc_softclipf(0.05f, out);
    *(y++) = f32_to_q31(sig);

    phase += w0;
    phase -= (uint32_t)phase;

    lfoz += lfo_inc;
  }

  s.phase = phase;
  s.lfoz = lfoz;
}

void OSC_NOTEON(const user_osc_param_t * const params)
{
  s.flags |= k_flag_reset;
}

void OSC_NOTEOFF(const user_osc_param_t * const params)
{
  (void)params;
}

void OSC_PARAM(uint16_t index, uint16_t value)
{
  const float valf = param_val_to_f32(value);

  switch (index) {
  case k_user_osc_param_id1:
    s.Freq = value;
  case k_user_osc_param_id2:
    s.NP = value;
  case k_user_osc_param_id3:
    s.lfo_target = value;
  case k_user_osc_param_id4:
  case k_user_osc_param_id5:
  case k_user_osc_param_id6:
    break;
  case k_user_osc_param_shape:
    s.M = valf * 0.7f + 0.1f;
    break;
  case k_user_osc_param_shiftshape:
    s.b = 2.f * valf - 1.f;
    break;
  default:
    break;
  }
}


#include <iostream>
#include <cmath>
#include <cstdlib>
#include "instrument_fmvib.h"
#include "keyvalue.h"

using namespace upc;
using namespace std;

InstrumentFMVib::InstrumentFMVib(const std::string &param) 
  : adsr(SamplingRate, param) {
  bActive = false;
  x.resize(BSIZE);

  KeyValue kv(param);

  // Índice de modulación en semitonos (luego se convierte a lineal)
  float I_semitones = 0;
  if (kv.to_float("I", I_semitones))
    I = 1. - pow(2, -I / 12.);
    //I = pow(2.0, I_semitones / 12.0f);
  else
    I = 1.0;

  if (!kv.to_float("N1", N1))
    N1 = 1.0;

  if (!kv.to_float("N2", N2))
    N2 = 1.0;

  if (!kv.to_float("A", A))
    A = 1.0;

  float Ivib_semitones = 0;
  if (!kv.to_float("Ivib", Ivib_semitones))
    Ivib_semitones = 0.0f;

  // Convertir profundidad de vibrato de semitonos a factor de frecuencia
  Ivib = pow(2.0f, Ivib_semitones / 12.0f) - 1.0f;

  if (!kv.to_float("fm_vib", fm_vib))
    fm_vib = 5.0f;

  phase_vib = 0.0f;
  inc_vib = 2 * M_PI * fm_vib / SamplingRate;

  phase_c = phase_m = 0;
  inc_c = inc_m = 0;
}


void InstrumentFMVib::command(long cmd, long note, long velocity) {
  if (cmd == 9) { // Note ON
    phase_vib = 0.0f;
    inc_vib = 2 * M_PI * fm_vib / SamplingRate;
    bActive = true;
    adsr.start();
    
    float f0 = 440.0f * pow(2.0f, (note - 69) / 12.0f); // Convert MIDI to Hz

    // Incrementos por muestra
    inc_c = 2 * M_PI * f0 * N1 / SamplingRate;
    inc_m = 2 * M_PI * f0 * N2 / SamplingRate;

    phase_c = phase_m = 0.0f;

    A *= velocity / 127.0f; // Ajustar amplitud según velocidad
  }
  else if (cmd == 8) { // Note OFF
    adsr.stop();
  }
  else if (cmd == 0) { // Extinguir nota
    adsr.end();
  }
}


const vector<float> & InstrumentFMVib::synthesize() {
  if (!adsr.active()) {
    x.assign(x.size(), 0);
    bActive = false;
    return x;
  }
  else if (!bActive)
    return x;

  for (unsigned int i = 0; i < x.size(); ++i) {
    float vib = Ivib * sin(phase_vib);          // vibrato proporcional
    float mod = I * sin(phase_m);               // FM rápida (timbre)
    x[i] = A * sin(phase_c + mod);              // señal final

    // Avanzar fases
    phase_c += inc_c * (1.0f + vib);            // aplicar vibrato a la portadora
    if (phase_c >= 2 * M_PI) phase_c -= 2 * M_PI;

    phase_m += inc_m;
    if (phase_m >= 2 * M_PI) phase_m -= 2 * M_PI;

    phase_vib += inc_vib;
    if (phase_vib >= 2 * M_PI) phase_vib -= 2 * M_PI;
}

  adsr(x);
  return x;
}

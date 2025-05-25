#include <iostream>
#include <cmath>
#include <cstdlib>
#include "instrument_fm.h"
#include "keyvalue.h"

using namespace upc;
using namespace std;

InstrumentFM::InstrumentFM(const std::string &param) 
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

  phase_c = phase_m = 0;
  inc_c = inc_m = 0;
}


void InstrumentFM::command(long cmd, long note, long velocity) {
  if (cmd == 9) { // Note ON
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


const vector<float> & InstrumentFM::synthesize() {
  if (!adsr.active()) {
    x.assign(x.size(), 0);
    bActive = false;
    return x;
  }
  else if (!bActive)
    return x;

  for (unsigned int i = 0; i < x.size(); ++i) {
    float mod = I * sin(phase_m);
    x[i] = A * sin(phase_c + mod);

    // Avanzar fases
    phase_c += inc_c;
    if (phase_c >= 2 * M_PI) phase_c -= 2 * M_PI;

    phase_m += inc_m;
    if (phase_m >= 2 * M_PI) phase_m -= 2 * M_PI;
  }

  adsr(x);
  return x;
}

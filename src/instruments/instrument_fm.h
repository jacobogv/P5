#ifndef INSTRUMENT_FM
#define INSTRUMENT_FM

#include <vector>
#include <string>
#include "instrument.h"
#include "envelope_adsr.h"

namespace upc {
  class InstrumentFM : public upc::Instrument {
    EnvelopeADSR adsr;
    std::vector<float> x;

    float phase_c, phase_m;   // Fase portadora y moduladora
    float inc_c, inc_m;       // Incrementos de fase

    float I;                  // Índice de modulación (ya en escala lineal)
    float N1, N2;             // Multiplicadores de frecuencia
    float A;                  // Amplitud (escalada por la velocity)
  public:
    InstrumentFM(const std::string &param = "");
    void command(long cmd, long note, long velocity = 1);
    const std::vector<float> &synthesize();
    bool is_active() const { return bActive; }
  };
}

#endif

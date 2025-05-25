#ifndef SENO_H
#define SENO_H

#include "instrument.h"
#include "envelope_adsr.h"   
#include <vector>
#include <string>

class Seno : public upc::Instrument {
private:
    std::vector<float> table;
    std::vector<float> x;
    float index;
    float increment;
    float A;
    bool bActive;
    upc::EnvelopeADSR adsr;   

public:
    Seno(const std::string &param);
    void command(long cmd, long note, long vel);
    const std::vector<float> &synthesize();
};

#endif

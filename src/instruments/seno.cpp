#include "seno.h"
#include "keyvalue.h"
#include <cmath>
#include <iostream>

using namespace std;
using namespace upc;

Seno::Seno(const std::string &param) : adsr(SamplingRate, param) {
    bActive = false;
    x.resize(BSIZE);
    KeyValue kv(param);

    int N;
    if (!kv.to_int("N", N)) N = 1024;

    table.resize(N);
    for (int i = 0; i < N; ++i) {
        table[i] = sin(2 * M_PI * i / N);
    }

    index = 0;
    increment = 1.0;
    A = 1.0;
}

void Seno::command(long cmd, long note, long vel) {
    if (cmd == 9) { // Nota presionada
        bActive = true;
        adsr.start();
        A = vel / 127.0;
        float f = 440.0 * pow(2.0, (note - 69) / 12.0);
        increment = f * table.size() / SamplingRate;
        index = 0;
    }
    else if (cmd == 8) { // Nota soltada
        adsr.stop();
    }
    else if (cmd == 0) { // Fin abrupto
        adsr.end();
    }
}

const vector<float> &Seno::synthesize() {
    if (!adsr.active()) {
        x.assign(x.size(), 0.0f);
        bActive = false;
        return x;
    }
    else if (!bActive) {
        return x;
    }

    for (unsigned int i = 0; i < x.size(); ++i) {
        int idx = static_cast<int>(index) % table.size();
        x[i] = A * table[idx];
        index += increment;
    }

    adsr(x);  // Aplicar envolvente a la señal
    return x;
}

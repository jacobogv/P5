#include <cmath>
#include <fstream>
#define _USE_MATH_DEFINES

const float SR = 44100.0;
const float PI = 3.14159265359;

int main() {
    const float dur = 2.0; // segundos
    const float f_base = 440.0;
    const float f_mod = 0.5; // Hz, frecuencia de paneo
    const int N = int(dur * SR);
    
    std::ofstream file("ejemplo.wav", std::ios::binary);
    if (!file) return -1;

    // Cabecera WAV (PCM estéreo 16 bits)
    file.write("RIFF", 4);
    int fileSize = 36 + 4 * N; file.write((char*)&fileSize, 4);
    file.write("WAVEfmt ", 8);
    int fmtChunkSize = 16; file.write((char*)&fmtChunkSize, 4);
    short audioFormat = 1; file.write((char*)&audioFormat, 2); // PCM
    short numChannels = 2; file.write((char*)&numChannels, 2);
    int sampleRate = int(SR); file.write((char*)&sampleRate, 4);
    int byteRate = sampleRate * 4; file.write((char*)&byteRate, 4);
    short blockAlign = 4; file.write((char*)&blockAlign, 2);
    short bitsPerSample = 16; file.write((char*)&bitsPerSample, 2);
    file.write("data", 4);
    int dataSize = 4 * N; file.write((char*)&dataSize, 4);

    for (int i = 0; i < N; ++i) {
        float t = i / SR;
        float base = sin(2 * PI * f_base * t);
        float mod = sin(2 * PI * f_mod * t);
        float left = base * (0.5f + 0.5f * mod);
        float right = base * (0.5f - 0.5f * mod);
        short L = short(left * 32767);
        short R = short(right * 32767);
        file.write((char*)&L, 2);
        file.write((char*)&R, 2);
    }

    file.close();
    return 0;
}

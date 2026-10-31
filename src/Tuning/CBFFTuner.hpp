#pragma once
#include "../Core/CBFFTypes.hpp"
#include <string>

namespace cbff {

struct Recommendation {
    bool enoughData = false;
    int bufferFrames = 0;
    int coyoteFrames = 0;
    bool differs = false; // true if the suggestion is not what's currently set
    std::string note;     // short human-readable reason
};

// Adaptive tuning: reads the tracked stats and SUGGESTS values.
// It never changes a setting by itself - the menu applies it only when you tap Apply.
class CBFFTuner {
public:
    static Recommendation recommend(Mode m);
    static void apply(Mode m, Recommendation const& r); // user action only
    static int safeCap(Mode m);                         // highest buffer we will ever suggest
};

} // namespace cbff

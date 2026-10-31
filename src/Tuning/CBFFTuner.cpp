#include "CBFFTuner.hpp"
#include "../Core/CBFFInputManager.hpp"
#include "../Core/CBFFSettings.hpp"
#include <algorithm>
#include <vector>

namespace cbff {

static constexpr int kMinSamples = 8;
static constexpr int kMinNearMisses = 3;
static constexpr int kCoyoteCap = 5;

int CBFFTuner::safeCap(Mode m) {
    switch (m) {
        case Mode::Cube: return 10;
        case Mode::Robot: return 10;
        case Mode::Ball: return 8;
        case Mode::Spider: return 8;
        case Mode::UFO: return 6;
        case Mode::Ship: return 4;
        case Mode::Swing: return 4;
        case Mode::Wave: return 3;
        default: return 6;
    }
}

static int median(int const* data, int n) {
    std::vector<int> v(data, data + n);
    std::sort(v.begin(), v.end());
    return v[v.size() / 2];
}

Recommendation CBFFTuner::recommend(Mode m) {
    auto& st = CBFFInputManager::get().stats(m);
    auto const& cfg = CBFFSettings::get().mode(m);

    Recommendation r;
    r.bufferFrames = cfg.bufferFrames;
    r.coyoteFrames = cfg.coyoteFrames;

    int samples = st.fired + st.expired;
    bool coyoteData = isGroundJumpMode(m) && st.nearMissCoyote >= kMinNearMisses;
    if (samples < kMinSamples && !coyoteData) {
        r.note = "need more data";
        return r;
    }
    r.enoughData = true;

    std::string note;

    // 1) buffer too small? expired clicks that would have worked a few frames later
    if (st.nearMissBuffer >= kMinNearMisses) {
        int gap = std::max(1, median(st.bufferMissGaps, st.bufferMissCount));
        r.bufferFrames = std::min({cfg.bufferFrames + gap, limits::kMaxBuffer, safeCap(m)});
        note = std::to_string(st.nearMissBuffer) + " near-misses: buffer +" + std::to_string(r.bufferFrames - cfg.bufferFrames);
    }
    // 2) buffer bigger than needed? tighten so inputs stay closer to the real ones
    else if (st.fired >= 10 && st.expired == 0) {
        int total = 0;
        for (int c : st.fireAgeHist) total += c;
        int run = 0, p90 = 0;
        for (int age = 0; age < 16; ++age) {
            run += st.fireAgeHist[age];
            if (run * 10 >= total * 9) { p90 = age; break; }
        }
        int tight = std::max(1, p90 + 1);
        if (tight < cfg.bufferFrames) {
            r.bufferFrames = tight;
            note = "90% fire within " + std::to_string(p90) + "f: tighten";
        }
    }

    // 3) coyote too short?
    if (coyoteData) {
        int gap = std::max(1, median(st.coyoteMissGaps, st.coyoteMissCount));
        r.coyoteFrames = std::min({cfg.coyoteFrames + gap, limits::kMaxCoyote, kCoyoteCap});
        if (!note.empty()) note += ", ";
        note += "late clicks: coyote +" + std::to_string(r.coyoteFrames - cfg.coyoteFrames);
    }

    r.differs = (r.bufferFrames != cfg.bufferFrames) || (r.coyoteFrames != cfg.coyoteFrames);
    r.note = note.empty() ? "values look good" : note;
    return r;
}

void CBFFTuner::apply(Mode m, Recommendation const& r) {
    auto& s = CBFFSettings::get();
    auto& c = s.mode(m);
    c.bufferFrames = r.bufferFrames;
    c.coyoteFrames = r.coyoteFrames;
    s.clamp();
    s.markCustom(); // saves
}

} // namespace cbff

#ifndef CORE_LIBS_PRX_LIBSCEAUDIOOUT_INCLUDE_HOSTVOLUME_HPP
#define CORE_LIBS_PRX_LIBSCEAUDIOOUT_INCLUDE_HOSTVOLUME_HPP

#include <stdexcept>

class HostVolume {
public:
    static constexpr int MaxVolume = 100;
    static constexpr int VolumeStep = 5;

    int Volume() const { return volume; }
    bool Muted() const { return muted; }

    bool SetVolume(int value) {
        if (value < 0 || value > MaxVolume) throw std::invalid_argument("HostVolume: volume out of range");
        const bool changed = value != volume;
        volume = value;
        return changed;
    }

    bool ToggleMute() {
        muted = !muted;
        return true;
    }

    bool VolumeUp() {
        const bool wasMuted = muted;
        muted = false;
        if (volume >= MaxVolume) return wasMuted;
        volume = volume + VolumeStep > MaxVolume ? MaxVolume : volume + VolumeStep;
        return true;
    }

    bool VolumeDown() {
        const bool wasMuted = muted;
        muted = false;
        if (volume <= 0) return wasMuted;
        volume = volume - VolumeStep < 0 ? 0 : volume - VolumeStep;
        return true;
    }

private:
    int volume = MaxVolume;
    bool muted = false;
};

#endif

#ifndef CORE_LIBS_PRX_LIBSCEAUDIOOUT_INCLUDE_HOSTVOLUME_HPP
#define CORE_LIBS_PRX_LIBSCEAUDIOOUT_INCLUDE_HOSTVOLUME_HPP

class HostVolume {
public:
    static constexpr int MaxVolume = 100;
    static constexpr int VolumeStep = 5;

    int Volume() const { return volume; }

    bool VolumeUp() {
        if (volume >= MaxVolume) return false;
        volume = volume + VolumeStep > MaxVolume ? MaxVolume : volume + VolumeStep;
        return true;
    }

    bool VolumeDown() {
        if (volume <= 0) return false;
        volume = volume - VolumeStep < 0 ? 0 : volume - VolumeStep;
        return true;
    }

private:
    int volume = MaxVolume;
};

#endif

#include "sound.hpp"
#include <core/global/globals.hpp>
#include <core/objects/sound.hpp>
#include <fancy.hpp>

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

namespace Soundux::Objects
{
    std::atomic<std::uint64_t> PlayingSound::idCounter = 0;
    std::shared_ptr<PlayingSound> PlayingSound::create(const std::shared_ptr<Sound> &sound,
                                                       const AudioDevice &playbackDevice)
    {
        auto rtn = std::shared_ptr<PlayingSound>(new PlayingSound());

        rtn->sound = sound;
        rtn->id = ++idCounter;
        rtn->playbackDevice = playbackDevice;

        auto device = rtn->device.write();
        auto decoder = rtn->decoder.write();

        *device = std::make_shared<ma_device>();
        *decoder = std::make_shared<ma_decoder>();

        // TODO(): Use wide path universally
#if defined(_WIN32)
        auto res = ma_decoder_init_file_w(widen(sound.path).c_str(), nullptr, &*decoder);
#else
        auto res = ma_decoder_init_file(sound->getPath().c_str(), nullptr, decoder->get());
#endif

        if (res != MA_SUCCESS)
        {
            Fancy::fancy.logTime().failure()
                << "Failed to create decoder from file: " << sound->getPath() << " (" << res << ")" << std::endl;

            return nullptr;
        }

        rtn->sampleRate = decoder->get()->outputSampleRate;
        rtn->length = ma_decoder_get_length_in_pcm_frames(decoder->get());

        auto config = ma_device_config_init(ma_device_type_playback);

        config.pUserData = rtn.get();
        config.dataCallback = data_callback;
        config.sampleRate = rtn->sampleRate;
        config.playback.format = decoder->get()->outputFormat;
        config.playback.pDeviceID = &rtn->playbackDevice.raw.id;
        config.playback.channels = decoder->get()->outputChannels;

        if (ma_device_init(nullptr, &config, device->get()) != MA_SUCCESS)
        {
            Fancy::fancy.logTime().failure() << "Failed to create device" << std::endl;
            ma_decoder_uninit(decoder->get());
            return nullptr;
        }

        if (playbackDevice.isDefault)
        {
            if (sound->getLocalVolume())
            {
                device->get()->masterVolumeFactor = static_cast<float>(*sound->getLocalVolume()) / 100.f;
            }
            else
            {
                device->get()->masterVolumeFactor = static_cast<float>(Globals::gSettings.localVolume) / 100.f;
            }
        }
        else
        {
            if (sound->getRemoteVolume())
            {
                device->get()->masterVolumeFactor = static_cast<float>(*sound->getRemoteVolume()) / 100.f;
            }
            else
            {
                device->get()->masterVolumeFactor = static_cast<float>(Globals::gSettings.remoteVolume) / 100.f;
            }
        }

        if (ma_device_start(device->get()) != MA_SUCCESS)
        {
            ma_device_uninit(device->get());
            ma_decoder_uninit(decoder->get());

            Fancy::fancy.logTime().warning() << "Failed to start device for sound " << sound->getPath() << std::endl;
            return nullptr;
        }

        return rtn;
    }
    PlayingSound::~PlayingSound()
    {
        if (device.copy())
        {
            stop();
        }
    }
    void PlayingSound::stop()
    {
        if (device.copy() && decoder.copy())
        {
            auto device = this->device.write();
            auto decoder = this->decoder.write();

            done = true;
            ma_device_uninit(device->get());
            ma_decoder_uninit(decoder->get());

            *device = nullptr;
            *decoder = nullptr;
        }
    }
    void PlayingSound::pause()
    {
        if (device.copy())
        {
            auto device = this->device.write();
            if (ma_device_get_state(device->get()) == MA_STATE_STARTED)
            {
                ma_device_stop(device->get());
            }
            else
            {
                Fancy::fancy.logTime().warning()
                    << "Tried to pause sound " << id << " which is not playing" << std::endl;
            }
        }
    }
    bool PlayingSound::isPaused() const
    {
        auto device = this->device.read();
        return ma_device_get_state(device->get()) == MA_STATE_STOPPED;
    }
    void PlayingSound::resume()
    {
        if (device.copy())
        {
            auto device = this->device.write();
            if (ma_device_get_state(device->get()) == MA_STATE_STOPPED)
            {
                ma_device_start(device->get());
            }
            else
            {
                Fancy::fancy.logTime().warning()
                    << "Tried to resume sound " << id << " which is not paused" << std::endl;
            }
        }
    }
    void PlayingSound::repeat(bool state)
    {
        shouldRepeat = state;
    }
    std::uint64_t PlayingSound::getLength() const
    {
        return static_cast<std::uint64_t>(static_cast<double>(length) / static_cast<double>(sampleRate) * 1000);
    }
    std::uint64_t PlayingSound::getRead() const
    {
        return static_cast<std::uint64_t>((static_cast<double>(readFrames) / static_cast<double>(length)) *
                                          static_cast<double>(getLength()));
    }
    void PlayingSound::seek(const std::uint64_t &position)
    {
        auto newPos = static_cast<std::uint64_t>((static_cast<double>(position) / static_cast<double>(getLength())) *
                                                 static_cast<double>(length));

        seekTo = newPos;
        readFrames = newPos;
    }
    std::shared_ptr<Sound> PlayingSound::getSound() const
    {
        return sound;
    }
    std::uint64_t PlayingSound::getId() const
    {
        return id;
    }
    bool PlayingSound::isRepeating() const
    {
        return shouldRepeat;
    }
    AudioDevice PlayingSound::getPlaybackDevice() const
    {
        return playbackDevice;
    }
    void PlayingSound::setVolume(float newVolume)
    {
        if (device.copy())
        {
            auto device = this->device.write();
            device->get()->masterVolumeFactor = newVolume;
        }
    }
    void PlayingSound::onProgressed(const std::uint64_t &frames)
    {
        readFrames += frames;
        buffer += frames;

        if (buffer > (sampleRate / 2))
        {
            buffer = 0;
            Globals::gGui->onSoundProgressed(shared_from_this());
        }
    }
    void PlayingSound::onFinished()
    {
        if (device.copy() && done)
        {
            stop();
            Globals::gGui->onSoundFinished(shared_from_this());
        }
    }
    void PlayingSound::data_callback(ma_device *rawDevice, void *output, [[maybe_unused]] const void *input,
                                     std::uint32_t frameCount)
    {
        auto *thiz = reinterpret_cast<PlayingSound *>(rawDevice->pUserData);

        if (!thiz->done)
        {
            auto decoder = thiz->decoder.write();
            auto readFrames = ma_decoder_read_pcm_frames(decoder->get(), output, frameCount);

            if (thiz->seekTo.load().has_value())
            {
                ma_decoder_seek_to_pcm_frame(decoder->get(), thiz->seekTo.load().value());
                thiz->readFrames = thiz->seekTo.load().value();
                thiz->seekTo = std::nullopt;
            }

            if (thiz->playbackDevice.isDefault && readFrames > 0)
            {
                thiz->onProgressed(readFrames);
            }

            if (readFrames <= 0)
            {
                if (thiz->shouldRepeat)
                {
                    ma_decoder_seek_to_pcm_frame(decoder->get(), 0);
                    thiz->readFrames = 0;
                }
                else
                {
                    thiz->done = true;
                    Globals::gQueue.push_unique(thiz->id, [thiz = thiz->shared_from_this()]() { thiz->onFinished(); });
                }
            }
        }
    }
} // namespace Soundux::Objects
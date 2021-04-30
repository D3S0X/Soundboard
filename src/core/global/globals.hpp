#pragma once
#include <helper/audio/audio.hpp>
#if defined(__linux__)
#include <helper/audio/linux/pulse.hpp>
#endif
#include "objects.hpp"
#include <core/config/config.hpp>
#include <core/hotkeys/hotkeys.hpp>
#include <helper/cli/cli.hpp>
#include <helper/icons/icons.hpp>
#include <helper/rest/client.hpp>
#include <helper/rest/server.hpp>
#include <helper/threads/processing.hpp>
#include <helper/ytdl/youtube-dl.hpp>
#include <memory>
#include <ui/ui.hpp>

namespace Soundux
{
    namespace Globals
    {
        inline Objects::Data gData;
        inline Objects::Audio gAudio;
#if defined(__linux__)
        inline Objects::Pulse gPulse;
        inline Objects::IconFetcher gIcons;
#endif
        inline Objects::Config gConfig;
        inline Objects::YoutubeDl gYtdl;
        inline Objects::Hotkeys gHotKeys;
        inline Objects::Settings gSettings;
        inline std::unique_ptr<Objects::Window> gGui;
        inline Objects::ProcessingQueue<std::uintptr_t> gQueue;

        inline Objects::SounduxServer gServer;
        inline Objects::SounduxClient gClient;
        inline Objects::CommandLineInterface gCli;

        /* Allows for fast & easy sound access, is populated on start up */
        inline std::shared_mutex gSoundsMutex;
        inline std::shared_mutex gFavoritesMutex;
        inline std::map<std::uint32_t, std::reference_wrapper<Objects::Sound>> gSounds;
        inline std::map<std::uint32_t, std::reference_wrapper<Objects::Sound>> gFavorites;
    } // namespace Globals
} // namespace Soundux
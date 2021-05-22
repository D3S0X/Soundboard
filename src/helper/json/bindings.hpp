#pragma once
#include <core/global/globals.hpp>
#include <helper/version/check.hpp>
#include <nlohmann/json.hpp>

namespace nlohmann
{
    template <> struct adl_serializer<Soundux::Objects::Sound>
    {
        static void to_json(json &j, const Soundux::Objects::Sound &obj)
        {
            j = {{"name", obj.name},
                 {"hotkeys", obj.hotkeys},
                 {"hotkeySequence",
                  Soundux::Globals::gHotKeys.getKeySequence(obj.hotkeys)}, //* For frontend and config readability
                 {"id", obj.id},
                 {"path", obj.path},
                 {"isFavorite", obj.isFavorite},
                 {"modifiedDate", obj.modifiedDate}};
        }
        static void from_json(const json &j, Soundux::Objects::Sound &obj)
        {
            j.at("name").get_to(obj.name);
            j.at("hotkeys").get_to(obj.hotkeys);
            j.at("id").get_to(obj.id);
            j.at("path").get_to(obj.path);
            j.at("modifiedDate").get_to(obj.modifiedDate);
            if (j.find("isFavorite") != j.end())
            {
                j.at("isFavorite").get_to(obj.isFavorite);
            }
        }
    };
    template <> struct adl_serializer<Soundux::Objects::AudioDevice>
    {
        static void to_json(json &j, const Soundux::Objects::AudioDevice &obj)
        {
            j = {{"name", obj.name}, {"isDefault", obj.isDefault}};
        }
        static void from_json(const json &j, Soundux::Objects::AudioDevice &obj)
        {
            j.at("name").get_to(obj.name);
            j.at("isDefault").get_to(obj.isDefault);
        }
    };
    template <> struct adl_serializer<Soundux::Objects::PlayingSound>
    {
        static void to_json(json &j, const Soundux::Objects::PlayingSound &obj)
        {
            j = {
                {"sound", obj.sound},           {"id", obj.id},
                {"length", obj.length},         {"paused", obj.paused.load()},
                {"lengthInMs", obj.lengthInMs}, {"repeat", obj.repeat.load()},
                {"readFrames", obj.readFrames}, {"readInMs", obj.readInMs.load()},
            };
        }
        static void from_json(const json &j, Soundux::Objects::PlayingSound &obj)
        {
            j.at("id").get_to(obj.id);
            j.at("sound").get_to(obj.sound);
            j.at("length").get_to(obj.length);
            j.at("readFrames").get_to(obj.readFrames);
            j.at("lengthInMs").get_to(obj.lengthInMs);

            obj.paused.store(j.at("paused").get<bool>());
            obj.repeat.store(j.at("repeat").get<bool>());
            obj.readInMs.store(j.at("readInMs").get<std::uint64_t>());
        }
    };
    template <> struct adl_serializer<Soundux::Objects::Settings>
    {
        static void to_json(json &j, const Soundux::Objects::Settings &obj)
        {
            j = {
                {"theme", obj.theme},
                {"output", obj.output},
                {"viewMode", obj.viewMode},
                {"sortMode", obj.sortMode},
                {"stopHotkey", obj.stopHotkey},
                {"syncVolumes", obj.syncVolumes},
                {"selectedTab", obj.selectedTab},
                {"localVolume", obj.localVolume},
                {"remoteVolume", obj.remoteVolume},
                {"audioBackend", obj.audioBackend},
                {"deleteToTrash", obj.deleteToTrash},
                {"pushToTalkKeys", obj.pushToTalkKeys},
                {"tabHotkeysOnly", obj.tabHotkeysOnly},
                {"minimizeToTray", obj.minimizeToTray},
                {"allowOverlapping", obj.allowOverlapping},
                {"muteDuringPlayback", obj.muteDuringPlayback},
                {"useAsDefaultDevice", obj.useAsDefaultDevice},
                {"allowMultipleOutputs", obj.allowMultipleOutputs},
            };
        }

        template <typename T> static void getToIfExists(const json &j, const std::string &key, T &member)
        {
            if (j.find(key) != j.end())
            {
                j.at(key).get_to(member);
            }
        }

        static void from_json(const json &j, Soundux::Objects::Settings &obj)
        {
            getToIfExists(j, "theme", obj.theme);
            getToIfExists(j, "output", obj.output);
            getToIfExists(j, "sortMode", obj.sortMode);
            getToIfExists(j, "viewMode", obj.viewMode);
            getToIfExists(j, "stopHotkey", obj.stopHotkey);
            getToIfExists(j, "localVolume", obj.localVolume);
            getToIfExists(j, "selectedTab", obj.selectedTab);
            getToIfExists(j, "syncVolumes", obj.syncVolumes);
            getToIfExists(j, "audioBackend", obj.audioBackend);
            getToIfExists(j, "remoteVolume", obj.remoteVolume);
            getToIfExists(j, "deleteToTrash", obj.deleteToTrash);
            getToIfExists(j, "pushToTalkKeys", obj.pushToTalkKeys);
            getToIfExists(j, "minimizeToTray", obj.minimizeToTray);
            getToIfExists(j, "tabHotkeysOnly", obj.tabHotkeysOnly);
            getToIfExists(j, "allowOverlapping", obj.allowOverlapping);
            getToIfExists(j, "useAsDefaultDevice", obj.useAsDefaultDevice);
            getToIfExists(j, "muteDuringPlayback", obj.muteDuringPlayback);
        }
    };
    template <> struct adl_serializer<Soundux::Objects::Tab>
    {
        static void to_json(json &j, const Soundux::Objects::Tab &obj)
        {
            j = {{"id", obj.id}, {"name", obj.name}, {"path", obj.path}, {"sounds", obj.sounds}};
        }
        static void from_json(const json &j, Soundux::Objects::Tab &obj)
        {
            j.at("id").get_to(obj.id);
            j.at("name").get_to(obj.name);
            j.at("path").get_to(obj.path);
            j.at("sounds").get_to(obj.sounds);
        }
    };
    template <> struct adl_serializer<Soundux::Objects::Data>
    {
        static void to_json(json &j, const Soundux::Objects::Data &obj)
        {
            j = {{"height", obj.height},
                 {"width", obj.width},
                 {"tabs", obj.tabs},
                 {"soundIdCounter", obj.soundIdCounter}};
        }
        static void from_json(const json &j, Soundux::Objects::Data &obj)
        {
            j.at("soundIdCounter").get_to(obj.soundIdCounter);
            j.at("height").get_to(obj.height);
            j.at("width").get_to(obj.width);
            j.at("tabs").get_to(obj.tabs);
        }
    };
    template <> struct adl_serializer<Soundux::Objects::Config>
    {
        static void to_json(json &j, const Soundux::Objects::Config &obj)
        {
            j = {{"data", obj.data}, {"settings", obj.settings}};
        }
        static void from_json(const json &j, Soundux::Objects::Config &obj)
        {
            j.at("data").get_to(obj.data);
            j.at("settings").get_to(obj.settings);
        }
    };
    template <> struct adl_serializer<Soundux::Objects::VersionStatus>
    {
        static void to_json(json &j, const Soundux::Objects::VersionStatus &obj)
        {
            j = {{"current", obj.current}, {"latest", obj.latest}, {"outdated", obj.outdated}};
        }
        static void from_json(const json &j, Soundux::Objects::VersionStatus &obj)
        {
            j.at("latest").get_to(obj.latest);
            j.at("current").get_to(obj.current);
            j.at("outdated").get_to(obj.outdated);
        }
    };
#if defined(__linux__)
    template <> struct adl_serializer<std::shared_ptr<Soundux::Objects::IconRecordingApp>>
    {
        static void to_json(json &j, const std::shared_ptr<Soundux::Objects::IconRecordingApp> &obj)
        {
            j = {
                {"name", obj->name},
                {"appIcon", obj->appIcon},
                {"application", obj->application},
            };
        }
        static void from_json(const json &j, std::shared_ptr<Soundux::Objects::IconRecordingApp> &obj)
        {
            if (obj)
            {
                j.at("name").get_to(obj->name);
                j.at("appIcon").get_to(obj->appIcon);
                j.at("application").get_to(obj->application);
            }
        }
    };
    template <> struct adl_serializer<std::shared_ptr<Soundux::Objects::IconPlaybackApp>>
    {
        static void to_json(json &j, const std::shared_ptr<Soundux::Objects::IconPlaybackApp> &obj)
        {
            j = {
                {"name", obj->name},
                {"appIcon", obj->appIcon},
                {"application", obj->application},
            };
        }
        static void from_json(const json &j, std::shared_ptr<Soundux::Objects::IconPlaybackApp> &obj)
        {
            if (obj)
            {
                j.at("name").get_to(obj->name);
                j.at("appIcon").get_to(obj->appIcon);
                j.at("application").get_to(obj->application);
            }
        }
    };
#endif
} // namespace nlohmann
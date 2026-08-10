// pt_language mod - registers "Portuguese" as a new entry in the game's
// Settings > Idioma dropdown, via the shared mod registry's
// "settings.language_option" event, and provides the strings data
// (game/MEDIA/strings_pt.bin) the game itself looks for once that language
// is selected (default.xex's boot-time language switch maps XLanguage id 9
// to the "pt" suffix and loads D:\media\strings_pt.bin -- see
// making-mods.md's "Patching static game text/data" section).
//
// The base game's own screens (item/enemy names, dialogue, most of the
// native options screen) come from that string table and need nothing
// further here. A handful of native-options rows are synthesized by
// NocturneRecomp itself rather than read from the string table though (see
// making-mods.md's "Translating the native options screen's own text"), so
// this also publishes a "settings.native_string" translation for each of
// those.

#include <rex/system/mod_plugin.h>
#include <rex/system/mod_registry.h>
#include <rex/runtime.h>

#include <cstring>
#include <string>

namespace {

constexpr uint64_t kPortugueseLanguageId = 9;

class PtLanguageMod : public rex::system::IModPlugin {
public:
    explicit PtLanguageMod(rex::Runtime* runtime) : runtime_(runtime) {}

    // Producers register in OnCreateDialogs -- see making-mods.md's
    // "Ordering" note: NocturneRecomp itself subscribes to
    // "settings.language_option"/"settings.native_string" right after
    // Runtime exists, but before any mod's OnCreateDialogs runs, so
    // publishing here is guaranteed to reach both.
    void OnCreateDialogs(rex::ui::ImGuiDrawer* drawer) override {
        PublishLanguageOption();
        PublishNativeString("resolution_label", "Resolução");
        PublishNativeString("fullscreen_label", "Tela Cheia");
        PublishNativeString("fullscreen_off", "Desligado");
        PublishNativeString("fullscreen_on", "Ligado");
        PublishNativeString("language_label", "Idioma");
        PublishNativeString("gpu_backend_label", "Backend de GPU");
        PublishNativeString("preset_label", "Predefinição");
        PublishNativeString("custom_prompt_label", "PERSONALIZAR");
    }

private:
    void PublishLanguageOption() {
        rex::system::ModRegistry::EventPayload payload;
        payload.u64 = kPortugueseLanguageId;  // must not collide with an
                                               // existing id (built-in or
                                               // published by an
                                               // earlier-loaded mod), or
                                               // it's dropped with a WARN
                                               // log.
        const char* label = "Portuguese";
        payload.bytes = {reinterpret_cast<const uint8_t*>(label), std::strlen(label)};
        runtime_->mod_registry()->Publish("settings.language_option", payload);
    }

    // `key` must match one of native_options.cpp's kNativeStringKey*
    // constants; `value` is the UTF-8 Portuguese translation.
    void PublishNativeString(const char* key, const char* value) {
        std::string kv = std::string(key) + "=" + value;
        rex::system::ModRegistry::EventPayload payload;
        payload.u64 = kPortugueseLanguageId;
        payload.bytes = {reinterpret_cast<const uint8_t*>(kv.data()), kv.size()};
        runtime_->mod_registry()->Publish("settings.native_string", payload);
    }

    rex::Runtime* runtime_ = nullptr;
};

}  // namespace

extern "C" REX_MOD_PLUGIN_EXPORT uint32_t rex_mod_abi_version(void) {
    return rex::system::kModPluginAbiVersion;
}

extern "C" REX_MOD_PLUGIN_EXPORT rex::system::IModPlugin* rex_mod_create(
    uint32_t abi_version, const rex::system::ModHostContext* ctx) {
    if (abi_version != rex::system::kModPluginAbiVersion || !ctx) {
        return nullptr;
    }
    return new PtLanguageMod(ctx->runtime);
}

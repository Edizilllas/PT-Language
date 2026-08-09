// pt_language mod - registers "Portuguese" as a new entry in the game's
// Settings > Idioma dropdown, via the shared mod registry's
// "settings.language_option" event.
//
// This ONLY adds the dropdown option (the id/label pair). It does not, by
// itself, make the game render Portuguese text when that option is
// selected -- that's a separate piece (patching/overlaying the actual
// string data for XLanguage id 9), see making-mods.md's
// "Patching static game text/data" section.

#include <rex/system/mod_plugin.h>
#include <rex/system/mod_registry.h>
#include <rex/runtime.h>

#include <cstring>

namespace {

class PtLanguageMod : public rex::system::IModPlugin {
public:
    explicit PtLanguageMod(rex::Runtime* runtime) : runtime_(runtime) {}

    // Producers register in OnCreateDialogs -- see making-mods.md's
    // "Ordering" note: NocturneRecomp itself subscribes to
    // "settings.language_option" right after Runtime exists, but before
    // any mod's OnCreateDialogs runs, so publishing here is guaranteed to
    // reach it.
    void OnCreateDialogs(rex::ui::ImGuiDrawer* drawer) override {
        rex::system::ModRegistry::EventPayload payload;
        payload.u64 = 9;  // XLanguage id -- must not collide with an
                           // existing id (built-in or published by an
                           // earlier-loaded mod), or it's dropped with a
                           // WARN log.
        const char* label = "Portuguese";
        payload.bytes = {reinterpret_cast<const uint8_t*>(label), std::strlen(label)};
        runtime_->mod_registry()->Publish("settings.language_option", payload);
    }

private:
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

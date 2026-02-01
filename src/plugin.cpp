#include "plugin.hpp"

Plugin* pluginInstance;

void init(Plugin* p) {
    pluginInstance = p;

    // Register all modules
    p->addModel(modelAcidusVersio);
}

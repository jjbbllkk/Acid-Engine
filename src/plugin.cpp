#include "plugin.hpp"

// Define the global plugin instance
Plugin* pluginInstance;

// --- EXPORT THE ABI VERSION ---
// This is the symbol Rack looks for to verify the version.
extern "C" __attribute__((visibility("default"))) uint32_t rack_plugin_abi_version = 1;

// --- INITIALIZE THE PLUGIN ---
extern "C" __attribute__((visibility("default"))) void init(Plugin* p) {
	pluginInstance = p;
	
	// Register the module model
	p->addModel(modelAcidusVersio);
}
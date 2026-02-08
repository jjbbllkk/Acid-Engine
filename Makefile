# The path to the Rack SDK (defaults to two levels up)
RACK_DIR ?= ../..

# FLAGS -----------------------------------------------------------------------

# Add the 'src/open303' folder to the include path so we can find headers
CXXFLAGS += -I./src/open303

# SOURCES ---------------------------------------------------------------------

# 1. The new standard plugin file (handles registration & versioning)
SOURCES += src/plugin.cpp

# 2. Your module logic
SOURCES += src/AcidEngine.cpp

# 3. All the synth engine files
SOURCES += $(wildcard src/open303/*.cpp)

# DISTRIBUTABLES --------------------------------------------------------------

# Files to include in the final plugin package
DISTRIBUTABLES += res
DISTRIBUTABLES += $(wildcard LICENSE*)
DISTRIBUTABLES += plugin.json

# INCLUDE RACK FRAMEWORK ------------------------------------------------------
include $(RACK_DIR)/plugin.mk
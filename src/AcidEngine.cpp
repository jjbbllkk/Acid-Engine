#include "plugin.hpp"
#include "open303/rosic_Open303.h"

using namespace rosic;

struct AcidEngine : Module {
	enum ParamId {
		TUNING_PARAM,
		CUTOFF_PARAM,
		RESONANCE_PARAM,
		DECAY_PARAM,
		ENVMOD_PARAM,
		SLIDE_PARAM,
		ACCENT_PARAM,
		WAVEFORM_PARAM,
		MODE_PARAM,
		TRIG_BUTTON_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		TUNING_INPUT,
		CUTOFF_INPUT,
		RES_INPUT,
		ACCENT_INPUT,
		DECAY_INPUT,
		SLIDE_INPUT,
		ENVMOD_INPUT,
		TRIG_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_L_OUTPUT,
		OUT_R_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		VU_LIGHT_1,
		VU_LIGHT_2,
		VU_LIGHT_3,
		LIGHTS_LEN
	};

	Open303 tb303;
	bool gateHigh = false;

	float sampleRate = 44100.f;
	int active_note = 60;
	float vuLevel = 0.f;

	AcidEngine() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

		// Knobs
		configParam(TUNING_PARAM, -12.f, 12.f, 0.f, "Tuning", " semitones");
		configParam(CUTOFF_PARAM, 0.f, 1.f, 0.5f, "Cutoff Frequency");
		configParam(RESONANCE_PARAM, 0.f, 1.f, 0.5f, "Resonance");
		configParam(DECAY_PARAM, 0.f, 1.f, 0.5f, "Decay");
		configParam(ENVMOD_PARAM, 0.f, 1.f, 0.5f, "Envelope Mod");
		configParam(SLIDE_PARAM, 0.f, 1.f, 0.0f, "Slide");
		configParam(ACCENT_PARAM, 0.f, 1.f, 0.0f, "Accent");

		// Switches
		configSwitch(WAVEFORM_PARAM, 0.f, 2.f, 0.f, "Waveform", {"Saw", "Blend", "Square"});
		configSwitch(MODE_PARAM, 0.f, 2.f, 1.f, "Parameter Mode", {"Baby Fish", "Momma Fish", "Devil Fish"});

		// Button
		configButton(TRIG_BUTTON_PARAM, "Trigger");

		// Inputs
		configInput(TUNING_INPUT, "V/Oct Tuning");
		configInput(CUTOFF_INPUT, "Cutoff CV");
		configInput(RES_INPUT, "Resonance CV");
		configInput(ACCENT_INPUT, "Accent CV");
		configInput(DECAY_INPUT, "Decay CV");
		configInput(SLIDE_INPUT, "Slide CV");
		configInput(ENVMOD_INPUT, "Envelope Mod CV");
		configInput(TRIG_INPUT, "Trigger");

		// Outputs
		configOutput(OUT_L_OUTPUT, "Left Audio");
		configOutput(OUT_R_OUTPUT, "Right Audio");

		tb303.setSampleRate(sampleRate);
		tb303.setVolume(0);
		tb303.setWaveform(0.0);

		// Authentic 303 settings
		tb303.setAmpDecay(4000);
		tb303.setAmpRelease(15);
		tb303.setAttack(3.0f);
		tb303.setAmpAttack(3.0f);
	}

	void process(const ProcessArgs& args) override {
		if (sampleRate != args.sampleRate) {
			sampleRate = args.sampleRate;
			tb303.setSampleRate(sampleRate);
		}

		// Read mode switch - CKSSThree: top=2, bottom=0, so invert
		// Top=Baby Fish, Middle=Momma Fish, Bottom=Devil Fish
		int mode = 2 - (int)params[MODE_PARAM].getValue();

		// Mode-dependent parameter ranges
		float cutoffMin, cutoffMax, resMax, decayMin, decayMax, envmodMax, accentMax;
		switch (mode) {
			case 0: // Baby Fish - restricted ranges
				cutoffMin = 200.f; cutoffMax = 2000.f;
				resMax = 50.f;
				decayMin = 200.f; decayMax = 1000.f;
				envmodMax = 50.f;
				accentMax = 25.f;
				break;
			case 2: // Devil Fish - full extended ranges
				cutoffMin = 20.f; cutoffMax = 8000.f;
				resMax = 100.f;
				decayMin = 30.f; decayMax = 3000.f;
				envmodMax = 100.f;
				accentMax = 100.f;
				break;
			default: // Momma Fish - standard 303 ranges
				cutoffMin = 100.f; cutoffMax = 4000.f;
				resMax = 80.f;
				decayMin = 200.f; decayMax = 2000.f;
				envmodMax = 80.f;
				accentMax = 50.f;
				break;
		}

		// Read parameters with CV modulation (CV is 0-10V, scaled to 0-1 range)
		// Note: TUNING_PARAM is only for master detuning (+/- 12 semitones from A=440)
		// V/Oct pitch control is handled separately via MIDI note calculation
		float tuning = params[TUNING_PARAM].getValue();
		float cutoff = clamp(params[CUTOFF_PARAM].getValue() + inputs[CUTOFF_INPUT].getVoltage() * 0.1f, 0.f, 1.f);
		float resonance = clamp(params[RESONANCE_PARAM].getValue() + inputs[RES_INPUT].getVoltage() * 0.1f, 0.f, 1.f);
		float decay = clamp(params[DECAY_PARAM].getValue() + inputs[DECAY_INPUT].getVoltage() * 0.1f, 0.f, 1.f);
		float envmod = clamp(params[ENVMOD_PARAM].getValue() + inputs[ENVMOD_INPUT].getVoltage() * 0.1f, 0.f, 1.f);
		float slide = clamp(params[SLIDE_PARAM].getValue() + inputs[SLIDE_INPUT].getVoltage() * 0.1f, 0.f, 1.f);

		// Accent: knob controls intensity, CV acts as gate trigger
		float accentAmount = params[ACCENT_PARAM].getValue();
		bool accentTriggered = inputs[ACCENT_INPUT].getVoltage() > 2.5f;

		// Waveform switch - CKSSThree: top=2, bottom=0, so invert
		// Top=Saw, Middle=Blend, Bottom=Square
		int waveform = 2 - (int)params[WAVEFORM_PARAM].getValue();
		tb303.setWaveform(waveform * 0.5f);

		// Apply tuning (semitone offset from 440Hz)
		float tuningHz = 440.f * std::pow(2.f, tuning / 12.f);
		tb303.setTuning(tuningHz);

		// Apply mode-scaled parameters to Open303
		tb303.setCutoff(cutoffMin + cutoff * (cutoffMax - cutoffMin));
		tb303.setResonance(resonance * resMax);
		float decayMs = decayMin + decay * (decayMax - decayMin);
		tb303.setDecay(decayMs);
		// Accent decay is ~20% of normal decay (authentic 303 behavior: shorter, punchier)
		tb303.setAccentDecay(decayMs * 0.2f);
		tb303.setEnvMod(envmod * envmodMax);
		tb303.setAccent(accentAmount * accentMax);

		// Handle Gate & Note (gate input OR button)
		bool buttonPressed = params[TRIG_BUTTON_PARAM].getValue() > 0.5f;
		bool gateInput = (inputs[TRIG_INPUT].getVoltage() + (buttonPressed ? 10.f : 0.f)) > 2.5f;

		if (gateInput && !gateHigh) {
			// Rising edge — note on
			float volts = inputs[TUNING_INPUT].getVoltage();
			int midi_note = (int)std::round(volts * 12.0f + 60.0f);

			bool is_sliding = (slide > 0.05f);
			if (is_sliding) {
				tb303.setSlideTime(slide * 400.0f);
			} else {
				tb303.setSlideTime(60.0f);
			}

			// Velocity < 100 = no accent, >= 100 = accent triggered
			int velocity = accentTriggered ? 127 : 80;

			active_note = midi_note;

			if (!is_sliding) {
				tb303.allNotesOff();
				tb303.noteOn(active_note, velocity);
			} else {
				tb303.trimNoteList();
				tb303.noteOnPortamento(active_note, velocity);
			}
		}

		if (!gateInput && gateHigh) {
			// Falling edge — note off (velocity 0 triggers noteOff in Open303)
			tb303.noteOn(active_note, 0);
		}

		gateHigh = gateInput;

		float out = (float)tb303.getSample() * 5.0f;
		outputs[OUT_L_OUTPUT].setVoltage(out);
		outputs[OUT_R_OUTPUT].setVoltage(out);

		// VU meter - track peak level with decay
		float absOut = std::fabs(out) / 5.0f; // Normalize to 0-1 range
		if (absOut > vuLevel) {
			vuLevel = absOut; // Attack - instant
		} else {
			vuLevel -= 0.0001f; // Decay - slow falloff
			if (vuLevel < 0.f) vuLevel = 0.f;
		}

		// Set VU lights based on level thresholds
		lights[VU_LIGHT_1].setBrightness(vuLevel > 0.1f ? 1.f : vuLevel * 10.f);
		lights[VU_LIGHT_2].setBrightness(vuLevel > 0.4f ? 1.f : (vuLevel > 0.1f ? (vuLevel - 0.1f) * 3.33f : 0.f));
		lights[VU_LIGHT_3].setBrightness(vuLevel > 0.7f ? 1.f : (vuLevel > 0.4f ? (vuLevel - 0.4f) * 3.33f : 0.f));
	}
};

struct AcidEngineWidget : ModuleWidget {
	AcidEngineWidget(AcidEngine* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/AcidEngine.svg")));

		// Screws
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		// VU meter lights
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(24.0, 11.5)), module, AcidEngine::VU_LIGHT_1));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(30.48, 11.5)), module, AcidEngine::VU_LIGHT_2));
		addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(36.96, 11.5)), module, AcidEngine::VU_LIGHT_3));

		// Top row knobs: TUNING (left), RESONANCE (right)
		addParam(createParamCentered<Rogan1PWhite>(mm2px(Vec(12.0, 27.0)), module, AcidEngine::TUNING_PARAM));
		addParam(createParamCentered<Rogan1PWhite>(mm2px(Vec(48.96, 27.0)), module, AcidEngine::RESONANCE_PARAM));

		// Second row: CUTOFF (center, larger)
		addParam(createParamCentered<Rogan3PSWhite>(mm2px(Vec(30.48, 35.0)), module, AcidEngine::CUTOFF_PARAM));

		// Third row: DECAY (left), ENVMOD (right) - aligned with TUNE/RES columns
		addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(12.0, 45.5)), module, AcidEngine::DECAY_PARAM));
		addParam(createParamCentered<Rogan1PSWhite>(mm2px(Vec(48.96, 45.5)), module, AcidEngine::ENVMOD_PARAM));

		// Fourth row: Waveform switch (left), SLIDE (center)
		addParam(createParamCentered<CKSSThree>(mm2px(Vec(12.0, 62.0)), module, AcidEngine::WAVEFORM_PARAM));
		addParam(createParamCentered<Rogan1PWhite>(mm2px(Vec(30.48, 59.0)), module, AcidEngine::SLIDE_PARAM));

		// Fifth row: Mode switch (left), ACCENT (center), TRIG button (right)
		addParam(createParamCentered<CKSSThree>(mm2px(Vec(12.0, 76.0)), module, AcidEngine::MODE_PARAM));
		addParam(createParamCentered<Rogan1PWhite>(mm2px(Vec(30.48, 76.0)), module, AcidEngine::ACCENT_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(48.96, 76.0)), module, AcidEngine::TRIG_BUTTON_PARAM));

		// CV inputs - Row 1: TUNING, CUTOFF, RES, ACCENT
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.0, 92.0)), module, AcidEngine::TUNING_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.32, 92.0)), module, AcidEngine::CUTOFF_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(38.64, 92.0)), module, AcidEngine::RES_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(52.96, 92.0)), module, AcidEngine::ACCENT_INPUT));

		// CV inputs - Row 2: DECAY, SLIDE, ENVMOD, TRIG
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.0, 104.0)), module, AcidEngine::DECAY_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.32, 104.0)), module, AcidEngine::SLIDE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(38.64, 104.0)), module, AcidEngine::ENVMOD_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(52.96, 104.0)), module, AcidEngine::TRIG_INPUT));

		// Outputs: OUT L, OUT R
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(20.32, 118.0)), module, AcidEngine::OUT_L_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(40.64, 118.0)), module, AcidEngine::OUT_R_OUTPUT));
	}
};

// Define the Model object (this is referenced in plugin.cpp)
Model* modelAcidEngine = createModel<AcidEngine, AcidEngineWidget>("AcidEngine");
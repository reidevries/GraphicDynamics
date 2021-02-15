#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#
# Modified by Patrick Desaulniers
# Further modified by Rei de Vries
#
# --------------------------------------------------------------
# Project name, used for binaries

NAME = graphic_dynamics

# --------------------------------------------------------------
# Plugin types to build

BUILD_LV2 ?= true
BUILD_VST2 ?= true
BUILD_JACK ?= true
BUILD_DSSI ?= true

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	Common/Utils/src/Mathf.cpp \
	Common/Structures/src/Graph.cpp \
	Common/Structures/src/Oversampler.cpp \
	Common/Structures/src/ParamSmooth.cpp \
	Common/Structures/src/RCFilter.cpp \
	Libs/DSPFilters/source/Butterworth.cpp \
	Libs/DSPFilters/source/Biquad.cpp \
	Libs/DSPFilters/source/Cascade.cpp \
	Libs/DSPFilters/source/Filter.cpp \
	Libs/DSPFilters/source/Param.cpp \
	Libs/DSPFilters/source/Butterworth.cpp \
	Libs/DSPFilters/source/PoleFilter.cpp \
	Libs/DSPFilters/source/RBJ.cpp \
	Libs/DSPFilters/source/Bessel.cpp \
	Libs/DSPFilters/source/RootFinder.cpp \
	GraphicDynamicsPlugin.cpp

FILES_UI  = \
	Common/Utils/src/Mathf.cpp \
	Common/Structures/src/Graph.cpp \
	Common/Structures/src/Margin.cpp \
	Common/Structures/src/Animation.cpp \
	Common/Widgets/src/WolfWidget.cpp \
	Common/Widgets/src/GraphNode.cpp \
	Common/Widgets/src/GraphWidget.cpp \
	Common/Widgets/src/NanoSwitch.cpp \
	Common/Widgets/src/RemoveDCSwitch.cpp \
	Common/Widgets/src/NanoButton.cpp \
	Common/Widgets/src/ResetGraphButton.cpp \
	Common/Widgets/src/NanoWheel.cpp \
	Common/Widgets/src/OversampleWheel.cpp \
	Common/Widgets/src/NanoKnob.cpp \
	Common/Widgets/src/VolumeKnob.cpp \
	Common/Widgets/src/ResizeHandle.cpp \
	Common/Widgets/src/LabelBox.cpp \
	Common/Widgets/src/LabelBoxList.cpp \
	Common/Widgets/src/BipolarModeSwitch.cpp \
	Common/Widgets/src/LabelContainer.cpp \
	Common/Widgets/src/GlowingLabelsBox.cpp \
	Common/Widgets/src/NanoLabel.cpp \
	Common/Widgets/src/WidgetBar.cpp \
	Common/Widgets/src/ArrowButton.cpp \
	Common/Widgets/src/RightClickMenu.cpp \
	Config/src/Config.cpp \
	Resources/Fonts/chivo_bold.cpp \
	Resources/Fonts/chivo_italic.cpp \
	GraphicDynamicsUI.cpp

# --------------------------------------------------------------
# Do some magic

include ../Makefile.plugins.mk

# --------------------------------------------------------------
# Additional flags

ifeq ($(PARANOID_MODE),true)
BASE_FLAGS += -Wall -Wextra -Werror
endif

# --------------------------------------------------------------
# Enable all possible plugin types

ifeq ($(BUILD_JACK),true)
ifneq ($(HAVE_JACK), true)
$(error "Could not find JACK! Make sure the JACK development files are installed on your machine (libjack-jackd2-dev on a Debian-based machine).")
endif
TARGETS += jack
endif

ifeq ($(BUILD_DSSI),true)
ifneq ($(HAVE_LIBLO), true)
$(error "Could not find liblo! Make sure the liblo development files are installed on your machine (liblo-dev on a Debian-based machine).")
endif
TARGETS += dssi
endif

ifeq ($(BUILD_LV2),true)
TARGETS += lv2_sep
endif

ifeq ($(BUILD_VST2),true)
TARGETS += vst
endif

all: $(TARGETS)

# --------------------------------------------------------------

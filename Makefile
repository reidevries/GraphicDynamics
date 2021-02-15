#!/usr/bin/make -f
# Makefile for DPF #
# ---------------- #
# Created by falkTX
# Modified by Rei de Vries for GraphicDynamics

include dpf/Makefile.base.mk

all: dgl plugins gen

# --------------------------------------------------------------

ifneq ($(CROSS_COMPILING),true)
CAN_GENERATE_TTL = true
else ifneq ($(EXE_WRAPPER),)
CAN_GENERATE_TTL = true
endif

# missing submodules error taken from pdesaulniers wolf-shaper Makefile
define MISSING_SUBMODULES_ERROR

Cannot find DGL! Run "git submodule update --init --recursive" and retry

endef

dgl:
ifeq (,$(wildcard dpf/dgl))
	$(error $(MISSING_SUBMODULES_ERROR))
else
	$(MAKE) -C dpf dgl
endif

plugins: dgl
	$(MAKE) all -C plugins/graphic-dynamics

ifeq ($(CAN_GENERATE_TTL),true)
gen: plugins utils/lv2_ttl_generator
	@$(CURDIR)/dpf/utils/generate-ttl.sh
ifeq ($(MACOS),true)
	@$(CURDIR)/dpf/utils/generate-vst-bundles.sh
endif

utils/lv2_ttl_generator:
	$(MAKE) -C dpf/utils/lv2-ttl-generator
else
gen:
endif

# --------------------------------------------------------------

clean:
	$(MAKE) clean -C dpf/dgl
	$(MAKE) clean -C plugins/graphic-dynamics
	rm -rf bin build

# --------------------------------------------------------------

.PHONY: dgl plugins

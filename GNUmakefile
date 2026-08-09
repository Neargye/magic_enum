# GNUmakefile
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

MAKEFLAGS+= --no-builtin-rules          # Disable the built-in implicit rules.
MAKEFLAGS+= --no-builtin-variables      # Disable the built-in variable settings.
MAKEFLAGS+= --warn-undefined-variables  # Warn when an undefined variable is referenced.
.SUFFIXES:                              # Disable all suffix rules.
##################################################

ifeq ($(origin CXX),default)
  export CXX:= g++-16
  export CC:= gcc-16
  # export CXXFLAGS:= -stdlib=libc++ -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0
endif

export CMAKE_EXPORT_COMPILE_COMMANDS:=1

CMAKE_BUILD_TYPE?=Release
CMAKE_GENERATOR?=Ninja

PRESET?=release
IMAGE?=ghcr.io/bemanproject/infra-containers-gcc:16

_build_path:=build/$(PRESET)

.PHONY: all ctest check distclean format dockerbuild

# default target rule
all: ## Make all with cmake workflow preset
	cmake --version
	cmake -S . -B $(_build_path) -G $(CMAKE_GENERATOR) -D MAGIC_ENUM_OPT_BUILD_EXAMPLES=NO -D CMAKE_CXX_STANDARD=26 \
		-D MAGIC_ENUM_MODULE_IMPORT_STD=NO -D MAGIC_ENUM_USE_MODULES=NO -D CMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) \
		-D CMAKE_EXPERIMENTAL_CXX_IMPORT_STD="451f2fe2-a8a2-47c3-bc32-94786d8fc91b" # for cmake 4.3.x only
	ninja -C $(_build_path) all

.init: CMakeUserPresets.json CMakePresets.json CMakeLists.txt ## Configure cmake preset in VERBOSE mode
	cmake --version
	cmake --preset $(PRESET) --fresh --log-level=VERBOSE
	touch $@

check: .init compile_commands.json ## Run clang-tidy on examples
	run-clang-tidy examples

.PHONY: compile_commands.json
compile_commands.json: $(_build_path)/compile_commands.json
	if [ "X$(shell readlink compile_commands.json)" != "X$(_build_path)/compile_commands.json" ] ; then \
		ln -fs $< $@; \
	fi

CMakeUserPresets.json:: cmake/CMakeUserPresets.json
	-ln -fs $< $@

distclean: ## Remove all build artifacts
	rm -rf build .cache
	rm -f CMakeUserPresets.json \
	compile_commands.json \
	.init
	find . -name '*~' -delete

format: distclean  ## Format all files with pre-commit
	git ls-files ::*.cmake ::*CMakeLists.txt | xargs gersemi -i --indent 2 --line-length 100

dockerbuild: ## Start docker image interactive
	docker run -it -v $(CURDIR):/src $(IMAGE)

ctest: all compile_commands.json
 ## Run ctest
	ctest --test-dir $(_build_path) --rerun-failed --output-on-failure || \
	$(_build_path)/test/test_flags-cpp26 -tc='enum_c*'

# Helper targets
.PHONY: env info

env: ## Show env
	$(foreach v, $(.VARIABLES), $(info $(v) = $($(v))))

info: ## Show this help.
	@awk 'BEGIN {FS = ":.*?## "} /^[.a-zA-Z_-]+:.*?## / {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST) | sort

# NOTE: double-colon targets which has no prerequisites must always remade by gmake?
#  Prerequisite 'GNUmakefile' is newer than target '.init'.
#  -> Must remake target '.init'.
# NOTE: impizit handled by gmake! GNUmakefile :: ;
# NO if exists TODO(CK): cmake/CMakeUserPresets.json :: ;

# Anything we don't know how to build will use this rule:
% ::
	ninja -C $(_build_path) $(@)

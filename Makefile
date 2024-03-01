.PHONY: \
	all
	clean \
	clean-debug \
	clean-relwithdebinfo
	clean-release \
	debug \
	relwithdebinfo \
	release \

GEN:="Unix Makefiles"
NINJA_PATH:=$(shell command -v ninja 2> /dev/null)
ifdef NINJA_PATH
	GEN:="Ninja"
endif

build-debug:
	cmake -B $@ -DCMAKE_BUILD_TYPE=Debug -G $(GEN)

debug: build-debug
	cmake --build build-$@;

clean-debug:
	rm -rf build-debug;

build-relwithdebinfo:
	cmake -B $@ -DCMAKE_BUILD_TYPE=relwithdebinfo -G $(GEN)

relwithdebinfo: build-relwithdebinfo
	cmake --build build-$@;

clean-relwithdebinfo:
	rm -rf build-relwithdebinfo;


build:
	cmake -B $@ -DCMAKE_BUILD_TYPE=release -G $(GEN) -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

release: build
	cmake --build build;

clean-release:
	rm -rf build-release;

all: debug relwithdebinfo release
	@:

clean: clean-debug clean-relwithdebinfo clean-release
	@:

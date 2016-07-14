.PHONY: auto compile debug release clean astyle dependencies

auto: release

compile:
	cd build; make -j5

release: build dependencies
	cd build; cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
	make compile

debug: build dependencies
	cd build; cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
	make compile

clean:
	rm -rf build bin lib

build:
	mkdir -p build

astyle:
	astyle --options="extra/astyle.config" "src/*.hpp" "src/*.cpp"

dependencies:
	git submodule sync
	git submodule update --init --recursive


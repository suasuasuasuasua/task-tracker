BUILD_DIR=build
CMAKE_BUILD_TYPE=Release

build: setup
	@printf "\nBuilding...\n"
	cmake --build  ${BUILD_DIR} --parallel

setup:
	@printf "\nSetting up ...\n"
	cmake -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -S . -B ${BUILD_DIR}

test: build
	@printf "\nRunning tests\n"
	cmake --build ${BUILD_DIR} --target test

clean:
	@printf "\nCleaning build directory '${BUILD_DIR}'\n"
	cmake --build ${BUILD_DIR} --target clean

.PHONY: build setup test clean

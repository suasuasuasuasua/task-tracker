BUILD_DIR=build
CMAKE_BUILD_TYPE=Release

setup:
	@printf "\nSetting up ...\n"
	cmake -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -S . -B ${BUILD_DIR}

build: setup
	@printf "\nBuilding...\n"
	cmake --build  ${BUILD_DIR} --parallel

clean:
	@printf "\nCleaning build directory '${BUILD_DIR}'\n"
	cmake --build ${BUILD_DIR} --target clean

test: build
	@printf "\nRunning tests\n"
	cmake --build ${BUILD_DIR} --target test

.PHONY: seutp build clean

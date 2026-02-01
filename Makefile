BUILD_DIR=build
CMAKE_BUILD_TYPE=Release # or Debug, RelWithDebInfo, and MinSizeRel
CMAKE_INSTALL_PREFIX=/usr/local/bin # or ~/.local

build: setup
	@printf "\nBuilding...\n"
	cmake --build  ${BUILD_DIR} --parallel

setup:
	@printf "\nSetting up...\n"
	cmake -S . -B ${BUILD_DIR} \
		-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
		-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}

test: build
	@printf "\nRunning tests\n"
	cmake --build ${BUILD_DIR} --target test

install: build
	@printf "\nInstalling...\n"
	cmake --build ${BUILD_DIR} --target install

uninstall:
	@printf "\nUninstalling...\n"
	xargs rm < ${BUILD_DIR}/install_manifest.txt

clean:
	@printf "\nCleaning build directory '${BUILD_DIR}'\n"
	cmake --build ${BUILD_DIR} --target clean

.PHONY: build setup test install uninstall clean

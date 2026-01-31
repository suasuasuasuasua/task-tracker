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

bump:
	@if [ -z "$(VERSION)" ]; then \
		echo "Error: VERSION not specified. Usage: make bump VERSION=major|minor|patch|X.Y.Z"; \
		exit 1; \
	fi
	@./scripts/bump-version.sh $(VERSION)

.PHONY: build setup test clean bump

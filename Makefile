.PHONY: build release test

# build with tests
build:
	@cmake --preset=build
	@cmake --build build

release:
	@cmake --preset=release
	@cmake --build release

test:
	# filter test cases: make run_test cases='ToString.*'
	# run all test: make run_test cases='*'
	@./build/tests/gtest_main --gtest_filter=$(cases)

test-all:
	@./build/tests/gtest_main #--gtest_filter="*"

os-test:
	@./build/modules/object-storage/tests/os_gtest_main --gtest_filter=$(cases)


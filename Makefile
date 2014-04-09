default: build run
	
build:
	@mkdir -p build
	@cd build && cmake .. && make -j5

run:
	bin/fluidsim

clean:
	rm -r build bin

.PHONY: build clean

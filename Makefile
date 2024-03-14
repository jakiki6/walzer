all: tests

tests: tests/all
	echo "Hello world!" | LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:. ./$<

tests/all: libwalzer.so tests/all.c
	clang -O2 -o $@ tests/all.c -L. -lwalzer -Isrc

libwalzer.so: src/walzer.c src/walzer.h
	clang -O2 -Werror -shared -o $@ src/walzer.c

clean:
	rm -f tests/all libwalzer.so

.PHONY: all tests clean

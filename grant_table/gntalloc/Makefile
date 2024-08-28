.PHONY: all fmt clean

all:
	gcc -o getting_access getting_access.c
	gcc -o granting_access granting_access.c

fmt:
	clang-format -i getting_access.c
	clang-format -i granting_access.c

clean:
	rm -f getting_access granting_access



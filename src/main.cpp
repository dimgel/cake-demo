#include <stdio.h>
#include "greeter/greeter.h"


int main(int argc, char** argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s {name}\n", argv[0]);
		return 1;
	}
	greet(argv[1]);
	return 0;
}

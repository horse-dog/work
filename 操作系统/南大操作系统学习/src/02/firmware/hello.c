#include <stdio.h>
#include <math.h>

static int count = 0;

static const char* strs[] = {"hello", "world"};

int is_prime(int x) {
	int n = (int)sqrt(x) + 1;
	for (int i = 2; i < n; i++) {
		if (x % i == 0) return 0;
	}
	return x > 1;
}

int main(int argc, const char* argv[]) {
	for (int i = 0; i < 20; i++) {
		if (is_prime(i)) {
			count++;
			printf("%d\n", i);
		}
	}
	return 0;
}

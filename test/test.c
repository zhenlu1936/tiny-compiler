int global;

int add(int x, int y) { return x + y; }

int main() {
	int a, b, i;
	float c;
	a = 2;
	b = 1;
	c = 0.1;
	for (i = 0; i < 5; i++) {
		b = add(a--, b);
		if (a == b) {
			break;
		}
	}
	while (a == 1) {
		a = 1;
	}
	if (i == b) {
		i = 5;
	}
	// c = 1;
	return a;
}

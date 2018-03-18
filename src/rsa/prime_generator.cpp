#include <iostream>
#include <random>
#include <cmath>

using namespace std;

long long int generate() {
	std::random_device rd;     // only used once to initialise (seed) engine
	std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
	long long int low = 2;
	long long int high = 4;

	for (int i = 0; i < 1023; i++) {
		low *= 2;
		high *= 2;
	}

	std::uniform_int_distribution<long long int> uni(low, high); // guaranteed unbiased

	long long int random_integer = 0;

	while (true) {
		random_integer = uni(rng);
		cout << random_integer << endl;

		for (long long int i = 0; i < random_integer / 2; i++) {
			if (random_integer % i == 0) {
				continue;
			}
		}
		return random_integer;
	}
	return 0;
}


int main() {
	long long int prime_one = generate();
	long long int prime_two = generate();
	cout << prime_one << endl;
	cout << endl;
	cout << prime_two << endl;
}
#include <iostream>
#include <cassert>

#include <map>
#include <vector>
#include <unordered_map>
#include <cstring>

#include "zeek/IP.h"
#include "zeek/IPAddr.h"
#include "zeek/Sessions.h"
#include "zeek/Hash.h"

#include <netinet/in.h>
#include <arpa/inet.h>


// Just forward to HashKey::HashBytes
struct ConnIDKeyHash {

	std::size_t operator()(const zeek::detail::ConnIDKey& k) const {
		return zeek::detail::HashKey::HashBytes(&k, sizeof(zeek::detail::ConnIDKey));
	}
};

// Mostly to comapre to HashKey::HashBytes.
struct ConnIDKeyHashAddition {
	std::size_t operator()(const zeek::detail::ConnIDKey& k) const {
		assert(sizeof(k.ip1) == 16);
		const uint32_t *ip1 = (const uint32_t*)&k.ip1;
		const uint32_t *ip2 = (const uint32_t*)&k.ip2;
		size_t hash = 5381;
		hash = hash + k.port1;
		hash = hash + k.port2;
		hash = hash + ip1[0];
		hash = hash + ip1[1];
		hash = hash + ip1[2];
		hash = hash + ip1[3];
		hash = hash + ip2[0];
		hash = hash + ip2[1];
		hash = hash + ip2[2];
		hash = hash + ip2[3];
		return hash;
	}
};

template<class mapType>
class Benchmark {

public:
	Benchmark(int size, int lookups) : size(size), lookups(lookups) {}

	void Insert() {
		srand(42);
		for (int i = 0; i < size; i++) {
			int v = rand();
			zeek::detail::ConnIDKey k;
			*((int*)&k.ip1) = v;
			*((int*)&k.ip2) = v + v;
			k.port1 = (uint16_t)(i + 1);
			k.port2 = (uint16_t)(i + 2);
			the_map[k] = nullptr;
			keys.push_back(k);
		}
	}

	void Test() {
		for (int i = 0; i < lookups; i++) {
			std::vector<int *> values;

			for (int i = size - 1; i >= 0; i--) {
				values.push_back(the_map[keys[i]]);
			}
		}
	}

private:
	int size;
	int lookups;
	mapType the_map;
	std::vector<zeek::detail::ConnIDKey> keys;
};

template <class B>
void run(const char *name, int size, int lookups) {

	B b(size, lookups);

	double start, end;
	start = zeek::util::current_time(true);
	b.Insert();
	end = zeek::util::current_time(true);

	double inserttime = end - start;

	start = zeek::util::current_time(true);
	b.Test();
	end = zeek::util::current_time(true);
	double testtime = end - start;
	std::printf("%-32s insert=%.3f testtime=%.4f\n", name, inserttime, testtime);
}


int main(int argc, char *argv[]) {

	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <size> <lookups>" << std::endl;
		std::exit(1);
	}

	int size = atoi(argv[1]);
	int lookups = atoi(argv[2]);

	run <Benchmark<std::map<zeek::detail::ConnIDKey, int*> > >("map", size, lookups);
	run <Benchmark<std::unordered_map<zeek::detail::ConnIDKey, int*, ConnIDKeyHash> > >("unordered_map/hash=HashBytes", size, lookups);
	run <Benchmark<std::unordered_map<zeek::detail::ConnIDKey, int*, ConnIDKeyHashAddition> > >("unordered_map/hash=Addition", size, lookups);

}

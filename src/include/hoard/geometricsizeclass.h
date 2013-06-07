// -*- C++ -*-

#ifndef HOARD_GEOMETRIC_SIZECLASS_H
#define HOARD_GEOMETRIC_SIZECLASS_H

namespace Hoard {

  template <int MaxOverhead = 20,  // percent
	    int Alignment = 16>
  class GeometricSizeClass {
  public:

    static int size2class (const size_t sz) {
      int index = 0;
      while (sz > c2s(index)) {
	index++;
      }
      return index;
      // size = Alignment * (1 + MaxOverhead) ^ class
      // log(size) = log (Alignment * (1 + MaxOverHead) ^ class)
      //           = log (Alignment) + class * log (1 + MaxOverhead)
      // => class = (log(size) - log(Alignment)) / log (1 + MaxOverhead)
      //      return floor((log (sz) - log (Alignment))
      //		  / log (1.0 + (float) MaxOverhead / 100.0));
    }

    static size_t class2size (const int cl) {
      return c2s (cl);
      //      return Alignment * floor (pow (1.0 + (float) MaxOverhead / 100.0, cl));
    }

    static void test() {
      for (size_t sz = Alignment; sz < 1048576; sz += Alignment) {
	int cl = size2class (sz);
	assert (sz <= class2size(cl));
      }
      for (int cl = 0; cl < NUM_SIZES; cl++) {
	size_t sz = class2size (cl);
	assert (cl == size2class (sz));
      }
    }

  private:

    enum { NUM_SIZES = 80 };

    static unsigned long c2s (int cl) {
      static size_t sizes[NUM_SIZES];
      static bool init = createTable (sizes);
      init = init;
      return sizes[cl];
    }

    static bool createTable (unsigned long * sizes)
    {
      const double base = (1.0 + (double) MaxOverhead / 100.0);
      size_t sz = Alignment;
      for (int i = 0; i < NUM_SIZES; i++) {
	sizes[i] = sz;
	size_t newSz = sz;
	// (l(n) - l(s)) / l(b) >= 1
	// l(n) - l(s) >= l(b)
	// l(n) >= l(b) + l(s)
	// n >= e(l(b) + l(s))
	newSz = (size_t) ceil(exp(log((base) + log((float) sz))));
	newSz = newSz - (newSz % Alignment);
	while ((log((float) newSz) - log((float) sz)) / log(base) < 1.0) {
	  newSz += Alignment;
	}
	sz = newSz;
      }
      return true;
    }

  };

}

#endif


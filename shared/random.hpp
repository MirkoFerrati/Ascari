
#include <exprtk.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverloaded-virtual"


template <typename T>
struct rndom : public exprtk::ifunction<T> 
//TODO(MIRKO): Spostare in un file a parte con i Warning disabilitati
{
	rndom() : exprtk::ifunction<T>(2),name("RNDOM") {
    }

    std::string name;

    // Returns random number. v1 is inclusive and v2 is inclusive too.
    T operator()(const T& v1, const T& v2) {

        // If v1 or v2 are smaller than 0 or v2 is smaller than v1 (v1 is min, v2 is max)
        // or v2 is bigger than RAND_MAX, then return nan.
        if (v2 < v1 || v2 > RAND_MAX) {
            return std::numeric_limits<T>::quiet_NaN();
        }

        double min = v1;
        double max = v2;

        if (max < min) {
            return T(min);
        }

        double result;
        result = (rand()*(max-min)/RAND_MAX+min);
        //% (max + 1 - min)) + min;
        return T(result);
    }
};

#pragma GCC diagnostic pop

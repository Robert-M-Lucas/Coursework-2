#include "Arduino.h"

namespace Util {
    /// Convert a value to a byte array
    template<class T>
    void toBytes(T const &x, byte *output) {
        auto bytes = reinterpret_cast<byte *>(x);
        for (unsigned int i = 0; i < sizeof(T); ++i) {
            output[i] = bytes[i];
        }
    }

    /// Convert a byte array back into a value
    template <class T>
    T fromBytes(byte *input) {
        return *reinterpret_cast<T*>(input);
    }
}

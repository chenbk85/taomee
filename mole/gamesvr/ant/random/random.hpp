#ifndef ANT_RANDOM_HPP_
#define ANT_RANDOM_HPP_

#include <cstdlib>

namespace ant {

/**
 * ranged_random - generates a random number with the given range
 *  @min: minimum random number
 *  @max: maximum random number
 *
 * return the randomly generated number with the given range
 */
inline int ranged_random(int min, int max)
{
    // generates ranged random number
    return (rand() % (max - min + 1)) + min;
}

} // namespace ant

#endif // ANT_RANDOM_HPP_

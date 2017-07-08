#pragma once

#include <Eigen/Eigen>
#include <array>

namespace Kernel {

template <unsigned int N>
class Region
{
public:
    typedef Eigen::Array<float, N, 1> Pt;
    typedef Eigen::Array<float, 3 - N, 1> Perp;

    /*
     *  Constructs a region with the given bounds
     */
    Region(Pt lower, Pt upper) : lower(lower), upper(upper), perp(0) {}

    /*
     *  Construct a region with the given bounds
     *  and perpendicular coordinate(s)
     */
    Region(Pt lower, Pt upper, Perp p) : lower(lower), upper(upper), perp(p) {}

    /*
     *  Default constructor for an empty region
     */
    Region() : lower(0), upper(0) {}

    /*
     *  Splits along all axes
     */
    std::array<Region, 1 << N> subdivide() const
    {
        auto middle = (upper.array() / 2) + (lower.array() / 2);

        // Default-construct empty regions
        std::array<Region, 1 << N> out = {};

        for (unsigned i=0; i < (1 << N); ++i)
        {
            auto a = Eigen::Array<float, N, 1>(0);
            for (unsigned j=0; j < N; ++j)
            {
                a(j) = (i & (1 << j)) > 0;
            }
            out[i] = Region(a.select(middle, lower),
                            a.select(upper, middle));
        }
        return out;
    }

    /*
     *  Returns the volume of the region in arbitrary units
     */
    float volume() const
    {
        float out = 1;
        for (unsigned i=0; i < N; ++i)
        {
            out *= upper(i) - lower(i);
        }
        return out;
    }

    bool empty() const
    {
        return (lower.array() == 0).all() &&
               (upper.array() == 0).all();
    }

    Eigen::Array3f lower3() const
    {
        Eigen::Array3f out;
        out.head<N>() = lower;
        out.tail<3 - N>() = perp;
        return out;
    }

    Eigen::Array3f upper3() const
    {
        Eigen::Array3f out;
        out.head<N>() = upper;
        out.tail<3 - N>() = perp;
        return out;
    }

    /*  Lower and upper bounds for the region  */
    Pt lower, upper;

    /*  perp is the coordinates on perpendicular axes, used when converting
     *  a 2D region into 3D coordinates for Interval evaluation  */
    Eigen::Array<float, 3 - N, 1> perp;

    /*  Boilerplate for an object that contains an Eigen struct  */
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

}   // namespace Kernel
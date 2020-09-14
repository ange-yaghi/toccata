#ifndef TOCCATA_HEAT_MAP_H
#define TOCCATA_HEAT_MAP_H

#include "delta.h"

#include <piranha.h>

#include <string>
#include <vector>

namespace toccata {

    class HeatMap {
    public:
        struct SamplePoint {
            double s;
            ysVector v;
        };

    public:
        HeatMap();
        ~HeatMap();

        void Clear();
        void AddSample(double s, const ysVector &v);

        ysVector Sample(double s) const;

    protected:
        std::vector<SamplePoint> m_samples;
    };

} /* namespace toccata */

#endif /* TOCCATA_HEAT_MAP_H */

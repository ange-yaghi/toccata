#ifndef TOCCATA_CORE_MUSIC_POINT_CONTAINER_H
#define TOCCATA_CORE_MUSIC_POINT_CONTAINER_H

#include "music_point.h"

#include <assert.h>
#include <memory>

namespace toccata {

    class MusicPointContainer {
    public:
        MusicPointContainer() {
            m_points = nullptr;
            m_capacity = 0;
            m_pointCount = 0;
        }

        ~MusicPointContainer() {
            delete[] m_points;
        }

        MusicPoint *GetPoints() const { return m_points; }
        int GetCount() const { return m_pointCount; }

        void Initialize(int n) {
            m_points = new MusicPoint[n];
            m_capacity = n;
        }

        int AddPoint(const MusicPoint &point) {
            assert(m_pointCount <= m_capacity);

            if (m_pointCount == m_capacity) {
                m_capacity = (m_capacity + 1) * 2;
                MusicPoint *newPoints = new MusicPoint[m_capacity];

                memcpy((void *)newPoints, (void *)m_points, sizeof(MusicPoint) * m_pointCount);
                delete[] m_points;

                m_points = newPoints;
            }

            for (int i = m_pointCount - 1; i >= -1; --i) {
                if (i == -1 || m_points[i].Timestamp < point.Timestamp) {
                    InsertPoint(point, i + 1);
                    return i + 1;
                }
            }
        }

        void RemovePoint(int index) {
            assert(index >= 0);
            assert(index < m_pointCount);

            if (index != m_pointCount - 1) {
                memmove(
                    (void *)(m_points + index),
                    (void *)(m_points + index + 1),
                    sizeof(MusicPoint) * ((size_t)m_pointCount - index)
                );
            }

            --m_pointCount;
        }

        void Clear() {
            m_pointCount = 0;
        }

    protected:
        void InsertPoint(const MusicPoint &point, int index) {
            if (index < m_pointCount) {
                memmove(
                    (void *)(m_points + index + 1), 
                    (void *)(m_points + index), 
                    sizeof(MusicPoint) * ((size_t)m_pointCount - index)
                );
            }

            m_points[index] = point;
            ++m_pointCount;
        }

    protected:
        MusicPoint *m_points;
        int m_pointCount;
        int m_capacity;
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_MUSIC_POINT_CONTAINER_H */

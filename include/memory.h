#ifndef TOCCATA_CORE_MEMORY_H
#define TOCCATA_CORE_MEMORY_H

namespace toccata {

    class Memory {
    public:
        template<typename T>
        static T *Allocate(int n) {
            return new T[n];
        }

        template<typename T>
        static void Free(T *memory) {
            delete[] memory;
        }

        template<typename T>
        static T **Allocate2d(int n, int m) {
            T **memory = new T *[n + 1];
            for (int i = 0; i < n; ++i) {
                memory[i] = new T[m];
            }

            memory[n] = nullptr;

            return memory;
        }

        template<typename T>
        static void Free2d(T **memory) {
            for (int i = 0; memory[i] != nullptr; ++i) {
                delete memory[i];
            }

            delete memory;
        }
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_MEMORY_H */

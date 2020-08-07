#ifndef TOCCATA_CORE_MUNKRES_SOLVER_H
#define TOCCATA_CORE_MUNKRES_SOLVER_H

namespace toccata {

    class MunkresSolver {
    public:
        struct Request {
            struct MemorySpace {
                int **Path; // size = (n + m + 1) x 2

                double **C; // size = n x m
                int **D; // size = n x m

                int **Starred; // size = n x m
                bool *RowCover; // size = n
                bool *ColumnCover; // size = m
            };

            struct State {
                int PathCount;
                int PathRow0;
                int PathCol0;
            };

            int n;
            int m;
            double *const *Costs;
            bool *const *DisallowedMappings;
            int *Target;

            MemorySpace Memory;
            State State;
        };

        static void AllocateMemorySpace(Request::MemorySpace *memory, int n, int m);
        static void InitializeRequest(Request *request);
        static int *Solve(Request *request);
        static void FreeMemorySpace(Request::MemorySpace *memory);

    private:
        enum class Step {
            Step_1,
            Step_2,
            Step_3,
            Step_4,
            Step_5,
            Step_6,
            Step_7,
            Complete
        };

        static Step DoStep_1(Request *request);
        static Step DoStep_2(Request *request);
        static Step DoStep_3(Request *request);
        static Step DoStep_4(Request *request);
        static Step DoStep_5(Request *request);
        static Step DoStep_6(Request *request);
        static Step DoStep_7(Request *request);

        static void Add(Request *request, int r1, int c1, int r2, int c2);
        static void Subtract(Request *request, int r1, int c1, int r2, int c2);
        static bool LessThan(const Request *request, int r1, int c1, int r2, int c2);
        static bool IsZero(const Request *request, int r, int c);

        static bool FindZero(const Request *request, int *r, int *c);
        static int FindStarInRow(const Request *request, int row);
        static int FindStarInCol(const Request *request, int col);
        static void FindSmallestUncovered(const Request *request, int *r, int *c);
        
        static int FindPrimeInRow(const Request *request, int row);
        static void AugmentPath(const Request *request);
        static void ClearCovers(const Request *request);
        static void ErasePrimes(const Request *request);
    };

} /* namespace toccata */

#endif /* TOCCATA_CORE_MUNKRES_SOLVER_H */

#include "../include/munkres_solver.h"

#include "../include/memory.h"

void toccata::MunkresSolver::AllocateMemorySpace(Request::MemorySpace *memory, int n, int m) {
    memory->ColumnCover = Memory::Allocate<bool>(m);
    memory->RowCover = Memory::Allocate<bool>(n);
    
    memory->Path = Memory::Allocate2d<int>(n + m + 1, 2);
    memory->Starred = Memory::Allocate2d<int>(n, m);

    memory->C = Memory::Allocate2d<double>(n, m);
    memory->D = Memory::Allocate2d<int>(n, m);
}

void toccata::MunkresSolver::InitializeRequest(Request *request) {
    request->State.PathCount = 0;
    request->State.PathRow0 = -1;
    request->State.PathCol0 = -1;

    for (int i = 0; i < request->n; ++i) {
        request->Memory.RowCover[i] = false;
    }

    for (int i = 0; i < request->m; ++i) {
        request->Memory.ColumnCover[i] = false;
    }

    for (int i = 0; i < request->n; ++i) {
        for (int j = 0; j < request->m; ++j) {
            request->Memory.Starred[i][j] = 0;
            request->Memory.C[i][j] = request->Costs[i][j];
            request->Memory.D[i][j] = request->DisallowedMappings[i][j]
                ? 1
                : 0;
        }
    }
}

int *toccata::MunkresSolver::Solve(Request *request) {
    Step step = Step::Step_1;

    while (step != Step::Complete) {
        Step next = step;

        switch (step) {
        case Step::Step_1:
            next = DoStep_1(request);
            break;
        case Step::Step_2:
            next = DoStep_2(request);
            break;
        case Step::Step_3:
            next = DoStep_3(request);
            break;
        case Step::Step_4:
            next = DoStep_4(request);
            break;
        case Step::Step_5:
            next = DoStep_5(request);
            break;
        case Step::Step_6:
            next = DoStep_6(request);
            break;
        case Step::Step_7:
            next = DoStep_7(request);
            break;
        }

        step = next;
    }

    return request->Target;
}

void toccata::MunkresSolver::FreeMemorySpace(Request::MemorySpace *memory) {
    Memory::Free(memory->ColumnCover);
    Memory::Free(memory->RowCover);
    Memory::Free2d(memory->Path);
    Memory::Free2d(memory->Starred);
}

toccata::MunkresSolver::Step toccata::MunkresSolver::DoStep_1(Request *request) {
    for (int i = 0; i < request->n; ++i) {
        int smallest = 0;
        for (int j = 1; j < request->m; ++j) {
            if (LessThan(request, i, j, i, smallest)) {
                smallest = j;
            }
        }

        for (int j = 0; j < request->m; ++j) {
            if (smallest == j) continue;
            Subtract(request, i, j, i, smallest);
        }

        request->Memory.D[i][smallest] = 0;
        request->Memory.C[i][smallest] = 0.0;
    }

    return Step::Step_2;
}

toccata::MunkresSolver::Step toccata::MunkresSolver::DoStep_2(Request *request) {
    for (int i = 0; i < request->n; ++i) {
        for (int j = 0; j < request->m; ++j) {
            if (IsZero(request, i, j) && !request->Memory.RowCover[i] && !request->Memory.ColumnCover[j]) {
                request->Memory.Starred[i][j] = 1;
                request->Memory.RowCover[i] = true;
                request->Memory.ColumnCover[j] = true;
            }
        }
    }

    for (int i = 0; i < request->n; ++i) {
        request->Memory.RowCover[i] = false;
    }

    for (int j = 0; j < request->m; ++j) {
        request->Memory.ColumnCover[j] = false;
    }

    return Step::Step_3;
}

toccata::MunkresSolver::Step toccata::MunkresSolver::DoStep_3(Request *request) {
    for (int i = 0; i < request->n; ++i) {
        for (int j = 0; j < request->m; ++j) {
            if (request->Memory.Starred[i][j] == 1) {
                request->Memory.ColumnCover[j] = true;
            }
        }
    }

    int columnCount = 0;
    for (int j = 0; j < request->m; ++j) {
        if (request->Memory.ColumnCover[j]) {
            ++columnCount;
        }
    }

    if (columnCount >= request->m || columnCount >= request->n) {
        return Step::Step_7;
    }
    else {
        return Step::Step_4;
    }
}

toccata::MunkresSolver::Step toccata::MunkresSolver::DoStep_4(Request *request) {
    int row = -1, col = -1;

    while (true) {
        bool foundZero = FindZero(request, &row, &col);
        if (!foundZero) return Step::Step_6;
        else {
            request->Memory.Starred[row][col] = 2;
            int starIndex = FindStarInRow(request, row);
            if (starIndex != -1) {
                request->Memory.RowCover[row] = true;
                request->Memory.ColumnCover[starIndex] = false;
            }
            else {
                request->State.PathCol0 = col;
                request->State.PathRow0 = row;
                return Step::Step_5;
            }
        }
    }
}

toccata::MunkresSolver::Step toccata::MunkresSolver::DoStep_5(Request *request) {
    int row = -1;
    int col = -1;

    request->State.PathCount = 1;
    int pathCount = request->State.PathCount;

    request->Memory.Path[pathCount - 1][0] = request->State.PathRow0;
    request->Memory.Path[pathCount - 1][1] = request->State.PathCol0;

    while (true) {
        int star = FindStarInCol(request, request->Memory.Path[pathCount - 1][1]);
        if (star != -1) {
            ++pathCount;
            request->Memory.Path[pathCount - 1][0] = star;
            request->Memory.Path[pathCount - 1][1] = request->Memory.Path[pathCount - 2][1];
        }
        else break;

        int prime = FindPrimeInRow(request, request->Memory.Path[pathCount - 1][0]);
        ++pathCount;
        request->Memory.Path[pathCount - 1][0] = request->Memory.Path[pathCount - 2][0];
        request->Memory.Path[pathCount - 1][1] = prime;
    }

    request->State.PathCount = pathCount;
    AugmentPath(request);
    ClearCovers(request);
    ErasePrimes(request);
    return Step::Step_3;
}

toccata::MunkresSolver::Step toccata::MunkresSolver::DoStep_6(Request *request) {
    int min_r = -1;
    int min_c = -1;
    FindSmallestUncovered(request, &min_r, &min_c);
    for (int i = 0; i < request->n; ++i) {
        for (int j = 0; j < request->m; ++j) {
            if (i == min_r && j == min_c) continue;

            if (request->Memory.RowCover[i]) {
                Add(request, i, j, min_r, min_c);
            }

            if (!request->Memory.ColumnCover[j]) {
                Subtract(request, i, j, min_r, min_c);
            }
        }
    }

    request->Memory.C[min_r][min_c] = 0.0;
    request->Memory.D[min_r][min_c] = 0;

    return Step::Step_4;
}

toccata::MunkresSolver::Step toccata::MunkresSolver::DoStep_7(Request *request) {
    for (int i = 0; i < request->n; ++i) {
        for (int j = 0; j < request->m; ++j) {
            if (IsZero(request, i, j) && request->Memory.Starred[i][j] == 1) {
                if (request->DisallowedMappings[i][j]) {
                    request->Target[i] = -1;
                }
                else {
                    request->Target[i] = j;
                }
            }
        }
    }

    return Step::Complete;
}

void toccata::MunkresSolver::Add(Request *request, int r1, int c1, int r2, int c2) {
    request->Memory.C[r1][c1] += request->Memory.C[r2][c2];
    request->Memory.D[r1][c1] += request->Memory.D[r2][c2];
}

void toccata::MunkresSolver::Subtract(Request *request, int r1, int c1, int r2, int c2) {
    request->Memory.C[r1][c1] -= request->Memory.C[r2][c2];
    request->Memory.D[r1][c1] -= request->Memory.D[r2][c2];
}

bool toccata::MunkresSolver::LessThan(const Request *request, int r1, int c1, int r2, int c2) {
    double cost1 = request->Memory.C[r1][c1];
    int d1 = request->Memory.D[r1][c1];

    double cost2 = request->Memory.C[r2][c2];
    int d2 = request->Memory.D[r2][c2];

    if (d1 != d2) return d1 < d2;
    else return cost1 < cost2;
}

bool toccata::MunkresSolver::IsZero(const Request *request, int r, int c) {
    double cost = request->Memory.C[r][c];
    int d = request->Memory.D[r][c];

    return cost == 0.0 && d == 0;
}

bool toccata::MunkresSolver::FindZero(const Request *request, int *row, int *col) {
    *row = -1;
    *col = -1;

    for (int i = 0; i < request->n; ++i) {
        for (int j = 0; j < request->m; ++j) {
            if (IsZero(request, i, j) && !request->Memory.RowCover[i] && !request->Memory.ColumnCover[j]) {
                *row = i;
                *col = j;
                return true;
            }
        }
    }

    return false;
}

int toccata::MunkresSolver::FindStarInRow(const Request *request, int row) {
    for (int j = 0; j < request->m; ++j) {
        if (request->Memory.Starred[row][j] == 1) return j;
    }

    return -1;
}

int toccata::MunkresSolver::FindStarInCol(const Request *request, int col) {
    for (int i = 0; i < request->n; ++i) {
        if (request->Memory.Starred[i][col] == 1) return i;
    }

    return -1;
}

void toccata::MunkresSolver::FindSmallestUncovered(const Request *request, int *r, int *c) {
    int row = -1;
    int col = -1;
    for (int i = 0; i < request->n; ++i) {
        if (request->Memory.RowCover[i]) continue;

        for (int j = 0; j < request->m; ++j) {
            if (!request->Memory.ColumnCover[j]) {
                if (row == -1 || LessThan(request, i, j, row, col)) {
                    row = i;
                    col = j;
                }
            }
        }
    }

    *r = row;
    *c = col;
}

int toccata::MunkresSolver::FindPrimeInRow(const Request *request, int row) {
    for (int j = 0; j < request->m; ++j) {
        if (request->Memory.Starred[row][j] == 2) return j;
    }

    return -1;
}

void toccata::MunkresSolver::AugmentPath(const Request *request) {
    for (int p = 0; p < request->State.PathCount; ++p) {
        int p0 = request->Memory.Path[p][0];
        int p1 = request->Memory.Path[p][1];
        if (request->Memory.Starred[p0][p1] == 1) {
            request->Memory.Starred[p0][p1] = 0;
        }
        else {
            request->Memory.Starred[p0][p1] = 1;
        }
    }
}

void toccata::MunkresSolver::ClearCovers(const Request *request) {
    for (int i = 0; i < request->n; ++i) {
        request->Memory.RowCover[i] = false;
    }

    for (int j = 0; j < request->m; ++j) {
        request->Memory.ColumnCover[j] = false;
    }
}

void toccata::MunkresSolver::ErasePrimes(const Request *request) {
    for (int i = 0; i < request->n; ++i) {
        for (int j = 0; j < request->m; ++j) {
            if (request->Memory.Starred[i][j] == 2) {
                request->Memory.Starred[i][j] = 0;
            }
        }
    }
}

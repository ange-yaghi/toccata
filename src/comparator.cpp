#include "../include/comparator.h"

#include "../include/math.h"
#include "../include/transform.h"

bool toccata::Comparator::CalculateError(const Request &request, Result *result) {
    const int n = request.Reference->NoteContainer.GetCount();
    const MusicPoint *referencePoints = request.Reference->NoteContainer.GetPoints();
    const MusicPoint *points = request.Segment->NoteContainer.GetPoints();

    double totalError = 0.0;
    int mappedNotes = 0;
    for (int i = 0; i < n; ++i) {
        int mapped = request.Mapping[i];
        if (mapped == -1) continue;

        const MusicPoint &ref = referencePoints[i];
        const MusicPoint &p = points[request.Mapping[i]];

        const double p_t = Transform::f(p.Timestamp, request.s, request.t);
        const double diff = Math::Abs(ref.Timestamp - p_t);

        totalError += diff;
        ++mappedNotes;
    }

    if (mappedNotes == 0) return false;

    result->AverageError = totalError / mappedNotes;
    return true;
}

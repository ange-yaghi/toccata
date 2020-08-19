#include "../include/comparator.h"

#include "../include/math.h"
#include "../include/transform.h"

#include <vector>

bool toccata::Comparator::CalculateError(const Request &request, Result *result) {
    const int n = request.Reference->NoteContainer.GetCount();
    const MusicPoint *referencePoints = request.Reference->NoteContainer.GetPoints();
    const MusicPoint *points = request.Segment->NoteContainer.GetPoints();

    double totalError = 0.0;
    int mappedNotes = 0;
    int mappingStart = INT_MAX;
    int mappingEnd = INT_MIN;
    for (int i = 0; i < n; ++i) {
        int mapped = request.Mapping[i];
        if (mapped == -1) continue;

        if (mapped < mappingStart) mappingStart = mapped;
        if (mapped > mappingEnd) mappingEnd = mapped;

        const MusicPoint &ref = referencePoints[i];
        const MusicPoint &p = points[request.Mapping[i]];

        const double p_t = Transform::f(p.Timestamp, request.s, request.t);
        const double diff = Math::Abs(ref.Timestamp - p_t);

        totalError += diff;
        ++mappedNotes;
    }

    if (mappedNotes == 0) return false;

    result->MappingStart = mappingStart;
    result->MappingEnd = mappingEnd;
    result->MappedNotes = mappedNotes;
    result->AverageError = totalError / mappedNotes;

    return true;
}

int toccata::Comparator::CalculateAddedNotes(const Request &request) {
    const int n = request.Reference->NoteContainer.GetCount();
    const int n0 = request.Segment->NoteContainer.GetCount();
    const MusicPoint *referencePoints = request.Reference->NoteContainer.GetPoints();
    const MusicPoint *points = request.Segment->NoteContainer.GetPoints();

    std::vector<bool> mapped(n0, false);

    for (int i = 0; i < n; ++i) {
        if (request.Mapping[i] == -1) continue;
        mapped[request.Mapping[i]] = true;
    }

    int addedNotes = 0;
    for (int i = 0; i < n0; ++i) {
        if (mapped[i]) continue;

        const MusicPoint &p = points[i];
        const double timestamp = Transform::f(p.Timestamp, request.s, request.t);

        if (timestamp > 0.0 || timestamp < request.Reference->Length) ++addedNotes;
    }

    return addedNotes;
}

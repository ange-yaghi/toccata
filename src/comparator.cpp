#include "../include/comparator.h"

#include "../include/math.h"
#include "../include/transform.h"

#include <vector>

bool toccata::Comparator::CalculateError(const Request &request, Result *result) {
    const int n = request.Reference->NoteContainer.GetCount();
    const MusicPoint *referencePoints = request.Reference->NoteContainer.GetPoints();
    const MusicPoint *points = request.Segment->NoteContainer.GetPoints();

    int mappingStart = INT_MAX;
    int mappingEnd = INT_MIN;

    double totalError = 0.0;
    int mappedNotes = 0;
    for (int i = 0; i < n; ++i) {
        int mapped = request.Mapping[i];
        if (mapped == -1) continue;

        if (mapped > mappingEnd) mappingEnd = mapped;
        if (mapped < mappingStart) mappingStart = mapped;

        if (result->Target != nullptr) {
            result->Target->insert(mapped);
        }

        const MusicPoint &ref = referencePoints[i];
        const MusicPoint &p = points[request.Mapping[i]];

        const double p_t = request.T.f(request.Segment->Normalize(request.T.Local(p.Timestamp)));
        const double diff = Math::Abs(request.Reference->Normalize(ref.Timestamp) - p_t);

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

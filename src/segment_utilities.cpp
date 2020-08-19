#include "../include/segment_utilities.h"

void toccata::SegmentUtilities::SortByPitch(const MusicSegment *segment, int start, int end, int pitchCount, int **target) {
    int n = segment->NoteContainer.GetCount();
    const MusicPoint *points = segment->NoteContainer.GetPoints();

    // nth column used as a counter
    for (int i = 0; i < pitchCount; ++i) {
        target[i][n] = 0;
    }

    for (int i = start; i <= end; ++i) {
        const int pitch = points[i].Pitch;
        const int index = target[pitch][n]++;

        target[pitch][index] = i;
    }

    for (int i = 0; i < pitchCount; ++i) {
        for (int j = target[i][n]; j <= n; ++j) {
            target[i][j] = -1;
        }
    }
}

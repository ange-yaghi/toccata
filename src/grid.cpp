#include "../include/grid.h"

toccata::Grid::Grid() {
    m_verticalDivisions = 1;
    m_horizontalDivisions = 1;

    m_verticalMargin = 0.0f;
    m_horizontalMargin = 0.0f;
}

toccata::Grid::Grid(const BoundingBox &box, int horizontalCells, int verticalCells, float margin) {
    m_boundingBox = box;
    m_horizontalDivisions = horizontalCells;
    m_verticalDivisions = verticalCells;
    m_horizontalMargin = m_verticalMargin = margin;
}

toccata::Grid::Grid(const BoundingBox &box, int horizontalCells, int verticalCells, float horizontalMargin, float verticalMargin) {
    m_boundingBox = box;
    m_horizontalDivisions = horizontalCells;
    m_verticalDivisions = verticalCells;
    m_horizontalMargin = horizontalMargin;
    m_verticalMargin = verticalMargin;
}

toccata::Grid::~Grid() {
    /* void */
}

void toccata::Grid::GetCell(int x, int y, BoundingBox &target) const {
    const float cellWidth = m_boundingBox.Width() / m_horizontalDivisions;
    const float cellHeight = m_boundingBox.Height() / m_verticalDivisions;

    const float innerWidth = max(cellWidth - m_horizontalMargin * 2.0f, 0.0f);
    const float innerHeight = max(cellHeight - m_verticalMargin * 2.0f, 0.0f);

    target = BoundingBox(innerWidth, innerHeight)
        .AlignCenterX(m_boundingBox.Left() + cellWidth * x + cellWidth / 2)
        .AlignCenterY(m_boundingBox.Bottom() + cellHeight * y + cellHeight / 2);
}

toccata::BoundingBox toccata::Grid::GetCell(int x, int y) const {
    BoundingBox box;
    GetCell(x, y, box);
    return box;
}

toccata::BoundingBox toccata::Grid::GetFullCell(int x, int y) const {
    const float cellWidth = m_boundingBox.Width() / m_horizontalDivisions;
    const float cellHeight = m_boundingBox.Height() / m_verticalDivisions;

    return BoundingBox(cellWidth, cellHeight)
        .AlignCenterX(m_boundingBox.Left() + cellWidth * x + cellWidth / 2)
        .AlignCenterY(m_boundingBox.Bottom() + cellHeight * y + cellHeight / 2);
}

toccata::BoundingBox toccata::Grid::GetRange(int x0, int x1, int y0, int y1) const {
    BoundingBox empty;
    for (int x = x0; x <= x1; ++x) {
        for (int y = y0; y <= y1; ++y) {
            empty = empty.Add(GetCell(x, y));
        }
    }

    return empty;
}

toccata::BoundingBox toccata::Grid::GetFullRange(int x0, int x1, int y0, int y1) const {
    BoundingBox empty;
    for (int x = x0; x <= x1; ++x) {
        for (int y = y0; y <= y1; ++y) {
            empty = empty.Add(GetFullCell(x, y));
        }
    }

    return empty;
}

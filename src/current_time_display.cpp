#include "../include/current_time_display.h"

#include "../include/midi_handler.h"

toccata::CurrentTimeDisplay::CurrentTimeDisplay() {
    /* void */
}

toccata::CurrentTimeDisplay::~CurrentTimeDisplay() {
    /* void */
}

void toccata::CurrentTimeDisplay::Construct() {
    /* void */
}

void toccata::CurrentTimeDisplay::Render() {
    std::stringstream ss;
    ss << MidiHandler::Get()->GetEstimatedTimestamp();

    const float textHeight = m_boundingBox.Height() / 4;

    RenderText(
        ss.str(),
        { m_boundingBox.Left(), m_boundingBox.CenterY() - (float)textHeight / 2 },
        (float)textHeight);
}

void toccata::CurrentTimeDisplay::Update() {
    /* void */
}

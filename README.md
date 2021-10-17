# Toccata

*Toccata* is an open-source MIDI analyzer currently under development. *Toccata* doesn't make any promises about making you a better or more creative musician, but it can be very useful for perfecting musical timing, automatically maintaining a record of your musical progress and providing real-time feedback while practicing.

## Current Progress
This project is a work in progress. The screenshot below shows it's current state of development and features that it already supports:
![Alt text](docs/public/screenshot_20211016.PNG?raw=true)
Shown above:
- Recognition of individual bars of music
- Readout of average jitter and deviation from "perfect" time
- Display of played vs theoretical notes
- Inferred tempo

## Features
*Toccata* will be able to:
- Show all metrics and input notes in a fast, responsive GUI
- Recognize individuals bars of music from an imperfect input MIDI stream
- Approximate the input stream's tempo without any human aid
- Provide real-time metrics about pitch, timing and dynamics accuracy
- Catalogue all recorded MIDI data in a database (sorted by music piece) for future reference
- Support thousands of different musical pieces, each with hundreds of bars

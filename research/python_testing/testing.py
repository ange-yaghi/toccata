from structures import MidiSegment, MidiNote, Analyzer
from testing_fixtures import Utilities

import matplotlib.pyplot as plt
import numpy as np

THEME_COLOR_BACKGROUND = '#0c163e'
THEME_COLOR_ACCENT = '#f79700'
THEME_COLOR_NEUTRAL_1 = '#bb4eab'
THEME_COLOR_NEUTRAL_2 = '#5b7afa'


def test_no_tricks():
    played = MidiSegment()
    reference = MidiSegment()

    s = 10

    played.add_note(MidiNote(0, 0 * s, 100 * s))
    played.add_note(MidiNote(2, -5 * s, 100 * s))
    played.add_note(MidiNote(1, 110 * s, 100 * s))
    played.add_note(MidiNote(2, 195 * s, 100 * s))

    reference.add_note(MidiNote(0, 0, 100))
    reference.add_note(MidiNote(2, 0, 100))
    reference.add_note(MidiNote(1, 100, 100))
    reference.add_note(MidiNote(2, 200, 100))

    mapping = Analyzer.stage_1_mapping(reference, played).note_mapping
    print(Analyzer.stage_2_calculate_tempo(reference, played, mapping))


def test_with_error():
    played = MidiSegment()
    reference = MidiSegment()

    s = 12.5

    played.add_note(MidiNote(0, 0 * s, 100 * s))
    played.add_note(MidiNote(2, -5 * s, 100 * s))
    played.add_note(MidiNote(1, -5 * s, 100 * s))
    played.add_note(MidiNote(1, 110 * s, 100 * s))
    played.add_note(MidiNote(2, 195 * s, 100 * s))

    reference.add_note(MidiNote(0, 0, 100))
    reference.add_note(MidiNote(2, 0, 100))
    reference.add_note(MidiNote(1, 100, 100))
    reference.add_note(MidiNote(2, 200, 100))

    mapping = Analyzer.stage_1_mapping(reference, played).note_mapping
    print(Analyzer.stage_2_calculate_tempo(reference, played, mapping))


def test_random():
    reference = Utilities.create_random_bar(32, 1000)

    played = reference.create_copy()
    Utilities.add_note_omissions(played, 15)
    Utilities.add_nonsense_notes(played, 10, 100)
    Utilities.scale_segment(played, 20.0)
    Utilities.jitter(played, 30.0)

    mapping = Analyzer.stage_1_mapping(reference, played).note_mapping
    print(Analyzer.stage_2_calculate_tempo(reference, played, mapping))


def analyze_error_wrt_jitter():
    Utilities.seed_random(1)

    x = []
    jitter_amplitude = []

    SAMPLES = 100
    JITTER_STEPS = 50
    SCALE = 10.0

    for i in range(JITTER_STEPS):
        jitter_amplitude.append((i / float(JITTER_STEPS)) * (1000 / 8))
        x.append((jitter_amplitude[i] / 1000.0) * 4) 

    all_plots = []
    errs = [0.0] * JITTER_STEPS

    for j in range(SAMPLES):
        print("samples = {}".format(j + 1))

        reference = Utilities.create_random_bar_quantized(16, 1000, grid_spaces=16, notes=16)

        tempos = []

        for i in range(JITTER_STEPS):
            played = reference.create_copy()
            Utilities.jitter(played, jitter_amplitude[i])
            Utilities.scale_segment(played, SCALE)

            error_count, mapping = Analyzer.stage_1_mapping(reference, played)
            err, offset, scale = Analyzer.stage_2_calculate_tempo(reference, played, mapping)

            tempos.append(scale)
            errs[i] += (abs(scale - SCALE) / SCALE) * 100

        if j < 300:
            all_plots.append(tempos)

    for i in range(JITTER_STEPS):
        errs[i] /= SAMPLES

    fig, ax1 = plt.subplots()
    fig.suptitle('Tempo Estimation With Jitter', c=THEME_COLOR_NEUTRAL_2)

    for p in all_plots:
        ax1.scatter(x, p, c = THEME_COLOR_ACCENT, alpha=0.02, edgecolors='none')

    ax2 = ax1.twinx()
    ax2.plot(x, errs, c = THEME_COLOR_NEUTRAL_1)

    ax1.tick_params(axis='x', colors=THEME_COLOR_NEUTRAL_2)
    ax1.tick_params(axis='y', colors=THEME_COLOR_NEUTRAL_2)
    ax1.set_xlabel('Jitter (Beats)')
    ax1.set_ylabel('Stretch Estimation')

    ax2.tick_params(axis='x', colors=THEME_COLOR_NEUTRAL_2)
    ax2.tick_params(axis='y', colors=THEME_COLOR_NEUTRAL_2)
    ax2.set_ylabel('Average % Error')

    ax1.set_ylim(7.0, 13.0)
    ax1.set_xlim(0.0, 0.5)
    ax2.set_ylim(-10, 10)

    ax1.yaxis.label.set_color(THEME_COLOR_NEUTRAL_2)
    ax1.xaxis.label.set_color(THEME_COLOR_NEUTRAL_2)

    ax2.yaxis.label.set_color(THEME_COLOR_NEUTRAL_2)
    ax2.xaxis.label.set_color(THEME_COLOR_NEUTRAL_2)

    ax1.title.set_color(THEME_COLOR_NEUTRAL_2)
    ax2.title.set_color(THEME_COLOR_NEUTRAL_2)

    for spine in ax1.spines.values():
        spine.set_edgecolor(THEME_COLOR_NEUTRAL_2)

    for spine in ax2.spines.values():
        spine.set_edgecolor(THEME_COLOR_NEUTRAL_2)

    fig.tight_layout()

    plt.savefig('jitter_variation.png', transparent=True, dpi=600)

    plt.show()


def bar_recognition():
    Utilities.seed_random(1)

    library = []
    for i in range(1):
        library.append(Utilities.create_random_bar_quantized(16, 1000, grid_spaces=16, notes=64, name=str(i)))

    selected = library[0]
    played_segment = selected.create_copy()
    #Utilities.add_nonsense_notes_range(played_segment, 10, -500, 0, notes=64)
    #Utilities.add_nonsense_notes_range(played_segment, 10, 1000, 1500, notes=64)
    Utilities.jitter(played_segment, 50)
    #Utilities.add_note_omissions(played_segment, 3)
    #Utilities.add_nonsense_notes(played_segment, 5, 1000)
    Utilities.scale_segment(played_segment, 5.04)

    detected_bar, offset, scale = Analyzer.detect_bar(played_segment, library)
    print("bar={} | offset={}, scale={}".format(detected_bar.name, offset, scale))


def analyze_error_wrt_note_omissions():
    Utilities.seed_random(1)
    reference = Utilities.create_random_bar_quantized(32, 1000, 16, notes=32)

    x = []
    note_omissions = []

    SAMPLES = 100
    NOTE_OMISSIONS = 8

    for i in range(NOTE_OMISSIONS):
        note_omissions.append(i)
        x.append(i) 

    all_plots = []

    for j in range(SAMPLES):
        print("samples = {}".format(j + 1))

        err = []

        for i in range(NOTE_OMISSIONS):
            played = reference.create_copy()
            Utilities.add_note_omissions(played, note_omissions[i])
            Utilities.scale_segment(played, 10.0)

            mapping = Analyzer.stage_1_mapping(reference, played).note_mapping
            tempo = Analyzer.stage_2_calculate_tempo(reference, played, mapping)

            err.append(tempo)

        all_plots.append(err)

    for p in all_plots:
        plt.scatter(x, p, c = 'red', alpha=0.02)

    plt.show()


def analyze_error_wrt_nonsense_notes():
    Utilities.seed_random(1)
    reference = Utilities.create_random_bar_quantized(32, 1000, 16, notes=32)

    x = []
    nonsense_notes = []

    SAMPLES = 100
    NONSENSE_NOTES = 16

    for i in range(NONSENSE_NOTES):
        nonsense_notes.append(i)
        x.append(i)

    all_plots = []

    for j in range(SAMPLES):
        print("samples = {}".format(j + 1))

        err = []

        for i in range(NONSENSE_NOTES):
            played = reference.create_copy()
            Utilities.add_nonsense_notes(played, nonsense_notes[i], 1000)
            Utilities.scale_segment(played, 10.0)

            mapping = Analyzer.stage_1_mapping(reference, played).note_mapping
            tempo = Analyzer.stage_2_calculate_tempo(reference, played, mapping)

            err.append(tempo)

        all_plots.append(err)

    for p in all_plots:
        plt.scatter(x, p, c = 'red', alpha=0.02)

    plt.show()


def find_edge_case():
    Utilities.seed_random(1)
    reference = Utilities.create_random_bar_quantized(16, 1000, 16, notes=32)

    SAMPLES = 100
    for j in range(SAMPLES):
        print("samples = {}".format(j + 1))

        played = reference.create_copy()
        Utilities.add_note_omissions(played, 1)
        Utilities.scale_segment(played, 10.0)

        mapping = Analyzer.stage_1_mapping(reference, played).note_mapping
        tempo = Analyzer.stage_2_calculate_tempo(reference, played, mapping)

        if tempo > 15 or tempo < 5:
            print(reference.notes)
            print(played.notes)


if __name__ == "__main__":
    analyze_error_wrt_jitter()

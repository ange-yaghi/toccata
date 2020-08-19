from rpm_midi import MidiSegment
from rpm_utilities import Utilities

import numpy as np
import matplotlib.pyplot as plt

THEME_COLOR_BACKGROUND = '#0c163e'
THEME_COLOR_ACCENT = '#f79700'
THEME_COLOR_NEUTRAL_1 = '#bb4eab'
THEME_COLOR_NEUTRAL_2 = '#5b7afa'

def analyze_error_wrt_jitter():
    Utilities.seed_random(1)

    x = []
    jitter_amplitude = []

    SAMPLES = 100
    JITTER_STEPS = 50
    SCALE = 10.0
    TEST_PATTERNS = 3

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
            Utilities.add_note_omissions(played, 3)
            Utilities.add_nonsense_notes(played, 32, 1000)
            Utilities.add_nonsense_notes_range(played, 10, -1000, 0)
            Utilities.add_nonsense_notes_range(played, 10, 1000, 2000)
            Utilities.shift_segment(played, 1000)
            Utilities.scale_segment(played, SCALE)

            err, s, t = None, None, None

            for j in range(2):
                test_pattern = reference.find_test_pattern(4)
                result = played.find_best_solution(test_pattern, reference)

                if result is not None:
                    err_new, s_new, t_new = result
                    if err is None or err_new < err:
                        err, s, t = err_new, s_new, t_new
            
            if s is not None:
                s = 1 / s

                tempos.append(s)
                errs[i] += (abs(s - SCALE) / SCALE) * 100
            else:
                tempos.append(0)
                errs[i] += (abs(0 - SCALE) / SCALE) * 100            

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

    plt.savefig('jitter_variation_new_algorithm.png', transparent=True, dpi=600)

    plt.show()


if __name__ == "__main__":
    analyze_error_wrt_jitter()

    Utilities.seed_random(0)
    reference_segment = Utilities.create_random_bar_quantized(16, 1000, 16)

    played_segment = reference_segment.create_copy()
    Utilities.scale_segment(played_segment, 5)
    Utilities.jitter(played_segment, 200)

    test_notes = reference_segment.find_test_pattern(2)
    err, s, t = played_segment.find_best_solution(test_notes, reference_segment)

    print((err, s, t))

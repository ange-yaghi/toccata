notes_in_bar = 50
missed_notes = 0.25 * notes_in_bar

p_choosing_missed_note = (missed_notes / notes_in_bar)
p_choosing_good_pattern = (1 - p_choosing_missed_note) * (1 - p_choosing_missed_note)
p_choosing_bad_pattern = 1 - p_choosing_good_pattern

print(pow(1 - p_choosing_bad_pattern, 16))

26EDO (26 equal divisions of the octave)
Richard Reeman

This program reads from a text file, converts the lower case letters to upper case,
then converts the ASCII values of the letters to frequencies in a 26 tone octave.
Each frequency is used to generate a sawtooth wave. An amplitude envelope is then applied to each tone,
and the tones are written to a PCM file. Punctuation and whitespace are replaced by silence.

Sawtooth waves are used because they contain both odd and even harmonics.
The frequency range chosen (110Hz to 220Hz) falls within the range of human speech.

The output PCM file should be imported to Audacity as Raw Data file with:
16-bit encoding, 1 Channel, 44100 Sample Rate
and exported as a WAV file with the same settings.
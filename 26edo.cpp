/*

“… I set myself many more constraints:
I placed the sounds as you do letters,
one after the other, so as to create
forms and sequences”

- Bernard Parmegiani: Sound Thinking


26EDO (26 equal divisions of the octave)

This program reads from a text file, converts the lower case letters to upper case,
then converts the ASCII values of the letters to frequencies in a 26 tone octave 110Hz - 220Hz
A sawtooth wave is generated with each frequency.
The program then applies an amplitude envelope to each tone,
and writes the tones to a PCM file.
Punctuation and whitespace are replaced by silence.

Sawtooth waves are used because they contain both odd and even harmonics.
The frequency range chosen (110Hz to 220Hz) falls within the range of human speech.

*/


#include <iostream>
#include <vector>
#include <algorithm>    // for min_element, max_element (scale function)
#include <fstream>      // for file I/O (convert_text function)
#include <cmath>        // for sin function (Square and Sawtooth functions)
#include <cstdint>      // for int16_t (write_to_pcm function)
#include <list>         // for linked list (storeText function)
#include <string>       // for string class and getline function (store_text function)
#include <thread>       // for sleep_for (pause between lines of text)
#include <chrono>       // for seconds (pause between lines of text)

using namespace std;

const int sr = 44100;                // sample rate (number of samples per second)
const int bit_depth = 16;            // bit depth (16-bits per sample)
const double fundamental = 110.0;    // fundamental pitch of human speech (110Hz)

vector<double> scale(vector<double> data, double dB); // Function declaration


// Function to read from txt file and store ascii values in a vector
void convert_text(string filename, vector<int>& ascii_values) {
    // Open the file
    ifstream file(filename);            // Open the text file

    // Check if the file is open
    if (!file.is_open()) {
        cout << "Error opening text file." << endl;
        return;
    }

    // Read the file character by character
    char c;
    while (file.get(c)) {
        // Check if the read operation was successful
        if (file.fail()) {
            cout << "Error reading from text file." << endl;
            return;
        }

        // Convert the character to its ASCII value
        int ascii = static_cast<int>(c);

        // Convert lower case letters to upper case
        if (ascii >= 97 && ascii <= 122) {
            ascii -= 32;
        }

        // Store the ASCII value in the vector
        ascii_values.push_back(ascii);
    }
    // Close the file
    file.close();
}


// Function to scale a vector of samples - calculates scaling factor from dB level
vector<double> scale(vector<double> data, double dB) {
    double scale = pow(10.0, dB / 20.0);                    // Convert dB to linear scale: amplitude = 10^(dB/20)
    double xmin = *min_element(data.begin(), data.end());   // Find minimum element in data
    double xmax = *max_element(data.begin(), data.end());   // Find maximum element in data
    double ymin = xmin * scale;                             // Scale the minimum value
    double ymax = xmax * scale;                             // Scale the maximum value

    for (size_t t = 0; t < data.size(); t++) {              // Loop through data
        data[t] = ymin + (data[t] - xmin) * scale;          // Make sure data is within the range ymin to ymax
    }

    return data;
}


// Function to generate a pcm file from a vector of samples
class Wave {
public:
    Wave(double dur, int nOvertones, double dB, int sr)             // Constructor
        : dur(dur), nOvertones(nOvertones), dB(dB), sr(sr) {        // Initialize member variables
        twopi = 2 * M_PI;                                           // Initialize twopi
    }

    // Function to generate a square wave
    vector<double> Square(double fundamental) {                     // Function declaration
        double nyquist = sr / 2.0;                                  // Nyquist frequency (half the sample rate)
        if ((2 * nOvertones - 1) * fundamental >= nyquist) {        // Check if frequency is within Nyquist limit
            nOvertones = static_cast<int>((nyquist / fundamental + 1) / 2);     // Adjust number of overtones
        }

        int nSamples = static_cast<int>(dur * sr);                  // Number of samples (duration * sample rate)
        vector<double> data(nSamples, 0.0);                         // Vector to store data

        for (int n = 1; n < nOvertones; ++n) {                      // Loop through overtones (1 to nOvertones)
            int N = 2 * n - 1;                                      // Calculate N  (N is odd for square wave)
            double freq = N * fundamental;                          // Calculate frequency  (N * fundamental)
            double A = 1.0 / N;                                     // Calculate amplitude  (1 / N)
            double phase = 0;                                       // Initialize phase to 0
            double phaseInc = (twopi * freq) / sr;                  // Calculate phase increment

            for (int t = 0; t < nSamples; ++t) {                    // Loop through samples
                data[t] += A * sin(phase);                          
                phase += phaseInc;                                  // Increment phase
                if (phase >= twopi) {                               // Check if phase is greater than twopi
                    phase -= twopi;                                 // Subtract twopi from phase
                }
            }
        }

        scale(data, dB);

        return data;
    }

    // Function to generate a sawtooth wave
    vector<double> Sawtooth(double fundamental) {
        double nyquist = sr / 2.0;                                  // Nyquist frequency (half the sample rate)
        if (nOvertones * fundamental >= nyquist) {                  // Check if frequency is within Nyquist limit
            nOvertones = static_cast<int>(nyquist / fundamental);   // Adjust number of overtones
        }

        int nSamples = static_cast<int>(dur * sr);                  // Number of samples (duration * sample rate)
        vector<double> data(nSamples, 0.0);                         // Vector to store data

        for (int n = 1; n <= nOvertones; ++n) {                     // Loop through overtones
            double freq = n * fundamental;                          // Calculate frequency
            double A = 1.0 / n;                                     // Calculate amplitude
            double phase = 0;                                       // Initialize phase to 0
            double phaseInc = (twopi * freq) / sr;                  // Calculate phase increment

            for (int t = 0; t < nSamples; ++t) {                    // Loop through samples
                data[t] += A * sin(phase);                          // Add sample to data
                phase += phaseInc;                                  // Increment phase
                if (phase >= twopi) {                               // Check if phase is greater than twopi
                    phase -= twopi;                                 // Subtract twopi from phase
                }
            }
        }

        scale(data, dB);

        return data;
    }

private:
    double dur;
    int nOvertones;
    double dB;
    int sr;
    double twopi;
};


// Function to apply an amplitude envelope to a tone
vector<double> apply_envelope(vector<double>& tone, int sr) {
    // Apply amplitude envelope
    size_t tone_size = tone.size();
    for (size_t i = 0; i < tone_size; ++i) {
        double t = static_cast<double>(i) / tone_size; // normalized time from 0 to 1
        double envelope;
        if (t < 0.5) {
            // First half of the tone: linear increase from 0 to 1
            envelope = 2 * t;
        } else {
            // Second half of the tone: linear decrease from 1 to 0
            envelope = 2 * (1 - t);
        }
        tone[i] *= envelope;
    }

    return tone;
}


// Function to generate tones from ASCII values
vector<double> generate_tones_from_ascii(const vector<int>& ascii_values, int sr) {
    vector<double> result; // Declare result
    Wave wave(0.1, 10, 83, sr); // duration, overtones, dB, signal rate

    for (int ascii_value : ascii_values) {
        
        if (ascii_value >= 65 && ascii_value <= 90) {                       // A - Z
            double frequency = 110 * pow(2, (ascii_value - 65) / 26.0);     // Each letter becomes a tone in a 26 tone octave
            vector<double> tone = wave.Sawtooth(frequency);                 // Generate Sawtooth wave

            // Apply amplitude envelope
            tone = apply_envelope(tone, sr);

            result.insert(result.end(), tone.begin(), tone.end());
        } else {
            int nSamples = static_cast<int>(0.2 * sr);
            vector<double> silence(nSamples, 0.0);
            result.insert(result.end(), silence.begin(), silence.end());
        }
    }

    return result;
}


// Function to generate a pcm file from a vector of samples
void write_to_pcm(const string& filename, vector<double>& data, double dB) {
    ofstream file(filename, ios::binary);                                       // Open file in binary mode
    if (!file.is_open()) {
        cout << "Failed to open PCM file " << filename << " for writing.\n";
        return;
    }

    // Scale the data
    scale(data, dB);                                                            

    for (double sample : data) {
        // Scale the sample to the range of int16_t and write it to the file
        int16_t value = static_cast<int16_t>(sample * 32767);                   // Scale the sample to the range of int16_t
        file.write(reinterpret_cast<const char*>(&value), sizeof(int16_t));     // int16_t represents a 16-bit signed integer
        // Check if the write operation was successful
        if (file.fail()) {
            cout << "Failed to write to PCM file " << filename << ".\n";
            return;
        }
    }
}

// Function to store text & print, with a pause between each line
void store_text() {
    // Create a linked list of strings
    list<string> lines;

    // Add the lines to the list
    lines.push_back("");
    lines.push_back("I set myself many more constraints:");
    lines.push_back("I placed the sounds as you do letters,");
    lines.push_back("one after the other, so as to create");
    lines.push_back("forms and sequences");
    lines.push_back("");
    lines.push_back("- Bernard Parmegiani: Sound Thinking");
    lines.push_back("");

    // Print the lines with a 0.6 second pause between each line
    for (const string& line : lines) {                              // Loop through the lines
        cout << line << endl;
        this_thread::sleep_for(chrono::milliseconds(600));          // Pause for 0.6 seconds
    }
}


// main calls functions to convert text to ASCII values,
// generate tones from ASCII values, and write the tones to a PCM file
// Then calls the storeText function to print the quote

int main() {

    int sr = 44100;     // sample rate (44100Hz allows for 20Hz - 20kHz audio range)
    double dB = 85.0;   // decibels    (85dB is a typical volume level for music)
    int overtones = 10; // number of overtones for the waves (how many harmonics to include)

    // Vector to store ASCII values
    vector<int> ascii_values;

    // Prepare the filename
    string filename = "26EDO.pcm";

    // Call the convertText function
    convert_text("README.txt", ascii_values);

    // Prepare the audio data
    vector<double> data = generate_tones_from_ascii(ascii_values, sr); 

    // Call write_to_pcm function to write the sequence to a PCM file
    write_to_pcm("26EDO.pcm", data, dB);

    // Call the storeText function
    store_text();

    return 0;
}

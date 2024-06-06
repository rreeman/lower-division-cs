/*
******************
* Note Sequences *
* DMath Project  *
* Richard Reeman *
******************

This program calculates the number of 4 note sequences
which are possible if the first note is fixed

The remaining three notes can be any of the 12 notes in an octave
(These notes are A, A#, B, C, C#, D, D#, E, F, F#, G, G#)
As is music, repetition is allowed. The same note can be used multiple times
Order is also important. The program will be counting permutations

The program outputs the number of possible sequences

*/

#include <iostream>     // for input/output
#include <cmath>        // for pow
#include <string>
#include <vector>       
#include <sstream>      // for istringstream (to split strings)
#include <limits>       // for numeric_limits (print_with_pause)

using namespace std;


// Function to split a string into a vector of strings
vector<string> split(const string& str, char delimiter) {
    vector<string> tokens;                              // Vector to store the tokens
    string token;                                       // String to store each token
    istringstream tokenStream(str);                     // Stream to read from the string
    while (getline(tokenStream, token, delimiter)) {    // Read from stream until delimiter is found
        tokens.push_back(token);                        // Add the token to the vector
    }
    return tokens;
}


// Function to print text a few lines at a time
void print_with_pause(string text, int num_lines) {         // num_lines is number of lines to print before pausing
    // Split the text into lines
    vector<string> lines = split(text, '\n');               // Split text into lines

    // Print the lines with a pause after every num_lines lines
    for (int i = 0; i < lines.size(); i++) {
        cout << lines[i] << endl;

        if ((i + 1) % num_lines == 0) {                         // If we have printed num_lines lines
            // Prompt the user to press Enter
            cout << "Press Enter to continue...";               // Prompt the user to press Enter
            cin.ignore(numeric_limits<streamsize>::max(),'\n'); // Wait for the user to press Enter
        }
    }
}


// Function to calculate the number of permutations
int permutations(int n, int r) {
    return pow(n, r);
}


int main() {

    // The text to be printed
    string text = "******************\n"
        "* Note Sequences *\n"
        "* DMath Project  *\n"
        "* Richard Reeman *\n"
        "******************\n"
        "This program calculates the number of possible 4 note musical sequences.\n"
        "Any musical sequence which uses the twelve tone octave can be transcribed up or down.\n"
        "To avoid sequences which can be duplicated by transcription, the first note is fixed.\n"
        "The remaining three notes can be chosen from any of the 12 notes in an octave.\n"
        "(These notes are A, A#, B, C, C#, D, D#, E, F, F#, G, G#)\n"
        "In music, note order is important, which makes each sequence a permutation.\n"
        "Repetition is allowed, and with 12 choices for each note, we have 12 choices every time.\n"
        "In a four note sequence, with the first note fixed, there are 12^3 permutations.\n"
        "A permutation function raises the number of choices (n) to the power of the positions (r).\n"
        "This is the function:\n"
        " \n"
        "   int permutations(int n, int r) {\n"
        "       return pow(n, r);\n"
        "   }\n"
        " \n"
        "It is called from main with the following code:\n"
        " \n"
        "   int num_notes = 12;\n"
        "   int num_sequence = 4;\n"
        "   int num_possible_sequences = permutations(num_notes, num_sequence -1);\n"
        "int num_notes represents the number of notes in an octave, and\n"
        "int num_sequence represents the number of notes in a sequence.\n"
        "The code can be adjusted to calculate possible sequences of different lengths using different scales.\n"
        "To do this, adjust num_notes to the number of notes in the desired scale and num_sequence to the number of notes in the sequence.\n";

    // Print the text 5 lines at a time
    print_with_pause(text, 5);

    // Number of notes in an octave
    int num_notes = 12;

    // Number of notes in the sequence we want to calculate
    int num_sequence = 4;

    // Number of possible sequences
    int num_possible_sequences = permutations(num_notes, num_sequence -1);  // -1 because the first note is fixed

    cout << "Number of possible 4-note sequences starting with a fixed note: " << num_possible_sequences << endl;

    return 0;
}

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;
const string englishOrder = "ETAOINSHRDLCUMWFGYPBVKJXQZ";

// reading ciphertext.txt
// returns cipher string
string readCipherFile(const string fileName) {
    ifstream inputFile(fileName);
    string cipher = "";
    string line;
    vector<string> lines;

    if (!inputFile.is_open()) {
        cout << "Could not open cipher file!: " << fileName << endl;
        return "";
    }
    while (getline(inputFile, line)) {
        lines.push_back(line);
    }
    inputFile.close();
    for (int i = 0; i < lines.size(); i++) {
        cipher += lines[i];
        if (i != lines.size() - 1) {
            cipher += "\n";
        }
    }
    return cipher;
}

// reading dictionary.txt
// returns dict of words from file
unordered_map<string, bool> readDictionaryFile(const string fileName) {
    ifstream inputFile(fileName);
    unordered_map<string, bool> dictionary;
    
    if (!inputFile.is_open()) {
        cout << "Could not open dictionary file!: " << fileName << endl;
        return dictionary;
    }

    string word;
    while (getline(inputFile, word)) {
        dictionary[word] = true;
    }

    inputFile.close();
    return dictionary;
}

// sort cipher characters in descending order of frequency
// used for sort algorithm (comparrison function)
bool sortByFreq(const pair<char, int> &a, const pair<char, int> &b) {
    return a.second > b.second;
}

// maps the count of each char in a cipher based on 
// {letter: (int) count}
// returns mapped dict
unordered_map<char, int> countCipherLetters(const string cipher) {
    unordered_map<char, int> freqMap;
    for (char c : cipher) {
        if (isalpha(c)) {
            freqMap[toupper(c)]++;
        }
    }
    // works up to here
    return freqMap;
}

// takes mapped of cipher characters, with user cipher
// sorts the cipher based on highest count, then lines that up with english letter freq
// after sorting dict of characters, goes through and 
string alphabeticallySortCipher(unordered_map<char, int> freqMap, const string cipher) {
    vector<pair<char, int>> freqVec(freqMap.begin(), freqMap.end());
    sort(freqVec.begin(), freqVec.end(), sortByFreq);

    // map cipher letters to english freq based on sorted freq, dict, and english order
    unordered_map<char, char> cipherMappedOnEnglish;
    for (int i = 0; i < freqVec.size() && i < englishOrder.size(); i++) {
        cipherMappedOnEnglish[freqVec[i].first] = englishOrder[i];
    }

    // generate our new cipher based on analyzed frequency
    string newCipher = "";
    for (int i = 0; i < cipher.size(); i++) {
        char newLetter = cipherMappedOnEnglish[cipher[i]];
        newCipher.push_back(newLetter);
    }
    return newCipher;
}

string findWords(const string& sortedCipher, unordered_map<string, bool>& dictionary) {
    string decipheredText;  // final deciphered text

    // loop
    for (int p = 0; p < sortedCipher.size();) {
        string bestWord;
        string word;

        // build words starting from position "p"
        for (int i = p; i < sortedCipher.size(); i++) {
            word += sortedCipher[i];  // add next letter to word

            // if word is found, update bestWord
            if (dictionary.find(word) != dictionary.end() && word.length() > bestWord.length()) {
                    bestWord = word;
            }
        }

        // add bestWord (if found) to deciphered text
        if (!bestWord.empty()) {
            if (!decipheredText.empty() && decipheredText.back() != ' ') {
                decipheredText += ' ';
            }
            decipheredText += bestWord;
            p += bestWord.length();
        } else {
            decipheredText += sortedCipher[p];
            p++;
        }
    }

    return decipheredText;
}




int main() {
    string userCipher = readCipherFile("ciphertext.txt");
    unordered_map<string, bool> myDict = readDictionaryFile("dictionary.txt");
    cout << "starting cipher:\n" + userCipher << endl;

    unordered_map<char, int> frequencyMap = countCipherLetters(userCipher);
    string sortedCipher = alphabeticallySortCipher(frequencyMap, userCipher);
    string deCipher = findWords(sortedCipher, myDict);


    cout << "alphabetically sorted cipher:\n" + sortedCipher << endl;
    cout << "Deciphered cipher:\n" + deCipher << endl;




    return 0;
}
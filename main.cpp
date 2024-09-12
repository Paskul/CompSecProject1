
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

// by the frequency of english alphabet,
// find a initial mapping
// returns mapping
unordered_map<char, char> alphabeticallyFindKey(unordered_map<char, int> freqMap, const string cipher) {
    vector<pair<char, int>> freqVec(freqMap.begin(), freqMap.end());
    sort(freqVec.begin(), freqVec.end(), sortByFreq);

    unordered_map<char, char> cipherMappedOnEnglish;
    for (int i = 0; i < freqVec.size() && i < englishOrder.size(); i++) {
        cipherMappedOnEnglish[freqVec[i].first] = englishOrder[i];
    }

    return cipherMappedOnEnglish;
}

// apply any mapping of letters
// to any cipher
// returns the new cipher (string)
string applyKeyToCipher(unordered_map<char, char> keyMap, const string cipher) {
    string newCipher = "";
    for (char c : cipher) {
        if (isalpha(c)) {
            char upperC = toupper(c);
            newCipher += keyMap.at(upperC);
        } else {
            newCipher += c;
        }
    }
    return newCipher;
}

// count how many words are present in a cipher
// expects cipher and dictionary of words
// currently grows off word length, can off # words
int countWords(const string cipher, unordered_map<string, bool> wordsDict) {
    int wordCount = 0;
    for (int start = 0; start < cipher.size(); start++) {
        string word = "";
        for (int end = start; end < cipher.size(); end++) {
            if (isalpha(cipher[end])) {
                word += cipher[end];
            }
            string upperWord = word;
            transform(upperWord.begin(), upperWord.end(), upperWord.begin(), ::toupper);

            if (wordsDict.find(word) != wordsDict.end()) {
                //wordCount++;
                // INCREASING ON WORD SIZE, BETTER RESULT
                // ALWAYS CAN REVERT TO BASE WORD COUNT +1
                wordCount+=word.size();
            }
        }
    }
    return wordCount;
}

// swap two letters in a key and return new key
// used for improveBySwapping
unordered_map<char, char> swapKeyLetters(const unordered_map<char, char>& keyMap, char letter1, char letter2) {
    unordered_map<char, char> newKey = keyMap;
    char mappedLetter1 = newKey[letter1];
    char mappedLetter2 = newKey[letter2];

    newKey[letter1] = mappedLetter2;
    newKey[letter2] = mappedLetter1;

    return newKey;
}

// brute-force swapping of key letters to improve word count (decryption)
// keeps operating as long as we are making improvements
// scans entire keyspace to find an improvement
unordered_map<char, char> improveKeyBySwapping(unordered_map<char, char> currentKey, const string& cipherText, const unordered_map<string, bool>& dictionary) {
    bool improved = true;
    int bestWordCount = countWords(applyKeyToCipher(currentKey, cipherText), dictionary);

    while (improved) {
        improved = false;
        // swap every letter pairs
        for (auto firstLetter = currentKey.begin(); firstLetter != currentKey.end(); firstLetter++) {
            for (auto secondLetter = next(firstLetter); secondLetter != currentKey.end(); secondLetter++) {

                unordered_map<char, char> newKey = swapKeyLetters(currentKey, firstLetter->first, secondLetter->first);
                string newDecryptedText = applyKeyToCipher(newKey, cipherText);
                int newWordCount = countWords(newDecryptedText, dictionary);

                if (newWordCount > bestWordCount) {
                    currentKey = newKey;
                    bestWordCount = newWordCount;
                    improved = true;
                }
            }
        }
    }
    return currentKey;
}
string spacesImproved(const string& improvedCipher, unordered_map<string, bool>& dictionary) {
    string decipheredText;
    string bestWord;

    for (int i = 0; i < improvedCipher.size(); ) {
        string word;
        string longestWord;

        // Try to build the longest word starting from position i
        for (int j = i; j < improvedCipher.size(); j++) {
            word += improvedCipher[j];

            // If this word exists in the dictionary and is longer than the previous best
            if (dictionary.find(word) != dictionary.end()) {
                longestWord = word;  // Update longest word found
            }
        }

        // If we found a valid word, add it to the deciphered text
        if (!longestWord.empty()) {
            decipheredText += longestWord + ' ';  // Append word and space
            i += longestWord.size();  // Move pointer forward by the word's length
        } else {
            // No valid word found, so just add the current character
            decipheredText += improvedCipher[i];
            i++;  // Move forward by 1 character
        }
    }

    return decipheredText;
}

string manualSwap(string cipherText, char old, char mew) {
    for(char& i : cipherText) {
        if (i == old){
            i = mew;
        }
    }
    return cipherText;
}

void manuallyUpdateKey(unordered_map<char, char>& keyMap) {
    char cipherLetter, newMappedLetter;

            cout << "Enter the cipher letter you want to change: ";
            cin >> cipherLetter;

            // Prompt the user for the new letter to map it to
            cout << "Enter the new letter to map '" << cipherLetter << "' to: ";
            cin >> newMappedLetter;

            // Update the mapping in the key
            keyMap[toupper(cipherLetter)] = toupper(newMappedLetter);

            // Show the updated key mapping
            cout << "Updated Key Mapping:\n";
            for (auto& pair : keyMap) {
                cout << pair.first << " -> " << pair.second << endl;
            }
        }



int main() {
    string userCipher = readCipherFile("ciphertext.txt");
    unordered_map<string, bool> myDict = readDictionaryFile("dictionary.txt");

    cout << "starting cipher:\n" + userCipher << endl;

    unordered_map<char, int> frequencyMap = countCipherLetters(userCipher);
    unordered_map<char, char> key = alphabeticallyFindKey(frequencyMap, userCipher);
    string sortedCipher = applyKeyToCipher(key, userCipher);

    cout << "alphabetically sorted cipher:\n" + sortedCipher << endl;
    int totalWords = countWords(sortedCipher, myDict);
    cout << "total words with first key: " << totalWords << endl;

    // -----------------------------

    unordered_map<char, char> improvedKey = improveKeyBySwapping(key, userCipher, myDict);
    string improvedCipher = applyKeyToCipher(improvedKey, userCipher);

    cout << "improved decrypted text:\n" << improvedCipher << endl;

    string spacedImprovedCipher = spacesImproved(improvedCipher, myDict);

    cout << "Improved deciphered text with spaces:\n" << spacedImprovedCipher << endl;

    int improvedWordCount = countWords(improvedCipher, myDict);
    cout << "total valid words with improved key: " << improvedWordCount << endl;

    cout << "\nFinal key:" << endl;
    for (auto& pair : improvedKey) {
        cout << pair.first << " = " << pair.second << endl;
    }

    while(true) {

        manuallyUpdateKey(improvedKey);

        // Apply the new key after manual updates
        string manuallyUpdatedCipher = applyKeyToCipher(improvedKey, userCipher);
        cout << "\nManually updated decrypted text:\n" << spacesImproved(manuallyUpdatedCipher, myDict) << endl;
        int manualWordCount = countWords(manuallyUpdatedCipher, myDict);
        cout << "total valid words with improved key: " << manualWordCount << endl;
    }

    return 0;
}
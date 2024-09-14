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

void printKey(unordered_map<char, char> printingKey) {
    for (const auto& pair : printingKey) {
        cout << pair.first << " -> " << pair.second << endl;
    }
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

int main() {
    string userCipher = readCipherFile("ciphertext.txt");
    cout << "--CIPHER LOADED--" << endl;
    unordered_map<string, bool> myDict = readDictionaryFile("dictionary.txt");
    cout << "--DICT LOADED--" << endl;

    cout << "Cipher:\n" << userCipher << endl;
    int firstWords = countWords(userCipher, myDict);
    cout << "Word points with starting cipher: " << firstWords << endl;

    // -----------------------------

    cout << "--STARTING ALPHABETICAL FREQUENCY DECIPHER--" << endl;
    unordered_map<char, int> frequencyMap = countCipherLetters(userCipher);
    unordered_map<char, char> key = alphabeticallyFindKey(frequencyMap, userCipher);
    string sortedCipher = applyKeyToCipher(key, userCipher);
    cout << "--COMPLETED ALPHABETICAL FREQUENCY DECIPHER--" << endl;

    cout << "Alphabetically made key:" << endl;
    printKey(key);
    cout << "Alphabetically sorted cipher:\n" + sortedCipher << endl;
    int totalWords = countWords(sortedCipher, myDict);
    cout << "Word points with alphabetical frequency key: " << totalWords << endl;

    // -----------------------------

    cout << "--STARTING BRUTE FORCE DECIPHER--" << endl;
    unordered_map<char, char> improvedKey = improveKeyBySwapping(key, userCipher, myDict);
    string improvedCipher = applyKeyToCipher(improvedKey, userCipher);

    cout << "Brute force made key:" << endl;
    printKey(improvedKey);
    cout << "Brute force decrypted cipher:\n" << improvedCipher << endl;
    int improvedTotalWords = countWords(improvedCipher, myDict);
    cout << "Word points with brute force key: " << improvedTotalWords << endl;
    key = improvedKey;
    
    // -----------------------------

    cout << "USER: Would you like to manually decipher/alter the key mapping?" << endl;
    cout << "(y/n)" << endl;
    string yesOrNo;
    cin >> yesOrNo;
    bool keepGoing = true;
    while (keepGoing) {
        if (yesOrNo == "y") {
            char a, b;
            cout << "Enter letter to swap: " << endl;
            cin >> a;
            cout << "Enter letter to swap with: " << endl;
            cin >> b;

            char newA = toupper(a);
            char newB = toupper(b);
            unordered_map<char, char> newKey = swapKeyLetters(key, newA, newB);
            key = newKey;

            cout << "Sucsess swap of " << a << " and " << b << endl;            
            cout << "Would you like to continue deciphering?" << endl;
            cout << "(y/n)" << endl;
            cin >> yesOrNo;
        } else if (yesOrNo == "n") {
            cout << "Recieved 'n'. Closing program" << endl;
            keepGoing = false;
        } else {
            cout << "Invalid input! Must type 'y' or 'n'" << endl;
            cin >> yesOrNo;
        }

        cout << "Current key:" << endl;
        printKey(key);
        string curCipher = applyKeyToCipher(key, userCipher);
        cout << "Current cipher:\n" << curCipher << endl;
        int curTotalWords = countWords(curCipher, myDict);
        cout << "Current word points:\n" << curTotalWords << endl;
    }
    
    return 0;
}

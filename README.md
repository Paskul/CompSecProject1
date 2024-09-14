Pascal Sikorski Computer Security - Project 1

This program operates with the expected directory running containing:
- ciphertext.txt
- dictionary.txt
- main.cpp
- Makefile

To execute our program, make the Makefile, where you will have decipherExec as an executable. 

1. Display the frequency analysis of the ciphertext.
On execution, the program will automatically run the frequency analysis after the dictionary and cipher text are loaded as the first step (1 of 3). The analysis results will automatically be displayed after frequency is mapped from one letter to the next.

2. Use the program to guess the cipher key and decrypt the text using it.
After alphabetical analysis of the ciphertext, the next step automatically commences brute forcing through all permutations of keys, where 'word points' are accumulated for each attempt, these being increased based on the word length of found words in our partial decipher. The program keeps iterating until it finds absolutely no further options to improve it's 'word points', and returns the new key. The partial decipher, key, and points are displayed at this step as well. (2 of 3)

3. Decrypt the text using a key entered by the user.
Finally, after both steps of automation, the last step is to accept manual input from the user, already acting on a mostly correct key. Here the user can make exact changes to the keymap, swapping by pairs, and see the results of the change in real-time. After each pair swap, you can see the new cipher, new key map, and the 'word points' of the current cipher. (3 of 3)

The program ends when the user operates through the I/O and is able to say 'n' to continue manual swaps.

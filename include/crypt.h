#ifndef CRPYT_H
#define CRPYT_H

typedef char* string;

string generate_random_key();

string transposition_cipher   (string originalText, string key);
string transposition_decipher (string cryptedText,  string key);

#endif CRPYT_H

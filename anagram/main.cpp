#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include "md5.h"

using namespace std;


struct Anagram {
    // freqs counts the frequency of each english letter
    // We are not currently supporting UTF-8
    size_t freqs[26];
    vector<string> strings;

    Anagram() {  for (int i = 0; i < 26; i++) { freqs[i] = 0; } }

    void Reset() { for (int i = 0; i < 26; i++) { freqs[i] = 0; } }
    void AddString(string str) { strings.push_back(str); }
    size_t GetHash() const
    {
        size_t ret = 0;
        MD5_CTX ctx;
        MD5Init(&ctx);
        MD5Update(&ctx, (unsigned char *)freqs, 26*sizeof(size_t));
        MD5Final(&ctx);
        for (int i = 0; i < (DIGEST_LEN/sizeof(size_t)); i++) {
            ret += ((size_t *)ctx.digest)[i];
        }
        return ret;
    }

    void GetFreq( string str ) {
        for (string::const_iterator it = str.begin(); it != str.end(); it++) {
            char val = *it - 'A';
            val = (val > 25) ? val - 0x20 : val;
            freqs[val]++;
        }
    }
};

struct AnaEq {
    bool operator()(const Anagram * const lhs, const Anagram * const rhs) const {
        for (int i = 0; i < 26; i++) {
            if (lhs->freqs[i] != rhs->freqs[i]) { return false; }
        }
        return true;
    }
};

struct HashAnagramPtr {
    bool operator()( const Anagram *ana ) const { return ana->GetHash(); }
};

typedef unordered_map<Anagram *, Anagram *, HashAnagramPtr, AnaEq> AnagramMap;

void FindAnagrams( size_t strCount, vector<string> strings )
{
    AnagramMap Anagrams;
    Anagram * curr = new Anagram();
    AnagramMap::iterator ana_it;

    for (int i = 0; i < strCount; i++) {
        curr->GetFreq( strings[i] );
        if ((ana_it = Anagrams.find(curr)) != Anagrams.end()) {
            ana_it->first->AddString( strings[i] );
            curr->Reset();
        }
        else {
            curr->AddString(strings[i]);
            Anagrams[curr] = curr;
            curr = new Anagram();
        }
    }

    for (ana_it = Anagrams.begin(); ana_it != Anagrams.end(); ++ana_it) {
        for (int i = 0; i < ana_it->first->strings.size(); i++) {
            cout << ana_it->first->strings[i] << endl;
        }
    }
    delete curr;

}

int main(int argc, char ** argv)
{
    vector<string> strings;
    for (string str; getline(cin, str); /* none */) {
        strings.push_back(str);
    }
    FindAnagrams( strings.size(), strings );
}

#ifndef MAKE_PLURAL_H
#define MAKE_PLURAL_H
#include<vector>
#include<string>
using std::string;

inline
string make_plural(size_t ctr, const string &words,
	const string &ending)
{
	return (ctr > 1) ? words + ending : words;
}
#endif

#ifndef MY_QUERYRESULT_H
#define MY_QUERYRESULT_H
#include<iostream>
#include<string>
#include<vector>
#include<memory>
#include<set>
#include"my_StrBlob.h"

class QueryResult {
	friend std:: ostream& print(std::ostream &os, const QueryResult &qr);
public:
	typedef std::vector<std::string>::size_type line_no;
	typedef std::set<line_no>::const_iterator line_it;
	QueryResult(std::string s,
		        StrBlob p,
		        std::shared_ptr<std::set<line_no> >q) : 
		sought(s), text(p), lines(q) {}
	std::set<line_no>::size_type size() const { return lines->size(); }
	line_it begin() const { return lines->cbegin(); }
	line_it end() const { return lines->cend(); }
	StrBlob get_file() { return text; }
private:
	std::string sought;
	StrBlob text;
	std::shared_ptr<std::set<line_no> > lines;
};

#endif 

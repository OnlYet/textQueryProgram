#include"my_TextQuery.h"
#include"make_plural.h"

#include<cstddef>
#include<memory>
#include<sstream>
#include<string>
#include<vector>
#include<map>
#include<set>
#include<iostream>
#include<fstream>
#include<cctype>
#include<cstring>
#include<utility>

using std::size_t;
using std::shared_ptr;
using std::istringstream;
using std::string;
using std::getline;
using std::vector;
using std::map;
using std::set;
using std::cerr;
using std::cout;
using std::cin;
using std::ostream;
using std::endl;
using std::ifstream;
using std::ispunct;
using std::tolower;
using std::strlen;
using std::pair;

TextQuery::TextQuery(ifstream &in) : text(new vector<string> ) 
{
	string line;
	while (getline(in, line))
	{
		text.push_back(line);
		int n = text.size() - 1; //保存当前行号
		istringstream istr(line);
		string word;
		while (istr >> word)
		{   
			word = cleanup_str(word);
			auto &c = search[word];
			if (!c)
				c.reset(new set<line_no>);
			c->insert(n);
		}
	}
}

ostream& print(ostream& os, const QueryResult &qr)
{
     //如果找到了单词，打印出现次数及所有出现的行号
	os << qr.sought << " occurs " << qr.lines->size()
		<< make_plural(qr.lines->size(), " time", "s") << endl;
	//打印单词出现的每一行
	for (auto c : *(qr.lines))
		os << "(line " << c+1 << ")\t" 
		   << qr.text.begin().deref(c) << endl;
	return os;
}

QueryResult TextQuery::Query(const string &s) const         //为什么不能返回QueryResult &
{
	//如果未找到sought，将返回一个指向下面这个set的指针
	static shared_ptr<set<line_no> > nodata(new set<line_no>());
	//使用find而不是下标操作的原因是避免将不在search中的单词添加进去！
	auto loc = search.find(cleanup_str(s));
	if (loc == search.end())
		return QueryResult(s, text, nodata);
	else
		return QueryResult(s, text, loc->second);
}

//cleanup_str删除标点，并将所有文本转换为小写形式，从而查询是大小写不敏感的
string TextQuery::cleanup_str(const string &word)
{
	string ret;
	for (auto it = word.begin(); it != word.end(); ++it) {
		if (!ispunct((unsigned char)*it))
			ret += tolower(*it);
	}
	return ret;
}

void TextQuery::display_map()
{
	auto iter = search.cbegin(), iter_end = search.cend();

	//对map中的每个单词
	for (; iter != iter_end; ++iter) {
		cout << "word: " << iter->first << " {";

		//以常量引用方式获取位置向量，避免拷贝
		auto text_locs = iter->second;
		auto loc_iter = text_locs->cbegin(),
			loc_iter_end = text_locs->cend();

		//打印此单词出现的所有行号
		while (loc_iter != loc_iter_end)
		{
			cout << *loc_iter;
			if (++loc_iter != loc_iter_end)
				cout << ", ";
		}
		cout << "}\n";			//此单词的输出列表结束
	}
	cout << endl;				//结束整个map的输出
}

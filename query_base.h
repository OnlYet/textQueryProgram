#ifndef QUERY_BASE_H
#define	QUERY_BASE_H
#include<string>	
#include<memory>
#include<algorithm>			//set_intersection
#include<iterator>			//inserter
#include"my_TextQuery.h"

class Query_base {
	friend class Query;
protected:
	using line_no = TextQuery::line_no;
	virtual ~Query_base() = default;
private:
	virtual QueryResult eval(const TextQuery&) const = 0;
	virtual std::string rep() const = 0;
};

class Query {
	//这些运算符需要访问接受shared_ptr的构造函数， 而该函数是私有的
	friend Query operator~(const Query &);		//使用指向NotQuery对象的智能智能来初始化Query构造函数（私有）
	friend Query operator|(const Query&, const Query&);
	friend Query operator&(const Query&, const Query&);
public:
	Query(const std::string&);		//生成一个新的WordQuery对象
	Query(const Query& query) :
		q(query.q), user(query.user)
	{
		++*user;
	}
	Query& operator=(const Query& query);

	QueryResult eval(const TextQuery &t) const
	{ return q->eval(t); }
	std::string rep() const
	{ 
		return q->rep();
		std::cout << "Query::rep()" << std::endl;
	}
	~Query()		//destructor
	{
		if (--*user == 0)
		{
			delete user;
			delete q;
		}
	}
private:
	//Query(std::shared_ptr<Query_base> query) : q(query)
	//{ std::cout << "Query(std::shared_ptr<Query_base> query)" << std::endl; }
	//std::shared_ptr<Query_base> q;
	Query(Query_base* query) : q(query), user(new int(1)) {}
	Query_base* q;
	int* user;		//记录有多少对象共享*q
};

Query& Query::operator=(const Query& query)
{
	++*query.user;
	if (--*user == 0)
	{
		delete user;
		delete q;
	}
	q = query.q;
	user = query.user;
	return *this;
}

std::ostream&
operator<<(std::ostream &os, const Query &query)
{
	//Query::rep通过它的Query_base指针对rep()进行了虚调用
	return os << query.rep();
}

class WordQuery : public Query_base {
	friend class Query;			//Query使用WordQuery构造函数
	WordQuery(const std::string &s) : query_word(s)
	{ std::cout << "WordQuery(const std::string&)" << std::endl; }
	QueryResult eval(const TextQuery &t) const
				{ return t.Query(query_word); }
	std::string rep() const
	{ 
		return query_word;
		std::cout << "WordQuery::rep()" << std::endl;
	}
	std::string query_word;
};
 
inline Query::Query(const std::string &s) :
	q(new WordQuery(s)), user(new int(1))
{ std::cout << "Query(const std::string&)" << std::endl; }

class NotQuery : public Query_base {
	friend Query operator~(const Query &);
	NotQuery(const Query &q) : query(q)
	{ std::cout << "NotQuery(const Query&)" << std::endl; }
	std::string rep() const 
	{ 
		return "~(" + query.rep() + ")";
		std::cout << "NotQuery::rep()" << std::endl;
	}
	QueryResult eval(const TextQuery&) const;
	Query query;		//保存一个需要对其取反的Query
};

inline Query operator~(const Query &operand)
{
	//return std::shared_ptr<Query_base>(new NotQuery(operand));
	//NotQuery nq(operand);
	//return &nq;
	return new NotQuery(operand);
}

class BinaryQuery : public Query_base {
protected:
	BinaryQuery(const Query &l, const Query &r, std::string s) :
		lhs(l), rhs(r), opSym(s)
	{std::cout << "BinaryQuery(const Query&,const Query&,std::string)" << std::endl;}
	std::string rep() const {
		return "(" + lhs.rep() + " "
				   + opSym + " "
				   + rhs.rep() + ")";
		std::cout << "BinaryQuery::rep()" << std::endl;
	}
	Query lhs, rhs;
	std::string opSym;
};

class AndQuery : public BinaryQuery {
	friend Query operator&(const Query&, const Query&);
	AndQuery(const Query &left, const Query &right) :
		BinaryQuery(left, right, "&") 
	{
		std::cout << "AndQuery(const Query&, const Query&)" << std::endl;
	}
	QueryResult eval(const TextQuery&) const;
};

inline Query operator&(const Query &lhs, const Query &rhs)
{
	//return std::shared_ptr<Query_base>(new AndQuery(lhs, rhs));
	return new AndQuery(lhs, rhs);
}

class OrQuery : public BinaryQuery {
	friend Query operator| (const Query&, const Query&);
	OrQuery(const Query &left, const Query &right) :
		BinaryQuery(left, right, "|")
	{
		std::cout << "OrQuery(const Query&, const Query&)" << std::endl;
	}
	QueryResult eval(const TextQuery&) const;
};

inline Query operator|(const Query &lhs, const Query &rhs)
{
	//return std::shared_ptr<Query_base>(new OrQuery(lhs, rhs));
	return new OrQuery(lhs, rhs);
}

QueryResult
OrQuery::eval(const TextQuery& text) const
{
	auto right = rhs.eval(text), left = lhs.eval(text);
	auto ret_lines =
		make_shared<set<line_no>>(left.begin(), left.end());
	ret_lines->insert(right.begin(), right.end());
	return QueryResult(rep(), left.get_file(), ret_lines);
}

QueryResult
AndQuery::eval(const TextQuery& text) const
{
	auto left = lhs.eval(text), right = rhs.eval(text);
	auto ret_lines = make_shared<set<line_no>>();
	set_intersection(left.begin(), left.end(),
					 right.begin(), right.end(),
					 inserter(*ret_lines, ret_lines->begin()));
	return QueryResult(rep(), left.get_file(), ret_lines);
}

QueryResult
NotQuery::eval(const TextQuery& text) const
{
	auto result = query.eval(text);
	auto ret_lines = make_shared<set<line_no>>();
	auto beg = result.begin(), end = result.end();
	auto sz = result.get_file().size();
	for (size_t n = 0; n != sz; ++n) {
		if (beg == end || *beg != n)
			ret_lines->insert(n);
		else if (beg != end)
			++beg;
	}
	return QueryResult(rep(), result.get_file(), ret_lines);
}
#endif

#include"my_TextQuery.h"
#include"make_plural.h"
#include"Query_base.h"
#include<iostream>
#include<string>
#include<fstream>
#include<cstdlib>
#include<sstream>

using std::string;
using std::ifstream;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;


void runQueries(ifstream &infile)
{
	//infile是一个ifstream，是我们要查询的文件
	TextQuery tq(infile);

	//tq.display_map();         //用作调试

	while (true) {
		cout << "enter word to look for, or q to exit" << endl;
		string s;
		getline(cin, s);
		ostringstream output;
		//若遇到文件尾或用户输入了‘q’,则退出循环
		if (!(cin >> s) || s == "q")
			break;
		//执行查询并打印结果
		print(cout, tq.Query(s));
	}
}

Query Test()
{
	return Query("fiery")&Query("bird") | Query("wind");
}

//程序接受单一参数，指出要查询的文件
int main(int argc, char* argv[])
{
	//打开文件，将在其中查询用户指定的单词
	ifstream in(argv[1]);
	if (!in)
	{
		cerr << "file open error" << endl;
		system("pause");
		exit(-1);
	}
	runQueries(in);
	//Query q = Test();
	Query q = Query("fiery") & Query("bird") | Query("wind");
	cout << q << endl;
	system("pause");
	return 0;
}



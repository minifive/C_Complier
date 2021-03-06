#include "stdafx.h"
#include "YaccReader.h"
#include "TOKEN.h"

using namespace std;
YaccReader::YaccReader(FILE* file,TokenManager& tokenManager, ProductionManager& productionManager) 
	:file(file), productionManager(productionManager), tokenManager(tokenManager)
{

}


YaccReader::~YaccReader()
{
	if (file != NULL){
		fclose(file);
		file = NULL;
	}
		
}

int YaccReader::read()
{
	readUntil("%{"); 
	userHeader = readUntil("%}");
	tokenDefine = readUntil("%%");
	productionDefine = readUntil("%%");
	int c;
	while ((c = fgetc(file)) != EOF){ //逐个字符读文件
		userCode += (char)c;
	}

	readtoken(tokenDefine);
	readproduct(productionDefine);
	return 0;
}


string YaccReader::readUntil(const string& s)
{
	string result = "";
	int c;
	while ((c=fgetc(file)) != EOF){
		result += (char)c;
		if (result.length() >= s.length()){
			const string& last = result.substr(result.length() - s.length(), s.length());
			if (last==s){
				result = result.substr(0, result.length() - s.length());
				break;
			}
		}
	}
	return result;
}

void YaccReader::readtoken(string tokenDefine)
{
	string buffer[100];
	int n = 0;
	int count;
	while ((count = tokenDefine.find("%", 1)) != tokenDefine.npos)//find函数找到成功返回‘%’的标记，失败返回npos
	{
		buffer[n] = tokenDefine.substr(1, count - 1);
		tokenDefine = tokenDefine.substr(count, tokenDefine.length() - count);
		if (buffer[n] =="\n")                               //忽略空白行。
			continue;
		else
			n++;		
	}
	buffer[n] = tokenDefine.substr(1, tokenDefine.length() - 1);
	for (int o = 0; o <= n; o++)               //处理‘/*****/’
	{
		count = buffer[o].find("/*");
		buffer[o] = buffer[o].substr(0, count);
	}
	for (int i = 0; i <= n; i++)
	{
		string temp = buffer[i];
		int begin;
		string hed;
		begin = buffer[i].find('\t');                      //以制表符为区分点 找出head部分。
		hed = temp.substr(0, begin);
		temp = temp.substr(begin + 1, temp.length() - begin - 1);
		if (hed=="token")
		{
			TerminalToken ttoken;    //创建一个终结符
			char seg = ' ';
			while ((begin = temp.find(seg)) !=temp.npos)
			{			
				ttoken.name= temp.substr(0, begin);
				tokenManager.buildToken(ttoken.name,ttoken.type,ttoken.associativity,ttoken.precedence);
				ttoken.name = "";
				temp = temp.substr(begin + 1, temp.length() - begin - 1);
			}
			for (int i = 0; i < temp.length(); i++)        //此for循环处理最后一个token （忽略多个回车产生的空白行）
			{
				if (temp[i] != '\n')
					ttoken.name += temp[i];
			}
			tokenManager.buildToken(ttoken.name, ttoken.type, ttoken.associativity, ttoken.precedence);
			ttoken.name = "";
		}
		else if (hed == "union")
		{
			while ((begin = temp.find("\n")) != temp.npos)             //找出union中的每一行
			{
				string line;
				line=temp.substr(0, begin);
				temp = temp.substr(begin + 1, temp.length() - begin - 1);
				if (line=="{")                 //忽略大括号
					continue;
				if (line == "\t}"||line=="}")
					break;
				else
				{
					Union uni;
					int t=line.find("\t", 1);
					string type;
					for (int i = 0; i <t; i++)
					{ 
						if ((line[i]!='\t')&&(line[i]!=' '))//忽略空格和制表符
							type += line[i];
					}
					uni.type = type;
					line = line.substr(t+1, line.length() - t-1);
					t = line.find(";", 1);
					string typenames;
					for (int i = 0; i < t; i++)
					{
						if ((line[i] >= 'a'&&line[i] <= 'z') || (line[i] >= 'A'&&line[i] <= 'Z'))  //忽略空格和制表符
							typenames += line[i];
					}
					uni.typenames = typenames;
					if (uni.type!="")
						unions.push_back(uni);
				}
			}
		} 
		else if (hed == "type")
		{
			begin = temp.find("\t");
			string typn;
			typn= temp.substr(1,begin-2);        //从1到begin-2 为了消除两端的“<>”
			temp = temp.substr(begin + 1, temp.length() - begin - 1);
			int t;
			while ((t = temp.find(" ", 1)) != temp.npos)
			{
				Type type;
				type.typenames = typn;
				type.tokennames = temp.substr(0, t);
				types.push_back(type);
				temp = temp.substr(t + 1, temp.length() - t - 1);
			}
			Type type;
			type.typenames = typn;
			for (int i = 0; i < temp.length(); i++)//处理最后一个type  -1 为了去掉 \n
			{
				if (temp[i]!='\n')
				{
					type.tokennames += temp[i];
				}
			}
			types.push_back(type);
		}
		else if (hed == "start")
		{

		}
	} //end of for
	/*为终结符添加type并根据vector<Type>types构造非终结符*/
	for (int i = 0; i < types.size(); i++)
	{
		string name=types[i].tokennames;
		string type = types[i].typenames;
		if (tokenManager.isTerminal(name))
		{
				tokenManager.setType(name, type);
		}
		else
			tokenManager.buildToken(name, type);		 //构造非中介符，存入map<int, NonterminalToken> nonterminals;中
	}	
	for (auto& i : tokenManager.allToken())
	{
		cout << "\n-------alltoken-------\n" << i.first << "--" << i.second;
	}

	/*下面的代码用来测试*/

	cout << "\n-----------The unions:---------------\n";
	for (int i = 0; i < unions.size(); i++)
		cout << unions[i].type << "\t\t" << unions[i].typenames << "\t" << endl;
	cout << "\n-----------The types:---------------\n";
	for (int i = 0; i < types.size(); i++)
		cout << types[i].tokennames << "\t" << types[i].typenames << "\t" << endl;
}
void YaccReader::readproduct(string productionDefine)
{
	string left="";
	vector<string>right;
	string action = "";
	string righttemp="";
	int flag = 1;//标记一个产生式的状态
	int flag2;//临时的。用于解析注释时来临时存放flag
	string buffer[100]; //用来存放产生式左边的内容，供测试以及查找start
	int n = 0;
	string temp = productionDefine;
	for (int i = 0; i < temp.length(); i++)
	{
		
		if ((flag == 1) && ((temp[i] >= 'a'&&temp[i] <= 'z') || (temp[i] >= 'A'&&temp[i] <= 'Z') || (temp[i] == '_') || (temp[i] == '(') || (temp[i] == ')')))//状态1遇到字母。读左侧
		{
			left += temp[i];
		}
		else if (flag == 2 && ((temp[i] >= 'a'&&temp[i] <= 'z') || (temp[i] >= 'A'&&temp[i] <= 'Z') || (temp[i] == '_')||(temp[i] == '(') || (temp[i] == ')')))//状态2遇到字母。读右侧
		{
			righttemp += temp[i];
		}
		else if (flag==1&&temp[i] == ':')
		{
			flag = 2;   //进入状态2
		}
		else if (flag==2&&(temp[i] == ' '||temp[i]=='\n'))//状态2读到空格获回车
		{
			if (righttemp == "")
				continue;
			else
			{
				right.push_back(righttemp);
				righttemp = "";
			}
		}
		else if (flag ==2 && (temp[i] == '|'))//状态2遇到|
		{
			if (right.size() == 0)
				right.push_back("");
			productionManager.buildProduction(left, right, action);
			right.clear();
		}
		else if (flag == 2 && (temp[i] == ';'))
		{
			if (right.size() == 0)
				right.push_back("");
			productionManager.buildProduction(left, right, action);
			buffer[n] = left;
			n++;
			left = "";
			action = "";
			right.clear();
			flag = 1;
		}
		else if (flag == 2 && (temp[i] == '{'))
		{
			flag = 4;
		}
		else if (flag == 4 && (temp[i]) != '}')
		{
			action += temp[i];
		}
		else if (flag == 4 && (temp[i]) == '}')//遇到}回到状态2
		{
			flag = 2;
		}
		else if (temp[i] == '/'&&temp[i + 1] == '*')
		{
			flag2 = flag;
			flag = 100;   //在还没读到*/之前不做任何事
			i++;
		}
		else if (temp[i] == '*'&&temp[i + 1] == '/')
		{
			flag = flag2;
			i++;
		}

	}//end of read
	/*处理最后一个产生式（不以分号结尾）*/
	if (left != "")
	{
		if (right.size() == 0)
			right.push_back("");
		productionManager.buildProduction(left, right, action);
		buffer[n] = left;
		left = "";
		action = "";
		right.clear();
	}
	start = buffer[0];//标记开始符
	cout << "start::" << start<<endl;
	for (auto& i : productionManager.allproductions())
	{
		/*Token emptyt;
		emptyt.name = "empty";
		if (i.second.right.size() == 0)
		i.second.right.push_back(emptyt);*/

		cout << "\n-------allproductions-------\n" << i.first << "--" << i.second.left.name << "--";
		for (int j = 0; j < i.second.right.size();j++)
		{
			cout << " " << i.second.right[j].name;
		}
	}
}

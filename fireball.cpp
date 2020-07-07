/* 
 * Fireball JSON Software for C++14
 * Author: Anthony Pulse, Jr.
 *
 * Commercial Software: Not for resale.
 *
 * This code may not be modified without
 * written permission of the author.
 * This code may not also be used for
 * purposes outside of the given license.
 * 
 */

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <regex>
#include <iterator>
#include <exception>
#include <cctype>

using namespace std;

// Save some typing here
typedef vector<pair<string,string>> vps;
typedef vector<tuple<string,string,string>> vts;
typedef map<string,map<string,string>> m2s;
typedef map<string,m2s> m3s;

template<typename Target = string, typename Source = string>
Target to(Source arg)
{
	stringstream interpreter;
	Target result;

	if (!(interpreter << arg)
		|| !(interpreter >> result)
		|| !(interpreter >> std::ws).eof())
		throw runtime_error{"to<>() failed"};

	return result;
}

class jball {
	public:
	jball();
	~jball();
// turn JSON into vps (vector<pair<string,string>>)
	vps Mapjball(string) throw(exception);

// turn raw JSON (no quotes) into useable JSON
	string cmdlJSON(string);
	inline void clear() { values.clear(); arl.clear(); depth.clear(); mapvals.clear(); };
// Renames duplicate objects of 1 char long
	vps clean(vps);
// search through parm[0] for key parm[1]
	vts pathsearch(vps, string);

// search through parm[0] for obj parm[1]
	vps objsearch(vps, string);

// This function takes a vps
// and returns it as a raw JSON for sending to client
// and server sockets in order to communicate.
	string toJSON(vps);

// These JSON functions return containers as strings
// to be put in cmdlJSON(string) which uses Mapjball(string)
// to put them in vps format
	string JSON(map<string,string>);
	string JSON(pair<string, string>) throw(exception);
	string JSON(vector<pair<string, string>>);
	string JSON(vector<tuple<string, string, string>>);
	string JSON(map<string,pair<string,string>>);
	string JSON(pair<string,pair<string,string>>) throw(exception);
	string JSON(m2s);
	string JSON(pair<string,map<string,string>>) throw(exception);
	string JSON(m3s);
	string JSON(pair<string,m2s>) throw(exception);

// parm[1] is obj in parm[0] where parm[2] will be inserted
	vps insert(vps, string, string);
// parm[1] is obj in parm[0] where parm[2] will be inserted in the parm[3] occurence
	vps insert(vps, string, string, int);

// remove parm[1] from parm[0]
	vps rem(vps, string);

// display vectors returned from Mapjball(string),
// or pathsearch(vps, string), or objsearch(vps, string)
	string display(vps);
	void display(vts);

// rename parm[1] from parm[0] to parm[2]
	vps rename(vps, string, string);

// change value in parm[1]
	vps refactor(vps, string, string);

// rename parm[1][parm[3]] as parm[2] from parm[0]
	vps rename(vps, string, string, int);

// change value of parm[1], count parm[3], to parm[2]
	vps refactor(vps, string, string, int);

// place all keys and values in quotes or unquote them
	vps quote(vps);
	vps unquote(vps);

// replace key (parm[1] in parm[0]) to k:parm[2]/v:parm[3]
	vps replace(vps vp, string, string, string);
	
	const string regex_mapper = "[\[\{}]]{1}|[0-9_/+/-/.]+[,$]?|[\"']{1}[!#@$&^?,\\/%A-z0-9/._[:blank:]]+[\"']{1}[:,$]?|(.){1}"; ///[:,$]|(.){1}";
	const string regex_mapper1 = "[0-9_/+/-/.]+[,$]?|['\"]{1}[[^\"'][:graph:][:blank:]]+['\"]{1}[,:]?|(.){1}";

// member variables
	vector<string> values, depth, arl;
	vps mapvals;
	bool oper = false;

// return key
	inline string JSON(int x, int y) throw(exception) {return to<string,int>(x);};
	inline string JSON(double x, int y) throw(exception) {return to<string,double>(x);};
	inline string JSON(long x, int y) throw(exception) {return to<string,long>(x);};
	inline string JSON(long long x, int y) throw(exception) {return to<string,long long>(x);};
	inline string JSON(char * x, int y) throw(exception) {return to<string,char*>(x);};
	inline string JSON(char x, int y) throw(exception) {return to<string,char>(x);};
	inline string JSON(string x, int y) throw(exception) {return x;};
// return value
	inline string JSON(int x) throw(exception) {return to<string,int>(x);};
	inline string JSON(double x) throw(exception) {return to<string,double>(x);};
	inline string JSON(long x) throw(exception) {return to<string,long>(x);};
	inline string JSON(long long x) throw(exception) {return to<string,long long>(x);};
	inline string JSON(char * x) throw(exception) {return to<string,char*>(x);};
	inline string JSON(char x) throw(exception) {return to<string,char>(x);};
	inline string JSON(string x) throw(exception) {return x;};
};

jball::jball() {
}

jball::~jball() {
	this->clear();
}


vps jball::clean(vps values) {

	string charset = "abcdefghijklmnopqrstuvwxyz0123456789";
	vector<string> e;
	vps mapvals;
	for (pair<string,string> mark : values) {
		string w = mark.first;
		string d = mark.second;
		for (int g = 1; g < values.size() ; g++) {
			if (w.at(0) == charset.at(g) && w.length() == 1) {
				e.push_back(w);
				break;
			}
		}
	}

	int j = 1;
	int lvl = 0;
	for (pair<string,string> mark : values) {
		string w = mark.first;
		string d = mark.second;
		for (auto k : e)
			if (to<>(charset.at((j)%charset.size())) == to<>(k))
				j++;
		string u = to<>(charset.at((j-2)%charset.size()));
		if (w.length() <= 1 && d == "[") {
			j++;
			mapvals.push_back(make_pair(u, d));
		}
		else mapvals.push_back(make_pair(w, d));
	}

	return mapvals;
}

vps jball::quote(vps vp) {
	vps set;
	for (pair<string,string> a : vp) {
		if (a.first != "}," && a.first != "]," && a.second != "]" && a.second != "}" && a.second != "[" && a.second != "{") {
			a.first = "\"" + a.first + "\"";
			a.second = "\"" + a.second + "\"";
		}
		else if (a.first != "}," && a.first != "]," && a.second != "]" && a.second != "}") {
			a.first = "\"" + a.first + "\"";
			//second is same;
		}
		set.push_back(make_pair(a.first,a.second));
	}
	return set;
}

vps jball::unquote(vps vp) {
	vps set;
	for (pair<string,string> a : vp) {
		string m = a.first, n = a.second;
		if (m.front() == '\"' && n.front() == '\"') {
			a.first = m.substr(1,a.first.length()-2);
			a.second = n.substr(1,a.second.length()-2);
		}
		else if (m.front() != '\"' && n.front() != '\"') {
			a.first = m.substr(1,a.first.length()-2);
			//second is same;
		}
		else {
			a.first = m.substr(1,a.first.length()-2);
			a.second = n.substr(1,a.second.length()-2);
		}
			
		set.push_back(make_pair(a.first,a.second));
	}
	return set;
}

string jball::display(vps kvs) {
	int lvl = 0, cnt = 0, obj = 0;
	cout << "\n";
	vector<string> tst;
	for (auto x : kvs) {
		if (lvl > 1 && x.second == "]")
			lvl--;
		for (int i = 0 ; i < lvl ; i++)
			tst.push_back("|\t");
		if (x.first == "]" && x.first == x.second)
			tst.push_back(x.first + "\n");
		else {
			if (x.second != "[" && x.second != "")
				cnt++;
			if (x.second == "")
				tst.push_back(x.first + " [" + "\n");
			else
				tst.push_back(x.first + " " + x.second + "\n");
		}
		if ((x.second == "" || x.second == "[") || x.first == "[") {
			obj++;
			lvl++;
		}
	}

	if (lvl > 1) {
		tst.push_back("]\n");
	}
	string v = "";
	for (auto l : tst)
		v += l;

	v.append("\n\nFound: " + to<>(cnt) + " elements in " + to<>(obj) + " objects\n");
	return v;
}

void jball::display(vts r) {
	int x = 0;
	cout << "\n" << "#" << "\tPath:\t\tK:\tV:\n";
	for (auto a : r) {
		cout << x << ".\t" << get<0>(a) << "\t\t" << get<1>(a) << "\t" << get<2>(a) << endl;
		x++;
	}
	return;
}

string jball::toJSON(vps t) {
	string y = "";
	for (auto x : t) {
		string c = JSON(x);
		for (auto z : c)
			(isspace(z)) ? y = y + "%20" : y = y + z;
		if (y.at(y.length()-2) == ',' && y.back() == ']') {
			y = y.substr(0,y.length()-2);
			y = y + "],";
		}
	}
	y = y.substr(0,y.length()-1);

	return y;
}
/*
string jball::JSON(vector<tuple<string, string, string>> s) {
	string output = "[";
	for (auto j : s) {
		auto h = JSON(j);
		output = output + to<>(h);
	}

	output = output.substr(0, output.length() - 1);

	output = output + "]";

	return output;

}
*/
string jball::JSON(vector<pair<string, string>> s) {
	string output = "[";
	for (auto j : s) {
		auto h = JSON(j);
		output = output + to<>(h);
	}

	output = output.substr(0, output.length() - 1);

	output = output + "]";

	return output;

}

string jball::JSON(map<string,string> s) {
	string output = "[";
	for (auto j = s.cbegin() ; j != s.cend() ; j++) {
		string b = JSON(*j);
		output = output + JSON(b);
	}

	output = output.substr(0,output.length()-1);

	return output + "]";

}

string jball::JSON(m2s s) {
	string output = "[";
	for (auto j = s.cbegin() ; j != s.cend() ; j++) {
		string b = JSON(*j);
		output = output + JSON(b);
	}

	output = output.substr(0,output.length()-1);

	return output + "]";

}

string jball::JSON(m3s s) {
	string output = "[";
	for (auto j = s.cbegin() ; j != s.cend() ; j++) {
		string b = JSON(*j);
		output = output + JSON(b);
	}

	output = output.substr(0,output.length()-1);
	//cout << output;
	return output ;

}

string jball::JSON(pair<string,string> j) throw(exception) {
	string output = "";
	try {
		auto r = j.first;
		auto vt = JSON(r);
		if (isdigit(vt.front()) && ! vt.back() == ':') {
			if (j.second == "[" || j.second == "{")
				output = output + ":";
			else
				output = output;
		}
		else if (vt.back() == ':')
			output = output + vt;
		else if (j.second != "]" && j.second != "}")
			output = output + vt + ":";
		else if (j.second == "," || j.second == "")
			return output;
	}
	catch (...) {
		output = "";
	}
	try {
		auto r = j.second;
		auto vt = JSON(r);
		if (vt != "[" && j.first != "]" && vt != "{" && j.first != "}")
			output = output + vt + ",";
		else
			output = output + vt;
	}
	catch (...) {
		output = "";
	}
	return output;
}

string jball::JSON(pair<string,map<string,string>> j) throw(exception) {
	string output = "";
	
	try {
		auto r = j.first;
		auto vt = JSON(r);
		if (vt.back() != ':') {
			output = output + vt + ": ";
		}
		else
			output = output + vt + " [";
	}
	catch (...) {
		output = "";
	}
	try {
		auto r = j.second;
		auto vt = JSON(r);
		if (vt.back() != ':') 
			output = output + vt;
		else
			output = output + vt;
	}
	catch (...) {
		output = "";
	}

	return output;
}

string jball::JSON(map<string,pair<string,string>> s) {
	string output = "[";
	for (auto j = s.cbegin() ; j != s.cend() ; j++) {
		string b = JSON(*j);
		output = output + JSON(b);
	}

	output = output.substr(0,output.length()-2);
	return output + "]";

}

string jball::JSON(pair<string,m2s> j) throw(exception) {
	string output = "";
	
	try {
		auto r = j.first;
		auto vt = JSON(r);
		if (vt.back() != ':') {
			output = output + vt + ": ";
		}
		else
			output = output + vt + " [";
	}
	catch (...) {
		output = "";
	}
	try {
		auto r = j.second;
		auto vt = JSON(r);
		if (vt.back() != ':') 
			output = output + vt + ":";
		else
			output = output + vt;
	}
	catch (...) {
		output = "";
	}

	return output;
}

string jball::JSON(pair<string,pair<string,string>> j) throw(exception) {
	string output = "";
	
	try {
		auto r = j.first;
		auto vt = JSON(r);
		if (vt.back() != ':') {
			output = output + vt + ": [";
		}
		else
			output = output + vt + " [";
	}
	catch (...) {
		output = "";
	}
	try {
		auto r = j.second;
		auto vt = JSON(r);
		if (vt.back() != ':')
			output = output + vt + ":";
		else
			output = output + vt;
	}
	catch (...) {
		output = "";
	}
	return output;
}

// Returns Vector<pair<string,string>> for C++ in Function.
// Takes one parameter which is a JSON.
vector<pair<string,string>> jball::Mapjball(string maps) throw(exception) {
	vector<int> d;
	vector<string> e;
	int i = 0, j = 0, h = 0, oper = 1;
	cout << "\n" << flush;
	string title = "";
	string dlm = "'{[]}\":,";

	regex words_regex (this->regex_mapper);
	string charset = " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	auto words_begin = std::sregex_iterator(maps.begin(), maps.end(), words_regex);
	auto words_end = std::sregex_iterator();
 	for (std::sregex_iterator i = words_begin; i != words_end; i++) {
		std::smatch match = *i;
		std::string match_str = match.str();
		//cout << match_str << " " << flush;
		this->arl.push_back(match_str);
    	}

	for (auto arx : arl) {
		if ((arx.at(0) == dlm.at(0) || arx.at(0) == dlm.at(5)) && arx.at(arx.length()-1) == dlm.at(6)) {
			string w = arx.substr(1,arx.length()-3);
			values.push_back(w);
		}
	}
	for (string mark : values) {
		string w = mark;
		for (int g = 1; g < values.size() ; g++) {
			if (w.at(0) == charset.at(g) && w.length() == 1) {
				e.push_back(w);
				break;
			}
		}
	}
	for (auto arx : arl) {
		try {

		// +'+"+:
			if ((arx.at(0) == dlm.at(0) || arx.at(0) == dlm.at(5)) && arx.at(arx.length()-1) == dlm.at(6)) {
				if (depth.size() >= 1) {
					title = arx.substr(1,arx.length()-3) + ':';
					if (depth.size() > 1)
						depth.push_back(depth[depth.size()-1] + title);
					else
						depth.push_back(title);
				}	
				else {
					if (arx.length() == 1)
						title = arx;
					else
						title = arx.substr(1,arx.length()-3);
					depth.push_back(title);
					oper = 0;
				}
			}
		// +[+{
			if (arx.at(0) == dlm.at(1) || arx.at(0) == dlm.at(2)) {
				string val = arx;
				d.push_back(0);
			// Make sure, through recursion there are no
			// names doubled by making names for unnamed
			// object sets. Using 'charset' to find used/unused
				if (depth.size() == 0) {
					j++;
					for (auto k : e)
						if (to<>(charset.at(j%charset.size())) == to<>(k))
							j++;
					string u = to<>(charset.at(j%charset.size()));
					mapvals.push_back(make_pair(u, arx));
					values.clear();
				}
				if (depth.size() > 0) {
					mapvals.push_back(make_pair(depth[depth.size()-1], val));
					depth.clear();
				}
				title = "";
				continue;
			}
		// 0-9.
			if (isdigit(arx.at(0))) {
				title = "";
				for (char ar : arx) {	
					if (isdigit(ar) || ar == '.')
						title = title + ar;
				}
				if (title.length() == arx.length())
					mapvals.push_back(make_pair(depth[depth.size()-1], title));
				else title = "";
			}
		// -:+'+" 
			if (arx.at(arx.length()-1) != dlm.at(6) && (arx.at(0) == dlm.at(5) || arx.at(0) == dlm.at(0))) {
				string val = "";
				if (arx.back() != dlm.at(7))
					val = arx.substr(1,arx.length()-2);
				else
					val = arx.substr(1,arx.length()-3);
			// use counters to name elems from vector<string>
				if (depth.size() < 1) {
					d[d.size()-1]++;
					j++;
					string u = to<>(charset.at(j%charset.size()));
					mapvals.push_back(make_pair(u,val));
				}
				else if (depth.size() > 1) {
					mapvals.push_back(make_pair(depth[0], depth[depth.size()-1] + val));
					oper = 1;
				}
				else
					mapvals.push_back(make_pair(depth[depth.size()-1], val));
				depth.clear();
				title = "";
			}
		// +}+]
			if (arx.at(0) == dlm.at(3) || arx.at(0) == dlm.at(4)) {
				if (values.size() == 0)
					mapvals.push_back(make_pair(arx, arx));
				else {
					mapvals.push_back(make_pair(arx, arx));
				}
				depth.clear();
				if (d.size() == 0)
					i = 0;
			}
		}
		catch (...) {
			cout << "Invalid JSON... Exiting..";
			exit(0);
		}
	}
	vps t = mapvals;
	this->clear();
	return t;
}

string jball::cmdlJSON(string maps) {

	string rem = maps.substr(0,1);
	for (int i = 1 ; i < maps.length() ; i++) {
		if (rem[rem.size()-1] == ',' && (maps[i] == ']' || maps[i] == '}'))
			rem = rem.substr(0,rem.size()-1) + maps[i];
		else
			rem = rem + maps[i];
	}
	maps = rem;
	int lvl = 0;
	string output = "";
	int j = 0;
	string dlmo = "{[";
	string dlmc = "]}";
	for (int j = 0 ; j < maps.length() ; j++) {
		if (j + 3 < maps.length() && maps.substr(j,3) == "%20") {
			maps.replace(j,2,"");
			output = output + " ";
		}
		else if (isalnum(maps.at(j))) {
			output = output + maps.at(j);
			continue;
		}
		else if (j + 2 <maps.length() && (maps.at(j) == dlmo.at(0) || maps.at(j) == dlmo.at(1)) && (maps.at(j+2) == dlmo.at(0) || maps.at(j+2) == dlmo.at(1))) {
			output = output + "[ [ \"";
			j += 2;
		}
		else if (j + 1 <maps.length() && (maps.at(j) == dlmo.at(0) || maps.at(j) == dlmo.at(1)) && (maps.at(j+1) == dlmo.at(0) || maps.at(j+1) == dlmo.at(1))) {
			output = output + "[ [ \"";
			j += 1;
		}
		else if (maps.at(j) == dlmo.at(0) || maps.at(j) == dlmo.at(1)) {
			if (j + 1 <maps.length() && (maps.at(j+1) == dlmc.at(0) || maps.at(j+1) == dlmc.at(1))) {
				output = output + "[ ]";
				j += 1;
			}
			else if (j + 2 <maps.length() && (maps.at(j+2) == dlmc.at(0) || maps.at(j+2) == dlmc.at(1))) {
				output = output + "[ ]";
				j += 2;
			}
			else if (j + 1 <maps.length() && (maps.at(j+1) != dlmo.at(0) && maps.at(j+1) != dlmo.at(1)))
				output = output + "[ \"";
			else if (j + 2 <maps.length() && (maps.at(j+2) != dlmo.at(0) && maps.at(j+2) != dlmo.at(1)))
				output = output + "[ \"";
		}
		else if (maps.at(j) == ']' && !(maps.at(j-1) == dlmc.at(0)) && !(maps.at(j-1) == dlmc.at(1))) {
			if (maps.at(j-1) == ',' || maps.at(j-2) == ',')
				output = output + " ]";
			else if ((maps.at(j-2) == dlmo.at(0) || maps.at(j-2) == dlmo.at(1)))
				output = output + " ] ";
			else if (!(maps.at(j-2) == dlmc.at(0)) && !(maps.at(j-2) == dlmc.at(1)))
				output = output + "\" ] ";
			else output = output + "] \"";
		}
		else if (j + 1 <maps.length() && maps.at(j) == ':' && (dlmo.at(1) == maps.at(j+1) || maps.at(j+1) == dlmo.at(0))) {
			if (j + 2 <maps.length() && (dlmo.at(1) == maps.at(j+2) || maps.at(j+2) == dlmo.at(0))) {
				output = output + "\": [ [ \"";
				j += 2;
			}
			else if (j + 3 <maps.length() && (dlmo.at(1) == maps.at(j+3) || maps.at(j+3) == dlmo.at(0))) {
				output = output + "\": [ [ \"";
				j += 3;
			}
			else {
				output = output + "\": ";
			}
		}
		else if (j + 2 <maps.length() && maps.at(j) == ':' && (dlmo.at(1) == maps.at(j+2) || maps.at(j+2) == dlmo.at(0))) {
			if (j + 3 <maps.length() && (dlmo.at(1) == maps.at(j+3) || maps.at(j+3) == dlmo.at(0))) {
				output = output + "\": [ \"";
				j += 3;
			}
			else if (j + 2 <maps.length() && maps.at(j) == ':' && (dlmo.at(1) == maps.at(j+2) || maps.at(j+2) == dlmo.at(0))) {
				output = output + "\": [ \"";
				j += 2;
			}
		}
		else if (maps.at(j) == ':') {
			output = output + "\": \"";
		}
		else if (j + 1 <maps.length() && (dlmo.at(1) == maps.at(j) || maps.at(j) == dlmo.at(0)) && maps.at(j+1) == ',') {
			if (j + 3 <maps.length() && (dlmo.at(1) != maps.at(j+3) || maps.at(j+3) != dlmo.at(0))) {
				if (j + 2 <maps.length() && (dlmo.at(1) != maps.at(j+2) || maps.at(j+2) != dlmo.at(0)))
					output = output + "], \"";
			}
			j += 1;
		}
	
		else if ((dlmc.at(1) != maps.at(j-1) && maps.at(j-1) != dlmc.at(0)) && maps.at(j) == ',') {

			if (j + 2 <maps.length() && (dlmc.at(1) == maps.at(j+2) || maps.at(j+2) == dlmc.at(0))) {
				output  = output + "\", [ ]";
				j += 2;
			}
			else if (j + 1 <maps.length() && (dlmo.at(1) == maps.at(j+1) || maps.at(j+1) == dlmo.at(0))) {
				output = output + ", [ ";
				j += 1;
			}
			else if (j + 2 <maps.length() && (dlmo.at(1) == maps.at(j+2) || maps.at(j+2) == dlmo.at(0))) {
				output  = output + ", [ ";
				j += 2;
			}
			else output = output + ", ";
		}
		else if (maps.at(j) == ',' && (maps.at(j-1) == dlmc.at(0) || maps.at(j-1) == dlmc.at(1))) {
			if (j + 1 <maps.length() && (maps.at(j+1) == dlmo.at(0) || maps.at(j+1) == dlmo.at(1))) {
				j += 1;
				output = output + ",";
			}
			else
				output = output + ", ";
		}
		else
			output = output + maps.at(j);
	}

	maps = output;
	output = "";
	for (int y = 0 ; y < maps.length() ; y++) {
		if (maps.at(y) == ',' && maps.at(y+1) == ']') {
			output = output + "]";
			y++;
		}
		else if (y + 3 < maps.length() && maps.substr(y,4) == "] :") {
			output = output + "]";
			y += 3;
		}
		else if (y + 2 <maps.length() && maps.substr(y,3) == "]:") {
			output = output + "]";
			y += 2;
		}
		else
			output = output + maps.at(y);
	}

	return output;
}

vts jball::pathsearch(vps vp, string key) {

	string red = "";
	tuple<string,string,string> create;
	vector<string> trace;
	vts found;
	for (auto a : vp) {
		if (a.second == "[" || a.second == "{") {
			trace.push_back(a.first);
			continue;
		}
		else if ((a.second == "]" || a.second == "}") && trace.size() >= 2)
			trace.pop_back();
		if (a.first == key) {
			for (auto b : trace) {
				red = red + " " + b;
			}
			create = make_tuple(red, key, a.second);
			found.push_back(create);
			red = "";
		}
	}
	return found;
}

vps jball::objsearch(vps vp, string key) {

	int p = 0, i = 0;
	vector<string> trace;
	vps found;
	
	for (int i = 0 ; i < vp.size() ; i++) {
		pair<string,string> a = vp[i];
		if (a.first == key && (a.second == "[" || a.second == "{")) {
			for (int j = i ; j < vp.size() ; j++) {
				pair<string,string> b = vp[j];

				if (b.second == "[" || b.second == "{")
					p++;
				if (b.second == "]" || b.second == "}")
					p--;
				if (b.second == "[" || b.second == "{")
					found.push_back(make_pair(b.first, b.second));
				else if (b.second == "]" || b.second == "}")
					found.push_back(make_pair(b.first, "]"));
				else {
					pair<string,string> hj = found[found.size()-1];
					if (hj.first == "" || hj.second == "")
						found.pop_back();
					found.push_back(make_pair(b.first, b.second));
				}
				if (p == 0)
					break;
			}
			p = 0;
		}
		//i++;
	}

	return found;
}

vps jball::rem(vps vp, string key) {

	string rem = "";
	int p = 0;
	vps found;
	int lvl = 0;
	for (int i = 0 ; i < vp.size() ; i++) {
		pair<string,string> a = vp[i];
		if (a.second == "[")
			lvl++;
		else if (a.second == "]")
			lvl--;
		if (a.first == key && (a.second == "[" || a.second == "{")) {
			for (int j = i ; j < vp.size() ; j++) {
				pair<string,string> b = vp[j];
				if (b.second == "[" || b.second == "{")
					p++;
				if (b.second == "]" || b.second == "}")
					p--;
				if (p == 0) {
					for (int h = j+1 ; h < vp.size() ; h++) {
						pair<string,string> c = vp[h];
						found.push_back(c);
						if (c.second == "]")
							lvl--;
						if (lvl == 1)
							return found;
						if (c.second == "[")
							lvl++;
					}
					return found;
				}
				i++;
			}
			p = 0;
		}
		else {
			found.push_back(a);
		}
	}

	return found;
}

vps jball::insert(vps vp, string key, string insrt) {
	if (insrt.front() == '[')
		insrt = insrt.substr(1,insrt.length()-1);
	vps c = Mapjball(insrt);
	vps b, d;
	bool temp = 0;
	for (int i = 0 ; i < vp.size() ; i++) {
		pair<string,string> a = vp[i];
		if (a.first == key && (a.second == "[" || a.second == "{")) {
			b.push_back(a);
			for (pair<string,string> x : c)
				b.push_back(x);
		}
		else
			b.push_back(a);
	}
	d = clean(b);
	return b;
}

vps jball::insert(vps vp, string key, string insrt, int pno) {
	if (insrt.front() == '[')
		insrt = insrt.substr(1,insrt.length()-1);
	vps c = Mapjball(insrt);
	vps b, d;
	bool temp = 0;
	int g = 0;
	for (int i = 0 ; i < vp.size() ; i++) {
		pair<string,string> a = vp[i];
		if (g == pno && a.first == key && (a.second == "[" || a.second == "{")) {
			b.push_back(a);
			for (pair<string,string> x : c)
				b.push_back(x);
			g++;
			continue;
		}
		if (a.first == key && (a.second == "[" || a.second == "{"))
			g++;
		b.push_back(a);
	}
	d = clean(b);
	return b;
}

vps jball::rename(vps vp, string key, string newkey) {

	vps b;
	for (pair<string,string> a : vp) {
		//pair<string,string> a = vp[i];
		if (a.first == key)
			b.push_back(make_pair(newkey,a.second));
		else
			b.push_back(a);
	}

	return b;
}

vps jball::refactor(vps vp, string key, string insr) {

	vps b;
	for (pair<string,string> a : vp) {
		//pair<string,string> a = vp[i];
		if (a.first == key && a.second != "[" && a.second != "{")
			b.push_back(make_pair(a.first,insr));
		else
			b.push_back(a);
	}

	return b;
}

vps jball::rename(vps vp, string key, string newkey, int g) {

	vps b;
	for (pair<string,string> a : vp) {
		//pair<string,string> a = vp[i];
		if (a.first == key && g >= 0){
			if (g == 0)
				b.push_back(make_pair(newkey,a.second));
			else
				b.push_back(a);
			g--;
		}
		else
			b.push_back(a);
	}

	return b;
}

vps jball::replace(vps vp, string key, string newkey, string newval) {

	vps b;
	for (pair<string,string> a : vp) {
		//pair<string,string> a = vp[i];
		if (a.first == key && a.second != "[" && a.second != "{")
			b.push_back(make_pair(newkey,newval));
		else
			b.push_back(a);
	}

	return b;
}

vps jball::refactor(vps vp, string key, string insr, int g) {

	vps b;
	for (pair<string,string> a : vp) {
		//pair<string,string> a = vp[i];
		if (a.first == key && g >= 0 && a.second != "[" && a.second != "{") {
			if (g == 0)
				b.push_back(make_pair(key,insr));
			else
				b.push_back(a);
			g--;
		}
		else
			b.push_back(a);
	}

	return b;
}

int main(int argc, char * argv[]) {

	string output = "";
	jball mapMe;
	string maps = "", v = "";
	vector<string> initi;
	for (int m = 1 ; m < argc ; m++) {
		string arg = argv[m];
		initi.push_back(to<>(arg));
	}

	for (string m : initi)
		maps = maps + m;
// cmdlJSON Uses regex to scan for formatting
	string y = mapMe.cmdlJSON(maps);
// Mapjball takes a cmldJSON string and makes
// suitable for for vps
	vps t = mapMe.Mapjball(y);
	t = mapMe.replace(t, "asd", "welcome", "here");
	t = mapMe.rename(t, "d", "welcome");
	t = mapMe.quote(t);
	string out = mapMe.toJSON(t);
	cout << out;
	t = mapMe.unquote(t);
	out = mapMe.display(t);
	cout << out;
	return 0;

}

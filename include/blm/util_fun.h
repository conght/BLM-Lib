#pragma once
#include <vector>
#include <string>
#include <memory>
using namespace std;
#pragma warning (disable: 4100)
#pragma warning (disable: 4389)
#pragma warning (disable: 4355)
#pragma warning (disable: 4996)

namespace BlmComm {

//���ַ������շָ����з�
vector<string> splitString(const char* cont, char sep);
//����ue�ж����Ƶķ�ʽ��ʾ���������ݵ�����
string getHexDump(const void* data, int sz);
//�����������
void randomBuf(void* data, int sz);
//���ظ�ʽ�����ַ���
string formatString(const char* fmt, ...);
//�ַ����ıȽϺ���
int strcmpNoCase(const char* s1, const char* s2);
inline bool compareNoCase(const string& s1, const string& s2) { return strcmpNoCase(s1.c_str(), s2.c_str()) == 0; }
int strcmpCase(const char* s1, const char* s2);
inline bool compareCase(const string& s1, const string& s2) { return strcmpCase(s1.c_str(), s2.c_str()) == 0; }
string intToStr(int i);
string longToStr(long l);
string doubleToStr(double d);
int strToInt(const string& str);
long strToLong(const string& str);
double strToDouble(const string& str);
bool strToBool(const string& str);
string toLower(const string& str);
string toUpper(const string& str);
string& replaceAll(string& str, const string& oldVal, const string& newVal);
string intListToStr(vector<int> l, char delim);
string strListToStr(vector<string> l, char delim);
string strListToStr(vector<string> l, const string& delim);
vector<int> strToIntList(const string& str, char delim);
vector<long> strToLongList(const string& str, char delim);
vector<string> strToStrList(const string& str, char delim);
vector<string> strToStrList(const string& str, const string& delim);
string trimSpaces(const string& str) ;


//�Զ��ͷ�������ڴ�
template<class C> class AutoArrayPtr {
	C* ptr;
public:
	AutoArrayPtr(C* p): ptr(p) {}
	~AutoArrayPtr() { delete [] ptr;}
};

//����ģʽ��ģ����
template<class T> class Singleton {
public:
	//ģ���಻�ܷ���cpp�ļ�����˷�����
	static T& instance(Singleton* ins=NULL) {
		static T* pins = (T*)ins;
		//���δ��ʼ�������߳�ʼ�������Σ��򱨴�
		if (pins == NULL) {
			printf("singleton object was not created by user before used");
			throw 0;
		}else if(ins != NULL && ins != pins) {
			printf("singleton object is created twice");
			throw 0;
		}
		return *pins;
	}
	Singleton() { instance(this); }
};


}


#pragma  once
#include <errno.h>
#include <string>
#include <stack>
#include <set>
#include <list>
#include <map>
#include <stdlib.h>
#include <stddef.h>
#include <blm/util_fun.h>
#include <algorithm>
#include <set>
#include <blm/blmutil.h>

namespace BlmComm {
	class CheckerPjStatus {
		std::string fileName;
		int lineNo;
	public:
		CheckerPjStatus(const char* fileName1, int lineNo1):fileName(fileName1), lineNo(lineNo1) {}
		void operator = (int status) ;
	};


	class CheckerTrue {
		std::string fileName;
		int lineNo;
	public:
		CheckerTrue(const char* fileName1, int lineNo1):fileName(fileName1), lineNo(lineNo1) {}
		void operator = (bool status) ;
		void operator = (void* p);
	};


	struct CallbackBase {
		virtual ~CallbackBase() {}
		//�����������ڱȽϻص������Ƿ���ͬ������
		virtual void* obj() = 0;
		void* arg;
	};

	//���ڲ��������Ļص�����onConnect��onClose
	struct SimpleCallback: public CallbackBase {
		virtual int call() = 0;
	};

	//���ڿͻ����յ���Ϣʱ�Ļص�
	struct MsgCallback: public CallbackBase {
		virtual int call(int mt, int st, const string& data, int msgid) = 0;
	};

	//���ں������ݵĻص�������http�������ʱ�Ļص�
	struct DataCallback: public CallbackBase {
		virtual int call(const string& data) = 0;
	};

	//�Գ�Ա����Ϊ����
	template<class T> class TSimpleCallback : public SimpleCallback {
		T* pobj;
		typedef int (T::*Mfn)(void*);
		Mfn mfn;
	public:
		TSimpleCallback(T* pobj1, Mfn mfn1, void* arg1): pobj(pobj1), mfn(mfn1) { arg = arg1; } 
		virtual int call() { return (pobj->*mfn)(arg); }
		virtual void* obj() { return pobj; }
	};
	template<class T> SimpleCallback* makeSimpleCallback(T* obj, int (T::*mfn)(void*), void* arg1) { return new TSimpleCallback<T>(obj, mfn, arg1); }

	template<class T> class TMsgCallback : public MsgCallback {
		T* pobj;
		typedef int (T::*Mfn)(int, int, const string&, int msgid, void*);
		Mfn mfn;
	public:
		TMsgCallback(T* pobj1, Mfn mfn1, void* arg1):  pobj(pobj1), mfn(mfn1){ arg = arg1; } 
		virtual int call(int mt, int st, const string& data, int msgid) { return (pobj->*mfn)(mt, st, data, msgid, arg); }
		virtual void* obj() { return pobj; }
	};
	template<class T> MsgCallback* makeMsgCallback(T* obj, int (T::*mfn)(int, int, const string&, int, void*), void* arg1) { return new TMsgCallback<T>(obj, mfn, arg1); }

	template<class T> class TDataCallback : public DataCallback {
		T* pobj;
		typedef int (T::*Mfn)(const string&, void*);
		Mfn mfn;
	public:
		TDataCallback(T* pobj1, Mfn mfn1, void* arg1):  pobj(pobj1), mfn(mfn1) { arg = arg1; } 
		virtual int call(const string& data) { return (pobj->*mfn)(data, arg); }
		virtual void* obj() { return pobj; }
	};
	template<class T> DataCallback* makeDataCallback(T* obj, int (T::*mfn)(const string&, void*), void* arg1) { return new TDataCallback<T>(obj, mfn, arg1); }

	class TimerObj;

}

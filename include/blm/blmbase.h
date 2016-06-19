#pragma  once
#include <blm/imp/blmbase_imp.h>
using namespace std;
struct pj_caching_pool;
struct pj_pool_t;
struct pj_atomic_t;
struct pj_mutex_t;
struct pj_sem_t;
struct pj_thread_t;
namespace BlmComm {
	extern pj_caching_pool& pj_cp;
	//��ʼ��������
	void blminit();
	void blmshutdown();
	//������־�����Զ�����ã�
	//����detail_level��0Ϊ��ͼ��𣬴��󡢾�����0���������3Ϊ��ͨ��Ϣ�����5Ϊ����ϸ�����
	//modulesָ�������Щģ�����Ϣ�����磺""ȫ������� "blmcomm main"���blmcomm��mainģ�����־
	//���������blmSetLog����Ĭ���������Ϊ3���������ģ����־���������׼���
	void blmSetLog(int detail_level=3, const char* modules="", const char* logfile="");
	//������־�������������androidƽ̨���Ե��ô˺�������־�����logcat
	void blmSetLogOutput(void (*logfun)(int, const char*, int));
	//��ӡ��־
	void blmlog(int detail_level, const char* module, const char* fmt, ...);
	//��ӡ����������
	void blmlogDump(int detaillevel, const char* module, const char* tag, const void* data, int len);
	//ʹ�÷�ʽΪ��checkpj=pj_fun,���ڼ�鷵��ֵ���ʺ�pj����,��Ҫ��namespace BlmComm��ʵ����ʹ��
#define checkpj CheckerPjStatus(__FILE__, __LINE__)

	//ʹ�÷�ʽΪ��checktrue=fun, ���ڼ�鷵��ֵ���ʺϳɹ��򷵻�true�ĺ�������ʽ
#define checktrue CheckerTrue(__FILE__, __LINE__)

	//�ڴ�أ�pjlib���ڴ�ʹ�÷�ʽ�ǣ�1.����pool 2.��pool�����ڴ� 3.�ͷ�pool�����д�pool������ڴ���֮�ͷ�)
	class MemPool {
	protected:
		pj_pool_t* pool;
		bool owned;
		//����pΪNULL�������з���һ��pool�����p!=NULL��ʹ��p
		MemPool(MemPool* p=NULL);
		~MemPool();
	};

	//����������Ĺ��캯���У��и�MemPool����������ò���Ϊ0���򴴽�pool����pool�����ڴ棻�����Ϊ0����Ӵ����pool�����ڴ�
	//�������Ĵ������ٷǳ�Ƶ���Ļ������ʹ�÷�NULL��MemPool����

	//ԭ�ӼӼ�����
	class Atomic: protected MemPool {
		pj_atomic_t* ato;
	public:
		Atomic(int initial = 0, MemPool* pool1=NULL);
		~Atomic();
		void set(int val);
		int get() ;
		int add(int val = 0);
	};

	//�������
	class Mutex: protected MemPool {
		pj_mutex_t* mutex;
	public:
		Mutex(MemPool* p = NULL);
		~Mutex();
		void lock();
		void unlock();
		bool trylock();
	};
	
	//�Զ���
	class Autolock {
		Mutex& mutex;
	public:
		Autolock(Mutex& mutex1): mutex(mutex1) { mutex.lock(); }
		~Autolock() { mutex.unlock(); }
	};

	//�ź���
	class Semaphore: protected MemPool {
		pj_sem_t* sem;
	public:
		Semaphore(int initial, int max = 1024*1024, MemPool* pool1=NULL);
		~Semaphore() ;
		void wait();
		bool trywait();
		void post();
	};

	//�����������߻���������ÿ�����/ɾ��һ������Ŀ���̫�������ʹ��addAll/removeAll
	template<class T> class ProConPool: public Mutex {
		int sz; //����gcc��listʵ���У��������е�Ԫ�غܶ�ʱ��size���û���������Ե���ά��һ��size�ֶ�
		std::list<T> data;
		Semaphore available; //������������ж�����availableΪtrue
	public:
		ProConPool():sz(0), available(0) {}
		void add(T product) { Autolock al(*this); if (data.empty()) available.post(); data.push_back(product); sz++; }
		//�����������û����Ϣ���˺����������ȴ���Ϣ�����ȴ���Ӧ������tryRemove
		T remove() { available.wait(); Autolock al(*this); T ret = data.front(); data.pop_front(); sz--; if (!data.empty()) available.post(); return ret; }
		bool tryRemove(T& val) { Autolock al(*this); bool avail = available.trywait(); if (!avail) return false; val = data.front(); data.pop_front(); sz--; if (!data.empty()) available.post(); return true; }
		//ÿ��add��remove��Ҫ�������������Ϊ��Ч�ʿ��ǣ�����һ�ΰ�����list��ӣ����԰������������е�Ԫ�ر��浽list
		void addAll(std::list<T>& initems) { Autolock al(*this); if (data.empty() && !initems.empty()) available.post(); sz += initems.size(); data.splice(data.end(), initems); }
		//�����������û����Ϣ���˺����������ȴ���Ϣ�����ȴ���Ӧ������tryRemoveAll
		void removeAll(std::list<T>& resitems){ available.wait(); Autolock al(*this); resitems.splice(resitems.end(), data); sz=0; }
		bool tryRemoveAll(std::list<T>& resitems) { Autolock al(*this); bool avail = available.trywait(); if (!avail) return false; resitems.splice(resitems.end(), data); sz=0; return true; }
		int size() { Autolock al(*this); return sz; }
	};

	struct ParsedTime {
		int wday, day, mon, year, sec, min, hour, msec;
	};
	//ʱ�䴦��
	class Time {
	public:
		int sec, msec;
		Time(int sec1, int msec1): sec(sec1), msec(msec1) {}
		Time(): sec(0), msec(0) {}
		//pj_time_valֻ��������Ա��{ int sec, msec; }�μ�pjlib��types.h
		static Time getTimeofday();
		//pj_parsed_time�ж����Ա���ڱ�ʾ������ʱ���룬�μ�pjlib��types.h
		static ParsedTime value2Parsed(Time* tv);
		static Time parsed2Value(ParsedTime* pt);
		static Time localToGmt(Time* tv);
		static Time gmtToLocal(Time* tv) ;

		static Time getTickcount() ;
		static int getTickDiff(Time* tv1, Time* tv2) ;
		static void addTick(Time* tv, int millisec) ;

	};

	//�߳���
	class Thread: public MemPool {
		SimpleCallback* sc;
		pj_thread_t* pthread;
		static int threadproc(void* p);
		Thread(SimpleCallback* callback, const char* name1) ;
	public:
		~Thread() ;
		//�ȴ��߳̽���
		void join() ;
		static void sleep(unsigned msec) ;
		//����һ���̣߳����캯��Ϊ˽�У�ֻ��ͨ�����溯�������߳�
		template<class C> static Thread* startThread(const char* name, C* obj, int (C::*mfn)(void*), void* arg){
			SimpleCallback* sc = makeSimpleCallback(obj, mfn, arg);
			return new Thread(sc, name);
		}

	};
}

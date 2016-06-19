#pragma once
#include <blm/blmbase.h>
struct pj_timer_heap_t;
struct pj_ioqueue_t;
namespace BlmComm {
	class BlmIoqueue;
	class ClientTcp;
	class PjTcp;
	//�û�����ʹ��������������ͨ�ţ��������󣬴�������
	class BlmClient {
	private:
		friend class ClientTcp;
		ClientTcp* clientTcp;
		vector<ClientTcp*> tcpCandidates;
		BlmIoqueue* poller;
		list<SimpleCallback*> connCallbacks, closeCallbacks;
		typedef map<int, list<MsgCallback*> > MsgMap;
		MsgMap msgMap;
	
		//���º�����clientTcp����
		int processMsg(int mt, int st, const string& data, int msgid);
		int onConnect(PjTcp* tcp);
		int onClose(PjTcp* tcp);
	public:
		BlmClient(BlmIoqueue* poller1);
		~BlmClient();
		//ѡ�����ķ��������ӣ�ips����Ϊ "ip1 ip2 ip3"
		void connectFastest(const char* ips, short port);
		//���������������
		void connect(const char* ip, short port) { connectFastest(ip, port); }
		//�Ͽ�����
		void disconnect();
		//�������󷵻�һ����Ϣid����msgcallback�У��ᴫ��ͬһ��msgid
		int sendMsg(int mt, int st, const char* data, int sz);

		//������ص���أ�ֻ֧�ֳ�Ա�����Ļص����������Ա��ֽӿڵļ���ԣ�����������󲿷ֵ�����
		//ע��ص�ʱ����ע������ĺ�������ģ��ķ�ʽ��������ȷ����Ա������add*Callback��������ƥ�䣬����������
		//��Ա�����ķ���ֵΪint������0��ʾ�ɹ�������������ʾ������ʱ���ӻᱻ�ر�
		//��Ա���������һ������Ϊvoid*�����лص�ʱ�����ע��ص�ʱ���ݵ�arg�������ݸ���Ա�������������߳�ʱ���ݵĲ�����ʽ��ͬ
		//����ֵ����Ϊvoid*��ָ���ڲ�ʹ�õĻص��������removeCallback

		//ע��ص����������Ӻ󣬽��лص�
		template<class C> void* addConnectCallback(C* obj, int (C::*memFn)(void*), void* arg=0);
		//ע��ص����Ͽ�����ʱ�����лص����������δ�ܳɹ��������˺������ᱻ���ã�
		template<class C> void* addCloseCallback(C* obj, int (C::*memFn)(void*), void* arg=0);
		//ע��ص����յ�ָ����mt��Ϣ��ʱ�����лص�
		//memFn�Ĳ�������Ϊ��int mt, int st, const string& msg, int msgid, void* arg ����msgidΪsendMsg���ص���Ϣid
		template<class C> void* addMsgCallback(int mt, C* obj, int (C::*memFn)(int, int, const string&, int, void*), void* arg=0);

		//����ע������лص�����������ĺ���ע��
		//ע���ص��������ַ�ʽ��ͨ���ڶ���������е���removeAllCallback����ϸ���ȵĿ��������removeCallback

		//ע���ص�������Ĳ���Ϊע��ص�ʱ���ص�ָ��
		//�ɹ�����0��ʧ�ܷ���-1
		int removeCallback(void* cb);
		//ע����˶�����ص����лص�������Ĳ���Ϊע��ص�ʱ��ʹ�õ�obj����
		//�����Ƴ���callback����
		int removeObjCallbacks(void* obj);

	};


	//IO���У��û������ڴ�ע�ᶨʱ������
	class BlmIoqueue: private MemPool {
		friend class PjTcp;
		friend class PjHttp;
		pj_timer_heap_t *timer_heap;
		pj_ioqueue_t *ioque;
		vector<TimerObj*> timerCallbacks;
		void* addTimerCallback_imp(int interval, SimpleCallback* scb);
		void wget_imp(const char* url, DataCallback* dcb, int millsec);
	public:
		BlmIoqueue();
		~BlmIoqueue();
		//��ѯ�������ݣ��������ݴ���Ļص�����ѯ��ʱ�������ó�ʱ�Ķ�ʱ�������û�����ݣ������ȴ�milliseconds����
		void poll(int milliseconds=0);
		//ע�ᶨʱ���Ļص����������BlmClient�У�ע��ע��������˵��
		//ʹ�öѶԳ�ʱ�Ķ�ʱ�����б������㷨���Ӷ�ΪO(lg(n))
		template<class T> void* addTimerCallback(int interval, T* obj, int (T::*mfn)(void*), void* arg=0) ;
		//ע���ص�
		//�ɹ�����0��ʧ�ܷ���-1
		int removeTimer(void* timer);
		//ע����˶�����ص����лص�������Ĳ���Ϊע��ص�ʱ��ʹ�õ�obj����
		//�����Ƴ���timer����
		int removeObjTimers(void* obj);
		template<class T> void wget(const char* url, T* obj, int(T::*mfn)(const string&, void*), int millisec_timeout=5000, void* arg=0);
	};

	struct SingleComm: public BlmIoqueue, public BlmClient, public Singleton<SingleComm> { 
		SingleComm(): BlmClient((BlmIoqueue*)this) {}
		int removeObj(void* obj) { int r = removeObjTimers(obj); r += removeObjCallbacks(obj); checktrue = r > 0; return r; }
	};


	
	//������ģ��ʵ�֣��������ͷ�ļ��У�����������ӿ��޹�
	template<class T> void* BlmIoqueue::addTimerCallback(int interval, T* obj, int (T::*mfn)(void*), void* arg) {
		return addTimerCallback_imp(interval, makeSimpleCallback(obj, mfn, arg));
	}

	template<class T> void BlmIoqueue::wget(const char* url, T* obj, int(T::*mfn)(const string&, void*), int millisec, void* arg) {
		DataCallback* cb = makeDataCallback(obj, mfn, arg);
		wget_imp(url, cb, millisec);
	}


	template<class C> void* BlmClient::addConnectCallback(C* obj, int (C::*memFn)(void*), void* arg) {
		SimpleCallback* cb = makeSimpleCallback(obj, memFn, arg);
		connCallbacks.push_front(cb);
		return cb;
	}
	template<class C> void* BlmClient::addCloseCallback(C* obj, int (C::*memFn)(void*), void* arg) {
		SimpleCallback* cb = makeSimpleCallback(obj, memFn, arg);
		closeCallbacks.push_front(cb);
		return cb;
	}
	template<class C> void* BlmClient::addMsgCallback(int mt, C* obj, int (C::*memFn)(int, int, const string&, int, void*), void* arg) {
		MsgCallback* cb = makeMsgCallback(obj, memFn, arg);
		msgMap[mt].push_front(cb);
		return cb;
	}

}



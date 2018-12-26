#ifndef __TARS_TC_EPOLL_SERVER_H_
#define __TARS_TC_EPOLL_SERVER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <list>

#include <memory>

#include "tc_epoller.h"
#include "tc_socket.h"
#include "tc_thread.h"
#include "tc_thread_queue.h"
#include "tc_clientsocket.h"
#include "tc_buffer_pool.h"

using namespace std;

namespace tars
{

class TC_EpollServer : public TC_ThreadLock
{
public:

    enum EM_CLOSE_T
    {
        EM_CLIENT_CLOSE = 0,         //客户端主动关闭
        EM_SERVER_CLOSE = 1,        //服务端业务主动调用close关闭连接,或者框架因某种异常主动关闭连接
        EM_SERVER_TIMEOUT_CLOSE = 2  //连接超时了，服务端主动关闭
    };	

    class NetThread;

    class BindAdapter;    
	typedef shared_ptr<BindAdapter> BindAdapterPtr;

    class Handle;
	typedef shared_ptr<Handle> HandlePtr;

    struct tagRecvData
    {
        uint32_t        uid;            /**连接标示*/
        string          buffer;         /**需要发送的内容*/
        string          ip;             /**远程连接的ip*/
        uint16_t        port;           /**远程连接的端口*/
        int64_t         recvTimeStamp;  /**接收到数据的时间*/
        bool            isOverload;     /**是否已过载 */
        bool            isClosed;       /**是否已关闭*/
        int                fd;                /*保存产生该消息的fd，用于回包时选择网络线程*/
        BindAdapterPtr  adapter;        /**标识哪一个adapter的消息*/
        int             closeType;     /*如果是关闭消息包，则标识关闭类型,0:表示客户端主动关闭；1:服务端主动关闭;2:连接超时服务端主动关闭*/
    };

    struct tagSendData
    {
        char            cmd;            /**命令:'c',关闭fd; 's',有数据需要发送*/
        uint32_t        uid;            /**连接标示*/
        string          buffer;         /**需要发送的内容*/
        string          ip;             /**远程连接的ip*/
        uint16_t        port;           /**远程连接的端口*/
    };

   struct HandleGroup
   {
        string                      name;
        TC_ThreadLock               monitor;
        vector<HandlePtr>           handles;
        map<string, BindAdapterPtr> adapters;
   };

    typedef TC_ThreadQueue<tagRecvData*, deque<tagRecvData*> > recv_queue;
    typedef TC_ThreadQueue<tagSendData*, deque<tagSendData*> > send_queue;
    typedef recv_queue::queue_type recv_queue_type;

    class HandleGroup;
    typedef shared_ptr<HandleGroup> HandleGroupPtr;

    class Handle : public TC_Thread, public TC_ThreadLock
    {
    public:

        Handle();

        virtual ~Handle();

        void setEpollServer(TC_EpollServer *pEpollServer);

        TC_EpollServer* getEpollServer();

        virtual void run();

    public:

        void sendResponse(unsigned int uid, const string &sSendBuffer, const string &ip, int port, int fd);

        void close(unsigned int uid, int fd);

        virtual void initialize() {};

		virtual void notifyFilter();

		void setHandleGroup(TC_EpollServer::BindAdapterPtr& lsPtr);

		void setHandleGroup(HandleGroupPtr& pHandleGroup);

		HandleGroupPtr& getHandleGroup();	

		friend class BindAdapter;

    protected:

        TC_EpollServer  *_pEpollServer;

		HandleGroupPtr _handleGroup;

		BindAdapterPtr  _lsPtr;

        uint32_t  _iWaitTime;

        //vector<Handle>           handles;

   protected:

        virtual void handleImp();

		virtual void handle(const tagRecvData &stRecvData) = 0;

		virtual void heartbeat() {}
    };
	
	class BindAdapter : public TC_ThreadLock
	{
	public:
		
		BindAdapter(){}
		
		BindAdapter(TC_EpollServer *pEpollServer);

		~BindAdapter();

		void setName(const string &name);

		string getName() const;

		void setHandleNum(int n);

		int getHandleNum();

        void setEndpoint(const string &str,const int &port);

        TC_Endpoint getEndpoint() const;

        TC_Socket &getSocket();

        TC_EpollServer* getEpollServer();

        void insertRecvQueue(const recv_queue::queue_type &vtRecvData,bool bPushBack = true);

        bool waitForRecvQueue(tagRecvData* &recv, uint32_t iWaitTime);

        template<typename T> void setHandle()
		{
		
			BindAdapterPtr thisptr(this);	
			_pEpollServer->setHandleGroup<T>(_handleGroupName, _iHandleNum, thisptr);
		}

		void setHandleGroupName(const string& handleGroupName);

		string getHandleGroupName() const;

		void setHeartBeatTime(time_t t);

		time_t getHeartBeatTime() const;

	public:
	
        //TC_ThreadLock   monitor;

    protected:

        friend class TC_EpollServer;
        friend class NetThread;

        TC_EpollServer  *_pEpollServer;

		HandleGroupPtr  _handleGroup;

        TC_Socket       _s;

        TC_Endpoint     _ep;

        recv_queue      _rbuffer;		
        
		string         _name;

		string         _handleGroupName;

		size_t         _iHandleNum;

		volatile time_t          _iHeartBeatTime;
 
	};

	class NetThread : public TC_Thread, public TC_ThreadLock
	{
	public:

		class Connection
		{
		public:

			Connection(BindAdapter *pBindAdapter, int lfd, int timeout, int fd, const string& ip, uint16_t port);

			virtual ~Connection();

			BindAdapter* getBindAdapter()       { return _pBindAdapter; }

			void init(unsigned int uid)         { _uid = uid; }

			uint32_t getId() const              { return _uid; }

			int getListenfd() const             { return _lfd; }

			virtual int getfd() const                   { return _sock.getfd(); }

			string getIp() const                { return _ip; }

			uint16_t getPort() const            { return _port; }
	
			bool setClose();

		protected:
			
			void close();

			virtual int recv(recv_queue::queue_type &o);
			
			void insertRecvQueue(recv_queue::queue_type &vRecvData);

			virtual int send();

			virtual int send(const string& buffer, const string &ip, uint16_t port, bool byEpollout = false);
	
			int send(const std::vector<TC_Slice>& slices);			

			friend class NetThread;

		private:

			int tcpSend(const void* data, size_t len);

			int tcpWriteV(const std::vector<iovec>& buffers);

			void clearSlices(std::vector<TC_Slice>& slices);

			void adjustSlices(std::vector<TC_Slice>& slices, size_t toSkippedBytes);
		
		protected:
			
			BindAdapter         *_pBindAdapter;

			TC_Socket           _sock;

			volatile uint32_t   _uid;

			int                 _lfd;

			string              _ip;

			bool                _bClose;

			uint16_t             _port;

			char                *_pRecvBuffer;
		
			string              _recvbuffer;
		
		    std::vector<TC_Slice>  _sendbuffer;

		};		

		NetThread(TC_EpollServer *epollServer);
		virtual ~NetThread();	

		int bind(BindAdapterPtr &lsPtr);

        void bind(const TC_Endpoint &ep, TC_Socket &s);

		void run();		

		void createEpoll(uint32_t iIndex = 0);

		bool accept(int fd);

		void processNet(const epoll_event &ev);

		void processPipe();

		int recvBuffer(Connection *cPtr, recv_queue::queue_type &v);

		int sendBuffer(Connection *cPtr, const string &buffer, const string &ip, uint16_t port);

		int sendBuffer(Connection *cPtr);

		void terminate();

		void delConnection(Connection *cPtr, bool bEraseList = true,EM_CLOSE_T closeType=EM_CLIENT_CLOSE);

		enum
        	{
            	ET_LISTEN = 1,
            	ET_CLOSE  = 2,
            	ET_NOTIFY = 3,
            	ET_NET    = 0,
        	};

		struct
		{
			string response;
			uint32_t uid;	
		}_response;

		void send(unsigned int uid, const string &s, const string &ip, uint16_t port);

		void close(unsigned int uid);

		void addTcpConnection(Connection *cPtr);

		friend class BindAdapter;
        friend class ConnectionList;
        friend class TC_EpollServer;


	private:

		TC_EpollServer            *_epollServer;

		TC_Socket                  _shutdown;
	 	TC_Socket                  _notify;

		TC_Socket                  _bind_listen;

		TC_Epoller                 _epoller;

		string                     _recvbuffer;

		string                     response;

		map<int,int>               _listen_connect_id;

		map<int,Connection*>       _uid_connection;

		list<uint32_t>             _free;

		volatile size_t            _free_size;

	    send_queue                 _sbuffer;

		map<int, BindAdapterPtr>      _listeners;

		bool                        _bTerminate;
	};


public:

	TC_EpollServer(unsigned int iNetThreadNum);
	~TC_EpollServer();

public:

	unsigned int getNetThreadNum() { return _netThreadNum; }

	void stopThread();

    vector<TC_EpollServer::NetThread*> getNetThread() { return _netThreads; }
    
    void send(unsigned int uid, const string &s, const string &ip, uint16_t port, int fd);

	void close(unsigned int uid, int fd);

    template<class T> void setHandleGroup(const string& groupName, int32_t handleNum, BindAdapterPtr adapter)
    {
        map<string, HandleGroupPtr>::iterator it = _handleGroups.find(groupName);

        if (it == _handleGroups.end())
        {
            //HandleGroupPtr hg = new HandleGroup();
			
			HandleGroup* hgptr = new HandleGroup();

			HandleGroupPtr hg(hgptr);

            hg->name = groupName;

            adapter->_handleGroup = hg;

            for (int32_t i = 0; i < handleNum; ++i)
            {
				//ServantHandle* shptr = new T();
				
				Handle* hptr = new T();

				HandlePtr handle(hptr);

                //HandlePtr handle = new T();

                handle->setEpollServer(this);

                handle->setHandleGroup(hg);

                hg->handles.push_back(handle);
            }

            _handleGroups[groupName] = hg;

            it = _handleGroups.find(groupName);
        }
        it->second->adapters[adapter->getName()] = adapter;

        adapter->_handleGroup = it->second;
    }


	NetThread* getNetThreadOfFd(int fd)
    {
    	return _netThreads[fd % _netThreads.size()];
    }

	int  bind(TC_EpollServer::BindAdapterPtr &lsPtr);

	void startHandle();

	void addConnection(NetThread::Connection * cPtr, int fd, int iType);

	void createEpoll();

	bool isTerminate() const    { return _bTerminate; }
	
	void terminate();

protected:

    friend class BindAdapter;

private:

	bool                           _bTerminate;

	std::vector<NetThread*>        _netThreads;

	unsigned int                   _netThreadNum;

	bool                           _handleStarted;

	map<string, HandleGroupPtr>    _handleGroups;
};

typedef shared_ptr<TC_EpollServer> TC_EpollServerPtr;

}

#endif

g++ -g -Wall -std=c++11 -o tar-demo tc_epoll_server.cpp main.cpp tc_epoller.cpp tc_socket.cpp tc_thread_cond.cpp tc_thread.cpp tc_ex.cpp tc_thread_mutex.cpp tc_clientsocket.cpp  HelloImp.cpp ServantHelper.cpp Servant.cpp ServantHandle.cpp -lpthread
g++ -Wall -g -std=c++11 -o tar-client tar_client.cpp CommunicatorEpoll.cpp tc_thread.cpp tc_thread_mutex.cpp tc_epoller.cpp  tc_socket.cpp tc_thread_cond.cpp tc_ex.cpp NetworkUtil.cpp ObjectProxy.cpp -lpthread


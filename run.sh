g++ -g -Wall -std=c++11 -o tar-demo tc_epoll_server.cpp main.cpp tc_epoller.cpp tc_socket.cpp tc_thread_cond.cpp tc_thread.cpp tc_ex.cpp tc_thread_mutex.cpp tc_clientsocket.cpp  HelloImp.cpp ServantHelper.cpp Servant.cpp ServantHandle.cpp -lpthread
g++ -g -Wall -std=c++11 -o client client.cpp -lpthread

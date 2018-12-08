g++ -std=c++11 -o tar-demo tc_epoll_server.cpp main.cpp tc_epoller.cpp tc_socket.cpp tc_thread_cond.cpp tc_thread.cpp tc_ex.cpp tc_thread_mutex.cpp -lpthread
g++ -std=c++11 -o client client.cpp -lpthread

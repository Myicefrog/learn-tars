CC          = gcc
CXX         = g++
CFLAGS      += -std=c++11 -g -O2 -Wno-deprecated -Wall

LD_LIBS = -lpthread
LD_OPTS = 

PWD := $(shell pwd)

TARS_CPP_UTIL_SRC_DIR = ${PWD}/util/src
TARS_CPP_UTIL_INC_DIR = ${PWD}/util/include

TARS_CPP_SERVANT_SRC_DIR = ${PWD}/servant/libservant
TARS_CPP_SERVANT_INC_DIR = ${PWD}/servant/servant

TARS_CPP_FRAME_WORK_DIR = ${PWD}/framework
TARS_CPP_NODE_SERVER_DIR = ${TARS_CPP_FRAME_WORK_DIR}/NodeServer
TARS_CPP_ADMINREGISTRY_SERVER_DIR = ${TARS_CPP_FRAME_WORK_DIR}/AdminRegistryServer

TARS_CPP_EXAMPLE_DIR = ${PWD}/examples

#-----------------------------------------------------------
# util
#-----------------------------------------------------------

TARS_CPP_UTIL_SRC = ${TARS_CPP_UTIL_SRC_DIR}/tc_buffer.cpp \
	${TARS_CPP_UTIL_SRC_DIR}/tc_epoll_server.cpp \
	${TARS_CPP_UTIL_SRC_DIR}/tc_thread_cond.cpp \
	${TARS_CPP_UTIL_SRC_DIR}/tc_clientsocket.cpp\
	${TARS_CPP_UTIL_SRC_DIR}/tc_ex.cpp \
	${TARS_CPP_UTIL_SRC_DIR}/tc_thread.cpp \
	${TARS_CPP_UTIL_SRC_DIR}/tc_epoller.cpp \
	${TARS_CPP_UTIL_SRC_DIR}/tc_socket.cpp \
	${TARS_CPP_UTIL_SRC_DIR}/tc_thread_mutex.cpp

TARS_CPP_UTIL_INC = -I${TARS_CPP_UTIL_INC_DIR}

TARS_CPP_UTIL_OBJ = ${patsubst %.cpp, %.o, ${TARS_CPP_UTIL_SRC}}

TARS_COROUTINE = ${TARS_CPP_UTIL_SRC_DIR}/tc_jump_x86_64_sysv_elf_gas.s \
	${TARS_CPP_UTIL_SRC_DIR}/tc_make_x86_64_sysv_elf_gas.s 

#------------------------------------------------------------
# servant
#------------------------------------------------------------

TARS_CPP_SERVANT_SRC = ${TARS_CPP_SERVANT_SRC_DIR}/AdminServant.cpp \
	${TARS_CPP_SERVANT_SRC_DIR}/Application.cpp \
	${TARS_CPP_SERVANT_SRC_DIR}/AsyncProcThread.cpp \
	${TARS_CPP_SERVANT_SRC_DIR}/Communicator.cpp \
	${TARS_CPP_SERVANT_SRC_DIR}/CommunicatorEpoll.cpp \
	${TARS_CPP_SERVANT_SRC_DIR}/CoroutineScheduler.cpp \
	${TARS_CPP_SERVANT_SRC_DIR}/NetworkUtil.cpp \
	${TARS_CPP_SERVANT_SRC_DIR}/ObjectProxy.cpp \
	${TARS_CPP_SERVANT_SRC_DIR}/ObjectProxyFactory.cpp \
	${TARS_CPP_SERVANT_SRC_DIR}/Servant.cpp \
	${TARS_CPP_SERVANT_SRC_DIR}/ServantHandle.cpp \
	${TARS_CPP_SERVANT_SRC_DIR}/ServantHelper.cpp \
	${TARS_CPP_SERVANT_SRC_DIR}/ServantProxy.cpp \
	${TARS_CPP_SERVANT_SRC_DIR}/TarsNodeF.cpp \
	${TARS_CPP_SERVANT_SRC_DIR}/Transceiver.cpp

TARS_CPP_SERVANT_INC = -I${TARS_CPP_SERVANT_INC_DIR}

TARS_CPP_SERVANT_OBJ = ${patsubst %.cpp, %.o, ${TARS_CPP_SERVANT_SRC}}

#-------------------------------------------------------------
# HelloServer
#-------------------------------------------------------------

HELLO_SERVER_SRC = ${TARS_CPP_EXAMPLE_DIR}/HelloServer/HelloImp.cpp \
	${TARS_CPP_EXAMPLE_DIR}/HelloServer/HelloServer.cpp 
	
HELLO_SERVER_INC = ${TARS_CPP_SERVANT_INC} \
	${TARS_CPP_UTIL_INC} \
	-I${TARS_CPP_EXAMPLE_DIR}/HelloServer

HELLO_SERVER_OBJ = ${patsubst %.cpp, %.o, ${HELLO_SERVER_SRC}}

HELLO_SERVER_CLIENT_SRC = ${TARS_CPP_EXAMPLE_DIR}/HelloServer/Client/tar_client_improve.cpp

HELLO_SERVER_CLIENT_OBJ = ${patsubst %.cpp, %.o, ${HELLO_SERVER_CLIENT_SRC}}

HELLO_SERVER_CLIENT_ASYNC_SRC = ${TARS_CPP_EXAMPLE_DIR}/HelloServer/AsyncClient/tar_client_async_improve.cpp

HELLO_SERVER_CLIENT_ASYNC_OBJ = ${patsubst %.cpp, %.o, ${HELLO_SERVER_CLIENT_ASYNC_SRC}}

#---------------------------------------------------------------
# NodeServer
#---------------------------------------------------------------
NODE_SERVER_SRC = ${TARS_CPP_NODE_SERVER_DIR}/NodeImp.cpp \
	${TARS_CPP_NODE_SERVER_DIR}/NodeServer.cpp \
	${TARS_CPP_NODE_SERVER_DIR}/ServerImp.cpp \
	${TARS_CPP_NODE_SERVER_DIR}/ServerObject.cpp

NODE_SERVER_INC = ${TARS_CPP_SERVANT_INC} \
	${TARS_CPP_UTIL_INC} \
	-I${TARS_CPP_NODE_SERVER_DIR}

NODE_SERVER_OBJ = ${patsubst %.cpp, %.o, ${NODE_SERVER_SRC}}

NODE_SERVER_CLIENT_SRC = ${TARS_CPP_NODE_SERVER_DIR}/tar_client_NodeServer.cpp

NODE_SERVER_CLIENT_OBJ = ${patsubst %.cpp, %.o, ${NODE_SERVER_CLIENT_SRC}}

#----------------------------------------------------------------
# AdminRegistryServer
#----------------------------------------------------------------
ADMINREGISTRY_SERVER_SRC = ${TARS_CPP_ADMINREGISTRY_SERVER_DIR}/AdminRegistryImp.cpp \
	${TARS_CPP_ADMINREGISTRY_SERVER_DIR}/AdminRegistryServer.cpp

ADMINREGISTRY_SERVER_INC = ${TARS_CPP_SERVANT_INC} \
	${TARS_CPP_UTIL_INC} \
	-I${TARS_CPP_ADMINREGISTRY_SERVER_DIR}

ADMINREGISTRY_SERVER_OBJ = ${patsubst %.cpp, %.o, ${ADMINREGISTRY_SERVER_SRC}}

ADMINREGISTRY_CLIENT_SRC = ${TARS_CPP_ADMINREGISTRY_SERVER_DIR}/tar_client_AdminRegServer.cpp

ADMINREGISTRY_CLIENT_OBJ = ${patsubst %.cpp, %.o, ${ADMINREGISTRY_CLIENT_SRC}}


#----------------------------------------------------------------
# Build Target
#----------------------------------------------------------------

all: HelloServer HelloServer-client HelloServer-client-async NodeServer NodeServer-client AdminRegServer AdminRegServer-client
.PHONY : all

AdminRegServer: ${ADMINREGISTRY_SERVER_OBJ} ${TARS_CPP_UTIL_OBJ} ${TARS_CPP_SERVANT_OBJ}
	${CXX} ${ADMINREGISTRY_SERVER_OBJ} ${TARS_CPP_UTIL_OBJ} ${TARS_CPP_SERVANT_OBJ} ${TARS_COROUTINE} -o $@ ${LD_OPTS} ${LD_LIBS} -g
	@echo "Compile AdminRegServer done."

AdminRegServer-client: ${ADMINREGISTRY_CLIENT_OBJ} ${TARS_CPP_UTIL_OBJ} ${TARS_CPP_SERVANT_OBJ}
	${CXX} ${ADMINREGISTRY_CLIENT_OBJ} ${TARS_CPP_UTIL_OBJ} ${TARS_CPP_SERVANT_OBJ} ${TARS_COROUTINE} -o $@ ${LD_OPTS} ${LD_LIBS} -g
	@echo "Compile AdminRegServer-client done."

HelloServer: ${HELLO_SERVER_OBJ} ${TARS_CPP_UTIL_OBJ} ${TARS_CPP_SERVANT_OBJ}
	${CXX} ${HELLO_SERVER_OBJ} ${TARS_CPP_UTIL_OBJ} ${TARS_CPP_SERVANT_OBJ} ${TARS_COROUTINE} -o $@ ${LD_OPTS} ${LD_LIBS} -g
	@echo "Compile HelloServer done."

HelloServer-client: ${HELLO_SERVER_CLIENT_OBJ} ${TARS_CPP_UTIL_OBJ} ${TARS_CPP_SERVANT_OBJ}
	${CXX} ${HELLO_SERVER_CLIENT_OBJ} ${TARS_CPP_UTIL_OBJ} ${TARS_CPP_SERVANT_OBJ} ${TARS_COROUTINE}  -o $@ ${LD_OPTS} ${LD_LIBS} -g
	@echo "Compile HelloServer-client done."

HelloServer-client-async: ${HELLO_SERVER_CLIENT_ASYNC_OBJ} ${TARS_CPP_UTIL_OBJ} ${TARS_CPP_SERVANT_OBJ}
	${CXX} ${HELLO_SERVER_CLIENT_ASYNC_OBJ} ${TARS_CPP_UTIL_OBJ} ${TARS_CPP_SERVANT_OBJ} ${TARS_COROUTINE} -o $@ ${LD_OPTS} ${LD_LIBS} -g
	@echo "Compile HelloServer-client-async done."

NodeServer-client: ${NODE_SERVER_CLIENT_OBJ} ${TARS_CPP_UTIL_OBJ} ${TARS_CPP_SERVANT_OBJ}
	${CXX} ${NODE_SERVER_CLIENT_OBJ} ${TARS_CPP_UTIL_OBJ} ${TARS_CPP_SERVANT_OBJ} ${TARS_COROUTINE} -o $@ ${LD_OPTS} ${LD_LIBS} -g
	@echo "Compile NodeServer-client done."

NodeServer: ${NODE_SERVER_OBJ} ${TARS_CPP_UTIL_OBJ} ${TARS_CPP_SERVANT_OBJ}
	${CXX} ${NODE_SERVER_OBJ} ${TARS_CPP_UTIL_OBJ} ${TARS_CPP_SERVANT_OBJ} ${TARS_COROUTINE} -o $@ ${LD_OPTS} ${LD_LIBS} -g 
	@echo "Compile NodeServer done."

#-----------------------------------------------------------------
# Buile Object
#-----------------------------------------------------------------

${TARS_CPP_UTIL_OBJ}:%.o:%.cpp
	@echo "Compiling $< ==> $@"
	${CXX} ${CFLAGS} ${TARS_CPP_UTIL_INC} ${TARS_CPP_SERVANT_INC} -c $< -o $@

${TARS_CPP_SERVANT_OBJ}:%.o:%.cpp
	@echo "Compiling $< ==> $@"
	${CXX} ${CFLAGS} ${TARS_CPP_SERVANT_INC} ${TARS_CPP_UTIL_INC} -c $< -o $@

${ADMINREGISTRY_SERVER_OBJ}:%.o:%.cpp
	@echo "Compiling $< ==> $@"
	${CXX} ${CFLAGS} ${ADMINREGISTRY_SERVER_INC}  -c $< -o $@

${ADMINREGISTRY_CLIENT_OBJ}:%.o:%.cpp
	@echo "Compiling $< ==> $@"
	${CXX} ${CFLAGS} ${ADMINREGISTRY_SERVER_INC}  -c $< -o $@

${HELLO_SERVER_OBJ}:%.o:%.cpp
	@echo "Compiling $< ==> $@"
	${CXX} ${CFLAGS} ${HELLO_SERVER_INC} -c $< -o $@	

${NODE_SERVER_OBJ}:%.o:%.cpp
	@echo "Compiling $< ==> $@"
	${CXX} ${CFLAGS} ${NODE_SERVER_INC} -c $< -o $@

${HELLO_SERVER_CLIENT_OBJ}:%.o:%.cpp
	@echo "Compiling $< ==> $@"
	${CXX} ${CFLAGS} ${HELLO_SERVER_INC} -c $< -o $@

${HELLO_SERVER_CLIENT_ASYNC_OBJ}:%.o:%.cpp
	@echo "Compiling $< ==> $@"
	${CXX} ${CFLAGS} ${HELLO_SERVER_INC} -c $< -o $@

${NODE_SERVER_CLIENT_OBJ}:%.o:%.cpp
	@echo "Compiling $< ==> $@"
	${CXX} ${CFLAGS} ${NODE_SERVER_INC} -c $< -o $@

#------------------------------------------------------------------
# Clean
#------------------------------------------------------------------

clean:
	@rm -f ${TARS_CPP_SERVANT_OBJ} ${TARS_CPP_UTIL_OBJ}
	@rm -f ${HELLO_SERVER_OBJ}
	@rm -f ${NODE_SERVER_OBJ}
	@rm -f ${HELLO_SERVER_CLIENT_OBJ}
	@rm -f ${HELLO_SERVER_CLIENT_ASYNC_OBJ}
	@rm -f ${NODE_SERVER_CLIENT_OBJ}
	@rm -f ${ADMINREGISTRY_SERVER_OBJ}
	@rm -f ${ADMINREGISTRY_CLIENT_OBJ}
	@echo "Clean object files done."

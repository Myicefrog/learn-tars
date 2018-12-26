#ifndef __START_COMMAND_H_
#define __START_COMMAND_H_

#include "ServerCommand.h"
#include <sys/types.h>
#include<sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

static pid_t *childpid = NULL;

class CommandStart : public ServerCommand
{
public:
    CommandStart(const ServerObjectPtr& pServerObjectPtr, bool bByNode = false);

    ExeStatus canExecute(string& sResult);

    int execute(string& sResult);

	FILE* popen2(const char *cmdstring, const char *type);

	int pclose2(FILE *fp);

private:

	bool startByScript(string& sResult);

private:

	bool                _byNode;
	string              _exeFile;
	ServerObjectPtr     _serverObjectPtr;
	string 				_desc;

};

inline CommandStart::CommandStart(const ServerObjectPtr& pServerObjectPtr, bool bByNode)
: _byNode(bByNode)
, _serverObjectPtr(pServerObjectPtr)
{
    _exeFile   = _serverObjectPtr->getExeFile();
    _desc      = _serverObjectPtr->getServerDescriptor();
}

inline ServerCommand::ExeStatus CommandStart::canExecute(string& sResult)
{
	return EXECUTABLE;
}

inline bool CommandStart::startByScript(string& sResult)
{
    string sStartScript     = _serverObjectPtr->getStartScript();

	string sCmd  = sStartScript + " &";

	cout<<"CommandStart::startByScript sCmd is "<<sCmd<<endl;	

	FILE  *fp =  popen2(sCmd.c_str(), "r");

	int flags;
    if ((flags = fcntl(fileno(fp), F_GETFL, 0)) < 0)
    {
		cout<<"fcntl get error.script file: "<<sStartScript<<endl;
        fflush(fp);
        pclose2(fp);		
		return false;
	}

	flags |= O_NONBLOCK;

	if (fcntl(fileno(fp), F_SETFL, flags) < 0)
    {
		cout<<"fcntl set error.script file: "<<sStartScript<<endl;
        fflush(fp);
        pclose2(fp);

        return false;
	}

    fflush(fp);
    pclose2(fp);

	return true;

}

inline int CommandStart::execute(string& sResult)
{

	startByScript(sResult);

	return 0;
}

inline FILE* CommandStart::popen2(const char *cmdstring, const char *type)
{
    int     i, pfd[2];
    pid_t   pid;
    FILE    *fp;
    /*only allow "r" or "w" */
    if ((type[0] != 'r' && type[0] != 'w') || type[1] != 0)
    {
        errno = EINVAL;     /* required by POSIX.2 */
        return (NULL);
    }
    int maxfd = static_cast<int>(sysconf(_SC_OPEN_MAX));
    if (childpid == NULL)
    {  /* first time through */
        /* allocate zeroed out array for child pids */
        childpid = new pid_t[maxfd];
        memset(childpid, 0, sizeof(pid_t) * maxfd);
        //char** envArray = new char*[envCount];
        if (childpid  == NULL)
        {
            return (NULL);
        }
    }

    if (pipe(pfd) < 0)
    {
        return (NULL);   /* errno set by pipe() */
    }
	
    if ((pid = fork()) < 0)
    {
        return (NULL);   /* errno set by fork() */
    }
    else if (pid == 0)
    {                            /* child */
        if (*type == 'r')
        {
            close(pfd[0]);
            dup2(pfd[1], STDOUT_FILENO);
            dup2(pfd[1], STDERR_FILENO);
            close(pfd[1]);
        }
        else
        {
            close(pfd[1]);
            dup2(pfd[0], STDIN_FILENO);
            close(pfd[0]);
        }

		cout<<"close pfd"<<endl;

        /* close all descriptors opened in parent process*/
        for (i = 3; i < maxfd; i++)
        {
            if (i != pfd[0] && i !=  pfd[1])
            {
                close(i);
            }
        }
		
        execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
        _exit(127);
    }
    /* parent */
    if (*type == 'r')
    {
		cout<<"parent process"<<endl;
		
        close(pfd[1]);
        if ((fp = fdopen(pfd[0], type)) == NULL)
        {
            return (NULL);
        }
    }
    else
    {
        close(pfd[0]);
        if ((fp = fdopen(pfd[1], type)) == NULL)
        {
            return (NULL);
        }
    }
    childpid[fileno(fp)] = pid; /* remember child pid for this fd */
    return (fp);
}

inline int CommandStart::pclose2(FILE *fp)
{
    int     fd, stat;
    pid_t   pid;

    if (childpid == NULL)
    {
        return (-1);     /* popen() has never been called */
    }
    fd = fileno(fp);
    if ((pid = childpid[fd]) == 0)
    {
        return (-1);     /* fp wasn't opened by popen() */
    }
    childpid[fd] = 0;
    if (fclose(fp) == EOF)
    {
        return (-1);
    }

	cout<<"CommandStart::pclose2 waitpid"<<endl;

    while (waitpid(pid, &stat, 0) < 0)
    {
        if (errno != EINTR)
        {
            return (-1); /* error other than EINTR from waitpid() */
        }
    }
    return (stat);   /* return child's termination status */
}

#endif

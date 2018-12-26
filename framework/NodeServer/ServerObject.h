#ifndef __SERVER_OBJECT_H_
#define __SERVER_OBJECT_H_

#include "Node.h"
#include <string>
#include <memory>

using namespace tars;
using namespace std;

class ServerObject 
{
public:

	ServerObject( const string& tDesc);

	~ServerObject() { };

	string  getServerDescriptor() { return  _desc; }

	string getExeFile(){return _exeFile;}

	void setExeFile(const string &sExeFile){_exeFile = sExeFile;}

	string getStartScript() {return _startScript;}

	void setScript(const string &sStartScript) {_startScript = sStartScript;}

private:

	string  _desc;

	string _exeFile;

	string _startScript;
	
};

typedef shared_ptr<ServerObject> ServerObjectPtr;

#endif



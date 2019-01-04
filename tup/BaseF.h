#ifndef __BASEF_H_
#define __BASEF_H_

#include <map>
#include <string>
#include <vector>
#include "Tars.h"
using namespace std;


namespace tars
{
    const tars::Short TARSVERSION = 1;

    const tars::Short TUPVERSION = 3;

    const tars::Char TARSNORMAL = 0;

    const tars::Char TARSONEWAY = 1;

    const tars::Int32 TARSSERVERSUCCESS = 0;

    const tars::Int32 TARSSERVERDECODEERR = -1;

    const tars::Int32 TARSSERVERENCODEERR = -2;

    const tars::Int32 TARSSERVERNOFUNCERR = -3;

    const tars::Int32 TARSSERVERNOSERVANTERR = -4;

    const tars::Int32 TARSSERVERRESETGRID = -5;

    const tars::Int32 TARSSERVERQUEUETIMEOUT = -6;

    const tars::Int32 TARSASYNCCALLTIMEOUT = -7;

    const tars::Int32 TARSINVOKETIMEOUT = -7;

    const tars::Int32 TARSPROXYCONNECTERR = -8;

    const tars::Int32 TARSSERVEROVERLOAD = -9;

    const tars::Int32 TARSADAPTERNULL = -10;

    const tars::Int32 TARSINVOKEBYINVALIDESET = -11;

    const tars::Int32 TARSCLIENTDECODEERR = -12;

    const tars::Int32 TARSSERVERUNKNOWNERR = -99;

    const tars::Int32 TARSMESSAGETYPENULL = 0;

    const tars::Int32 TARSMESSAGETYPEHASH = 1;

    const tars::Int32 TARSMESSAGETYPEGRID = 2;

    const tars::Int32 TARSMESSAGETYPEDYED = 4;

    const tars::Int32 TARSMESSAGETYPESAMPLE = 8;

    const tars::Int32 TARSMESSAGETYPEASYNC = 16;

    const tars::Int32 TARSMESSAGETYPESETNAME = 128;


}



#endif

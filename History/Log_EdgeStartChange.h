#pragma once
#include "HistoryLog.h"

class Log_EdgeStartChange :
    public HistoryLog
{
public:
    const unsigned int m_index;

public:
    const int start;

public:
    Log_EdgeStartChange(unsigned int target,unsigned int theIndex,int theStart);
    std::string toString()
    {
        char temp[256];
        //sprintf(temp,"\t\t\t<Log Type=\"EdgeStartChange\" Target=\"%d\" Index=\"%d\" Start=\"%d\" />\n",target,index,start);
        std::string out(temp);
        return out;
    }
        void operator=(const Log_EdgeStartChange& in)
    {
        in;
    }

public:
    ~Log_EdgeStartChange(void);
};

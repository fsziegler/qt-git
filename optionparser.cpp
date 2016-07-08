#include "optionparser.h"

OptionParser::OptionParser(const string& optionStr)
{
    string optionStrCpy(optionStr);
    StripWS(optionStrCpy);
    TVarOptionVect varOptnVect;
    while(0 < optionStrCpy.length())
    {
        string nextOptionStr;
        ParseNextOption(optionStrCpy, nextOptionStr);
//        m_optnVect.push_back(nextOptionStr);
    }
}

const TStrVect& OptionParser::getOptnVect() const
{
    return m_optnVect;
}

void OptionParser::ParseNextOption(string& optionStr,
                                   string& nextOptionStr)
{
    if('[' == optionStr[0])
    {
        ParseNextDelimOption('[', ']', optionStr, nextOptionStr);
    }
    else if('<' == optionStr[0])
    {
        ParseNextDelimOption('<', '>', optionStr, nextOptionStr);
    }
    else
    {
        throw;
    }
}

void OptionParser::ParseNextDelimOption(char delimL, char delimR,
                                        string& optionStr,
                                        string& nextOptionStr)
{
    size_t endR(0);
    if(delimL != optionStr[endR])
    {
        throw;
    }
    // Count the nested layers and find the terminating delimiter
    int net(0);
    int layers(0);
    do
    {
        net += (delimL == optionStr[endR] ? 1 : 0);
        net -= (delimR == optionStr[endR] ? 1 : 0);
        layers = (net > layers ? net : layers);
        ++endR;
    } while(0 < net);

    nextOptionStr = optionStr.substr(1, endR - 2);
    optionStr.erase(0, nextOptionStr.length() + 2);
    if(1 == layers)
    {
        m_optnVect.push_back(nextOptionStr);
    }
    else if(2 == layers)
    {
        // Find the mid section to add & omit
        size_t midL(1);
        while(delimL != nextOptionStr[midL])
        {
            ++midL;
        }
        const size_t midR(nextOptionStr.find_first_of(delimR));

        string withStr(nextOptionStr);
        withStr.erase(midR, 1);
        withStr.erase(midL, 1);
        m_optnVect.push_back(withStr);

        string withoutStr(nextOptionStr);
        withoutStr.erase(midL, midR - midL + 1);
        m_optnVect.push_back(withoutStr);
    }
    else
    {
        throw;
    }
}

void OptionParser::StripWS(string& str)
{
    size_t ptr(str.length());
    while(0 < ptr)
    {
        --ptr;
        if(' ' == str[ptr])
        {
            str.erase(ptr, 1);
        }
    }
}

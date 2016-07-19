#include "optionparser.h"

OptionParser::OptionParser(const string& paramStr)
{
   string paramStrCpy(paramStr);
   StripWS(paramStrCpy);
   while(0 < paramStrCpy.length())
   {
      string nextParamStr;
      ParseNextOption(paramStrCpy, nextParamStr);
   }
}

const TStrVect& OptionParser::getOptnVect() const
{
   return m_optnVect;
}

bool OptionParser::RemoveChar(char c, string& str) const
{
   if(str.find_first_of(c) != str.find_last_of(c))
   {
      throw runtime_error("OptionParser::RemoveChar():\n"
                          "str.find_first_of(c) != str.find_last_of(c)");
   }
   size_t pos = str.find(c);
   if(string::npos != pos)
   {
      str.erase(pos, 1);
      return true;
   }
   return false;
}

void OptionParser::ParseNextOption(string& optionStr,
                                   string& nextOptionStr)
{
   string prefix;
   if('[' == optionStr[0])
   {
      ParseNextDelimOption('[', ']', prefix, optionStr, nextOptionStr);
   }
   else if('<' == optionStr[0])
   {
      ParseNextDelimOption('<', '>', prefix, optionStr, nextOptionStr);
   }
   else if(('=' == optionStr[0]) && ('<' == optionStr[1]))
   {
      prefix = "=";
      optionStr.erase(0, 1);
      ParseNextDelimOption('<', '>', prefix, optionStr, nextOptionStr);
   }
   else if(string::npos != optionStr.find('|'))
   {
      RemoveChar('{', optionStr);
      RemoveChar('}', optionStr);
      size_t pos0(0);
      size_t pos1(optionStr.find('|'));
      while(string::npos != pos1)
      {
         m_optnVect.push_back(optionStr.substr(pos0, pos1-pos0));
         pos0 = ++pos1;
         pos1 = optionStr.find('|', pos1);
      }
      m_optnVect.push_back(optionStr.substr(pos0, optionStr.length()-pos0));
      optionStr.clear();
   }
   else
   {
//        throw; //Add this back in
      optionStr.clear();  // Remove this
   }
}

void OptionParser::ParseNextDelimOption(char delimL, char delimR,
                                        const string& prefix,
                                        string& optionStr,
                                        string& nextOptionStr)
{
   nextOptionStr.clear();
   size_t endR(0);
   if((delimL != optionStr[endR])
      && ((prefix[0] != optionStr[endR])
          || (delimL != optionStr[endR + 1])))
   {
      throw runtime_error("OptionParser::ParseNextDelimOption():\n"
                          "if((delimL != optionStr[endR])"
                          "&& ((prefix[0] != optionStr[endR])"
                          "|| (delimL != optionStr[endR + 1])))");
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
   }
   while(0 < net);

   nextOptionStr = optionStr.substr(1, endR - 2);
   optionStr.erase(0, nextOptionStr.length() + 2);
   if(1 == layers)
   {
      m_optnVect.push_back(string(prefix + delimL + nextOptionStr + delimR));
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
      m_optnVect.push_back(string(prefix + withStr));

      string withoutStr(nextOptionStr);
      withoutStr.erase(midL, midR - midL + 1);
//        m_optnVect.push_back(withoutStr);
      m_optnVect.push_back(string(prefix + delimL + withoutStr + delimR));
   }
   else
   {
//        throw; //Add this back in
      optionStr.clear();  // Remove this
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

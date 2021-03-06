#ifndef OPTIONPARSER_H
#define OPTIONPARSER_H

#include "mainwindow.h"

// Class to parse git command line options; e.g.
// git-stash save [-p|--patch] [--[no-]keep-index] [-u|--include-untracked]
//  [-a|--all] [-q|--quiet] [<message>] becomes
//
// save [-p|--patch]
// save [--keep-index]
// save [--no-keep-index]
// save [-u|--include-untracked]
// save [-a|--all]
// save [-q|--quiet]
// save [<message>]

class OptionParser
{
public:
   OptionParser(const string& paramStr);
   ~OptionParser() {}
   const TStrVect& getOptnVect() const;

private:
   bool RemoveChar(char c, string& str) const;
   void ParseNextOption(string& optionStr, string& nextOptionStr);
   void ParseNextDelimOption(char delimL, char delimR, const string& prefix,
                             string& optionStr, string& nextOptionStr);
   void StripWS(string& str);

   OptionParser();
   OptionParser(const OptionParser&);
   OptionParser& operator=(const OptionParser&);
   bool operator==(const OptionParser&);
   bool operator!=(const OptionParser&);

   TStrVect m_optnVect;
};

#endif // OPTIONPARSER_H

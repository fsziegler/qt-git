#ifndef GITJSONTEXT_H
#define GITJSONTEXT_H

#include <vector>
#include <map>

using namespace std;

typedef vector<string> TStrVect;
typedef TStrVect::const_iterator TStrVectCItr;

typedef pair<string, TStrVect> TStrStrVectPair;
typedef map<string, TStrVect> TStrStrVectMap;
typedef TStrStrVectMap::const_iterator TStrStrVectMapCItr;

class GitJsonText
{
public:
    static GitJsonText& instance();
    const TStrVect& GetGitCmdVect(const string& gitCmdStr);

private:
#ifdef _WIN32
    void LoadGitPull();
    void LoadGitStash();
    void LoadGitPush();
    void LoadGitMerge();
    void LoadGitBranch();
    void LoadGitDiff();
    void LoadGitFetch();
    void LoadGitInit();
    void LoadGitCheckout();
    void LoadGitAdd();
    void LoadGitClone();
    void LoadGitRebase();
    void LoadGitCommit();
    void LoadGitStatus();
    void LoadGitLog();
#endif

    GitJsonText();
    ~GitJsonText(){}
    GitJsonText(GitJsonText&);
    GitJsonText& operator=(GitJsonText&);
    bool operator==(GitJsonText&);
    bool operator!=(GitJsonText&);


    TStrStrVectMap m_gitCmdMap;
    const TStrVect m_emptyVect;
};

#endif // GITJSONTEXT_H

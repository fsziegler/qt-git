#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QProcess>
#include <QDebug>
#include <QRegularExpression>
#include <QDesktopWidget>
#include <QInputDialog>

#include "gitinitdialog.h"
#include "gitclonedialog.h"
#include "testdialog.h"

#include <iostream>
#include <sstream>

using namespace std;

QFileInfo MainWindow::ms_rootGitDir;

MainWindow::MainWindow(const QString& cmdStr, QWidget *parent) :
   QMainWindow(parent),
   mc_appDir(QFileInfo(cmdStr).path()),
   mc_cfgFileStr(mc_appDir.filePath() + "/rootGitDir.txt"),
   mc_remoteRepoFileStr(mc_appDir.filePath() + "/remoteRepoUrl.txt"),
   ui(new Ui::MainWindow)
{
   ms_rootGitDir = cmdStr;
   ui->setupUi(this);
   const QRect screenRect(QApplication::desktop()->availableGeometry());
   if((screenRect.width() >= width()) &&
         (screenRect.height() >= height()))
   {
      setGeometry(
         QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            QApplication::desktop()->availableGeometry()));
   }
   setWindowTitle("Fred's qt-git");
   QString settingStr;
   ms_rootGitDir =
           QFileInfo(ReadSettingFromFile(mc_cfgFileStr,
                                         QString("[Git root unknown]"),
                                         ui->label_git_root,
                                         settingStr));
   ReadSettingFromFile(mc_remoteRepoFileStr,
                       QString("[Remote repo unknown]"),
                       ui->label_remote_repo,
                       settingStr);

//   ui->comboBox_stash->addItem("foo");
//   ui->comboBox_stash->addItem("bar");
//   ui->comboBox_stash->addItem("doo");
//   ui->comboBox_stash->addItem("echo");
   OnGitStatus();

   SetButtonFormattedToolTip(ui->btn_choose_git_root,
        QString("Choose the root directory for all git operations"));
//   ""
   SetButtonFormattedToolTip(ui->btn_remote_repo,
        QString("Choose the remote repository for all git operations"));

   SetButtonFormattedToolTip(ui->btn_git_add,
        QString("git-add - Add file contents to the index\n"
                "This command updates the index using the current content found"
                " in the working tree, to prepare the content staged for the ne"
                "xt commit. It typically adds the current content of existing p"
                "aths as a whole, but with some options it can also be used to "
                "add content with only part of the changes made to the working "
                "tree files applied, or remove paths that do not exist in the w"
                "orking tree anymore."));
   SetButtonFormattedToolTip(ui->btn_git_branch,
        QString("git-branch - List, create, or delete branches\n"
                "If --list is given, or if there are no non-option arguments, e"
                "xisting branches are listed; the current branch will be highli"
                "ghted with an asterisk. Option -r causes the remote-tracking b"
                "ranches to be listed, and option -a shows both local and remot"
                "e branches. If a <pattern> is given, it is used as a shell wil"
                "dcard to restrict the output to matching branches. If multiple"
                " patterns are given, a branch is shown if it matches any of th"
                "e patterns. Note that when providing a <pattern>, you must use"
                " --list; otherwise the command is interpreted as branch creati"
                "on.\n\nWith --contains, shows only the branches that contain t"
                "he named commit (in other words, the branches whose tip commit"
                "s are descendants of the named commit). With --merged, only br"
                "anches merged into the named commit (i.e. the branches whose t"
                "ip commits are reachable from the named commit) will be listed"
                ". With --no-merged only branches not merged into the named com"
                "mit will be listed. If the <commit> argument is missing it def"
                "aults to HEAD (i.e. the tip of the current branch)."));
   SetButtonFormattedToolTip(ui->btn_git_checkout,
        QString("git-checkout - Switch branches or restore working tree files\n"
                "Updates files in the working tree to match the version in the "
                "index or the specified tree. If no paths are given, git checko"
                "ut will also update HEAD to set the specified branch as the cu"
                "rrent branch."));
   SetButtonFormattedToolTip(ui->btn_git_clone,
        QString("git-clone - Clone a repository into a new directory\n"
                "Clones a repository into a newly created directory, creates re"
                "mote-tracking branches for each branch in the cloned repositor"
                "y (visible using git branch -r), and creates and checks out an"
                " initial branch that is forked from the cloned repository’s cu"
                "rrently active branch."));
   SetButtonFormattedToolTip(ui->btn_git_commit,
        QString("git-commit - Record changes to the repository\n"
                "Stores the current contents of the index in a new commit along"
                " with a log message from the user describing the changes."));
   SetButtonFormattedToolTip(ui->btn_git_diff,
        QString("git-diff - Show changes between commits, commit and working "
                "tree, etc\n"
                "Show changes between the working tree and the index or a tree,"
                " changes between the index and a tree, changes between two tre"
                "es, changes between two blob objects, or changes between two f"
                "iles on disk."));
   SetButtonFormattedToolTip(ui->btn_git_fetch,
        QString("git-fetch - Download objects and refs from another repository"
                "\n"
                "Fetch branches and/or tags (collectively, \"refs\") from one o"
                "r more other repositories, along with the objects necessary to"
                " complete their histories. Remote-tracking branches are update"
                "d (see the description of <refspec> below for ways to control "
                "this behavior)."));
   SetButtonFormattedToolTip(ui->btn_git_init,
        QString("git-init - Create an empty Git repository or reinitialize an "
                "existing one\n"
                "This command creates an empty Git repository - basically a .gi"
                "t directory with subdirectories for objects, refs/heads, refs/"
                "tags, and template files. An initial HEAD file that references"
                " the HEAD of the master branch is also created."));
   SetButtonFormattedToolTip(ui->btn_git_merge,
        QString("git-merge - Join two or more development histories together\n"
                "Incorporates changes from the named commits (since the time th"
                "eir histories diverged from the current branch) into the curre"
                "nt branch. This command is used by git pull to incorporate cha"
                "nges from another repository and can be used by hand to merge "
                "changes from one branch into another."));
   SetButtonFormattedToolTip(ui->btn_git_pull,
        QString("git-pull - Fetch from and integrate with another repository or"
                " a local branch\n"
                "Incorporates changes from a remote repository into the current"
                " branch. In its default mode, git pull is shorthand for git fe"
                "tch followed by git merge FETCH_HEAD.\n\nMore precisely, git p"
                "ull runs git fetch with the given parameters and calls git mer"
                "ge to merge the retrieved branch heads into the current branch"
                ". With --rebase, it runs git rebase instead of git merge."));
   SetButtonFormattedToolTip(ui->btn_git_push,
        QString("git-push - Update remote refs along with associated objects\n"
                "Updates remote refs using local refs, while sending objects ne"
                "cessary to complete the given refs."));
   SetButtonFormattedToolTip(ui->btn_git_rebase,
        QString("git-rebase - Reapply commits on top of another base tip\n"
                "If <branch> is specified, git rebase will perform an automatic"
                " git checkout <branch> before doing anything else. Otherwise i"
                "t remains on the current branch."));
   SetButtonFormattedToolTip(ui->btn_git_stash,
        QString("git-stash - Stash the changes in a dirty working directory "
                "away\n"
                "Use git stash when you want to record the current state of the"
                " working directory and the index, but want to go back to a cle"
                "an working directory. The command saves your local modificatio"
                "ns away and reverts the working directory to match the HEAD co"
                "mmit."));
   SetButtonFormattedToolTip(ui->btn_git_status,
        QString("git-status - Show the working tree status\n"
                "Displays paths that have differences between the index file an"
                "d the current HEAD commit, paths that have differences between"
                " the working tree and the index file, and paths in the working"
                " tree that are not tracked by Git (and are not ignored by giti"
                "gnore(5)). The first are what you would commit by running git "
                "commit; the second and third are what you could commit by runn"
                "ing git add before running git commit."));
}

MainWindow::~MainWindow()
{
   delete ui;
}

void MainWindow::ClearAll()
{
   ui->comboBox_stash->clear();
}

size_t MainWindow::GetProcessResults(const string& cmd, const string& execDir,
                                     const TStrVect& args, TStrVect& resultVect)
{
   // Setup
   resultVect.clear();
   QProcess proc;
   QStringList strList;
   for(TStrVectCItr itr = args.begin(); args.end() != itr; ++itr)
   {
      string str(*itr);
      strList.append(str.c_str());
   }
   if(0 < execDir.length())
   {
      proc.setWorkingDirectory(execDir.c_str());
   }

   // Execute
   proc.start(cmd.c_str(), strList);
//    proc.start(cmd.c_str(), QStringList() << args.c_str() << "-R");
   if (!proc.waitForStarted())
   {
      cout << "!proc.waitForStarted()" << endl;
      return 0;
   }

   proc.closeWriteChannel();
   if (!proc.waitForFinished())
   {
      cout << "!proc.waitForFinished()" << endl;
      return 0;
   }

   // Read results
   QByteArray result = proc.readAll();
   string tmpStr;
   tmpStr.clear();
   for(auto itr: result )
   {
      if('\n' != itr)
      {
         char c(itr);
         tmpStr += c;
      }
      else
      {
         resultVect.push_back(tmpStr);
         tmpStr.clear();
      }
   }
   return resultVect.size();
}

void MainWindow::OnGitStatus()
{
   ui->comboBox_stash->clear();
   const string cmd("git");
   const string execDir(ms_rootGitDir.filePath().toStdString());
   //git status --porcelain

   TStrVect args;
   args.push_back("status");
   args.push_back("--porcelain");
   TStrVect resultVect;
   GetProcessResults(cmd, execDir, args, resultVect);
   if(0 < resultVect.size())//Hello
   {
      /*
          2006> git status --porcelain
              AM qt/cs-ipc/Client.cpp
              AM qt/cs-ipc/EventMessage.cpp
              AM qt/cs-ipc/Server.cpp
              A  qt/cs-ipc/ZiegVersion.h
              AM qt/cs-ipc/cs-ipc.pro
              A  qt/cs-ipc/csipc/Client.h
              A  qt/cs-ipc/csipc/EventMessage.h
              A  qt/cs-ipc/csipc/Server.h
              A  qt/cs-ipc/csipc/defines.h
              AM qt/cs-ipc/internals.cpp
              A  qt/cs-ipc/internals.h
              AM qt/cs-ipc/main.cpp
              ?? qt/build-cs-ipc-Desktop_Qt_5_4_2_GCC_32bit-Debug/
              ?? qt/cs-ipc/cs-ipc.pro.user
              ?? qt/cs-ipc/ziegversion.sh
          [fred][~/dev/sandbox/cs-ipc][Sat Jul 02@19:31:13]
      */
      QRegularExpression reModified("AM (.+)");
      for(auto itr: resultVect)
      {
         const string str0(itr);
         const QString str(str0.c_str());
         QRegularExpressionMatch match = reModified.match(str);
         if(match.hasMatch())
         {
            cout << "Match: [" << match.captured(1).toStdString() << "]" << endl;
            ui->comboBox_stash->addItem(match.captured(1));
         }
      }
   }
}

const QFileInfo& MainWindow::getRootGitDir()
{
    return ms_rootGitDir;
}

void MainWindow::SetButtonFormattedToolTip(QAbstractButton *pCB,
                                        const QString& tooltip, int width)
{
    int len = tooltip.size();
    if(len <= width)
    {
        pCB->setToolTip(tooltip);
    }
    QString modToolTip(tooltip);
    int index(width);
    int floor(0);
    int ceiling(index);
    while(index < len)
    {
        bool hasCR(false);
        for(int i = index; floor < i; --i)
        {
//            if(QChar('\n') == modToolTip.at(i))
            if('\n' == modToolTip.at(i))
            {
                hasCR = true;
                index = i;
                break;
            }
        }
        if(!hasCR)
        {
            while((!modToolTip.at(index).isSpace()) && (floor < index))
            {
                --index;
            }
            if('\n' != modToolTip.at(index))
            {
                if(floor < index)
                {
                    modToolTip[index] = QChar('\n');
                }
                else
                {
                    modToolTip.insert(ceiling, QChar('\n'));
                    ++len;
                    index = ceiling;
                }
            }
        }
        floor = ++index;
        index += width;
        ceiling = index;
    }
    pCB->setToolTip(modToolTip);
//    cout << modToolTip.toStdString() << endl << endl;
}

const QString& MainWindow::ReadSettingFromFile(const QString& settingFileStr,
                                     const QString& altTextStr,
                                               QLabel* settingLabel,
                                               QString& settingStr)
{
    settingStr = altTextStr;
    QFile cfgFile(settingFileStr);
    if(cfgFile.exists())
    {
       if(cfgFile.open(QIODevice::ReadOnly | QIODevice::Text))
       {
          QByteArray line = cfgFile.readLine();
          settingStr = line;
//          ms_rootGitDir = QFileInfo(QString(line));
          settingLabel->setText(ms_rootGitDir.filePath());
       }
    }
    settingLabel->setText(settingStr);
    return settingStr;
}

void MainWindow::on_btn_choose_git_root_clicked()
{
   QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                 ms_rootGitDir.filePath(),
                 QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
   if(!dir.isNull())
   {
      ClearAll();
      ms_rootGitDir = dir;
      ui->label_git_root->setText(dir);
      QFile cfgFile(mc_cfgFileStr);
      if(cfgFile.open(QIODevice::WriteOnly | QIODevice::Text))
      {
         QTextStream out(&cfgFile);
         out << ms_rootGitDir.filePath();
         cfgFile.close();
      }
   }
}

void MainWindow::on_comboBox_stash_currentIndexChanged(const QString &arg1)
{
//    QMessageBox msgBox;
//    msgBox.setText(arg1);
//    msgBox.exec();
   QString sbMsg("Stash \"");
   sbMsg.append(arg1).append("\" chosen");
   ui->statusBar->showMessage(sbMsg);
//    ui->comboBox_stash->addItem("giddyup");
}

void MainWindow::on_btn_git_init_clicked()
{
   GitInitDialog initDlg;
   initDlg.exec();
}

void MainWindow::on_btn_git_clone_clicked()
{
    GitCloneDialog cloneDlg;
    cloneDlg.exec();
}

void MainWindow::on_btn_remote_repo_clicked()
{
    bool ok;
    QString text =
            QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                  tr("Remote repo URL:"), QLineEdit::Normal,
                                  ui->label_remote_repo->text(), &ok);
    if (ok && !text.isEmpty())
    {
        ui->label_remote_repo->setText(text);
        QFile cfgFile(mc_remoteRepoFileStr);
        if(cfgFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
           QTextStream out(&cfgFile);
           out << text;
           cfgFile.close();
        }
    }


//    if(!dir.isNull())
//    {
//       ClearAll();
//       ms_rootGitDir = dir;
//       ui->label_git_root->setText(dir);
//       QFile cfgFile(mc_cfgFileStr);
//       if(cfgFile.open(QIODevice::WriteOnly | QIODevice::Text))
//       {
//          QTextStream out(&cfgFile);
//          out << ms_rootGitDir.filePath();
//          cfgFile.close();
//       }
//    }

}

void MainWindow::on_btn_git_branch_clicked()
{
    TestDialog dlg;
    dlg.SetCommand("git", "branch");
    dlg.SetTitle("git-branch");
    dlg.AddCheckbox("--color[=<when>]", "Color branches to highlight current, "
                                        "local, and remote-tracking branches. T"
                                        "he value must be always (the default),"
                                        " never, or auto.",
                             0, 0, true);
    dlg.AddCheckbox("--no-color", "Turn off branch colors, even when the config"
                                  "uration file gives the default to color outp"
                                  "ut. Same as --color=never.", 0, 1);
    dlg.AddCheckbox("-r | --remotes",
                    "List or delete (if used with -d) the remote-tracking branc"
                    "hes.", 1, 0);
    dlg.AddCheckbox("-a | --all",
                    "List both remote-tracking branches and local branches.",
                    1, 1);
    dlg.AddCheckbox("--list", "Activate the list mode. git branch <pattern> wou"
                              "ld try to create a branch, use git branch --list"
                              " <pattern> to list matching branches.",
                    2, 0);
    dlg.AddCheckbox("-v | --verbose", "When in list mode, show sha1 and commit "
                                      "subject line for each head, along with r"
                                      "elationship to upstream branch (if "
                                      "any).", 3, 0);
    dlg.AddCheckbox("-vv", "When in list mode, show sha1 and commit subject lin"
                           "e for each head, along with relationship to upstrea"
                           "m branch (if any). If given twice, print the name o"
                           "f the upstream branch, as well (see also git remote"
                           " show <remote>).", 3, 1);
    dlg.AddCheckbox("--column[=<options>]",
                    "Display branch listing in columns. See configuration varia"
                    "ble column.branch for option syntax.--column and --no-colu"
                    "mn without options are equivalent to always and never resp"
                    "ectively. This option is only applicable in non-verbose mo"
                    "de.", 4, 0, true);
    dlg.AddCheckbox("--no-column",
                    "Display branch listing in columns. See configuration varia"
                    "ble column.branch for option syntax.--column and --no-colu"
                    "mn without options are equivalent to always and never resp"
                    "ectively. This option is only applicable in non-verbose mo"
                    "de.", 4, 1);
    dlg.AddCheckbox("--contains [<commit>]",
                    "Only list branches which contain the specified commit (HEA"
                    "D if not specified). Implies --list.", 5, 0, true);
    dlg.AddCheckbox("--merged [<commit>]",
                    "Only list branches whose tips are reachable from the speci"
                    "fied commit (HEAD if not specified). Implies --list.",
                    5, 1, true);
    dlg.AddCheckbox("--no-merged [<commit>]",
                    "Only list branches whose tips are not reachable from the s"
                    "pecified commit (HEAD if not specified). Implies --list.",
                    6, 0, true);
    dlg.AddCheckbox("--sort=<key>",
                    "Sort based on the key given. Prefix - to sort in descendin"
                    "g order of the value. You may use the --sort=<key> option "
                    "multiple times, in which case the last key becomes the pri"
                    "mary key. The keys supported are the same as those in git "
                    "for-each-ref. Sort order defaults to sorting based on the "
                    "full refname (including refs/... prefix). This lists detac"
                    "hed HEAD (if present) first, then local branches and final"
                    "ly remote-tracking branches.", 7, 0, true);
    dlg.AddCheckbox("--points-at <object>",
                    "Only list branches of the given object.", 7, 1, true);
    dlg.AddCheckbox("<pattern> ...",
                    "If a <pattern> is given, it is used as a shell wildcard to"
                    " restrict the output to matching branches. If multiple pat"
                    "terns are given, a branch is shown if it matches any of th"
                    "e patterns. Note that when providing a <pattern>, you must"
                    " use --list; otherwise the command is interpreted as branc"
                    "h creation.", 8, 0, true);
    dlg.AddCheckbox("-d | --delete",
                    "Delete a branch. The branch must be fully merged in its up"
                    "stream branch, or in HEAD if no upstream was set with --tr"
                    "ack or --set-upstream.", 9, 0);
    dlg.AddCheckbox("-D", "Shortcut for --delete --force.", 9, 1);
    dlg.AddCheckbox("-l | --create-reflog",
                    "Create the branch’s reflog. This activates recording of al"
                    "l changes made to the branch ref, enabling use of date bas"
                    "ed sha1 expressions such as \"<branchname>@{yesterday}\". "
                    "Note that in non-bare repositories, reflogs are usually en"
                    "abled by default by the core.logallrefupdates config optio"
                    "n.", 10, 0);
    dlg.AddCheckbox("-f | --force",
                    "Reset <branchname> to <startpoint> if <branchname> exists "
                    "already. Without -f git branch refuses to change an existi"
                    "ng branch. In combination with -d (or --delete), allow del"
                    "eting the branch irrespective of its merged status. In com"
                    "bination with -m (or --move), allow renaming the branch ev"
                    "en if the new branch name already exists.", 10, 1);
    dlg.AddCheckbox("-m | --move",
                    "Move/rename a branch and the corresponding reflog.",
                    11, 0);
    dlg.AddCheckbox("-M", "Shortcut for --move --force.", 11, 1);
    dlg.AddCheckbox("-q | --quiet",
                    "Be more quiet when creating or deleting a branch, suppress"
                    "ing non-error messages.", 12, 0);
    dlg.AddCheckbox("--abbrev=<length>",
                    "Alter the sha1’s minimum display length in the output list"
                    "ing. The default value is 7 and can be overridden by the c"
                    "ore.abbrev config option.", 13, 0, true);
    dlg.AddCheckbox("--no-abbrev",
                    "Display the full sha1s in the output listing rather than a"
                    "bbreviating them.", 13, 1);
    dlg.AddCheckbox("-t | --track",
                    "When creating a new branch, set up branch.<name>.remote an"
                    "d branch.<name>.merge configuration entries to mark the st"
                    "art-point branch as \"upstream\" from the new branch. This"
                    " configuration will tell git to show the relationship betw"
                    "een the two branches in git status and git branch -v. Furt"
                    "hermore, it directs git pull without arguments to pull fro"
                    "m the upstream when the new branch is checked out.\n\nThis"
                    " behavior is the default when the start point is a remote-"
                    "tracking branch. Set the branch.autoSetupMerge configurati"
                    "on variable to false if you want git checkout and git bran"
                    "ch to always behave as if --no-track were given. Set it to"
                    " always if you want this behavior when the start-point is "
                    "either a local or remote-tracking branch.", 14, 0);
    dlg.AddCheckbox("--no-track",
                    "Do not set up \"upstream\" configuration, even if the bran"
                    "ch.autoSetupMerge configuration variable is true.", 14, 1);
    dlg.AddCheckbox("--set-upstream",
                    "If specified branch does not exist yet or if --force has b"
                    "een given, acts exactly like --track. Otherwise sets up co"
                    "nfiguration like --track would when creating the branch, e"
                    "xcept that where branch points to is not changed.", 15, 0);
    dlg.AddCheckbox("-u <upstream> | --set-upstream-to=<upstream>",
                    "Set up <branchname>'s tracking information so <upstream> i"
                    "s considered <branchname>'s upstream branch. If no <branch"
                    "name> is specified, then it defaults to the current branch"
                    ".", 15, 1, true);
    dlg.AddCheckbox("--unset-upstream",
                    "Remove the upstream information for <branchname>. If no br"
                    "anch is specified it defaults to the current branch.",
                    16, 0);
    dlg.AddCheckbox("--edit-description",
                    "Open an editor and edit the text to explain what the branc"
                    "h is for, to be used by various other commands (e.g. forma"
                    "t-patch, request-pull, and merge (if enabled)). Multi-line"
                    " explanations may be used.", 17, 0);
    dlg.AddCheckbox("<branchname>",
                    "The name of the branch to create or delete. The new branch"
                    " name must pass all checks defined by git-check-ref-format"
                    "(1). Some of these checks may restrict the characters allo"
                    "wed in a branch name.", 18, 0, true);
    dlg.AddCheckbox("<start-point>",
                    "The new branch head will point to this commit. It may be g"
                    "iven as a branch name, a commit-id, or a tag. If this opti"
                    "on is omitted, the current HEAD will be used instead.",
                    18, 1);
    dlg.AddCheckbox("<oldbranch>",
                    "The name of an existing branch to rename.", 19, 0);
    dlg.AddCheckbox("<newbranch>",
                    "The new name for an existing branch. The same restrictions"
                    " as for <branchname> apply.", 19, 1);
    dlg.ExecuteLayout();
    dlg.exec();
}

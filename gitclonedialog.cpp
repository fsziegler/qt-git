#include "gitclonedialog.h"
#include "ui_gitclonedialog.h"
#include "mainwindow.h"
#include <QFileDialog>

GitCloneDialog::GitCloneDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GitCloneDialog)
{
    ui->setupUi(this);
    ui->lineEdit_directory->setText(MainWindow::getRootGitDir().filePath());
    setWindowTitle(QString("git clone"));

    ui->checkBox_local->setFocus();
    MainWindow::SetButtonFormattedToolTip(ui->checkBox_quiet,
                QString("Operate quietly. Progress is not reported to the "
                        "standard error stream."));
    MainWindow::SetButtonFormattedToolTip(ui->checkBox_bare,
                QString("Make a bare Git repository. That is, instead of creati"
                        "ng <directory> and placing the administrative files in"
                        " <directory>/.git, make the <directory> itself the $GI"
                        "T_DIR. This obviously implies the -n because there is "
                        "nowhere to check out the working tree. Also the branch"
                        " heads at the remote are copied directly to correspond"
                        "ing local branch heads, without mapping them to refs/r"
                        "emotes/origin/. When this option is used, neither remo"
                        "te-tracking branches nor the related configuration var"
                        "iables are created."));
    MainWindow::SetButtonFormattedToolTip(ui->checkBox_branch,
                QString("Instead of pointing the newly created HEAD to the bran"
                        "ch pointed to by the cloned repository’s HEAD, point t"
                        "o <name> branch instead. In a non-bare repository, thi"
                        "s is the branch that will be checked out. --branch can"
                        " also take tags and detaches the HEAD at that commit i"
                        "n the resulting repository."));
    MainWindow::SetButtonFormattedToolTip(ui->checkBox_config,
                QString("Set a configuration variable in the newly-created repo"
                        "sitory; this takes effect immediately after the reposi"
                        "tory is initialized, but before the remote history is "
                        "fetched or any files checked out. The key is in the sa"
                        "me format as expected by git-config(1) (e.g., core.eol"
                        "=true). If multiple values are given for the same key,"
                        " each value will be written to the config file. This m"
                        "akes it safe, for example, to add additional fetch ref"
                        "specs to the origin remote."));
    MainWindow::SetButtonFormattedToolTip(ui->checkBox_depth,
                QString("Create a shallow clone with a history truncated to the"
                        " specified number of commits. Implies --single-branch "
                        "unless --no-single-branch is given to fetch the histor"
                        "ies near the tips of all branches."));
    MainWindow::SetButtonFormattedToolTip(ui->checkBox_dissociate,
                QString("Borrow the objects from reference repositories specifi"
                        "ed with the --reference options only to reduce network"
                        " transfer, and stop borrowing from them after a clone "
                        "is made by making necessary local copies of borrowed o"
                        "bjects. This option can also be used when cloning loca"
                        "lly from a repository that already borrows objects fro"
                        "m another repository—​the new repository will borrow ob"
                        "jects from the same repository, and this option can be"
                        " used to stop the borrowing."));
    MainWindow::SetButtonFormattedToolTip(ui->checkBox_local,
                QString("When the repository to clone from is on a local machin"
                        "e, this flag bypasses the normal \"Gitaware\" transpor"
                        "t mechanism and clones the repository by making a copy"
                        " of HEAD and everything under objects and refs directo"
                        "ries. The files under .git/objects/ directory are hard"
                        "linked to save space when possible."));
    MainWindow::SetButtonFormattedToolTip(ui->checkBox_mirror,
                QString("Set up a mirror of the source repository. This implies"
                        " --bare. Compared to --bare, --mirror not only maps lo"
                        "cal branches of the source to local branches of the ta"
                        "rget, it maps all refs (including remote-tracking bran"
                        "ches, notes etc.) and sets up a refspec configuration "
                        "such that all these refs are overwritten by a git remo"
                        "te update in the target repository."));
    MainWindow::SetButtonFormattedToolTip(ui->checkBox_no_checkout,
                QString("No checkout of HEAD is performed after the clone is "
                        "complete."));
    MainWindow::SetButtonFormattedToolTip(ui->checkBox_no_hardlinks,
                QString("Force the cloning process from a repository on a local"
                        " filesystem to copy the files under the .git/objects d"
                        "irectory instead of using hardlinks. This may be desir"
                        "able if you are trying to make a back-up of your "
                        "repository."));
    MainWindow::SetButtonFormattedToolTip(ui->checkBox_origin,
                QString("Instead of using the remote name origin to keep track "
                        "of the upstream repository, use <name>."));
    MainWindow::SetButtonFormattedToolTip(ui->checkBox_progress,
                QString("Progress status is reported on the standard error stre"
                        "am by default when it is attached to a terminal, unles"
                        "s -q is specified. This flag forces progress status ev"
                        "en if the standard error stream is not directed to a t"
                        "erminal."));
    MainWindow::SetButtonFormattedToolTip(ui->checkBox_quiet,
                QString("Operate quietly. Progress is not reported to the "
                        "standard error stream."));
    MainWindow::SetButtonFormattedToolTip(ui->checkBox_recursive,
                QString("After the clone is created, initialize all submodules "
                        "within, using their default settings. This is equivale"
                        "nt to running git submodule update --init --recursive "
                        "immediately after the clone is finished. This option i"
                        "s ignored if the cloned repository does not have a wor"
                        "ktree/checkout (i.e. if any of --no-checkout/-n, --bar"
                        "e, or --mirror is given)"));
    MainWindow::SetButtonFormattedToolTip(ui->checkBox_reference,
                QString("If the reference repository is on the local machine, a"
                        "utomatically setup .git/objects/info/alternates to obt"
                        "ain objects from the reference repository. Using an al"
                        "ready existing repository as an alternate will require"
                        " fewer objects to be copied from the repository being "
                        "cloned, reducing network and local storage costs."));
    MainWindow::SetButtonFormattedToolTip(ui->checkBox_separate_git_dir,
                QString("Instead of placing the cloned repository where it is s"
                        "upposed to be, place the cloned repository at the spec"
                        "ified directory, then make a filesystem-agnostic Git s"
                        "ymbolic link to there. The result is Git repository ca"
                        "n be separated from working tree."));
    MainWindow::SetButtonFormattedToolTip(ui->checkBox_shared,
                QString("When the repository to clone is on the local machine, "
                        "instead of using hard links, automatically setup .git/"
                        "objects/info/alternates to share the objects with the "
                        "source repository. The resulting repository starts out"
                        " without any object of its own."));
    MainWindow::SetButtonFormattedToolTip(ui->checkBox_single_branch,
                QString("Clone only the history leading to the tip of a single "
                        "branch, either specified by the --branch option or the"
                        " primary branch remote’s HEAD points at. Further fetch"
                        "es into the resulting repository will only update the "
                        "remote-tracking branch for the branch this option was "
                        "used for the initial cloning. If the HEAD at the remot"
                        "e did not point at any branch when --single-branch clo"
                        "ne was made, no remote-tracking branch is created."));
    MainWindow::SetButtonFormattedToolTip(ui->checkBox_template,
                QString("Specify the directory from which templates will be "
                        "used"));
    MainWindow::SetButtonFormattedToolTip(ui->checkBox_uploadpack,
                QString("When given, and the repository to clone from is access"
                        "ed via ssh, this specifies a non-default path for the "
                        "command run on the other end."));
    MainWindow::SetButtonFormattedToolTip(ui->checkBox_verbose,
                QString("Run verbosely. Does not affect the reporting of progre"
                        "ss status to the standard error stream."));
}

GitCloneDialog::~GitCloneDialog()
{
    delete ui;
}

void GitCloneDialog::on_btn_directory_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                  MainWindow::getRootGitDir().filePath(),
                  QFileDialog::ShowDirsOnly
                  | QFileDialog::DontResolveSymlinks);
    if(!dir.isNull())
    {
        ui->lineEdit_directory->setText(dir);
    }
}

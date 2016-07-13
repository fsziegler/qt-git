# Power qt-git
A power git tool designed to support many major commands through a cross-platform UI.

This app has two fields for choosing the local and remote repos to use, and remembers the user's choice between sessions.

This app 
 * populates a dialog box for the major git commands, including 
  * init, 
  * clone, 
  * branch, 
  * checkout, 
  * merge, 
  * fetch, 
  * rebase, 
  * add, 
  * stash, 
  * commit, 
  * push, 
  * pull, 
  * diff, 
  * status, and 
  * log. 
 * The dialog box displays all options for that git command, 
  * grouped using colors, and 
  * if an option is checked its mutually exclusive options are disabled. 
 * If an option may or must have one or more parameters, the user is intelligently prompted for this. 
 * If the user selects \<OK\>, then that git command is executed.

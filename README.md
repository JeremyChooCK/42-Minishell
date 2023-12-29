Minishell Project
=================

Overview
--------

The Minishell project is an ambitious endeavor that replicates the functionalities of a Unix shell. It is designed to provide a deep understanding of system programming, focusing on the mechanics of Unix system calls, shell behaviors, and process management. The project aims to emulate a standard Unix shell with a range of features and capabilities.
Features
--------

Minishell includes several key features:

-   **Command Prompt:** Displays a user input prompt.
-   **Command History:** Maintains a record of executed commands.
-   **Executable Search and Launch:** Identifies and executes executables based on the PATH environment variable or specified paths.
-   **Signal Handling:** Manages Unix signals with minimal reliance on global variables.
-   **Quoting Mechanisms:** Differentiates between single and double quotes and special character handling.
-   **Redirections and Pipes:** Implements input/output redirections (`<`, `>`, `<<`, `>>`) and command chaining using pipes (`|`).
-   **Environment Variables:** Handles and expands environment variables.
-   **Exit Status Management:** Utilizes `$?` to display the exit status of the last executed foreground pipeline.
-   **Control Key Responses:** Mimics behaviors for ctrl-C, ctrl-D, and ctrl-.
-   **Built-in Commands:** Implements basic shell built-ins like `echo`, `cd`, `pwd`, `export`, `unset`, `env`, and `exit`.

Learning Outcomes
-----------------

-   **System Programming Skills:** Enhanced understanding of Unix system calls, including `fork`, `execve`, `pipe`, and various signal handling techniques.
-   **Process Management Knowledge:** Gained insights into process creation, execution, and synchronization within the Unix environment.
-   **Signal Handling Proficiency:** Developed a comprehensive understanding of handling Unix signals effectively.
-   **Command Parsing and Interpretation:** Acquired skills in parsing complex user inputs and interpreting shell commands and their parameters.
-   **Environment Variable Handling:** Learned the intricacies of managing and expanding environment variables in a Unix system.
-   **File Descriptor and Redirection Insight:** Understood the significance of file descriptors in managing input/output streams and implementing redirection.
-   **Error Handling Techniques:** Improved abilities in robust error handling and generating user-friendly error messages.

Project Workflow
----------------

1.  **Read Prompt:** Initiates interaction by displaying a command prompt for user input.
2.  **Signal Checking:** Monitors and handles Unix signals such as ctrl-C and ctrl-D.
3.  **Pipe Analysis:** Checks for pipe characters in the input to determine command chaining requirements.
4.  **Quote Parsing:** Parses the input for single and double quotes, affecting command interpretation.
5.  **Redirection Handling:** Identifies redirection symbols and sets up necessary input/output streams.
6.  **Command Execution:** Utilizes `fork` to create a child process for command execution, with the parent process waiting for its completion.

Below is a flow chart of the program and how it works
<img src="img/minishell flowchart.png"></a>

## Tester

https://github.com/LucasKuhn/minishell_tester

it is currently integrated in the pipeline

## Functions used

readline, rl_on_new_line, rl_replace_line, rl_redisplay,
add_history, printf, malloc, free, write, access, open, read,
close, fork, waitpid, signal, exit, getcwd, chdir, stat, unlink,
execve, dup2, pipe, perror, getenv, ft_setenv

Conclusion
----------

The Minishell project is a comprehensive exercise in understanding and implementing core functionalities of a Unix shell. It serves as a practical platform for learning system programming, process management, and the nuanced operations of a Unix-like environment. This project is an invaluable asset for anyone looking to deepen their understanding of Unix systems and shell programming.

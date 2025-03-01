/*
 *  Project B: Inter-Process Communication (IPC)
 *  Name: Brien Kim
 *  Course: CS 3502 Section W03
 *  NetID: bkim50
 *
 *  Overview: this file is a main task for Project B: Inter-Process Communication (IPC)
 *
 *  Although C language may be a better option for pipe (|) implementation for IPC,
 *  C++ language was kept used, since it was also used in Project A.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <csignal>
#include <iostream>
#include <chrono>
#include <cstdlib>
#include <sstream>
#include <vector>

using namespace std;

// a simple program to function similar to "grep" command
class CustomGrep
{
private:
    static string seeking_file;

public:
    CustomGrep() = default;

    CustomGrep(string file)
    {
        seeking_file = file;
    }

    // a simple custom "grep" function that identifies a file(s), whose name contains "text", from processed output of "ls -R ~/[path]" command
    static void custom_grep(string files)
    {
        string file;
        char delimiter = ',';
        stringstream stream(files);
        vector<string> sought_files;

        // separate files by comma (,) to store each individual file into a vector
        while (getline(stream, file, delimiter))
        {
            // IF the seeking data is a substring of or equal to files, THEN store it into a vector
            if (file.find(seeking_file) != string::npos)
            {
                sought_files.push_back(file);
            }
        }

        // IF nothing is found, THEN terminate the program
        if (sought_files.size() == 0)
        {
            cout << "\nFile does not found\n"
                 << endl;
            return;
        }

        cout << "\nRESULT:\n"
             << endl;

        // display identified file and directory
        for (string file : sought_files)
        {
            cout << file << endl;
        }

        // break a new line
        cout << "\n"
             << endl;
    }
};

/*
    Adapted from: "Execute a command and get both output and exit status in C++ (Windows & Linux)" by Remy van Elst,
                    07-06-2021, https://raymii.org/s/articles/Execute_a_command_and_get_both_output_and_exit_code.html
    Modified the function to be suitable for this project

    return the processed output of "ls ~/[path]" command
*/
string process_ls_output(string command, int BUFFER_SIZE)
{
    /*
        popen function creates a pipe and executes a shell command within the process
        for a mode "r", read the output from the command
    */
    FILE *pipe = popen(command.c_str(), "r");
    char buffer[BUFFER_SIZE];
    string output = "";

    if (!pipe)
    {
        fprintf(stderr, "Failed to execute the command\n");
        exit(0);
    }

    while (fgets(buffer, BUFFER_SIZE, pipe) != nullptr)
    {
        output += buffer;
    }

    pclose(pipe);

    string files;
    stringstream processed(output);
    string file;

    while (getline(processed, file))
    {
        files += file + ",";
    }

    return files;
}

/* IPC implementation using a pipe (|) */

const int BUFFER_SIZE = 4096;
const int READ_END = 0;
const int WRITE_END = 1;

// collect a start time for the program
auto time_start = chrono::high_resolution_clock::now();

// implement Ordinary Pipe and Producer-Consumer pattern
void pipeline(string parent_process, CustomGrep child_process)
{
    // parent process produces an output by executing "ls ~/[path]" command
    string output = process_ls_output(parent_process.c_str(), BUFFER_SIZE);

    // IF the size of the output exceeds the buffer size, THEN terminate the program
    if (output.length() > BUFFER_SIZE)
    {
        fprintf(stderr, "\nERROR: Buffer Overflow\n");
        return;
    }

    // SIGPIE: a signal, which notifies a process that the pipe it is attempting to write to is closed
    // It terminates the process by default, since this occurrence may be considered as a broken pipe
    // To gracefully handle such error, first ignore the SIGPIE
    signal(SIGPIPE, SIG_IGN);

    char write_msg[BUFFER_SIZE];
    char read_msg[BUFFER_SIZE];

    strcpy(write_msg, output.c_str());

    // file descriptors (have a parent-child relationship)
    // - write by the parent to its write end of the pipe: fd[1]
    // - read by the child from its read end of the pipe: fd[0]
    int fd[2];

    // create the pipe
    if (pipe(fd) == -1)
    {
        fprintf(stderr, "\nPipe Creation Failed\n");
        return;
    }

    // fork a child process
    pid_t pid = fork();

    // IF failed to create a child process, THEN terminate the program
    if (pid < 0)
    {
        fprintf(stderr, "\nFork Failed\n");
        return;
    }

    /* Parent process (Producer)*/
    if (pid > 0)
    {
        printf("Parent Process PID: %d\n", getpid());

        // close the un-used end of the pipe
        close(fd[READ_END]);

        // write to the pipe
        ssize_t write_byte = write(fd[WRITE_END], write_msg, strlen(write_msg) + 1);

        // handle errors gracefully
        if (write_byte == -1)
        {
            // EPIPE: "broken pipe" error
            if (errno == EPIPE)
            {
                cerr << "\n\"BROKEN PIPE\" ERROR: " << strerror(errno) << endl;
                return;
            }
            // EAGAIN: "resource temporarily unavailable" error
            // it occurs when an operation is attempted on non-blocking object
            else if (errno == EAGAIN)
            {
                cerr << "\n\"RESOURCE TEMPORARILY UNAVAILABLE\" ERROR: " << strerror(errno) << endl;
                return;
            }
            // EINTR: "interrupted system call" error
            else if (errno == EINTR)
            {
                cerr << "\n\"INTERRUPTED SYSTEM CALL\" ERROR: " << strerror(errno) << endl;
                return;
            }
            // EBADF: "bad file descriptor" error
            else if (errno == EBADF)
            {
                cerr << "\n\"BAD FILE DESCRIPTOR\" ERROR: " << strerror(errno) << endl;
                return;
            }
            // ENOMEM: "cannot allocate memory" error
            else if (errno == ENOMEM)
            {
                cerr << "\n\"CANNOT ALLOCATE MEMORY\" ERROR: " << strerror(errno) << endl;
                return;
            }
            else
            {
                cerr << "\nERROR: " << strerror(errno) << endl;
                return;
            }
        }

        printf("WRITE: %s\n\n", write_msg);

        // close the write end of the pipe
        close(fd[WRITE_END]);
    }
    /* Child process (Consumer)*/
    else
    {
        printf("Child Process PID: %d\n", getpid());

        // close the un-used end of the pipe
        close(fd[WRITE_END]);

        // read from the pipe
        ssize_t read_byte = read(fd[READ_END], read_msg, BUFFER_SIZE);

        // handle errors gracefully
        if (read_byte == -1)
        {
            // EPIPE: "broken pipe" error
            if (errno == EPIPE)
            {
                cerr << "\n\"BROKEN PIPE\" ERROR: " << strerror(errno) << endl;
                return;
            }
            // EAGAIN: "resource temporarily unavailable" error
            // it occurs when an operation is attempted on non-blocking object
            else if (errno == EAGAIN)
            {
                cerr << "\n\"RESOURCE TEMPORARILY UNAVAILABLE\" ERROR: " << strerror(errno) << endl;
                return;
            }
            // EINTR: "interrupted system call" error
            else if (errno == EINTR)
            {
                cerr << "\n\"INTERRUPTED SYSTEM CALL\" ERROR: " << strerror(errno) << endl;
                return;
            }
            // EBADF: "bad file descriptor" error
            else if (errno == EBADF)
            {
                cerr << "\n\"BAD FILE DESCRIPTOR\" ERROR: " << strerror(errno) << endl;
                return;
            }
            // ENOMEM: "cannot allocate memory" error
            else if (errno == ENOMEM)
            {
                cerr << "\n\"CANNOT ALLOCATE MEMORY\" ERROR: " << strerror(errno) << endl;
                return;
            }
            else
            {
                cerr << "\nERROR: " << strerror(errno) << endl;
                return;
            }
        }

        printf("READ: %s\n", read_msg);

        // close the read end of the pipe
        close(fd[READ_END]);

        // collect a end time for the program
        auto time_end = chrono::high_resolution_clock::now();

        // calculate estimated excution time for the program
        auto execution_time = chrono::duration_cast<chrono::milliseconds>(time_end - time_start);

        // display the estimated execution time
        cout << "\nEstimated Data Transmission Time: " << execution_time.count() << " milliseconds (ms)\n"
             << endl;

        // child process produces an output with read message
        child_process.custom_grep(read_msg);
    }

    return;
}

string CustomGrep::seeking_file;

// the main function to execute the program
int main()
{

    string directory;

    // prompt a name of a directory
    cout << "\nThis pipeline is for two specific processes: \"ls\" and \"CustomGrep\".\n\nPlease type a name of a directory: ls ~/";

    cin >> directory;

    string seeking_file;
    // prompt a name of a file seeking for
    cout << "\nPlease type a name of a file or directory: ";
    cin >> seeking_file;

    // break a new line
    cout << "\n"
         << endl;

    string command = "ls ~/" + directory;

    pipeline(command, CustomGrep(seeking_file));

    return 0;
}
/* ---- FETCH.C MODULE ----
* fetch() function:
*   Fetch the content of the given url by running 'curl' as a child
*   process and reading the response from a pipe. The response is
*   returned as a malloc'ed string, or NULL if there was an error.
*/

// #include <quiz.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

char* fetch(char* url) {
    // Allocate pipe in kernel
    int pfd[2];
    if (pipe(pfd) == -1) {
        perror("pipe");
        return NULL;
    }

    // Fork for a child process
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return NULL;
    }

    // Child process
    if (pid == 0) {
        // Close pipe's read end
        close(pfd[0]);

        // Redirect stdout to pipe's write end
        dup2(pfd[1], STDOUT_FILENO);

        // Fetch data from URL by running curl in execl()
        execl("/usr/bin/curl", "curl", "-s", url, NULL);
        perror("execl");
        return NULL;
    } else {                        // Parent process
        close(pfd[1]);              // Close pipe's write end

        // Read the response using a buffer
        char buf[1024];
        char* response;
        size_t res_len = 0;
        ssize_t data;

        while ((data = read(pfd[0], buf, sizeof(buf))) > 0) {
            size_t buf_len = strlen(buf);

            // Malloc size for the response string
            response = malloc((res_len + buf_len + 1) * sizeof *response);
            if (response == NULL) {
                perror("malloc");
                return NULL;
            }

            // Copy the characters read from buffer to response
            memcpy(response + res_len, buf, buf_len + 1);

            // Increment length of response with length of buffer
            res_len += buf_len;
        }

        // Handle errors for read()
        if (data < 0) {
            perror("read");
            free(response);
            return NULL;
        }

        // Terminate the response with null character
        if (res_len > 0)
            response[res_len] = '\0';

        return response;
    }
}
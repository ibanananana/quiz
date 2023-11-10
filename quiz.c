/* ---- QUIZ.C ----
* Play the quiz in a loop until either the user did end the
* standard input or the user pressed CTRL-C to interrupt the
* program.
*/

#include "quiz.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

volatile sig_atomic_t interrupt = 0;

// Signal handler for CTRL-D
void handle_interrupt(int signum) {
    interrupt = 1;
}

int main() {
    // Print some stuff
    printf("Answer multiple choice questions about computer science.\n");
    printf("You score points for each correctly answered question.\n");
    printf("If you need multiple attempts to answer a question, the\n");
    printf("points you score for a correct answer go down.\n\n");

    // Set up signal handler for ^D
    signal(SIGINT, handle_interrupt);

    // Construct our Quiz
    quiz_t the_game;
    the_game.max = 0;
    the_game.score = 0;
    the_game.n = 0;

    // Loop until user ends stdin or presses ^D
    while (!interrupt) {
        // Increment question number
        the_game.n++;

        // Play quiz
        if (play(&the_game) == 0) {
            // Clean up dynamically allocated memory
            free(the_game.question);
            free(the_game.answer);
            for (int i = 0; i < 4; i++)
                free(the_game.choices[i]);
        } else // Exit loop during error
            break;
    }

    // Print final comments
    printf("\nThanks for playing today.\n");
    printf("You viewed %u question/s today.\n", the_game.n);
    printf("Your final score is %u/%u points.\n", the_game.score, the_game.max);

    return 0;
}
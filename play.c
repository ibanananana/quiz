/* ---- PLAY.C MODULE ----
* play() function:
*   Play one round of the quiz game by first fetching and parsing a
*   quiz question and then interacting with the user. The function
*   returns 0 or -1 if there was an error.
*/

#include "quiz.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* URL = "https://opentdb.com/api.php?amount=1&category=18&type=multiple";

int play(quiz_t *quiz) {
    // Fetch and parse a quiz question
    char* response = fetch(URL);
    if (response == NULL) {
        fprintf(stderr, "Error on fetching URL.");
        free(response);
        return -1;
    }

    if (parse(quiz, response) == -1) {
        fprintf(stderr, "Error on parsing JSON input.");
        free(response);
        return -1;
    }

    // Remember to free dynamically allocated response
    free(response);

    // Initialize max score and points
    quiz->max += 8;
    unsigned points = 8;
    
    // Handle errors for invalid quiz, missing question, or missing answer
    if (!quiz || !quiz->question || !quiz->answer)
        return -1; // Invalid quiz or missing question/answer

    // Print question and choices
    printf("%u. %s\n\n", quiz->n, quiz->question);
    for (int i = 0; i < 4; i++)
        printf("[%c] %s\n", 'a' + i, quiz->choices[i]);

    // USER INTERACTION: Format based on Problem Sheet
    printf("\n(%dpt) > ", points);

    char answer[2];
    if (scanf("%1s", answer) != 1) {
        printf("Invalid input, please only enter a, b, c, or d.\n");
        return -1;          // Error reading user input
    }

    // Check if character is a, b, c, or d
    if (answer[0] >= 'a' && answer[0] <= 'd') {
        // As long as answer is wrong
        while (strcmp(quiz->choices[answer[0] - 'a'], quiz->answer) != 0) {
            printf("Answer [%c] is wrong, try again.\n", answer[0]);
            points -= 2;
            printf("(%upt) > ", points);
            if (scanf("%1s", answer) != 1) {
                printf("Invalid input, please only enter a, b, c, or d.\n");
                return -1; // Error reading user input
            }
        }

        // Increment score and print feedback
        quiz->score += points;
        printf("Congratulations, answer [%c] is correct! Your current score is %u/%u points.\n\n", answer[0], quiz->score, quiz->max);
    } else {
        // Invalid input
        printf("Invalid input, please only enter a, b, c, or d.\n");
        return -1;
    }
    
    return 0;
}
/* ---- PARSE.C MODULE ----
* parse() function:
*   Parse a JSON encoded msg and fill the next question into the
*   quiz_t. Use a JSON parsing library (e.g., jansson or json-c). The
*   function returns 0 or -1 if there was a parsing error.
*/

#include "quiz.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <json-c/json.h>

void shuffle(char **array, size_t n)
{   // Implementation from https://benpfaff.org/writings/clc/shuffle.html
    if (n > 1) {
        size_t i;
        for (i = 0; i < n - 1; i++) {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          char *t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

int parse(quiz_t *quiz, char *msg) {
    // Initialize JSON objects
    struct json_object *parsed, *results, *ques_obj, *choices;
    
    // Parse JSON result 
    parsed = json_tokener_parse(msg);

    // Get "results" part of the string
    if (json_object_object_get_ex(parsed, "results", &results)) {
        // Assuming there is only one question in the results array
        ques_obj = json_object_array_get_idx(results, 0);

        // Get "question" and "correct_answer" parts of "results"
        const char *question = json_object_get_string(json_object_object_get(ques_obj, "question"));
        const char *correct_answer = json_object_get_string(json_object_object_get(ques_obj, "correct_answer"));
        
        // Dynamically allocate memory for quiz members
        quiz->question = strdup(question);
        quiz->answer = strdup(correct_answer);
        
        // Get the incorrect choices and store in the choices array
        choices = json_object_object_get(ques_obj, "incorrect_answers");
        for (int i = 0; i < 3; i++) {
            quiz->choices[i] = strdup(json_object_get_string(json_object_array_get_idx(choices, i)));
        }

        // Add the correct answer to the choices array
        quiz->choices[3] = strdup(correct_answer);

        // Shuffle choices array
        shuffle(quiz->choices, 4);

        // Free JSON object
        json_object_put(parsed);

        // Parsing success
        return 0;
    }

    // Parsing failed
    return -1;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
    SIGN_GREATER_IDX = 0,
    SIGN_LESS_IDX,
    SIGN_EQUAL_IDX,
    WORD_GREATER_IDX,
    WORD_LESS_IDX,
    WORD_EQUAL_IDX,
    LOWER_BORDER = 0,
    UPPER_BORDER = 1000,
    MAX_ATTEMPT_NUMBER = 10
};

char *keywords[] = {
    [SIGN_GREATER_IDX] = ">",
    [SIGN_LESS_IDX] = "<",
    [SIGN_EQUAL_IDX] = "=",
    [WORD_GREATER_IDX] = "greater",
    [WORD_LESS_IDX] = "less",
    [WORD_EQUAL_IDX] = "equal"
};


int main(int argc, char* argv[]) {
    printf("The program will try to guess the number from %d to %d.\n", LOWER_BORDER, UPPER_BORDER);
    printf(
        "Use '%s'/'%s' (or '%s'/'%s') to give a clue if your number is greater or less.\n", 
        keywords[WORD_GREATER_IDX], 
        keywords[WORD_LESS_IDX], 
        keywords[SIGN_GREATER_IDX], 
        keywords[SIGN_LESS_IDX]
    );
    printf("Use '%s'(or '%s') if number is guessed.\n", keywords[WORD_EQUAL_IDX], keywords[SIGN_EQUAL_IDX]);

    int lowb = LOWER_BORDER, highb = UPPER_BORDER, attempt = 1;

    for (; attempt <= MAX_ATTEMPT_NUMBER; ++attempt) {
        char temp[255] = "";
        int target = lowb + (highb - lowb) / 2;

        printf("%4d attempt: %4d\nclue: ", attempt, target);
        fgets(temp, 255, stdin);

        if(strstr(temp, keywords[SIGN_GREATER_IDX]) || strstr(temp, keywords[WORD_GREATER_IDX]))
            lowb = target;
        else if(strstr(temp, keywords[SIGN_LESS_IDX]) || strstr(temp, keywords[WORD_LESS_IDX]))
            highb = target;
        else if(strstr(temp, keywords[SIGN_EQUAL_IDX]) || strstr(temp, keywords[WORD_EQUAL_IDX]))
            break;
        else {
            --attempt;
            continue;
        }
    }

    if(attempt > MAX_ATTEMPT_NUMBER)
        puts("out of attempts");

    puts("program complete");

    return 0;
}
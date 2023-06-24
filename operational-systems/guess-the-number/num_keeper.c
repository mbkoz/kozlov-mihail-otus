#include <stdio.h>
#include <time.h>
#include <stdlib.h>

enum {
    UPPER_BORDER = 1000,
    MAX_ATTEMPT_NUMBER = 10
};

int main(int argc, char* argv[]) {
    srand(time(NULL));

    int number = rand() % (UPPER_BORDER + 1);
    int attempt = 1;

    puts("Gues the number.");
    printf("It's between 0 and %d, you have %d attempts.\n", UPPER_BORDER, MAX_ATTEMPT_NUMBER);

    for(; attempt <= MAX_ATTEMPT_NUMBER; ++attempt){
        int temp = 0;
        printf("%4d attempt: ", attempt);
        scanf("%d", &temp);
        
        if(temp < number)
            puts("it's greater");
        else if(temp > number)
            puts("it's less");
        else
            break;
    }

    if(attempt > MAX_ATTEMPT_NUMBER)
        printf("you are out of attemts, number was: %4d\n", number);
    else
        puts("you are win!");

    return 0;
}

#include <stdio.h>
#include <stdlib.h>

int main()
{
    char word[40];
    printf("Enter a word:\n");
    fgets(word, 40, stdin);
    //scanf(" %s", word);
    printf("The word you wrote was:\t%s", word);
    return 0;
}
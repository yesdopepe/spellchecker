#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// bir kelimenin en buyuk uzunlugu
#define MAX_LENGTH 100

// bu fonksiyon correct_words.txt dosyasindaki kelimeleri ayirir ve bir dizi haline getirir
void loadWordsFromFile(char filename[], char words[][MAX_LENGTH], int *numWords) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("dosya acilmadi");
        exit(EXIT_FAILURE);
    }

    *numWords = 0;
    while (fscanf(file, "%[^,],", words[*numWords]) == 1) {
        (*numWords)++;
    }

    fclose(file);
}

// bu fonksiyon levenshtein mesafe teoremini kllanarak kelimelerin arasaindaki benzerligi bulur
int levenshteinDistance(char *s1, char *s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);

    int dp[len1 + 1][len2 + 1];

    for (int i = 0; i <= len1; i++) {
        for (int j = 0; j <= len2; j++) {
            if (i == 0)
                dp[i][j] = j;
            else if (j == 0)
                dp[i][j] = i;
            else if (s1[i - 1] == s2[j - 1])
                dp[i][j] = dp[i - 1][j - 1];
            else
                dp[i][j] = 1 + (dp[i - 1][j] < dp[i][j - 1] ? dp[i - 1][j] : dp[i][j - 1]);
        }
    }

    return dp[len1][len2];
}

// bu fonksiyon cumledeki noktalama isaretlerini kaldirir, bunun sebebi de cumleyi duzeltirken karsiklik olusturmamasi icin 
void removePunctuation(char word[]) {
    int len = strlen(word);
    for (int i = 0; i < len; i++) {
        if (ispunct(word[i])) {
            memmove(&word[i], &word[i + 1], len - i);
            len--;
            i--;
        }
    }
}



// bu fonksiyon da levenshtein mesafe teoreminin sonucuna gore cumlelerin benzer olup olmadigini bulur
void findSimilarWord(char incorrectWord[], char words[][MAX_LENGTH], int numWords, char *similarWord) {
    int minDistance = MAX_LENGTH; // Initialize to a large value

    for (int i = 0; i < numWords; i++) {
        int distance = levenshteinDistance(incorrectWord, words[i]);
        if (distance < minDistance) {
            minDistance = distance;
            strcpy(similarWord, words[i]);
        }
    }
}
//bu fonksiyon kelimelerin dogru olup olmadigini blur
int isWordCorrect(char word[], char words[][MAX_LENGTH], int numWords) {
    for (int i = 0; i < numWords; i++) {
        if (strcmp(word, words[i]) == 0) {
            return 1; // Word is correct
        }
    }
    return 0; // Word is incorrect
}
//bu fonksiyon diger fonksiyonlardan elde ettigimiz benzer olup olmamasi dogru olup olmamasi givi durumlara gore cumleyi duzenler ve dogru halini yazar
void correctSentence(char sentence[], char words[][MAX_LENGTH], int numWords) {
    char *token = strtok(sentence, " ");
    char correctedSentence[MAX_LENGTH] = ""; // Initialize an empty string

    while (token != NULL) {
        removePunctuation(token);

        if (!isWordCorrect(token, words, numWords)) {
            char similarWord[MAX_LENGTH];
            findSimilarWord(token, words, numWords, similarWord);
            int distance = levenshteinDistance(token, similarWord);

            // burada bir levenshtein mesafesi belirlenir(degistirebilirsiniz)
            if (distance <= 2) {
                strcat(correctedSentence, similarWord);
            } else {
                strcat(correctedSentence, token);
            }
        } else {
            strcat(correctedSentence, token);
        }

    
        strcat(correctedSentence, " ");

        token = strtok(NULL, " ");
    }

    correctedSentence[strlen(correctedSentence) - 1] = '\0';

    printf("dogru cumle: %s\n", correctedSentence);
}



int main() {
    char filename[] = "correct_words.txt";
    char words[1000][MAX_LENGTH];
    int numWords;

    loadWordsFromFile(filename, words, &numWords);

    char inputSentence[MAX_LENGTH];
    printf("bir cumle girin: ");
    fgets(inputSentence, sizeof(inputSentence), stdin);

    inputSentence[strcspn(inputSentence, "\n")] = 0;

    correctSentence(inputSentence, words, numWords);

    return 0;
}

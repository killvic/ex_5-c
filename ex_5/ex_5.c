#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h> 

typedef struct Symbol {
	char symbol;
	struct Symbol* next;
} Symbol;

typedef struct {
	Symbol *head, *last;
} String;

typedef struct Word {
	String* translations;
	struct Word* next;
} Word;

typedef struct Dictionary {
	String* languages;
	int numOfLanguages;
	Word *wordHead, *wordLast;
	struct Dictionary* next;
} Dictionary;

typedef struct {
	Dictionary *head, *last;
} Dictionaries;

void PrintWord(String* input) {
	Symbol* iterator = input->head;
	
	while (iterator != NULL) {
		printf("%c", iterator->symbol);
		iterator = iterator->next;
	}
}

void PrintList(String* input, int amount) {
	for (int i = 0; i < amount; i++) {
		PrintWord(&(input[i]));
		if (i != amount - 1)
			printf(",");
	}
}

int CompareWords(String* word1, String* word2) { // compares two strings
	Symbol* iterator = word1->head;
	Symbol* iterator2 = word2->head;

	while (iterator != NULL && iterator2 != NULL) {
		if (iterator->symbol != iterator2->symbol)
			return 0;
		iterator = iterator->next;
		iterator2 = iterator2->next;
	}

	if (iterator == NULL && iterator2 == NULL)
		return 1;
	else
		return 0;
}

void PrintAll(Dictionaries* dics) { // prints all dictionaries, for debug
	Dictionary* iterator = dics->head;
	Word* iteratorWord;
	int i = 0;

	while (iterator != NULL) {
		printf("%d: ", i++);
		printf("Languages: ");
		PrintList(iterator->languages, iterator->numOfLanguages);
		printf("\nNum of languages: %d", iterator->numOfLanguages);
		printf("\nWord: ");
		iteratorWord = iterator->wordHead;

		while (iteratorWord != NULL) {
			printf("[");
			PrintList(iteratorWord->translations, iterator->numOfLanguages);
			printf("]");
			iteratorWord = iteratorWord->next;
		}
		printf("\nNext %d", iterator->next);
		printf("\n");

		iterator = iterator->next;
	}
}

void FreeSymbols(Symbol* symbol) {
	if (symbol == NULL) {
		return;
	}
	if (symbol->next == NULL) {
		free(symbol);
		return;
	}
	FreeSymbols(symbol->next);
	free(symbol);
}

void freeStrings(String* strings, int langNum) {
	if (strings == NULL) {
		return;
	}
	for (int i = 0; i < langNum; i++) {
		FreeSymbols(strings[i].head);
	}
	free(strings);
}

void FreeWords(Word* word, int langNum) {
	if (word == NULL) {
		return;
	}
	if (word->next == NULL) {
		freeStrings(word->translations, langNum);
		free(word);
		return;
	}
	freeStrings(word->translations, langNum);
	FreeWords(word->next, langNum);
	free(word);
}

void FreeAll(Dictionary* dic) { // clears all dictionaries
	if (dic == NULL) {
		return;
	}
	if (dic->next == NULL) {
		freeStrings(dic->languages, dic->numOfLanguages);
		FreeWords(dic->wordHead, dic->numOfLanguages);
		free(dic);
		return;
	}
	FreeAll(dic->next);
	freeStrings(dic->languages, dic->numOfLanguages);
	FreeWords(dic->wordHead, dic->numOfLanguages);
	free(dic);
}

int Reader(String** output) { // reader function that can take string type input
	char c = 0;
	int counter = 0;
	String tmp = {NULL, NULL};
	Symbol* iterator;

	while (scanf("%c", &c)) {  //read the words from user
		Symbol* reader = (Symbol*)malloc(sizeof(Symbol));

		if (c == '\n') { 
			counter++;
			break;
		}
		if (c == ',') counter++; //counts amount of words in string

		reader->symbol = c;
		reader->next = NULL;
		if (tmp.head == NULL) {
			tmp.head = reader;
			tmp.last = reader;
		}
		else {
			tmp.last->next = reader;
			tmp.last = reader;
		}	
	}
	if (tmp.head == NULL) {
		*output = NULL;
		return 0;
	}

	iterator = tmp.head;
	
	*output = (String*)malloc(counter * sizeof(String));

	for (int i = 0; i < counter; i++) {
		String* word = (String*)malloc(sizeof(String));
		word->head = NULL;
		word->last = NULL;

		while (iterator != NULL) {
			if (word->head == NULL) {
				word->head = iterator;
				word->last = iterator;
			}
			else {
				word->last->next = iterator;
				word->last = iterator;
			}
			
			iterator = iterator->next;
			if (iterator == NULL) break;
			if (iterator->symbol == ',') {
				word->last->next = NULL;
				iterator = iterator->next;
				break;
			}
		}
		(*output)[i] = *word;
	}
	return counter;
}

void AddNewDictionary(Dictionaries* list) { // adds a dictionary
	printf("Define a new dictionary:\n");

	Dictionary* dic1 = (Dictionary*)malloc(sizeof(Dictionary));
	dic1->next = NULL;
	dic1->wordHead = NULL;
	dic1->wordLast = dic1->wordHead;
	dic1->numOfLanguages = Reader(&dic1->languages);
	if (dic1->numOfLanguages == 0) {
		printf("The creation of the dictionary has failed!\n");
	} 
	else {
		if (list->head == NULL) {
			list->head = dic1;
			list->last = dic1;
		}
		else {
			list->last->next = dic1;
			list->last = dic1;
		}
		printf("The dictionary has been created successfully!\n");
	}
}

void AddWordToDictionary(Dictionaries* list) { // adds word to a dictionary
	printf("Chose a dictionary:\n");
	Dictionary* iterator = list->head;
	int i = 1, option = 0;

	while (iterator != NULL) { // prints available dictionaries
		printf("%d. ", i);
		PrintList(iterator->languages, iterator->numOfLanguages);
		iterator = iterator->next;
		printf("\n");
		i++;
	}
	iterator = list->head;
	i = 1;

	scanf("%d", &option);
	printf("Enter a word in "); // takes user input
	while (iterator != NULL) {
		if (i == option) {
			PrintList(iterator->languages, iterator->numOfLanguages);
			break;
		} 
		iterator = iterator->next;
		i++;
	}
	printf("\n");
	Word* newWord = (Word*)malloc(sizeof(Word));
	newWord->next = NULL; // previous scanf leaves '\n' in buffer 
	getchar();
	if (Reader(&newWord->translations) == 0) {
		printf("The addition of the word has failed!\n");
		return;
	}
	else {
		if (iterator->wordHead == NULL) {
			iterator->wordHead = newWord;
			iterator->wordLast = newWord;
			iterator->wordHead->next = NULL;
		}
		else {
			iterator->wordLast->next = newWord;
			iterator->wordLast = newWord;
		}
		printf("The word has been added successfully!\n");
	}	 
}

void DeleteWordFromDictionaty(Dictionaries* list) { // function for deleting words in dictionary
	Dictionary* iterator = list->head;
	Word* iterator2 = NULL;
	String* newWord = (String*)malloc(sizeof(String));
	Word* lastWord = NULL;
	int i = 1, option = 0;
	char aproval;

	printf("Chose a dictionary:\n");
	while (iterator != NULL) { // prints available dictionaries
		printf("%d. ", i);
		PrintList(iterator->languages, list->head->numOfLanguages);
		iterator = iterator->next;
		printf("\n");
		i++;
	}
	iterator = list->head;
	i = 1;
	scanf("%d", &option);
	printf("Enter a word in "); // takes input from user
	while (iterator != NULL) {
		if (i == option) {
			PrintList(iterator->languages, iterator->numOfLanguages);
			break;
		}
		iterator = iterator->next;
		i++;
	}
	printf("\n");
	getchar();

	Reader(&newWord);
	printf("Are you sure? (y/n)\n");
	scanf("%c", &aproval);
	getchar();
	i = 1;
	if (aproval == 'n') {
		printf("The deletion of the word has been canceled.\n");
		return;
	}
	else {
		iterator = list->head; 
		while (iterator != NULL) { // searches for the right dictionary
			if (i == option) {
				iterator2 = iterator->wordHead;
				while (iterator2 != NULL) {
					if (CompareWords(iterator2->translations, newWord)) { 
						if (iterator2 == iterator->wordHead) { //  it the word if first int the list
							iterator->wordHead = iterator2->next;
						}
						else if (iterator2->next == NULL) { // if the word is last in the list
							lastWord->next = NULL;
							iterator->wordLast = lastWord;
						}
						else {
							lastWord->next = iterator2->next; // if the word is in the middle of the list
						}
						
						printf("The word has been deleted successfully!\n"); 
						free(newWord);
						return;
					}
					else {
						lastWord = iterator2;
						iterator2 = iterator2->next;
					}
				}
				break;
			}
			else {
				iterator = iterator->next;
				i++;
			}
		}
	}

	printf("The deletion of the word has failed!\n");
	free(newWord);
}

void FindWordInDictionary(Dictionaries* list) { // function for finding words in dictionary
	Dictionary* iterator = list->head;
	Word* iteratorDictionary = NULL;
	String* newWord = (String*)malloc(sizeof(String));
	int i = 1, option = 0;
	char aproval;

	printf("Chose a dictionary:\n");  // prints available dictionaries
	while (iterator != NULL) {
		printf("%d. ", i);
		PrintList(iterator->languages, list->head->numOfLanguages);
		iterator = iterator->next;
		printf("\n");
		i++;
	}
	scanf("%d", &option);

	printf("Enter a word in ");
	iterator = list->head;
	for (int j = 1; j < option; j++) {
		iterator = iterator->next;
	}
	PrintList(iterator->languages, iterator->numOfLanguages);
	printf("\n");
	getchar();
	Reader(&newWord); // takes input from user
	
	iteratorDictionary = iterator->wordHead;
	while (iteratorDictionary != NULL) {
		if (CompareWords(iteratorDictionary->translations, newWord)) { // searches for needed word
			printf("The translations are:\n");
			for (int j = 1; j < iterator->numOfLanguages; j++) {
				PrintWord(&(iterator->languages[j]));
				printf(": ");
				PrintWord(&(iteratorDictionary->translations[j]));
				if (j == iterator->numOfLanguages - 1)
					printf("\n");
				else
					printf(", ");
			}
			//printf("\n");
			return;
		}
		else {
			iteratorDictionary = iteratorDictionary->next;
		}
	}

	printf("There are no translations for “");
	PrintWord(newWord);
	printf("” in this dictionary.\n");
	free(newWord);
}

void DeleteDictionary(Dictionaries* list) { // function for deleting dictionaries
	Dictionary* iterator = list->head;
	Dictionary* nextDictionary = NULL;
	Dictionary* lastDictionary = NULL;
	int i = 1, option = 0;
	char aproval;

	printf("Chose a dictionary:\n");  // prints available dictionaries
	while (iterator != NULL) {
		printf("%d. ", i);
		PrintList(iterator->languages, list->head->numOfLanguages);
		iterator = iterator->next;
		printf("\n");
		i++;
	}
	scanf("%d", &option);
	printf("Are you sure? (y/n)\n");
	scanf("%c", &aproval);
	getchar();
	i = 1;
	if (aproval == 'n') {
		printf("The deletion of the dictionary has been canceled.\n");
		return;
	}
	else {
		iterator = list->head;
		while (iterator != NULL) { // searches for the right dictionary
			if (i == option) {
				if (iterator == list->head) { // if dictionary is first
					list->head = iterator->next;
				}
				else if (iterator == list->last) { // if dictionary is last
					lastDictionary->next = NULL;
				}
				else { // if dictionary is in the middle
					lastDictionary->next = iterator->next;
				}
				free(iterator);
				printf("The word has been deleted successfully!\n");
				return;
			}
			else {
				lastDictionary = iterator;
				iterator = iterator->next;
				i++;
			}
		}
	}
	printf("The deletion of the dictionary has failed!\n");
}

void menu() { 
	int option = 0, dumpForScanf = 0;
	Dictionaries dics = {NULL, NULL};

	while (option != 6) {
		printf("Welcome to the dictionaries manager!\n");
		printf("Choose an option:\n1. Create a new dictionary.\n2. Add a word to a dictionary.\n3. Delete a word from a dictionary.\n4. Find a word in a dictionary.\n5. Delete a dictionary.\n6. Exit.\n");
		scanf("%d", &option);
		getchar(); // me and my homies hate scanf
		switch (option) {
			case 1: AddNewDictionary(&dics); break;
			case 2: AddWordToDictionary(&dics); break;
			case 3: DeleteWordFromDictionaty(&dics); break;
			case 4: FindWordInDictionary(&dics); break;
			case 5: DeleteDictionary(&dics); break;
			case 6: FreeAll(dics.head); break;
			default: printf("Wrong option, try again:");
		}
	}	
}

void main() {
	menu();
}
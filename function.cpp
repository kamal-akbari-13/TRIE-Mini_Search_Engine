// implementation of trie functions


#include "function.h"

///CONSTRUCTOR

Node::Node()
{
	const int maxChildren = 40;
	for (int i = 0; i < maxChildren; i++)
		children[i] = NULL;
}

Trie::Trie()
{
	documentText.resize(3000);
	documentNames.resize(3000);
	documentTypes.resize(3000);
	titleLengths.resize(3000, 0);
	contentLengths.resize(3000, 0);
	outputSentences.resize(3000);
}

///DESTRUCTOR

Trie::~Trie()
{
	init();
}

///OTHER FUNCTION

int ascii(char character)
{
	if (character >= 'a' && character <= 'z')
		return character - 'a';
	if (character >= 'A' && character <= 'Z')
		return character - 'A';
	if (character >= '0' && character <= '9')
		return character - '0' + ('z' - 'a') + 1;
	return -1;
}

void toLower(string &inputString)
{
	int stringLength = inputString.length();
	for (int i = 0; i < stringLength; i++)
		if (inputString[i] >= 'A' && inputString[i] <= 'Z')
			inputString[i] += ('a' - 'A');
}

bool isNum(string inputString)
{
	int stringLength = inputString.length();
	for (int i = 0; i < stringLength; i++)
		if (inputString[i] < '0' || inputString[i] > '9')
			return false;
	return true;
}

bool valid(string inputString)
{
	int i, stringLength = inputString.size();
	for (i = 0; i<stringLength; i++)
		if (ascii(inputString[i]) == -1) return 0;
	return 1;
}

int isPunc(char character)
{
	if (character == '.' || character == '?' || character == '!')
		return 1;
	return 0;
}

void check_history(ostream &outputStream, vector<string> searchHistory, int historyLength) {
	int i;
	for (i = 0; i < historyLength; i++) outputStream << searchHistory[i] << "\n";
	outputStream << "\n";
}

///SPLIT AND INSERT

void Trie::split(int documentIndex)
{
	int textLength = static_cast<int>(documentText.at(documentIndex).length()), asciiValue;
	string currentWord = "";
	for (int i = 0; i < textLength; i++)
	{
		asciiValue = ascii(documentText.at(documentIndex)[i]);
		if (currentWord != "" && asciiValue == -1 && documentText.at(documentIndex)[i] != '\'')
		{
			insert(currentWord, i - 1, documentIndex);
			currentWord = "";
		}
		else if (asciiValue != -1)
			currentWord += documentText.at(documentIndex)[i];
	}
}

void Trie::insert(string keyword, int wordPosition, int documentIndex)
{
	if (root == NULL)
		root = new Node;
	int keywordLength = static_cast<int>(keyword.length()), asciiValue;
	Node* currentNode = root;
	for (int i = 0; i < keywordLength; i++)
	{
		asciiValue = ascii(keyword[i]);
		if (currentNode->children[asciiValue] == NULL)
		{
			currentNode->children[asciiValue] = new Node;
			currentNode->children[asciiValue]->parent = currentNode;
			currentNode->children[asciiValue]->wordLength = currentNode->wordLength + 1;
			currentNode->children[asciiValue]->characterValue = asciiValue;
		}
		currentNode = currentNode->children[asciiValue];
		if (i == (keywordLength - 1))
		{
			if (currentNode->fileList == NULL || currentNode->fileList->fileIndex != documentIndex)
				currentNode->insertOrderFile(documentIndex);
			currentNode->fileList->insertOrderWord(wordPosition);
			currentNode->occurrenceCount++;
		}
	}
}

void Node::insertOrderFile(int documentIndex)
{
	Order_file* newFile = new Order_file;
	newFile->nextFile = fileList;
	fileList = newFile;
	newFile->fileIndex = documentIndex;
}

void Order_file::insertOrderWord(int wordPosition)
{
	Order_word* newWord = new Order_word;
	newWord->nextWord = wordList;
	wordList = newWord;
	newWord->position = wordPosition;
}

void Trie::getTitleLength(int titleLength, int documentIndex)
{
	titleLengths.at(documentIndex) = titleLength;
}

void Trie::getContentLength(int contentLength, int documentIndex)
{
	contentLengths.at(documentIndex) = contentLength;
}

///Initialize trie

void Trie::initRecursion(Node* &currentNode)
{
	if (currentNode == NULL)
		return;
	for (int i = 0; i < 40; i++)
		initRecursion(currentNode->children[i]);
	if (currentNode->fileList != NULL)
		currentNode->fileList->init(currentNode->fileList);
	delete currentNode;
	currentNode = NULL;
}

void Order_file::init(Order_file* &currentFile)
{
	Order_word* tempWord = NULL;
	Order_file* tempFile = NULL;
	while (currentFile != NULL)
	{
		tempWord = NULL;
		while (currentFile->wordList != NULL)
		{
			tempWord = currentFile->wordList->nextWord;
			delete currentFile->wordList;
			currentFile->wordList = tempWord;
		}
		tempFile = currentFile->nextFile;
		delete currentFile;
		currentFile = tempFile;
	}
}

void Trie::init()
{
	initRecursion(root);
}

void Trie::initOutput()
{
	for (int i = 0; i < documentCount; i++)
		outputSentences.at(i) = "";
}

///INPUT

void Trie::inputData(string inputText)
{
	documentText.at(documentCount) = inputText;
	documentCount++;
}

void Trie::inputFileName()
{
	fstream inputFile;
	inputFile.open("file_name.txt", ios::in);
	for (int i = 0; i < documentCount; i++)
	{
		getline(inputFile, documentNames.at(i));
		getline(inputFile, documentTypes.at(i));
		documentTypes.at(i).erase(0, 2);
		documentTypes.at(i).erase(documentTypes.at(i).size()-1, documentTypes.at(i).size());
	}
	inputFile.close();
}

///SEARCH

Node* Trie::search(string keyword)
{
	if (root == NULL)
		return NULL;
	Node *currentNode = root;
	int keywordLength = keyword.length(), asciiValue;
	for (int i = 0; i < keywordLength; i++)
	{
		asciiValue = ascii(keyword[i]);
		if (currentNode->children[asciiValue] == NULL) return NULL;
		else currentNode = currentNode->children[asciiValue];
	}
	return currentNode;
}

///QUERY

void Trie::queryEXACT(ostream &outputStream, string searchKeywords[], int keywordCount)
{
	clock_t startTime = clock();
	int *documentMatches = new int[documentCount + 10], position, resultCount = 0;
	for (int i = 0; i < documentCount; i++)
		documentMatches[i] = 0;
	Node* searchResult;
	Order_file* currentFile;
	Order_word* currentWordPtr;
	bool hasResults = false;
	for (int i = 0; i < keywordCount; i++)
	{
		int keywordLength = static_cast<int>(searchKeywords[i].length()), asciiValue, j;
		std::string currentKeyword = "";
		for (j = 0; j < keywordLength; j++)
		{
			asciiValue = ascii(searchKeywords[i][j]);
			if (currentKeyword != "" && asciiValue == -1 && searchKeywords[i][j] != '\'')
				break;
			else if (asciiValue != -1)
				currentKeyword += searchKeywords[i][j];
		}
		searchResult = search(currentKeyword);
		if (!searchResult) continue;
		currentFile = searchResult->fileList;
		while (currentFile != NULL)
		{
			currentWordPtr = currentFile->wordList;
			while (currentWordPtr != NULL)
			{
				if (currentWordPtr->position + (keywordLength - j) + 1 <= contentLengths.at(currentFile->fileIndex))
				{
					position = currentWordPtr->position + 1;
					int p;
					for (p = j; p < keywordLength; p++)
					{
						if (tolower(documentText.at(currentFile->fileIndex)[position]) != tolower(searchKeywords[i][p]))
							break;
						position++;
					}
					if (p == keywordLength)
					{
						documentMatches[currentFile->fileIndex]++;
						storeSentence(currentFile->fileIndex, currentWordPtr->position);
						if (documentMatches[currentFile->fileIndex] == keywordCount)
							hasResults = true;
					}
				}
				if (documentMatches[currentFile->fileIndex] > 0)
					break;
				currentWordPtr = currentWordPtr->nextWord;
			}
			currentFile = currentFile->nextFile;
		}
	}
	outputStream << "Running time: " << (double)(clock() - startTime) / CLOCKS_PER_SEC << "\n";
	if (hasResults == false)
	{
		outputStream << "Cannot find the article sastisfied your request\n";
		return;
	}
	outputStream << "All the keywords inputed appeared in article:\n";
	for (int i = 0; i < documentCount; i++)
		if (documentMatches[i] == keywordCount)
		{
			outputStream << documentNames.at(i) << ": " << outputSentences.at(i) << "\n";
			resultCount++;
		}
	outputStream << "About " << resultCount << " result(s)\n";
	delete[] documentMatches;
}

void Trie::queryFILETYPE(ostream &outputStream, string searchKeywords[], int keywordCount)
{
	clock_t startTime = clock();
	int hasResults = 0, *documentMatches, resultCount = 0;
	documentMatches = new int[documentCount + 10];
	for (int i = 0; i < documentCount; i++)
		documentMatches[i] = -1;

	/*for (int i = 0; i < keywordCount; i++)
	{
		cout << searchKeywords[i] << " ";
	}
	cout << endl;
	for (int j = 0; j < documentCount; j++)
		cout << documentTypes[j] << " ";*/

	for (int i = 0; i < keywordCount; i++)
		for (int j = 0; j < documentCount; j++)
			if (searchKeywords[i] == documentTypes.at(j))
			{
				hasResults = 1;
				documentMatches[j] = 1;
			}
	outputStream << "Running time: " << (double)(clock() - startTime) / CLOCKS_PER_SEC << "\n";
	if (hasResults == 0)
	{
		outputStream << "Cannot find the article sastisfied your request\n";
		return;
	}
	else
		outputStream << "All the keywords inputed appeared in article:\n";
	for (int i = 0; i < documentCount; i++)
		if (documentMatches[i] == 1)
		{
			outputStream << documentNames.at(i) << "\n";
			resultCount++;
		}
	outputStream << "About " << resultCount << " result(s)\n";
	delete[] documentMatches;
}

void Trie::queryPRICE(ostream &outputStream, string searchKeywords[], int keywordCount)
{
	clock_t startTime = clock();
	int *documentMatches = new int[documentCount + 10], resultCount = 0;
	bool hasResults = false;
	for (int i = 0; i < documentCount; i++)
		documentMatches[i] = 0;
	Node* searchResult;
	Order_file* currentFile;
	Order_word* currentWordPtr;
	for (int i = 0; i < keywordCount; i++)
	{
		searchResult = search(searchKeywords[i]);
		if (searchResult == NULL)
		{
			outputStream << "Running time: " << (double)(clock() - startTime) / CLOCKS_PER_SEC << "\n";
			outputStream << "Cannot find the article sastisfied your request\n";
			return;
		}
		if (isNum(searchKeywords[i]))
		{
			currentFile = searchResult->fileList;
			while (currentFile != NULL)
			{
				currentWordPtr = currentFile->wordList;
				while (currentWordPtr != NULL)
				{
					if (currentWordPtr->position - searchResult->wordLength >= 0 && documentText.at(currentFile->fileIndex)[currentWordPtr->position - searchResult->wordLength] == '$')
					{
						documentMatches[currentFile->fileIndex]++;
						storeSentence(currentFile->fileIndex, currentWordPtr->position);
						if (documentMatches[currentFile->fileIndex] == keywordCount)
							hasResults = true;
						break;
					}
					currentWordPtr = currentWordPtr->nextWord;
				}
				currentFile = currentFile->nextFile;
			}
		}
		else
		{
			currentFile = searchResult->fileList;
			while (currentFile != NULL)
			{
				documentMatches[currentFile->fileIndex]++;
				storeSentence(currentFile->fileIndex, currentFile->wordList->position);
				if (documentMatches[currentFile->fileIndex] == keywordCount)
					hasResults = true;
				currentFile = currentFile->nextFile;
			}
		}
	}
	outputStream << "Running time: " << (double)(clock() - startTime) / CLOCKS_PER_SEC << "\n";
	if (hasResults == false)
	{
		outputStream << "Cannot find the article sastisfied your request\n";
		return;
	}
	outputStream << "All the keywords inputed appeared in article:\n";
	for (int i = 0; i < documentCount; i++)
		if (documentMatches[i] == keywordCount)
		{
			outputStream << documentNames.at(i) << ": " << outputSentences.at(i) << "\n";
			resultCount++;
		}
	outputStream << "About " << resultCount << " result(s)\n";
	delete[] documentMatches;
}

void Trie::queryHASHTAG(ostream &outputStream, string searchKeywords[], int keywordCount)
{
	clock_t startTime = clock();
	int *documentMatches = new int[documentCount + 10], resultCount = 0;
	bool hasResults = false;
	for (int i = 0; i < documentCount; i++)
		documentMatches[i] = 0;
	Node* searchResult;
	Order_file* currentFile;
	Order_word* currentWordPtr;
	for (int i = 0; i < keywordCount; i++)
	{
		searchResult = search(searchKeywords[i]);
		if (searchResult != NULL)
		{
			currentFile = searchResult->fileList;
			while (currentFile != NULL)
			{
				currentWordPtr = currentFile->wordList;
				while (currentWordPtr != NULL)
				{
					if (currentWordPtr->position - searchResult->wordLength >= 0 && documentText.at(currentFile->fileIndex)[currentWordPtr->position - searchResult->wordLength] == '#')
					{
						cout << currentWordPtr->position << " " << searchResult->wordLength << endl;
						documentMatches[currentFile->fileIndex]++;
						hasResults = true;
						storeSentence(currentFile->fileIndex, currentWordPtr->position);
						break;
					}
					currentWordPtr = currentWordPtr->nextWord;
				}
				currentFile = currentFile->nextFile;
			}
		}
	}
	outputStream << "Running time: " << (double)(clock() - startTime) / CLOCKS_PER_SEC << "\n";
	if (hasResults == false)
	{
		outputStream << "Cannot find the article sastisfied your request\n";
		return;
	}
	outputStream << "All the keywords inputed appeared in article:\n";
	for (int i = 0; i < documentCount; i++)
		if (documentMatches[i] > 0)
		{
			outputStream << documentNames.at(i) << ": " << outputSentences.at(i) << "\n";
			resultCount++;
		}
	outputStream << "About " << resultCount << " result(s)\n";
	delete[] documentMatches;
}

void Trie::queryINTITLE(ostream &outputStream, string searchKeywords[], int keywordCount)
{
	clock_t startTime = clock();
	int *documentMatches, hasResults = 0, resultCount = 0;
	documentMatches = new int[documentCount + 10];
	Node* searchResult = NULL;
	Order_file* currentFile;
	Order_word* currentWordPtr;
	for (int i = 0; i < documentCount; i++)
		documentMatches[i] = 0;
	for (int i = 0; i < keywordCount; i++)
	{
		searchResult = search(searchKeywords[i]);
		if (searchResult != NULL)
		{
			currentFile = searchResult->fileList;
			while (currentFile != NULL)
			{
				currentWordPtr = currentFile->wordList;
				while (currentWordPtr != NULL)
				{
					if (currentWordPtr->position <= titleLengths.at(currentFile->fileIndex))
					{
						documentMatches[currentFile->fileIndex]++;
						storeSentence(currentFile->fileIndex, currentWordPtr->position);
						hasResults = 1;
						break;
					}
					currentWordPtr = currentWordPtr->nextWord;
				}
				currentFile = currentFile->nextFile;
			}
		}
	}
	outputStream << "Running time: " << (double)(clock() - startTime) / CLOCKS_PER_SEC << "\n";
	if (hasResults == 0)
	{
		outputStream << "Cannot find the article sastisfied your request\n";
		return;
	}
	outputStream << "All the keywords inputed appeared in article:\n";
	for (int i = 0; i < documentCount; i++)
		if (documentMatches[i] > 0)
		{
			outputStream << documentNames.at(i) << ": " << outputSentences.at(i) << "\n";
			resultCount++;
		}
	outputStream << "About " << resultCount << " result(s)\n";
	delete[] documentMatches;
}

void Trie::queryAND(ostream &outputStream, string searchKeywords[], int keywordCount, int *queryResults) {
	vector <bool> documentFlags(documentCount, true);
	clock_t startTime = clock();
	int i, j;
	for (i = 0; i<keywordCount; i++) {
		Node *searchResult = search(searchKeywords[i]);
		if (searchResult != NULL) {
			vector <bool> currentMatches(documentCount, false);
			j = 0;
			Order_file *currentFile = searchResult->fileList;
			while (currentFile != NULL) {
				currentMatches[currentFile->fileIndex] = true;
				storeSentence(currentFile->fileIndex, currentFile->wordList->position);
				currentFile = currentFile->nextFile;
			}
			for (j = 0; j < documentCount; j++)	documentFlags[j] = documentFlags[j] & currentMatches[j];
		}
		else 
			return;
	}
	for (i = 0; i < documentCount; i++)
		if (documentFlags[i])
			queryResults[i] = 2;
}

void Trie::queryOR(ostream &outputStream, string searchKeywords[], int keywordCount, int *queryResults, bool &hasResults) {
	int i, j;
	for (i = 0; i<keywordCount; i++) {
		Node *searchResult = search(searchKeywords[i]);
		if (searchResult != NULL) {
			j = 0;
			Order_file *currentFile = searchResult->fileList;
			while (currentFile != NULL) {
				queryResults[currentFile->fileIndex] = 1;
				storeSentence(currentFile->fileIndex, currentFile->wordList->position);
				currentFile = currentFile->nextFile;
			}
			hasResults = true;
		}
	}
}

void Trie::queryMINUS(ostream &outputStream, string searchKeywords[], bool isMinusQuery[], int keywordCount) {
	vector <int> documentMatches(documentCount, 0);
	clock_t startTime = clock();
	bool hasResults = false;
	int i, j, resultCount = 0;
	for (i = 0; i < keywordCount; i++) {
		Node *searchResult = search(searchKeywords[i]);
		if (searchResult != NULL) {
			if (isMinusQuery[i] == false) {
				Order_file *currentFile = searchResult->fileList;
				while (currentFile != NULL) {
					documentMatches[currentFile->fileIndex]++;
					storeSentence(currentFile->fileIndex, currentFile->wordList->position);
					if (documentMatches[currentFile->fileIndex] == keywordCount)
						hasResults = true;
					currentFile = currentFile->nextFile;
				}
			}
			else {
				vector <bool> excludeFiles(documentCount, false);
				j = 0;
				Order_file *currentFile = searchResult->fileList;
				while (currentFile != NULL) {
					excludeFiles[currentFile->fileIndex] = true;
					currentFile = currentFile->nextFile;
				}
				for (j = 0; j < documentCount; j++)
					if (excludeFiles[j] == false)
					{
						documentMatches[j]++;
						if (documentMatches[j] == keywordCount)
							hasResults = true;
					}
			}
		}
		else {
			if (isMinusQuery[i] == false) {
				outputStream << "Running time: " << (double)(clock() - startTime) / CLOCKS_PER_SEC << "\n";
				outputStream << "Cannot find the article sastisfied your request\n";
				return;
			}
			else for (j = 0; j < documentCount; j++)
			{
				documentMatches[j]++;
				if (documentMatches[j] == keywordCount)
					hasResults = true;
			}
		}
	}
	outputStream << "Running time: " << (double)(clock() - startTime) / CLOCKS_PER_SEC << "\n";
	if (hasResults == false) 
	{
		outputStream << "Cannot find the article sastisfied your request\n";
		return;
	}
	outputStream << "The keywords inputed appeared in article:\n";
	for (i = 0; i < documentCount; i++)
		if (documentMatches[i] == keywordCount)
		{
			outputStream << documentNames.at(i) << ": " << outputSentences.at(i) << "\n";
			resultCount++;
		}
	outputStream << "About " << resultCount << " result(s)\n";
}

void Trie::storeSentence(int documentIndex, int wordPosition)
{
	string sentence = "";
	int i = wordPosition;
	//Go right
	while (1)
	{
		if (isPunc(documentText.at(documentIndex)[i]) == 1 && documentText.at(documentIndex)[i + 1] == ' ')
		{
			sentence = sentence + documentText.at(documentIndex)[i] + " ";
			break;
		}
		sentence += documentText.at(documentIndex)[i++];
	}
	i = wordPosition - 1;
	while (i >= 0)
	{
		if (documentText.at(documentIndex)[i] == ' ' && isPunc(documentText.at(documentIndex)[i - 1]) == 1)
			break;
		sentence = documentText[documentIndex][i--] + sentence;
	}

	string tempSentence = sentence, combinedString = "";
	toLower(tempSentence);
	combinedString = tempSentence + "|";
	tempSentence = outputSentences[documentIndex];
	toLower(tempSentence);
	combinedString += tempSentence;
	int combinedLength = static_cast<int>(combinedString.length()), leftBound = 0, rightBound = 0, sentenceLength = static_cast<int>(sentence.length()), *zArray;
	zArray = new int[combinedLength + 10];
	for (int i = 1; i < combinedLength; i++)
		zArray[i] = 0;
	zArray[0] = combinedLength;
	for (int i = 1; i < combinedLength; i++)
	{
		if (i <= rightBound)
			zArray[i] = min(zArray[i - leftBound], rightBound - i + 1);
		while (i + zArray[i] < combinedLength && combinedString[i + zArray[i]] == combinedString[zArray[i]])
			zArray[i]++;
		if (zArray[i] == sentenceLength)
			return;
		if (i + zArray[i] - 1 > rightBound)
		{
			leftBound = i;
			rightBound = i + zArray[i] - 1;
		}
	}

	outputSentences[documentIndex] += sentence;
	delete[]zArray;
}

void Trie::outputInQueryAndOr(ostream &outputStream, int documentIndex)
{
	outputStream << documentNames.at(documentIndex) << ": " << outputSentences.at(documentIndex) << "\n";
}
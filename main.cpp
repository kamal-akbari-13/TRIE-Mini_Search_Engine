// main program file


#include "function.h"

int main()
{
	clock_t startTime = clock();
	Trie contentTrie, stopwordTrie;
	string documentTitle, documentContent, fileName, currentLine;
	int documentIndex = 0;
	fstream inputFile, outputFile;
	bool isMinusQuery[100], hasResults;
	string searchKeywords[100], userQuery = "", historyEntry = "", currentWord;
	int keywordCount, queryLength, asciiValue, exactQueryFlag, wordLength = 0, historyLength = 0, resultCount;
	vector <string> searchHistory;
	int *queryResults;

	//INPUT DATA + OUTPUT FILENAME
	outputFile.open("file_name.txt", ios::out);
	for (auto& filePath : directory_iterator("data")) //Data files put in folder "data"
	{
		outputFile << filePath.path().filename() << "\n";
		outputFile << filePath.path().extension() << "\n";
		inputFile.open(filePath.path(), ios::in); //filePath.path() : file name
		getline(inputFile, documentTitle);
		documentTitle = ". " + documentTitle + ". ";
		contentTrie.getTitleLength(static_cast<int>(documentTitle.length()), documentIndex);
		documentContent = documentTitle;
		while (getline(inputFile, currentLine))
			documentContent += currentLine + ". ";
		contentTrie.getContentLength(static_cast<int>(documentContent.length()), documentIndex);
		inputFile.close();
		contentTrie.inputData(documentContent);
		contentTrie.split(documentIndex);
		documentIndex++;
	}
	outputFile.close();

	//CHECK ARRAY QUERY AND + OR
	queryResults = new int[documentIndex + 10];
	for (int i = 0; i < documentIndex; i++)
		queryResults[i] = 0;

	//GET FILENAME
	contentTrie.inputFileName();

	//Check time data
	cout << "MINI SEARCH ENGINE\n";
	printf("Loading time: %.2fs\n", (double)(clock() - startTime) / CLOCKS_PER_SEC);

	//Loading stopword
	inputFile.open("stopwords.txt", ios::in);
	userQuery = "";
	while (getline(inputFile, currentLine))
		userQuery += currentLine + " ";
	inputFile.close();
	stopwordTrie.inputData(userQuery);
	stopwordTrie.split(0);

	//CLASSIFY QUERY

	cout << "exit <- Close program\n";
	cout << "INPUT KEY: ";
	getline(cin, userQuery);
	toLower(userQuery);
	while (userQuery != "exit")
	{
		for (int i = 0; i < documentIndex; i++)
			queryResults[i] = 0;
		if (historyLength < 10) 
		{
			historyLength++;
			searchHistory.pb(userQuery);
		}
		else {
			searchHistory.erase(searchHistory.begin());
			searchHistory.pb(userQuery);
		}
		//SPLIT
		memset(isMinusQuery, false, sizeof(isMinusQuery));
		hasResults = false;
		contentTrie.initOutput();
		keywordCount = 0;
		userQuery = " " + userQuery;
		userQuery += " ";
		queryLength = userQuery.length();
		for (int i = 0; i < queryLength; i++)
		{
			asciiValue = ascii(userQuery[i]);
			if (i + 1 < queryLength && userQuery[i] == '-' && currentWord == "" && ascii(userQuery[i+1]) != -1)
			{
				isMinusQuery[keywordCount] = true;
				hasResults = true;
			}
			wordLength = currentWord.length();
			if ((currentWord != "" && asciiValue == -1 && userQuery[i] != '\'') && (stopwordTrie.search(currentWord) != NULL && i - wordLength - 1 >= 0 && userQuery[i - wordLength - 1] != '+' && userQuery[i - wordLength - 1] != '-'))
				currentWord = "";
			if (currentWord != "" && asciiValue == -1 && userQuery[i] != '\'')
			{
				searchKeywords[keywordCount++] = currentWord;
				currentWord = "";
			}
			else if (asciiValue != -1)
				currentWord += userQuery[i];
		}
		int hashtagPosition = userQuery.find("#"); //OR
		if (hashtagPosition != -1)
		{
			cout << "QUERY HASHTAG\n";
			contentTrie.queryHASHTAG(cout, searchKeywords, keywordCount);
		}
		else
		{
			int pricePosition = userQuery.find("$"); //AND
			if (pricePosition != -1)
			{
				cout << "QUERY PRICE\n";
				contentTrie.queryPRICE(cout, searchKeywords, keywordCount);
			}
			else
			{
				if (hasResults == true)
				{
					cout << "QUERY MINUS\n";
					contentTrie.queryMINUS(cout, searchKeywords, isMinusQuery, keywordCount);
				}
				else
				{
					int exactPosition = userQuery.find("\"");
					if (exactPosition != -1)
					{
						keywordCount = 0;
						exactQueryFlag = 0;
						currentWord = "";
						for (int i = 0; i < queryLength; i++)
						{
							asciiValue = ascii(userQuery[i]);
							if (exactQueryFlag != 0 && userQuery[i] == '\"')
							{
								exactQueryFlag = 0;
								searchKeywords[keywordCount++] = currentWord;
								currentWord = "";
							}
							else if (exactQueryFlag != 0)
								currentWord += userQuery[i];
							else if (userQuery[i] == '\"')
								exactQueryFlag = 1;
							else if (currentWord != "" && asciiValue == -1 && userQuery[i] != '\'')
							{
								searchKeywords[keywordCount++] = currentWord;
								currentWord = "";
							}
							else if (asciiValue != -1)
								currentWord += userQuery[i];
						}
						cout << "QUERY EXACT\n";
						contentTrie.queryEXACT(cout, searchKeywords, keywordCount);
					}
					else
						if (keywordCount > 0 && searchKeywords[0] == "filetype")
						{
							cout << "QUERY FILETYPE\n";
							contentTrie.queryFILETYPE(cout, searchKeywords, keywordCount);
						}
						else
							if (keywordCount > 0 && searchKeywords[0] == "intitle")
							{
								cout << "QUERY INTITLE\n";
								contentTrie.queryINTITLE(cout, searchKeywords, keywordCount);
							}
							else
							{
								hasResults = false;
								resultCount = 0;
								startTime = clock();
								cout << "QUERY AND + OR\n";
								contentTrie.queryOR(cout, searchKeywords, keywordCount, queryResults, hasResults);
								contentTrie.queryAND(cout, searchKeywords, keywordCount, queryResults);
								if (hasResults == false)
									cout << "Running time: " << (double)(clock() - startTime) / CLOCKS_PER_SEC << "\n" << "Cannot find the article sastisfied your request\n";
								else
								{
									cout << "Running time: " << (double)(clock() - startTime) / CLOCKS_PER_SEC << "\n";
									for (int i = 0; i < documentIndex; i++)
										if (queryResults[i] == 2)
										{
											contentTrie.outputInQueryAndOr(cout, i);
											resultCount++;
										}
									for (int i = 0; i < documentIndex; i++)
										if (queryResults[i] == 1)
										{
											contentTrie.outputInQueryAndOr(cout, i);
											resultCount++;
										}
									cout << "About " << resultCount << " result(s)\n";
								}
							}
				}
			}
		}
		cout << "ENTER TO CONTINUE FOR NEXT QUERY\n";
		cin.get();
		system("CLS");
		cout << "MINI SEARCH ENGINE \n";
		cout << "exit <- Close program\n";
		cout << "HISTORY SEARCH\n";
		check_history(cout, searchHistory, historyLength);
		cout << "INPUT KEY: ";
		getline(cin, userQuery);
		toLower(userQuery);
	}
	return 0;
}
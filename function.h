#pragma once

#include<iostream>
#include<cstdio>
#include<cstring>
#include<string>
#include<string.h>
#include<algorithm>
#include<cstdlib>
#include<fstream>
#include<filesystem>
//#include<experimental/filesystem>
#include<time.h>
#include<ctime>
#include<vector>
#define pb push_back

using namespace std;
//using namespace std::experimental::filesystem; 
using namespace std::filesystem; //Read all file

											 

struct Order_word //Mark the position of the word in the paragraph
{
	int position = -1; //position: The position in file
	Order_word* nextWord = NULL;
};

struct Order_file //The order of file
{
	int fileIndex = -1;
	Order_word* wordList = NULL; //root of list of position of words
	Order_file* nextFile = NULL;
	void init(Order_file*&);
	void insertOrderWord(int);
};

struct Node
{
	///DATA
	int characterValue = -1;
	//means what it is. For example: characterValue = 0 means it is 'a', 1 means it is 'b'
	Node* children[40];
	//26 alphabet + 10 number
	Node* parent = NULL;
	//Previous node
	int occurrenceCount = 0, wordLength = 0;
	//occurrenceCount: increase number occurence of that word (increase once when reaching the last character)
	//wordLength: is the length of the word count from the beginning to the character ! wordLength = parent->wordLength+1
	Order_file* fileList = NULL;
	//List of file contain list position of word occurs in n-order file
	//Root of list of file
	///FUNCTION
	Node(); //all children[] = NULL
	void insertOrderFile(int);
};

class Trie
{
private:
	Node* root = NULL;
	vector<string> documentText;
	vector<string> documentNames;
	vector<string> documentTypes;
	int documentCount = 0;
	vector<int> titleLengths;
	vector<int> contentLengths;
	vector<string> outputSentences;
public:
	Trie();
	~Trie();
	///Initialize trie
	void initRecursion(Node*&);
	void init();
	void initOutput();

	///Output sentences
	void storeSentence(int, int);
	void outputInQueryAndOr(ostream &, int);

	///SPLIT AND INSERT INTO TRIE
	void split(int);
	void insert(string, int, int);
	void getTitleLength(int, int);
	void getContentLength(int, int);
	//string: word
	//int: the order of file
	//int: the position of word

	Node* search(string); //return where NODE contain KEY

	///INPUT
	void inputData(string);
	//string: text
	void inputFileName(); //For 0 -> documentCount-1 get filename

	///QUERY
	void queryAND(ostream&, string[], int, int*);  //array of string (inputed keywords),  int: array size, int*: the array to check files satisfy query
	void queryOR(ostream&, string[], int, int*, bool&);	//array of string (inputed keywords),  int: array size, int*: the array to check files satisfy query, bool & check if having satisfied resuld or not
	void queryPRICE(ostream&, string[], int); //AND
	void queryEXACT(ostream&, string[], int); //AND
	void queryFILETYPE(ostream&, string[], int); //OR
	void queryHASHTAG(ostream&, string[], int); //OR
	void queryINTITLE(ostream&, string[], int); //OR
	void queryMINUS(ostream&, string[], bool[], int); //AND
};

///OTHER FUNCTION
int ascii(char);
void toLower(string&);
bool isNum(string);
bool valid(string);
int isPunc(char);
void check_history(ostream&, vector<string>, int);// header file for trie functions
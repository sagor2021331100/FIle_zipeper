#include <bits/stdc++.h>
#include <iostream>
#include <windows.h>
#include <fstream>
#include <sstream>
#include <bitset>
#include <string>
#include <queue>
#include <map>
#include <unordered_map>
#include <direct.h>
#include <sys/stat.h>
using namespace std;

// Function to create a directory
bool createDirectory(const string &path)
{
#ifdef _WIN32
	return _mkdir(path.c_str()) == 0;
#else
	return mkdir(path.c_str(), 0777) == 0;
#endif
}

// These are reserved characters for compressing and decompressing dont use these characters in your text
const char NEW_LINE = '$';
const char SPACE = '^';
const char FILE_SPLITTER = '#';

// A Tree node
struct Node
{
	char ch;
	int freq;
	Node *left, *right;
};

// Function to allocate a new tree node
Node *getNode(char ch, int freq, Node *left, Node *right)
{
	Node *node = new Node();
	node->ch = ch;
	node->freq = freq;
	node->left = left;
	node->right = right;

	return node;
}

// Comparison object to be used to order the heap
struct comp
{
	// highest priority item has lowest frequency
	bool operator()(Node *l, Node *r)
	{
		return l->freq > r->freq;
	}
};

// traverse the Huffman Tree and store Huffman Codes in a map.
void encode(Node *root, string str, unordered_map<char, string> &huffmanCode)
{
	if (root == nullptr)
		return;

	// found a leaf node
	if (!root->left && !root->right)
		huffmanCode[root->ch] = str;

	encode(root->left, str + "0", huffmanCode);
	encode(root->right, str + "1", huffmanCode);
}

// traverse the Huffman Tree and decode the encoded string
void decodeFromTree(Node *root, int &index, string str)
{
	if (root == nullptr)
	{
		return;
	}

	// found a leaf node
	if (!root->left && !root->right)
	{
		cout << root->ch;
		return;
	}

	index++;

	if (str[index] == '0')
		decodeFromTree(root->left, index, str);
	else
		decodeFromTree(root->right, index, str);
}

string decodeFromCharecterMapTree(string &encoded, unordered_map<string, char> &charMap, int totalCount = INT_MAX)
{
	string cur;
	int ct = 0;
	string decoded = "";
	for (int i = 0; i < encoded.size() && ct < totalCount; i++)
	{
		cur += encoded[i];

		if (charMap.count(cur) > 0) // charMap.count(cur) gives the number of occurrences of the key is present->it gives 1
		{
			if (charMap[cur] == NEW_LINE)
				decoded += '\n';
			else if (charMap[cur] == SPACE)
				decoded += ' ';
			else
				decoded += charMap[cur];
			cur = "";
			ct++;
		}
	}

	return decoded;
}

// Builds Huffman Tree and decode given input text
unordered_map<char, string> buildHuffmanTree(unordered_map<char, int> freq, Node *&return_root)
{
	// Create a priority queue to store live nodes of Huffman tree;
	priority_queue<Node *, vector<Node *>, comp> pq;

	// Create a leaf node for each character and add it to the priority queue.
	for (auto pair : freq)
	{
		pq.push(getNode(pair.first, pair.second, nullptr, nullptr));
	}

	// do till there is more than one node in the queue
	while (pq.size() != 1)
	{
		// Remove the two nodes of highest priority (lowest frequency) from the queue
		Node *left = pq.top();
		pq.pop();
		Node *right = pq.top();
		pq.pop();

		// Create a new internal node with these two nodes as children and with frequency equal to the sum
		// of the two nodes' frequencies. Add the new node to the priority queue.
		int sum = left->freq + right->freq;
		pq.push(getNode('\0', sum, left, right));
	}

	// root stores pointer to root of Huffman Tree
	Node *root = pq.top();

	// traverse the Huffman Tree and store Huffman Codes in a map. Also prints them
	unordered_map<char, string> huffmanCode;
	encode(root, "", huffmanCode);

	return_root = pq.top();

	return huffmanCode;
}

// Function to compress a file
void demoCompressFile(const string &inputFile, const string &outputFile)
{
	// Read the content of the input file
	ifstream inFile(inputFile);
	string line, text;
	while (getline(inFile, line))
	{
		text += line;
		text += "\n"; // Preserve newlines for text files
	}
	// text[text.size()-1]='\0';
	text.resize(text.size() - 1);
	// cout << text << endl;

	Node *root = nullptr;
	// count frequency of appearance of each character and store it in a map
	unordered_map<char, int> freq;
	for (char ch : text)
		freq[ch]++;
	unordered_map<char, string> huffmanCode = buildHuffmanTree(freq, root);
	ofstream outFile(outputFile);
	outFile << "There are " << freq.size() << " unique characters"
			<< "\n";
	string str = "";

	str += "Huffman tree are:\n";
	for (char ch : text)
	{
		str += huffmanCode[ch];
	}

	str += "\nHuffman Codes are :\n";
	for (auto pair : huffmanCode)
	{
		if (pair.first == ' ')
			str += SPACE;
		else if (pair.first == '\n')
			str += NEW_LINE;
		else
			str += pair.first;

		str += " ";
		str += pair.second;
		str += " ";
		str += to_string(freq[pair.first]);
		str += "\n";
	}
	// cout << "\nEncoded string is :\n" << str << '\n';

	// Write the compressed string to the output file
	outFile << str;

	outFile.close();
}

// Function to compress a file
void compressFile(const string &inputFileName, const string &outputFileName)
{
	// Read the content of the input file
	ifstream inFile(inputFileName);
	string line, text;
	while (getline(inFile, line))
	{
		text += line;
		text += "\n"; // Preserve newlines for text files
	}
	text.resize(text.size() - 1); // used to remove the trailing newline character

	Node *root = nullptr;

	// count frequency of appearance of each character and store it in a map
	unordered_map<char, int> freq;
	int totalCount = 0;
	for (char ch : text)
	{
		freq[ch]++;
		totalCount++;
	}

	unordered_map<char, string> huffmanCode = buildHuffmanTree(freq, root);

	// Create a folder for compressed files
// 	string folderName = outputFileName;
// #ifdef _WIN32
// 	if (_mkdir(folderName.c_str()) != 0)
// #else
// 	if (mkdir(folderName.c_str(), 0777) != 0)
// #endif
// 	{
// 		cerr << "Error creating directory!" << endl;
// 		return;
// 	}

	// Open binary file for writing
	string binaryFilePath = outputFileName + ".bin";
	ofstream binaryFile(binaryFilePath, ios::binary);
	if (!binaryFile)
	{
		cerr << "Error opening binary file for writing!" << endl;
		return;
	}

	// Write tree information to a file
	// string treeFilePath = folderName + "/" + outputFileName + ".tree";
	// ofstream treeFile(treeFilePath);
	// if (!treeFile)
	// {
	// 	cerr << "Error opening tree file for writing!" << endl;
	// 	return;
	// }
	string tree = "";
	int cnt = 1;
	for (auto pair : huffmanCode)
	{
		if (pair.first == ' ')
			tree += SPACE;
		else if (pair.first == '\n')
			tree += NEW_LINE;
		else
			tree += pair.first;

		tree += " ";
		tree += pair.second;
		tree += "\n";
		cnt++;
	}
	tree.resize(tree.size() - 1);
	string totalCountString = to_string(totalCount);
	
	// treeFile << totalCountString;
	// treeFile << '\n';
	// treeFile << tree;

	// binaryFile.write(reinterpret_cast<const char*>(&totalCount), sizeof(totalCount));
    tree += FILE_SPLITTER;  // Add '#' to mark the end of the tree input
	binaryFile.write(tree.c_str(), tree.size());

	// Write the compressed string to the binary file
	// raw    :  0100100010101011101000011110000001011000000010111111
	// input  :  01001000  10101011  10100001  11100000  01011000  00001010  11111
	// file   : [01001000][10101011][10100001][11100000][01011000][00001010][00011111]
	// output :  01001000  10101011  10100001  11100000  01011000  00001010  00011111
	// raw    :  01001000101010111010000111100000010110000000101000011111

	// raw    :  0100100010101011101000011110000001011000000010111111
	// input  :  01001000  10101011  10100001  11100000  01011000  00001010  11111
	// file   : [01001000][10101011][10100001][11100000][01011000][00001010][11111000]
	// output :  01001000  10101011  10100001  11100000  01011000  00001010  11111000
	// raw    :  0100100010101011101000011110000001011000000010111111000

	/*
	i = stoi("100", 10);
	i = 100 (01100100)
	i = stoi("100", 2);
	i = 4 (00000100)
	*/

	string encoded = "";
	for (char ch : text)
	{
		encoded += huffmanCode[ch];
	}

	int extra = encoded.size() % 8;
	for (int i = 0; i < (8 - extra); i++)
	{
		encoded += "0";
	}

	long long int len = encoded.length();
	// Process full 8-character chunks
	for (int i = 0; i < len; i += 8)
	{
		// Convert each 8-character chunk to a byte
		string byteString = encoded.substr(i, 8);
		char byte = (char)(stoi(byteString, nullptr, 2));

		// Write the byte to the binary file
		binaryFile.write(&byte, 1);
		// 1. stoi(byteString, nullptr, 2): This converts the binary string byteString to an integer using base 2 (binary). The resulting integer is assumed to be in the range of a byte (0 to 255).
		// a. Binary string: "10010101"
		// b. Integer representation (base 2): 149 (not 10010101, my mistake)
		// c. Cast to char: '\x95' (assuming 8-bit char)
		// #include <bits/stdc++.h>
		// using namespace std;
		// int main(){
		//     string binaryString = "10010101";
		//     int intValue = stoi(binaryString, nullptr, 2);// &ptr - Variable to store the position where the conversion stops
		//     cout << "Binary string: " << binaryString << endl;
		//     cout << "Integer value: " << intValue << endl;
		//     return 0;
		// }
		// 2. static_cast<char>: This explicitly converts the integer to a char. This is done because char is used to represent individual bytes in C++.
		// 3. binaryFile.write(&byte, 1): This writes the char (1 byte) to the binary file. The second argument 1 specifies the number of bytes to write, which is 1 in this case.
		// So, the cast to char is necessary to ensure that you are writing a single byte to the binary file, as a char is typically 8 bits (1 byte) in C++.
	}
	binaryFile.close();

	// Calculate the reduction in file size
	long long int totalChar = (totalCountString.size() + tree.size() + cnt);
	long long int totalSize = len + (totalChar * 8);
	long long int totalInputSize = (totalCount * 8);
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	if ((totalInputSize - totalSize) > 0)
	{
		SetConsoleTextAttribute(h, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		double reduce = ((totalInputSize - totalSize) * 100.00) / (double)totalInputSize;
		cout << "Compressed file reduce in: " << fixed << setprecision(2) << reduce << "%" << endl;
	}
	else
	{
		SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_INTENSITY);
		double increase = ((totalSize - totalInputSize) * 100.00) / (double)totalInputSize;
		cout << "Compressed file increase in: " << fixed << setprecision(2) << increase << "%" << endl;
	}
	SetConsoleTextAttribute(h, FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	// treeFile.close();
}

// Function to decompress a file
void demoDecompressFile(const string &inputFile, const string &outputFile)
{
	// Read the content of the input file
	ifstream inFile(inputFile);
	string line, str;
	getline(inFile, line);
	getline(inFile, line);
	getline(inFile, line);

	string encoded = line;

	getline(inFile, line);

	char ch;
	string code;
	int count;
	unordered_map<string, char> charMap;
	while (inFile >> ch >> code >> count)
	{
		charMap[code] = ch;
	}

	string decoded = decodeFromCharecterMapTree(encoded, charMap);

	// Write the decompressed string to the output file
	ofstream outFile(outputFile);
	outFile << decoded;
	outFile.close();
}

void decompressFile(const string &compressedFolder, const string &outputFileName)
{
	// Read the content of the binary file
	ifstream binaryInFile(compressedFolder + ".bin", ios::binary);
	if (!binaryInFile)
	{
		cerr << "Error opening binary input file: " << compressedFolder << "/compressedFile.bin" << endl;
		return;
	}

	string tree = "", encoded = "";
	char byte;
	bool isEncodingReading = false;

	// [01001000][10101011][10100001][11100000][01011000][00001010][11111000]
	while (binaryInFile.read(&byte, 1))
	{
		if(!isEncodingReading)
		{
			if(byte == '#')
			{
				isEncodingReading = true;
				continue;
			}
			tree += byte;
		}
		else
		{
			// Convert the byte to its binary representation and append to the string
			encoded += bitset<8>(byte).to_string();
		}
	}

	binaryInFile.close();

	// Read the content of the tree file
	// ifstream treeInFile(compressedFolder + "/" + compressedFolder + ".tree");
	// if (!treeInFile)
	// {
	// 	cerr << "Error opening tree input file: " << compressedFolder << "/compressedFile.tree" << endl;
	// 	return;
	// }
	// string totalCountString;
	// treeInFile >> totalCountString;
	// int totalCount = stoi(totalCountString);

	char ch;
	string code;
	unordered_map<string, char> charMap;
	
    std::istringstream treeInputStream(tree);

	while (treeInputStream >> ch >> code)
	{
		charMap[code] = ch;
	}
	string decoded = decodeFromCharecterMapTree(encoded, charMap/*, totalCount */);

	// Write the decompressed string to the output file
	ofstream outFile(outputFileName);
	outFile << decoded;
	outFile.close();
}

int main()
{
	ios_base:: sync_with_stdio(0);
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_BLUE);
	int option;
	cout << "Welcome to FILE ZIPPER SOFTWARE:\n";
	cout << endl;
	SetConsoleTextAttribute(h, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	do
	{
		cout << "Choose an option:\n";
		cout << endl;
		cout << "1. Compress a file\n";
		cout << "2. Decompress a file\n";
		cout << "3. Demo compress a file\n";
		cout << "4. Demo decompress a file\n";
		cout << "5. Exit\n";
		cout << "Enter option (1, 2, 3, 4 or 5): ";
		cin >> option;
		cout << endl;

		switch (option)
		{
		case 1:
		{
			int num;
			cout<<"How many file do you want to compress"<<endl;
			cin>>num;
			for (int i = 1; i <= num; i++){
				if(i==1 ){
					cout<<i<< "st: ";
				}
				else if(i==2){
					cout<<i<<"nd: ";
				}
				else if(i==3){
					cout<<i<<"rd: ";
				}
				else{
					cout<<i<<"th: ";
				}
				string inputFileName, outputFileName;
				cout << "Input filename : ";
				cin >> inputFileName;
				cout << "Compressed File name: ";
				cin >> outputFileName;
				cout << endl;

				int dotPosition = outputFileName.find_last_of(".");
				if (dotPosition != string::npos)
					outputFileName = outputFileName.substr(0, dotPosition);

				compressFile(inputFileName, outputFileName);

				cout << "File compressed successfully.\n";
				
			}
			break;
			
		}
		case 2:
		{
			int num;
			cout<<"How many file do you want to decompress"<<endl;
			cin>>num;
			for (int i = 1; i <= num; i++){
				if(i==1 ){
					cout<<i<< "st: ";
				}
				else if(i==2){
					cout<<i<<"nd: ";
				}
				else if(i==3){
					cout<<i<<"rd: ";
				}
				else{
					cout<<i<<"th: ";
				}
				string inputFileName, outputFileName;
				cout << "Compressed file name: ";
				cin >> inputFileName;
				cout << "Decompressed file name: ";
				cin >> outputFileName;
				cout << endl;

				int dotPosition = inputFileName.find_last_of(".");
				if (dotPosition != std::string::npos)
					inputFileName = inputFileName.substr(0, dotPosition);

				dotPosition = outputFileName.find_last_of(".");
				if (dotPosition == string::npos || dotPosition == 0)
					outputFileName += ".txt";

				decompressFile(inputFileName, outputFileName);

				cout << "File decompressed successfully.\n";
			}
			break;
		}
		case 3:
		{
			int num;
			cout<<"How many file do you want to compress"<<endl;
			cin>>num;
			for (int i = 1; i <= num; i++){
				if(i==1 ){
					cout<<i<< "st: ";
				}
				else if(i==2){
					cout<<i<<"nd: ";
				}
				else if(i==3){
					cout<<i<<"rd: ";
				}
				else{
					cout<<i<<"th: ";
				}
			string inputFileName, outputFileName;
			cout << "Input filename : ";
			cin >> inputFileName;
			cout << "Demo compressed filename: ";
			cin >> outputFileName;
			cout << endl;

			demoCompressFile(inputFileName, outputFileName);

			cout << "File compressed successfully.\n";
			}
			break;
		}
		case 4:
		{
			int num;
			cout<<"How many file do you want to decompress"<<endl;
			cin>>num;
			for (int i = 1; i <= num; i++){
				if(i==1 ){
					cout<<i<< "st: ";
				}
				else if(i==2){
					cout<<i<<"nd: ";
				}
				else if(i==3){
					cout<<i<<"rd: ";
				}
				else{
					cout<<i<<"th: ";
				}
			string inputFileName, outputFileName;
			cout << "Demo compressed filename: ";
			cin >> inputFileName;
			cout << "Decompressed filename: ";
			cin >> outputFileName;
			cout << endl;

			demoDecompressFile(inputFileName, outputFileName);

			cout << "File decompressed successfully.\n";
			}
			break;
		}
		case 5:
		{
			cout << "Exiting program.\n";
			break;
		}
		default:
		{
			cout << "Invalid option. Please choose again.\n";
		}
		}
	} while (option != 5);

	return 0;
}

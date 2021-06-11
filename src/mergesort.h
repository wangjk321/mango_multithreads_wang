// C++ External Merge Sort
// 2014 by Alan Boyle
// Heavily modified from https://code.google.com/p/external-mergesort/ by keithema...@gmail.com which also does not appear to work

#ifndef MERGESORT_H
#define MERGESORT_H
#include <deque>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>
#include <errno.h>
using namespace std;

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()
        
//class template declaration
template<class T> class externalMergesort{
public:
	externalMergesort(string inFileName, string outFileName);
	externalMergesort(string inFileName, string outFileName, int maxDequeSize);

private:
	deque<T> sort(deque<T> &data);
	deque<T> merge(deque<T> &left, deque<T> &right);
	int readInputFile(ifstream &inputFile, string tempName);
	bool readFile(ifstream &file, deque<T> &data);
	void printFile(string fileName, deque<T> &data, bool append);
	void mergeFiles(int numFiles, string tempName);
  string NumberToString(T Number);
  bool compareFront(string left, string right);
  bool is_number(const string& s);
	string random_string(const int length);
	int maxDequeSize;
};

/////////////////////////////////////////////////////////////////////////////////////
//								Implementation									   //
/////////////////////////////////////////////////////////////////////////////////////

////////////////
//constructors//
////////////////

//mergesort(inFileName, outFileName)
//takes a filename and a maximum number of deque items to have in memory at one time
template<class T> externalMergesort<T>::externalMergesort(string inFileName, string outFileName){
	ifstream inputFile;
  std::string tempName = "sort" + random_string(15);

	this->maxDequeSize = 0;

	//open the original input file
	try{
		inputFile.open(inFileName.c_str());
	} catch(int e){
	  cerr << "Could not open the input file!\nError " << e;
	}

	//read the input file
	cerr << "Reading the input file...";
	readInputFile(inputFile, tempName);
	cerr << "Done!\n\n";

	//close the input file
	inputFile.close();

	//sort and merge files
	cerr << "Merging files...";
	rename(tempName.c_str(), outFileName.c_str());
	remove(tempName.c_str());
	cerr << "Done!\n";
}

//mergesort(inFileName, outFileName, maxDequeSize)
//takes a filename and a maximum number of deque items to have in memory at one time
template<class T> externalMergesort<T>::externalMergesort(string inFileName, string outFileName, int maxDequeSize){
	int numFiles;
	ifstream inputFile;
  std::string tempName = "sort" + random_string(15);
	string fileName;

	this->maxDequeSize = maxDequeSize;

	//open the original input file
	try{
		inputFile.open(inFileName.c_str());
	} catch(int e){
		cerr << "Could not open the input file!\nError " << e;
	}

	//read the input file
	cerr << "Reading the input file...\n-Progress-\n";
	numFiles = readInputFile(inputFile, tempName);
	cerr << "Done!\n";

	//close the input file
	inputFile.close();

	//sort and merge files
	cerr << "\n\nMerging files...\n-Progress-\n";
	mergeFiles(numFiles, tempName);

	fileName = tempName; fileName += "_0.txt";
	rename(fileName.c_str(), outFileName.c_str());
	remove(fileName.c_str());
	cerr << "Done!\n";
}

//////////
//sort()//
//////////
//performs mergesort algorithm
//the original deque is destroyed, the sorted deque is returned be the function
template<class T> deque<T> externalMergesort<T>::sort(deque<T> &right){
	int size = right.size();

	if (size <= 1){
		return right;
	}

	int middle = size/2;

	deque<T> left;

	//push half of the data to the left deque
	for(int i = 0; i < middle; i++){
		left.push_back(right.front());
		right.pop_front();
	}

	//the remaining part of the right deque is used as the right deque for the merge

	//call recursively until the deques are of size 1
	left = sort(left);
	right = sort(right);

	//merge the deques
	return merge(left, right);
}

///////////
//merge()//
///////////
//merges two sorted deques together and returns the sorted deque
template<class T> deque<T> externalMergesort<T>::merge(deque<T> &left, deque<T> &right){
	deque<T> result;
  int counter = 0;
  
  //cerr << "Left size: " << left.size() << " Right: " << right.size() << "\n";
	//merge the left and right deque by pulling of the lesser value of the front two values
	while((left.size() > 0 || right.size() > 0) && (counter < maxDequeSize || maxDequeSize == 0)){
		if (left.size() > 0 && right.size() > 0){
			if (compareFront(left.front(), right.front())){
				result.push_back(left.front());
				left.pop_front();
			}
			else{
				result.push_back(right.front());
				right.pop_front();
			}
		} else if(left.size() > 0){
			result.push_back(left.front());
			left.pop_front();
		}	else if(right.size() > 0){
			result.push_back(right.front());
			right.pop_front();
		}
    counter++;
	}

	return result;
}

///////////////////
//readInputFile()//
///////////////////
//readInputFile reads from the input file and writes an output file for each deque
template<class T> int externalMergesort<T>::readInputFile(ifstream &inputFile, string tempName){
	deque<T> data;
	T temp;
	int fileCount = 0;
	int numFiles;
	string fileName;

	//if using external storage
	if(maxDequeSize > 0){
		//loop until out of input
		for(numFiles = 0; inputFile.good(); numFiles++){
			fileName = tempName; fileName += "_"; fileName += SSTR( numFiles ); fileName += ".txt";
			
  		for(int i = 0; (i < maxDequeSize && getline(inputFile, temp)); i++){
				data.push_back(temp);
			}

			data = sort(data);
			printFile(fileName, data, false);
			data.clear();
			cerr << "*";
		}
	}

	//if not using external storage
	else{
		//loop until out of input
		for(numFiles = 0; inputFile.good(); numFiles++){			
      for(int i = 0; getline(inputFile, temp); i++){
				data.push_back(temp);
			}

			data = sort(data);
			printFile(tempName, data, false);
			data.clear();
		}
	}

	cerr << endl;
	return numFiles + 1;
}

//////////////
//readFile()//
//////////////
//reads an ifstream file and stores the data in a deque. returns a bool indicating if the file has not reached EOF
template<class T> bool externalMergesort<T>::readFile(ifstream &file, deque<T> &data){
	T temp;

	if(maxDequeSize > 0){
		for(int i = data.size(); i < maxDequeSize && getline(file, temp); i++){
			data.push_back(temp);
		}
	}

	else{
		for(int i = 0; getline(file, temp); i++){
			data.push_back(temp);
		}
	}

	return file.good();
}

///////////////
//printFile()//
///////////////
//opens a file with the specified filename and prints the contents of the deque to it. 
//if append is true, the data will be appended to the file, else it will be overwritten
template<class T> void externalMergesort<T>::printFile(string fileName, deque<T> &data, bool append){

	ofstream outputFile;
	
	if(append)
		outputFile.open(fileName.c_str(), ios::app);
	else
		outputFile.open(fileName.c_str());

	outputFile.precision(23);	

	while(data.size() > 0){
		outputFile << data.front() << endl;
		data.pop_front();
	}

	outputFile.close();
  return;
}

////////////////
//mergeFiles()//
////////////////
//merges the sortfiles until there is one file left
template<class T> void externalMergesort<T>::mergeFiles(int numFiles, string tempName){
	ifstream inFile1, inFile2;
	ofstream outFile;
	string fileName1, fileName2;
	int i, k, max;
	deque<T> data1;
	deque<T> data2;
  deque<T> dataOUT;
	bool fileGood1, fileGood2;

	i = 0;	//i is the counter for the input file names
	k = 0;	//k is the counter for the output file names
	max = numFiles - 1;
	while(max >= 2){
		//change filenames
		fileName1 = tempName; fileName1 += "_"; fileName1 += SSTR(i); fileName1 += ".txt";
		fileName2 = tempName; fileName2 += "_"; fileName2 += SSTR(i+1); fileName2 += ".txt";
		
		//open the files
		try{
			inFile1.open(fileName1.c_str());
			inFile2.open(fileName2.c_str());
		} catch(int e){
			cerr << "Could not open the open the files!\nError " << e;
		}

		//read from the files until they are empty
		fileGood1 = true;
		fileGood2 = true;
		while(fileGood1 || fileGood2){
      //Needs to merge only up to first maxDequeSize datapoints with dequeuing - refill on empty
			fileGood1 = readFile(inFile1, data1);
			fileGood2 = readFile(inFile2, data2);

			dataOUT = merge(data1, data2);
			printFile(tempName, dataOUT, true);
		}

		//close and delete the unneeded files
		inFile1.close();
		inFile2.close();
		remove(fileName1.c_str());
		remove(fileName2.c_str());

		//rename the temp file to a sortfile
		fileName1 = tempName; fileName1 += "_"; fileName1 += SSTR(k); fileName1 += ".txt";
		rename(tempName.c_str(), fileName1.c_str());

		//increase counters
		i = i + 2;
		k++;

		//reset counters if all the files have been read
		if(i >= max){ 
			max = max / 2 + max % 2;
			i = 0;
			k = 0;
		}
		cerr << "*";
	}

	cerr << endl;
  return;
}

////////////////
//compareFront()//
////////////////
//sorts by columns by type instead of just strings
template<class T> bool externalMergesort<T>::compareFront(string left, string right) {
  string stringLeft, stringRight;
  double numLeft, numRight;
  
  size_t pos = 0;
  std::string delimiter = "\t";
  
  while(left.size() > 0 && right.size() > 0) { //Loop until we either find a difference or run out of data
    //Get left string
    pos = left.find(delimiter);
    stringLeft = left.substr(0,pos);
    left.erase(0, pos + delimiter.length());
    //Get right string
    pos = right.find(delimiter);
    stringRight = right.substr(0,pos);
    right.erase(0, pos + delimiter.length());
    
    //Check if number
    if(is_number(stringLeft) && is_number(stringRight)) {
      //convert to doubles and use this
      numLeft = atof(stringLeft.c_str());
      numRight = atof(stringRight.c_str());
      if(numLeft == numRight) {
        continue; // Same so check next field
      } else {
        return numLeft < numRight;
      }
    } else { //Just comparing strings
      if(stringLeft == stringRight) {
        continue; // Same so check next field
      } else {
        return stringLeft < stringRight;
      }
    }
  }

  //Everything was the same AFAIK
  return left < right;
}

template<class T> bool externalMergesort<T>::is_number(const std::string& s)
{
  std::istringstream ss(s);
  double d;
  return (ss >> d) && (ss >> std::ws).eof();
}

template<class T> std::string externalMergesort<T>::random_string(const int len) {
	std::string randomString;
	char s[len];

	static const char alphanum[] =
			"0123456789"
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	s[len] = 0;
	randomString = s;
	return randomString;
}
#endif

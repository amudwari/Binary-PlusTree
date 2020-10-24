//
//  main.cpp
//  BPlusTree.2a
//
//  Created by Amittai Aviram on 6/10/16.
//  Copyright © 2016 Amittai Aviram. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include <iostream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "BPlusTree.hpp"
#include "Definitions.hpp"

int extractLine(std::string currLine, int numWords) {
  std::stringstream stream(currLine);

  int wordLength;                 //length of current word in stream
  int wordsRemain = numWords;     //words in line that remain after zipCity read
  int cityPos = 0;                //keep track of city and county position
  int countyPos = 0;              //in their respected arrays
  int delim = (char)124;          //puts delimiter '|' between each field

  char zip[6], cityP1[25], zipCity[30], word[30];

  std::string zipCode, city, state, county, latitude, longitude;

  bool cityComplete = false;
  bool countyComplete = false;
  bool latComplete = false;
  bool longComplete = false;

  //get first word of line
  stream >> zipCity;
  wordsRemain--;

  //separate zip code from zipCity
  int i = 0;
  while (isdigit(zipCity[i])) {
    zip[i] = zipCity[i];
    i++;
  }

  //separate first part of city name from zipCity
  while (i < 25)
    cityP1[cityPos++] = zipCity[i++];

  zipCode = std::string(zip);
  city = std::string(cityP1);


  //reads all other words after zipCity in current line
  while (stream >> word) {
    wordsRemain--;
    std::string tmp = std::string(word);
    wordLength = tmp.length();

    //attempts to finish city field
    if (!cityComplete) {
      if (isupper(word[1])) {
        city += delim;
        cityComplete = true;
        state = tmp;
        state += delim;
      }
      else
        city = city + " " + tmp;
    }

    //if zip, city, and state assigned to their fields, try to complete county
    else if (cityComplete && !countyComplete) {
      //complete county field if first char of current word is a digit
      if (isdigit(word[0])) {
        county += delim;
        countyComplete = true;

        //if one word left, complete latitude
        if (wordsRemain == 1) {
          latitude = tmp;
          latitude += delim;
          latComplete = true;
        }

        //if wordsRemain == 0, complete latitude and longitude
        else if (wordsRemain == 0) {
          char tmpLat[11];
          char tmpLong[11];

          int m = 0;
          while (word[m] != '-') {
            tmpLat[m] = word[m];
            m++;
          }

          int n = 0;
          while (m < wordLength)
            tmpLong[n++] = word[m++];

          latitude = std::string(tmpLat);
          latitude += delim;
          latComplete = true;
          longitude = std::string(tmpLong);
          longComplete = true;
          longitude += delim;
        }
      }
      //keep adding to county if first char is not a digit
      else
        county = county + " " + tmp;
    }

    //last word, complete longitude
    else if (latComplete && !longComplete) {
      longitude = tmp;
      longitude += delim;
      longComplete = true;
    }
  }

  county.erase(0, 1);
    /*
  std::cout << zipCode << " ";
  std::string line = city + state + county + latitude + longitude;
  std::cout << line << std::endl;
    */
    std::stringstream zipAsInt(zipCode);
    int change = 0;
    zipAsInt >> change;

    return change;
}

int countWords(std::string str) {
	std::stringstream stream(str);
	std::string oneWord;
	int numWords = 0;

	while (stream >> oneWord)
		numWords++;

	return numWords;
}

std::string introMessage(int aOrder) {
    std::ostringstream oss;
    oss << "To build a B+ tree of a different order, start again and enter the order" << std::endl;
    oss << "as an integer argument:  bpt <order>  " << std::endl;
    oss << "(" << MIN_ORDER << " <= order <= " << MAX_ORDER << ")." << std::endl;
    oss << "To start with input from a file of newline-delimited integers," << std::endl;
    oss << "start again and enter the order followed by the filename:" << std::endl;
    oss << "bpt <order> <inputfile> ." << std::endl << std::endl;
    return oss.str();
}

std::string usageMessage() {
    std::string message =
    "Enter any of the following commands after the prompt > :\n"
    "\ti <k>  -- Insert <k> (an integer, <k> >= 0) as both key and value).\n"
    "\ti -- Insert key(integer) followed by Information(string, no spaces).\n"
    "\tf <k> -- Find the value under key <k>.\n"
    "\tF  -- Fills the tree with zipcodes.txt \n"
    "\tp <k> -- Print the path from the root to key k and its associated value.\n"
    "\tr <k1> <k2> -- Print the keys and values found in the range [<k1>, <k2>]\n"
    "\td <k>  -- Delete key <k> and its associated value.\n"
    "\tx -- Destroy the whole tree.  Start again with an empty tree of the same order.\n"
    "\tt -- Print the B+ tree.\n"
    "\tl -- Print the keys of the leaves (bottom row of the tree).\n"
    "\tv -- Toggle output of pointer addresses (\"verbose\") in tree and leaves.\n"
    "\tq -- Quit. (Or use Ctl-D.)\n"
    "\t? -- Print this help message.\n\n";
    return message;
}

int getOrder(int argc, const char * argv[]) {
    if (argc > 1) {
        int order = 0;
        std::istringstream iss(argv[1]);
        if ((iss >> order) && iss.eof() &&
            order >= MIN_ORDER && order <= MAX_ORDER) {
            return order;
        } else {
            std::cerr << "Invalid order specification: "
            << argv[1] << std::endl;
            std::cerr << "Order must be an integer such that " << MIN_ORDER
            << " <= <order> <= " << MAX_ORDER << std::endl;
            std::cerr << "Proceeding with order " << DEFAULT_ORDER << std::endl;
        }
    }
    return DEFAULT_ORDER;
}

int main(int argc, const char * argv[]) {
    char instruction;
    int key = 0;
    bool quit = false;
    bool verbose = false;
    std::string lineOfText;
    int wordsInLine;

	std::ifstream inputFile;
	std::string fileName;
	fileName = "zipcodes.txt";

	inputFile.open(fileName);

    int order = getOrder(argc, argv);
    std::cout << introMessage(order);
    std::cout << usageMessage();
    BPlusTree tree(order);
    if (argc > 2) {
        //tree.readInputFromFile(argv[2]);
        std::cout << "Input from file " << argv[2] << ":" << std::endl;
        tree.print();
    }
    while (!quit)
    {
        std::cout << "> ";
        std::cin >> instruction;
        switch (instruction) {
            case 'd':
                std::cin >> key;
                tree.remove(key);
                std::cout << "Removed: " << key << std::endl;
                break;
            case 'i':
                {
                    char buffer[BUFFER_SIZE];
                    int value = 0;
                    std::string valueS;
                    std::cout << "Enter the key: ";
                    std::cin >> key;
                    std::cout << std::endl;
                    std::cout << "Enter the information without spaces(Temporarily): ";
                    std::cin >> valueS;
                    // std::cin >> key;
                    // if (key < 0) {
                    // std::cout << usageMessage();
                    // }
                    tree.insert(key, valueS);
                    tree.print(verbose);
                    break;
                }
            case 'f':
                std::cin >> key;
                tree.printValue(key);
                break;
            case 'F':
                while(!inputFile.eof()) {
                    getline(inputFile, lineOfText);
                    if(inputFile.fail()){
                        break;
                    }
                    wordsInLine = countWords(lineOfText);
                    tree.insert(extractLine(lineOfText, wordsInLine), lineOfText);
                    }

                break;
            case 'l':
                tree.printLeaves(verbose);
                break;
            case 'p':
                std::cin >> key;
                tree.printPathTo(key, verbose);
                break;
            case 'q':
                quit = true;
                break;
            case 'r':
                {
                    int key2;
                    std::cin >> key;
                    std::cin >> key2;
                    tree.printRange(key, key2);
                    break;
                }
            case 't':
                tree.print(verbose);
                break;
            case 'v':
                verbose = !verbose;
                tree.print(verbose);
                break;
            case 'x':
                tree.destroyTree();
                tree.print();
                break;
            case '?':
                std::cout << usageMessage();
                break;
            default:
                std::cin.ignore(256, '\n');
                std::cout << usageMessage();
                break;
        }
    }
    return 0;
}

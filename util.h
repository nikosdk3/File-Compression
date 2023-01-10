// Project 6
// Author: Nikos Kotsopulos
// Huffman File Compression - File compression software that is capable of compressing
//                            and decompressing files through encoding and decoding.

#pragma once

#include "hashmap.h"
#include "bitstream.h"
#include <queue>

typedef hashmap hashmapF;
typedef unordered_map<int, string> hashmapE;

struct HuffmanNode
{
    int character;
    int count;
    HuffmanNode *zero;
    HuffmanNode *one;
};

struct compare
{
    bool operator()(const HuffmanNode *lhs,
                    const HuffmanNode *rhs)
    {
        return lhs->count > rhs->count;
    }
};

//
// *This method frees the memory allocated for the Huffman tree.
//
void freeTree(HuffmanNode *node)
{
    if (node == NULL) // base case
        return;
    freeTree(node->zero); // recursion to the left
    freeTree(node->one);  // recursion to the right
    delete node;
    node = NULL; // just in case
}

// helper function for building frequency map
void frequencyMapHelper(hashmapF &hmap, char &ch)
{
    if (!hmap.containsKey(ch)) // if the character is not in the map,
    {
        hmap.put(ch, 1); // add it and set its count to 1
    }
    else // character was found in the map
    {
        int oldVal = hmap.get(ch);
        hmap.put(ch, oldVal + 1); // increase its count by 1
    }
}

//
// *This function build the frequency map.  If isFile is true, then it reads
// from filename.  If isFile is false, then it reads from a string filename.
//
void buildFrequencyMap(string filename, bool isFile, hashmapF &map)
{
    if (isFile) // reading from a file
    {
        ifstream inFS(filename);
        char c;
        while (inFS.get(c))
        {
            frequencyMapHelper(map, c);
        }
    }
    else // reading from a string
    {
        for (char c : filename)
        {
            frequencyMapHelper(map, c);
        }
    }
    map.put(PSEUDO_EOF, 1); // manually adding PSEUDO_EOF
}

//
// *This function builds an encoding tree from the frequency map.
//
HuffmanNode *buildEncodingTree(hashmapF &map)
{
    priority_queue<HuffmanNode *, vector<HuffmanNode *>, compare> pq;
    vector<int> keys = map.keys();             // storing the keys into a vector to use for node creation
    for (unsigned i = 0; i < keys.size(); i++) // iterating over vector I just created
    {
        HuffmanNode *newNode = new HuffmanNode;
        newNode->character = keys.at(i);
        newNode->count = map.get(keys.at(i));
        newNode->zero = nullptr;
        newNode->one = nullptr;
        pq.push(newNode); // pushing each node into the priority queue
    }
    while (pq.size() > 1) // once all nodes from the map are in the priorityqueue, start combining them to create the encoding tree
    {
        HuffmanNode *leftChild = pq.top();
        pq.pop();
        HuffmanNode *rightChild = pq.top();
        pq.pop();
        // saving first and second nodes in the priorityqueue to combine
        // combination process:
        HuffmanNode *sumNode = new HuffmanNode;
        sumNode->zero = leftChild;
        sumNode->one = rightChild;
        sumNode->character = NOT_A_CHAR;
        sumNode->count = leftChild->count + rightChild->count;

        // pushing new node into priorityqueue
        pq.push(sumNode);
    }

    // returning root
    return pq.top();
}

//
// *Recursive helper function for building the encoding map.
//
void _buildEncodingMap(HuffmanNode *node, hashmapE &encodingMap, string str,
                       HuffmanNode *prev)
{
    if (!node) // base case
        return;
    if (node->character != NOT_A_CHAR) // once a leaf is reached, add it into the encoding map
    {
        encodingMap[node->character] = str;
    }
    else // recursion
    {
        _buildEncodingMap(node->zero, encodingMap, str += "0", node);
        str.erase(str.size() - 1, 1); // deleting last character put in because I have to go back up the tree
        _buildEncodingMap(node->one, encodingMap, str += "1", node);
        str.erase(str.size() - 1, 1); // deleting last character put in because I have to go back up the tree
    }
}

//
// *This function builds the encoding map from an encoding tree.
//
hashmapE buildEncodingMap(HuffmanNode *tree)
{
    hashmapE encodingMap;
    string s;
    _buildEncodingMap(tree, encodingMap, s, nullptr); // calling recursive function
    return encodingMap;
}

//
// *This function encodes the data in the input stream into the output stream
// using the encodingMap.  This function calculates the number of bits
// written to the output stream and sets result to the size parameter, which is
// passed by reference.  This function also returns a string representation of
// the output file, which is particularly useful for testing.
//
string encode(ifstream &input, hashmapE &encodingMap, ofbitstream &output,
              int &size, bool makeFile)
{
    string str;
    char c;
    while (input.get(c))
    {
        str += encodingMap[c]; // adding every character from encoding map to string
    }
    str += encodingMap[PSEUDO_EOF]; // maunally add encodingMap
    size = str.length();            // change size accordingly
    if (makeFile)                   // encoding from a file, so we have to write to output to edit the file
    {
        for (int i = 0; i < size; i++) // write to the output stream
        {
            if (str[i] == '1')
                output.writeBit(1);
            else if (str[i] == '0')
                output.writeBit(0);
        }
    }
    return str;
}

//
// *This function decodes the input stream and writes the result to the output
// stream using the encodingTree.  This function also returns a string
// representation of the output file, which is particularly useful for testing.
//
string decode(ifbitstream &input, HuffmanNode *encodingTree, ofstream &output)
{
    string str;
    HuffmanNode *root = encodingTree;
    while (!input.eof()) // read until end of file
    {
        int bit = input.readBit();
        if (encodingTree->character != NOT_A_CHAR) // found a char/leaf
        {
            str += encodingTree->character;      // add to string
            output.put(encodingTree->character); // write to output stream
            encodingTree = root;                 // reset to root
        }
        if (bit == 1) // go right
        {
            encodingTree = encodingTree->one;
        }
        else if (bit == 0) // go left
        {
            encodingTree = encodingTree->zero;
        }
        if (encodingTree->character == PSEUDO_EOF) // once PSEUDO_EOF is found, stop iterating
        {
            break;
        }
    }
    return str;
}

//
// *This function completes the entire compression process.  Given a file,
// filename, this function (1) builds a frequency map; (2) builds an encoding
// tree; (3) builds an encoding map; (4) encodes the file (don't forget to
// include the frequency map in the header of the output file).  This function
// should create a compressed file named (filename + ".huf") and should also
// return a string version of the bit pattern.
//
string compress(string filename)
{
    string str;
    hashmapF frequencyMap;
    ifstream input(filename);              // input file to compress
    ofbitstream output(filename + ".huf"); // name of output file
    buildFrequencyMap(filename, true, frequencyMap);
    output << frequencyMap;                              // frequency map will be contents of file
    HuffmanNode *root = buildEncodingTree(frequencyMap); // set root to be top of priorityqueue
    hashmapE encodingMap = buildEncodingMap(root);       // create encodingMap to use for encode()
    int size = 0;
    str = encode(input, encodingMap, output, size, true); // encode string
    freeTree(root);                                       // free memory
    return str;
}

//
// *This function completes the entire decompression process.  Given the file,
// filename (which should end with ".huf"), (1) extract the header and build
// the frequency map; (2) build an encoding tree from the frequency map; (3)
// using the encoding tree to decode the file.  This function should create a
// compressed file using the following convention.
// If filename = "example.txt.huf", then the uncompressed file should be named
// "example_unc.txt".  The function should return a string version of the
// uncompressed file.  Note: this function should reverse what the compress
// function did.
//
string decompress(string filename)
{
    string str;
    hashmapF frequencyMap;
    ifbitstream input(filename);               // file to decompress
    size_t txthuf = filename.find(".txt.huf"); // find position file extension
    filename.erase(txthuf);                    // delete
    string newFile = filename + "_unc.txt";    // add _unc.txt at the end
    ofstream output(newFile);                  // this will be the name of the new file
    input >> frequencyMap;                     // taking input from the frequency map from compressed file
    HuffmanNode *root = buildEncodingTree(frequencyMap);
    str = decode(input, root, output); // decode tree and save into string
    freeTree(root);                    // free memory
    return str;                        // this will be output to the file
}

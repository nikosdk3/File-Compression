# File-Compression
File compression/decompression software using Huffman Encoding/Decoding

## Usage

- Clone this repository
- Run `make build run`

### Compression
- Type `C` once the options menu appears
- Enter the filename that you want to compress

### Decompression
- Type `D` once the options menu appears
- Enter the filename that you want to decompress

### Other menu options
- Menu options 1-6 are for going through the encoding/decoding process step-by-step. Compress and decompress simply combine all these steps together.
- Menu options `T` and `B` are for viewing txt files either in their regular text form or in binary.

## Sample execution (with explanation)

Lets say you want to compress the file `medium.txt` and then decompress it after.

```
Welcome to the file compression app!
1.  Build character frequency map
2.  Build encoding tree
3.  Build encoding map
4.  Encode data
5.  Decode data
6.  Free tree memory

C.  Compress file
D.  Decompress file

B.  Binary file viewer
T.  Text file viewer
Q.  Quit

Enter choice: C
Enter filename: medium.txt
```

This will generate a file called medium.txt.huf, which contains the ASCII values of all the characters in the text file along with their frequencies. This is formatted with the ASCII values to the left side of the colon, and the frequency of that ASCII value to the right side of the colon. The random characters at the end of the file represent the binary tree which is used for decompression. To decompress the file we have just encoded, run the following:

```
Welcome to the file compression app!
1.  Build character frequency map
2.  Build encoding tree
3.  Build encoding map
4.  Encode data
5.  Decode data
6.  Free tree memory

C.  Compress file
D.  Decompress file

B.  Binary file viewer
T.  Text file viewer
Q.  Quit

Enter choice: D
Enter filename: medium.txt.huf
```

This will rebuild the txt file to the original way it was by traversing the binary tree from the encoded file and storing it in a file renamed to `medium_unc.txt`.

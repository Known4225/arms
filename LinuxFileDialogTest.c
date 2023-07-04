/* UbuntuFileDialogTest with zenity */

#include <stdio.h>

int main() {
    char filename[4096];
    FILE* filenameStream = popen("zenity --file-selection", "r"); // returns a pointer to a stream of data that contains only the filepath
    // popen with (zenity --file-selection) will not return a FILE* to the location of the file. You cannot read the file from this FILE*
    // I have therefore named it filenameStream to represent what it is
    fgets(filename, 4096, filenameStream); // filepaths in ubuntu can be 4096 bytes long
    printf("%s\n", filename);
}
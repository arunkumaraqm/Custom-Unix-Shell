#include<stdio.h>
#include <unistd.h>
#include <fcntl.h>
int main()
{

    int file_desc = open("dup.txt", O_WRONLY | O_APPEND);

    if(file_desc < 0)
        printf("Error opening the file\n");

    // dup() will create the copy of file_desc as the copy_desc
    // then both can be used interchangeably.

    int copy_desc = dup(file_desc);

    //------------DOUBT----------------
    write(copy_desc,"This will be output to the file named dup.txt\n", 46); // what do 46 and 51 mean?
    write(file_desc,"This will also be output to the file named dup.txt\n", 51);
    return 0;
}

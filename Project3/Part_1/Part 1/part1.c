#include <stdio.h>
#include <stdlib.h> 

static void evaluate(int n, int m, int v,int c) 
{

	int pagenum = v>>n;
	int offset = v%(1<<n);
	//The two above lines will calculate the actual pagenum and offset by using bitwise operations to calculate those values.
  	printf("The virtual address is v%d and the page number is %d and the offset is %d\n", c,pagenum,offset);
}

int main()
{
	FILE *fptr;
	fptr=fopen("./input1.txt","r");//Remember to enter the right file path.
	if(fptr == NULL){
		printf("Error!");   
		return 1;     
		//This block of code checks to see if any file has been detected, if not, then an error is printed and the code stops.       
	}

	int ch;
	int numLines=0;
    while (EOF != (ch=getc(fptr)))
        if ('\n' == ch)
            ++numLines;
	// An extra line is needed because the \n is used to correctly determine the number of lines, else you'll get n-1 lines detected.

	fptr=fopen("./input1.txt","r");
	//Input file has now bean re-loaded into fptr variable, and now the integers must be loaded in
	char buffer[100];
	fgets(buffer,100,fptr);
	//Increments the file for the first line and the below line stores the offset in bits
	int offset=atoi(buffer);
	fgets(buffer,100,fptr);
	//Increments the file for the second line and the below line stores the page number in bits
	int pageNum=atoi(buffer);

	//Above lines print out the offset and the page number.

	int count=1;
	for (int i=0;i<numLines-2;i++){
		fgets(buffer,100,fptr);
		int v = atoi(buffer);
		// Obtains the virtual address from the .txt file and then the below line calls the evaluate function on it.
		evaluate(offset,pageNum,v,count);
		count++;
	}
	return 0;
		
}
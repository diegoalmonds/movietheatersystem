//Diego Olmos 1002045722

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "BSTLib.h" //functions for binary search tree stuff
#include "MovieTheaterLib.h" //interacts with seat maps
#include "QueueLib.h" //queue functions for customer line
#include "ListLib.h" //linked list functions for tickets
#include "StackLib.h" //stack functions for record of all receipts

void PrintReceipts(SNODE **StackTop) //takes in address of top of receipt stack
{
	char ticket[4] = {};
	/* 
	   if the StackTop is empty, the print appropriate message - see example output - 
	   else print out the receipts.  A receipt is one node of the stack and each receipt
	   consists of a receipt number, a movie theater name and a TicketList (a linked list
	   of all seats sold to one customer).  Use ReturnAndFreeLinkedListNode() to traverse 
	   the linked list stored in each stack node and display the seats and free the 
	   linked list nodes. Each call to ReturnAndFreeLinkedListNode() brings back ONE ticket 
	   from the linked list in the Ticket variable.  Call pop() to remove the stack node.
	*/
	if (*StackTop == NULL) //dereferences stacktop, if NULL then there are no receipts
	{
		printf("No receipts.");
	}
	else
	{
		printf("\n\n\nToday's receipts");
		while (*StackTop != NULL)
		{
			printf("\n\n\nReceipt #%d\n\n\n", (*StackTop)->ReceiptNumber); //derefrence top and access receipt #
			printf("\t%s\n\n\n", (*StackTop)->MovieTheaterName); //same as above
			while ((*StackTop)->TicketList != NULL)
			{
				ReturnAndFreeLinkedListNode(&(*StackTop)->TicketList, ticket); 
				printf("\t%s", ticket);
			}
			pop(StackTop); //remove the processed ticket from the stack
		}
	}
}

BNODE *PickAndDisplayTheater(BNODE *BSTRoot, char MovieTheaterMap[][MAXCOLS], int *MapRow, int *MapCol)
{
	BNODE *MyTheater = NULL;
	char zip[6] = {};
	char MyDims[6] = {};
	
	printf("\n\nPick a theater by entering the zipcode\n\n");
				
	// Traverse the BST in order and print out the theaters in zip code order - use InOrder()
	InOrder(BSTRoot);
	// Prompt for a zip
	printf("\n\nEnter zip ");
	scanf("%s", zip);
	// Call SearchForBNODE()
	MyTheater = SearchForBNODE(BSTRoot, zip);
	// If theater is not found, then print message
	if (MyTheater == NULL)
	{
		printf("A theater was not found for that zip code.");
	}
	// If theater is found, then tokenize the theater's dimensions and
	// pass those dimensions and the MovieTheaterMap and the theater's filename
	// to ReadMovieTheaterFile()
	else
	{
		strcpy(MyDims, MyTheater->Dimensions);
		*MapRow = atoi(strtok(MyDims, "x"));
		*MapCol = atoi(strtok(NULL, "x"));
		if (ReadMovieTheaterFile(MovieTheaterMap, MyTheater->FileName, *MapRow, *MapCol) == 0)
		{
			printf("Unable to print that seat map at this time. Check back later.");
		}
		else
		{
			PrintSeatMap(MovieTheaterMap, *MapRow, *MapCol);
		}
	}

	// If ReadMovieTheaterFile() returns FALSE, then print
	// "Unable to print that seat map at this time.  Check back later."
	// else call PrintSeatMap()

	// return the found theater
	return MyTheater;
}	

void ReadFileIntoQueue(FILE *QueueFile, QNODE **QH, QNODE **QT)
{
	char customer[30] = {};
	while (fgets(customer, sizeof(customer)-1, QueueFile))
	{
		if (customer[strlen(customer)-1] == '\n')
			customer[strlen(customer)-1] = '\0';
		enQueue(customer, QH, QT);
	}
	//	read the passed in file and calls enQueue for each name in the file
}

void ReadFileIntoBST(FILE *BSTFile, BNODE **BSTnode)
{
	char FileLine[151] = {};
	char MovieTheaterName[41] = {};
	char ZipCode[6] = {};
	char FileName[101] = {};
	char Dimensions[6] = {};
	while (fgets(FileLine, sizeof(FileLine)-1, BSTFile))
	{
		if (FileLine[strlen(FileLine)-1] == '\n')
			FileLine[strlen(FileLine)-1] = '\0';
		memset(MovieTheaterName, '\0', sizeof(MovieTheaterName));
		strcpy(MovieTheaterName, strtok(FileLine, "|"));
		memset(ZipCode, '\0', sizeof(ZipCode));
		strcpy(ZipCode, strtok(NULL, "|"));
		memset(FileName, '\0', sizeof(FileName));
		strcpy(FileName, strtok(NULL, "|"));
		memset(Dimensions, '\0', sizeof(Dimensions));
		strcpy(Dimensions, strtok(NULL, "|"));
		AddBSTNode(BSTnode, MovieTheaterName, ZipCode, FileName, Dimensions);
	}
	//	read the passed in file and tokenize each line and pass the information to AddBSTNode
}

void get_command_line_parameter(char *argv[], char ParamName[], char ParamValue[])
{
	int i = 0; 
	
	while (argv[++i] != NULL)
	{
		if (!strncmp(argv[i], ParamName, strlen(ParamName)))
		{
			strcpy(ParamValue, strchr(argv[i], '=') + 1);
		}
	}
}

int PrintMenu()
{
	int choice = 0;
	
	printf("\n\n1.	Sell tickets to next customer\n\n");
	printf("2.	See who's in line\n\n");
	printf("3.	See the seat map for a given theater\n\n");
	printf("4.	Print today's receipts\n\n");
	printf("Choice : ");
	scanf("%d", &choice);
	
	while (choice < 1 || choice > 4)
	{
		printf("Invalid choice.  Please reenter. ");
		scanf("%d", &choice);
	}
	
	return choice;
}

int main(int argc, char *argv[])
{
	int i, j, k;
	FILE *queueFile = NULL;
	FILE *zipFile = NULL;
	char arg_value[20] = {};
	char queuefilename[20] = {};
	char zipfilename[20] = {};
	int ReceiptNumber = 0;
	int choice = 0;
	int SeatNumber;
	char Row;
	char Ticket[5] = {};
	int ArrayRow, ArrayCol;
	int MapRow, MapCol;
	char MovieTheaterMap[MAXROWS][MAXCOLS] = {};
	LNODE *TicketLinkedListHead = NULL;
	QNODE *QueueHead = NULL;
	QNODE *QueueTail = NULL;
	BNODE *BSTRoot = NULL;
	BNODE *MyTheater = NULL;
	SNODE *StackTop = NULL;
	int NumberOfTickets = 0;
	
	if (argc != 4)
	{
		printf("%s QUEUE=xxxxxx ZIPFILE=xxxxx RECEIPTNUMBER=xxxxx\n", argv[0]);
		exit(0);
	}
	
	get_command_line_parameter(argv, "QUEUE=", queuefilename);
	get_command_line_parameter(argv, "ZIPFILE=", zipfilename);
	get_command_line_parameter(argv, "RECEIPTNUMBER=", arg_value);
	ReceiptNumber = atoi(arg_value);
		
	/* call function to open queuefilename - if it does not open, print message and exit */	
	queueFile = fopen(queuefilename, "r");
	if (queueFile == NULL)
	{
		printf("%s did not open.", queuefilename);
		exit(0);
	}
	
	/* calll function to open zipfilename - if it does not open, print message and exit */
	zipFile = fopen(zipfilename, "r");
	if (zipFile == NULL)
	{
		printf("%s did not open", zipfilename);
		exit(0);
	}

	ReadFileIntoQueue(queueFile, &QueueHead, &QueueTail);
	ReadFileIntoBST(zipFile, &BSTRoot);
	
	while (QueueHead != NULL)
	{
		choice = PrintMenu();
	
		switch (choice)
		{
			case 1 :
				printf("\n\nHello %s\n", QueueHead->name);				
				MyTheater = PickAndDisplayTheater(BSTRoot, MovieTheaterMap, &MapRow, &MapCol);
				if (MyTheater != NULL)
				{
					printf("\n\nHow many movie tickets do you want to buy? ");
					scanf("%d", &NumberOfTickets);
					for (i = 0; i < NumberOfTickets; i++)
					{
						do
						{
							printf("\nPick a seat (Row Seat) ");
							scanf(" %c%d", &Row, &SeatNumber);
							Row = toupper(Row);
							ArrayRow = (int)Row - 65;
							ArrayCol = SeatNumber - 1;
						
							if ((ArrayRow < 0 || ArrayRow >= MapRow) ||
								(ArrayCol < 0 || ArrayCol >= MapCol))
							{
								printf("\nThat is not a valid seat.  Please choose again\n\n");
							}		
						}
						while ((ArrayRow < 0 || ArrayRow >= MapRow) ||
							   (ArrayCol < 0 || ArrayCol >= MapCol));
						
						if (MovieTheaterMap[ArrayRow][ArrayCol] == 'O')
						{	
							MovieTheaterMap[ArrayRow][ArrayCol] = 'X';
							sprintf(Ticket, "%c%d", Row, SeatNumber); 
							InsertNode(&TicketLinkedListHead, Ticket);
						}
						else
						{
							printf("\nSeat %c%d is not available.\n\n", Row, SeatNumber);
							i--;
						}
						PrintSeatMap(MovieTheaterMap, MapRow, MapCol);
					}
					
					WriteSeatMap(MyTheater, MovieTheaterMap, MapRow, MapCol);
					push(&StackTop, TicketLinkedListHead, ReceiptNumber, MyTheater->MovieTheaterName);
					TicketLinkedListHead = NULL;
					ReceiptNumber++;
					printf("\nThank you %s - enjoy your movie!\n", QueueHead->name);
					deQueue(&QueueHead);
				}
				break;
			case 2 : 
				printf("\n\nCustomer Queue\n\n");
				DisplayQueue(QueueHead);
				printf("\n\n");
				break;
			case 3 :
				PickAndDisplayTheater(BSTRoot, MovieTheaterMap, &MapRow, &MapCol);
				break;
			case 4 : 
				PrintReceipts(&StackTop);
				break;
			default :
				printf("Bad menu choice");
		}
	}
	
	printf("Good job - you sold tickets to all of the customers in line.\n");
	PrintReceipts(&StackTop);
	
	return 0;
}

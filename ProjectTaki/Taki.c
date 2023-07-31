#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_NAME_LEN 20 //the max number of letter player's name can has
#define START_CARDS_SIZE 4//
#define CARD_ROWS 6		//each card has 6 rows
#define CARD_COLS 9		//each card has 9 columns
#define MAX_TYPE_SIZE 6 //the max letter type is "color" which is 6(include \0)
#define NUMBER_OF_COLORS 4 //there are 4 kind of colors
#define NUMBER_OF_REGULAR_TYPE 9 //therea are 9 regular cards
#define NUMBER_OF_TYPES 14 //there are 14 diiferent types of cards includes special cards
#define UNINITIALIZED -1 //if the card after the rand got "COLOR" the value of its color should be uninitialized at first
#define EMPTY 0				// 
#define DRAW_ANOTHER_CARD 0  //there are two const for 0 inorder to place the right one in each situation and then make the code more readable

// card types
#define TYPE_PLUS 10
#define TYPE_STOP 11
#define TYPE_CHANGE_CIRECTION 12
#define TYPE_TAKI 13
#define TYPE_COLOR 14

struct Card
{
	int typeValue;   //can get values between 1-14
					 //1-9 wil be regular cards
					 //10 is plus(+)
					 //11 is stop
					 //12 is change direction(<->)
					 //13 is taki
					 //14 is change color
	int colorValue;  //can get values between 1-4
					 //1 is yellow
					 //2 is red
					 //3 is blue
					 //4 is green
	char type[MAX_TYPE_SIZE];   //We will use this array in order to print the card type in the middle of the card
	char color;					//We will use this in order to print the card color in the middle of the card
};
typedef struct Card CARD;

struct Player
{
	char name[MAX_NAME_LEN];  //Player's name
	CARD* hand;               //A set of cards that the player holds in each turn
	int numberOfCards;        //How manu cards the player has
	int optionChose;          //The option that th player picked
	int phySize;
};
typedef struct Player PLAYER;

struct CardInfo
{
	int CardType;				//the type of the card
	int numberOfPerformances;	//how many times the card have been drawn
};
typedef struct CardInfo CARD_INFO;


void welcome();
int getNumberOfPlayers();
void getPlayersNames(PLAYER* playersData, int numOfPlayers);
PLAYER* createPlayersData(int numOfPlayers);
CARD createRndCard();
void initializePlayerData(PLAYER* playersData, int numOfPlayers);
void initializeDeckInfo(CARD_INFO deckInfo[]);
void drawCards(PLAYER* playersData, int numOfPlayers, CARD_INFO deckInfo[]);
void drawUpperCard(CARD* uppercard, CARD_INFO deckInfo[]);
CARD castValueIntoType(CARD card);
CARD castValueIntoColor(CARD card);
void printCardPattern(char type[], char color);
void printPlayerCards(PLAYER player);
void printUpperCard(CARD uppercard);
CARD* CardRealloc(CARD* arr, int size, int newSize);
void getPlayerOptions(PLAYER* player, CARD upperCard);
bool isSpecialCard(int cardType);
void handleRegularCard(PLAYER* player, CARD* upperCard, bool* isCardHandeled,bool* lastCardWasThrown);
void handleSpecialCard(PLAYER* player, CARD* upperCard, bool* isCardHandeled,bool* lastCardWasThrown);
void drawAnotherCard(PLAYER* player, bool* isCardHandeled, CARD_INFO deckInfo[]);
bool areColorsSuitable(PLAYER* player, CARD* upperCard);
bool areTypesSuitable(PLAYER* player, CARD* upperCard);
void swap(CARD* card1, CARD* card2);
void removeCard(PLAYER* player,bool* lastCardWasThrown);
void resetUpperCard(PLAYER* player, CARD* upperCard);
void resetCard(CARD* card);
void takiOptions(PLAYER* player, CARD upperCard);
void colorOptions(PLAYER* player);
void deckInfoUpdate(CARD_INFO deckInfo[], CARD card);
void GetTurnAndDirection(PLAYER player, bool* shouldBeIncrement, CARD* upperCard, int* playerIndex, int numberOfPlayers);
void whoPlaysNext(int* playerIndex, int numberOfPlayers, bool shouldBeIncrement);
void specialCases(PLAYER* player, CARD* upperCard, int numOfPlayers, int* playerIndex, bool shouldBeIncrement, CARD_INFO deckInfo[]);
bool takiEndedWithColor(CARD upperCard);
void copyArr(CARD_INFO dest[], CARD_INFO src[], int size);
void merge(CARD_INFO a1[], int n1, CARD_INFO a2[], int n2, CARD_INFO res[]);
void mergeSort(CARD_INFO arr[], int size);
void printStatistics(CARD_INFO deckInfo[]);
void freePlayersData(PLAYER* playersData, int numOfPlayers);
void gameProcess();

void main()
{
	srand(time(NULL));
	gameProcess();
}

void welcome()//This function welcomes the user
{
	printf("************  Welcome to TAKI game !!! ***********\n");
}
int getNumberOfPlayers()//Gets from the user the number of players that will play
{
	int numOfPlayers;
	printf("Please enter the number of players:\n");
	scanf_s("%d", &numOfPlayers);
	return numOfPlayers;
}
void getPlayersNames(PLAYER* playersData, int numOfPlayers)//Get from the user the name of each player
{
	int i;
	for (i = 0; i < numOfPlayers; i++)
	{
		printf("Please enter the first name of player #%d:\n", i+1);
		scanf("%s",playersData[i].name);
		printf("\n");
	}
}
PLAYER* createPlayersData(int numOfPlayers)//Creates an  dynamic array of players
{
	PLAYER* playersData = (PLAYER*)malloc(sizeof(PLAYER) * numOfPlayers);
	if (playersData == NULL)
	{
		printf("Memory allocation failed!!!\n");
		exit(1);
	}
	return playersData;
}
CARD createRndCard()//This function draw a randon card with type and color.
{
	CARD temp;
	temp.typeValue = (rand() % NUMBER_OF_TYPES) + 1;
	temp = castValueIntoType(temp);
	temp.colorValue = (rand() % NUMBER_OF_COLORS) + 1;
	if (temp.typeValue == TYPE_COLOR)		//which means the type of the card will be "COLOR"
	{
		temp.colorValue = UNINITIALIZED;     //in that case we will itialize the colorValue with p-1
	}
	temp = castValueIntoColor(temp);
	return temp;
}
void initializePlayerData(PLAYER* playersData, int numOfPlayers)//This function initialize the playerData and create an dynymic array of cards for eeach play
{
	int i;
	for (i = 0; i < numOfPlayers; i++)
	{
		playersData[i].hand = NULL;
		playersData[i].hand = (CARD*)malloc(sizeof(CARD) * START_CARDS_SIZE);
		if (playersData[i].hand == NULL)
		{
			printf("Memory allocation failed!!!\n");
			exit(1);
		}
		playersData[i].numberOfCards = EMPTY;
		playersData[i].optionChose = UNINITIALIZED;
		playersData[i].phySize = START_CARDS_SIZE;
	}
}
void initializeDeckInfo(CARD_INFO deckInfo[])//This function initialize the deck info,
{
	int i;
	for (i = 0; i < NUMBER_OF_TYPES; i++)
	{
		deckInfo[i].CardType = i + 1;   //every elements will get the equal card
		deckInfo[i].numberOfPerformances = 0;
	}
}
void deckInfoUpdate(CARD_INFO deckInfo[], CARD card)//This function increase the number of performances of the same card
{
	int numberOfTheCard = card.typeValue - 1;
	deckInfo[numberOfTheCard].numberOfPerformances++;
}
void drawCards(PLAYER* playersData, int numOfPlayers, CARD_INFO deckInfo[])//this function draws for each player 4 start cards
{
	int i,j;
	for (i = 0; i < numOfPlayers; i++)
	{
		for (j = 0; j < START_CARDS_SIZE; j++)
		{
			playersData[i].hand[j] = createRndCard();
			playersData[i].numberOfCards++;
			deckInfoUpdate(deckInfo, playersData[i].hand[j]);
		}
	}
}
void drawUpperCard(CARD* uppercard, CARD_INFO deckInfo[])//This function draw a randon uppercard with color and type(type can get only regualr number 1-9)
{
	uppercard->colorValue = (rand() % NUMBER_OF_COLORS) + 1;
	uppercard->typeValue = (rand() % NUMBER_OF_REGULAR_TYPE) + 1;
	*uppercard = castValueIntoType(*uppercard);
	*uppercard = castValueIntoColor(*uppercard);
	deckInfoUpdate(deckInfo, *uppercard);
}
CARD castValueIntoType(CARD card)//This function cast the typeValue of the card into a string type
{
	char temp[MAX_TYPE_SIZE];
	temp[0] = '0';

	switch (card.typeValue)
	{
	case 10:
	{
		strcpy(card.type, "+");
		break;
	}
	case 11:
	{
		strcpy(card.type, "STOP");
		break;
	}
	case 12:
	{
		strcpy(card.type, "<->");
		break;
	}
	case 13:
	{
		strcpy(card.type, "TAKI");
		break;
	}
	case 14:
	{
		strcpy(card.type, "COLOR");
		break;
	}
	default:
	{
		temp[0] += card.typeValue; //which means it rugaulr number between 1-9
		temp[1] = 0;               //"\0"
		strcpy(card.type, temp);
		break;
	}
	}
	return card;

}
CARD castValueIntoColor(CARD card)//This function cast the colorValue of the card into a char color
{
	switch (card.colorValue)
	{
	case 1:
	{
		card.color = 'Y';
		break;
	}
	case 2:
	{
		card.color = 'R';
		break;
	}
	case 3:
	{
		card.color = 'B';
		break;
	}
	case 4:
	{
		card.color = 'G';
		break;
	}
	case -1:
		card.color = ' ';
		break;
	}
	return card;
}
void printCardPattern(char type[], char color)//This function prints 9x6 card with type and color in the middle
{
	int len = strlen(type);
	printf("*********\n");
	printf("*       *\n");
	if (len == 1)					//which means type is single letter(1-9,+)
		printf("*   %s   *\n", type);
	else if (len == 3)				//which means type is <->
		printf("*  %s  *\n", type);
	else if (len == 4)				//which means type is STOP or TAKI
		printf("*  %s *\n", type);
	else if (len == 5)				//which means type is COLOR
		printf("* %s *\n", type);
	printf("*   %c   *\n", color);
	printf("*       *\n");
	printf("*********\n");

}
void printPlayerCards(PLAYER player)//this function prints the card for the play which has to play
{
	int CardNumber;
	printf("%s's turn:\n\n", player.name);
	for (CardNumber = 0; CardNumber < player.numberOfCards; CardNumber++)
	{
		printf("Card #%d\n", CardNumber + 1);
		printCardPattern(player.hand[CardNumber].type, player.hand[CardNumber].color);
		printf("\n");
	}
}
void printUpperCard(CARD uppercard)//this function prints the uppercard
{
	printf("Upper card:\n");
	printCardPattern(uppercard.type, uppercard.color);
	printf("\n");
}
void getPlayerOptions(PLAYER* player, CARD upperCard)//This function ask for an option from the player,the option indicates which card he wants to throw to the center
													//If the option is invalid interm of wrong number or invalid card it return an propper messege
{
	bool isLegal = false;

	printf("Please enter 0 if you want to take a card from the deck\n");
	if (player->numberOfCards > 1)
	{
		printf("or 1-%d if you want to put one of your cards in the middle:\n", player->numberOfCards);
	}
	else
	{
		printf("or 1 if you want to put one of your cards in the middle:\n");
	}
	while (!isLegal)
	{
		scanf("%d", &player->optionChose);
		int optionChose = (*player).optionChose; //instead of wrtine (*player).optionChose over and over

		if (optionChose == 0)
			isLegal = true;
		else if (optionChose > player->numberOfCards || optionChose < 0)
		{
			printf("Invalid choice! Try again.\n");
			printf("Please enter 0 if you want to take a card from the deck\n");
			if (player->numberOfCards > 1)
			{
				printf("or 1-%d if you want to put one of your cards in the middle:\n", player->numberOfCards);
			}
			else
			{
				printf("or 1 if you want to put one of your cards in the middle:\n");
			}
		}
		else if ((player->hand[optionChose-1].colorValue != upperCard.colorValue)&&(player->hand[optionChose - 1].typeValue != upperCard.typeValue))
		{
			if (player->hand[optionChose-1].colorValue == UNINITIALIZED) //incase of the card is color which can suit to every card
			{
				isLegal = true;
			}
			else
			{
				printf("Invalid card! Try again.\n");
				printf("Please enter 0 if you want to take a card from the deck\n");
				if (player->numberOfCards > 1)
				{
					printf("or 1-%d if you want to put one of your cards in the middle:\n", player->numberOfCards);
				}
				else
				{
					printf("or 1 if you want to put one of your cards in the middle:\n");
				}
			}
		}
		else
			isLegal = true;
	}
}
bool isSpecialCard(int cardType)//This function checks if the card is special card
{
	return (cardType > 9);
}
void handleRegularCard(PLAYER* player, CARD* upperCard, bool* isCardHandeled,bool* lastCardWasThrown)//This function deals with cards with typeValue between 1-9,aka "regular cards"
{
	if (areColorsSuitable(player,upperCard)||areTypesSuitable(player,upperCard))//the card can be thrown to the center as long as it has proper color or type
	{
		resetUpperCard(player, upperCard);
		removeCard(player,lastCardWasThrown);
		*isCardHandeled = true;
	}
}
void handleSpecialCard(PLAYER* player, CARD* upperCard, bool* isCardHandeled,bool* lastCardWasThrown)//This function deals with cards with typeValue between 10-14,aka "special cards"
{
	int option = player->optionChose - 1, endTaki = 0;
	switch (player->hand[option].typeValue)
	{
	case TYPE_CHANGE_CIRECTION:// all the three type has the same routine
	case TYPE_PLUS:
	case TYPE_STOP:
	{
		if (areColorsSuitable(player, upperCard))
		{
			resetUpperCard(player, upperCard);
			removeCard(player,lastCardWasThrown);
		}
		else if (areTypesSuitable(player, upperCard))
		{
			resetUpperCard(player, upperCard);
			removeCard(player,lastCardWasThrown);
		}
		break;
	}
	case TYPE_TAKI:
	{
		if (areColorsSuitable(player, upperCard))
		{
			resetUpperCard(player, upperCard);
			removeCard(player, lastCardWasThrown);
			if ((*player).numberOfCards != 0)
			{
				printUpperCard(*upperCard);
				printPlayerCards(*player);
				takiOptions(player, *upperCard);
				if ((*player).optionChose != 0)
				{
					resetUpperCard(player, upperCard);
					removeCard(player, lastCardWasThrown);
				}
				while ((*player).numberOfCards != 0 && (*player).optionChose != 0)//as long as the player doesnt select zero,he still can throw proper cards
				{																//In addition the player can throw cards as long as he has some cards in his hand.

					if (!takiEndedWithColor(*upperCard))//if the player throw COLOR in "taki fall" the taki should over
					{
						printUpperCard(*upperCard);
						printPlayerCards(*player);
						takiOptions(player, *upperCard);
						if ((*player).optionChose != 0)
						{
							resetUpperCard(player, upperCard);
							removeCard(player, lastCardWasThrown);
						}
					}
					else
					{
						colorOptions(player);
						(*upperCard).colorValue = (*player).optionChose-1;
						*upperCard = castValueIntoColor(*upperCard);
						player->optionChose = endTaki;
					}

				}
			}
		}
		else if (areTypesSuitable(player, upperCard))
		{
			resetUpperCard(player, upperCard);
			removeCard(player, lastCardWasThrown);
			if ((*player).numberOfCards != 0)
			{
				printUpperCard(*upperCard);
				printPlayerCards(*player);
				takiOptions(player, *upperCard);
				if ((*player).optionChose != 0)
				{
					resetUpperCard(player, upperCard);
					removeCard(player, lastCardWasThrown);
				}
				while ((*player).numberOfCards != 0 && (*player).optionChose != 0)//as long as the player doesnt select zero,he still can throw proper cards
				{																//In addition the player can throw cards as long as he has some cards in his hand.

					if (!takiEndedWithColor(*upperCard))//if the player throw COLOR in "taki fall" the taki should over
					{
						printUpperCard(*upperCard);
						printPlayerCards(*player);
						takiOptions(player, *upperCard);
						if ((*player).optionChose != 0)
						{
							resetUpperCard(player, upperCard);
							removeCard(player, lastCardWasThrown);
						}
					}
					else
					{
						colorOptions(player);
						(*upperCard).colorValue = (*player).optionChose - 1;
						*upperCard = castValueIntoColor(*upperCard);
						player->optionChose = endTaki;
					}

				}
			}
		}
		break;
	}
	case TYPE_COLOR: 
	{
		colorOptions(player);
		resetUpperCard(player, upperCard);
		*upperCard = castValueIntoColor(*upperCard);
		removeCard(player,lastCardWasThrown);
		break;
	}
	}
	*isCardHandeled = true;
}
void drawAnotherCard(PLAYER* player, bool* isCardHandeled, CARD_INFO deckInfo[])//The player wants to draw anoter card
{
	int playerNumberOfCards = (*player).numberOfCards;
	int PlayerPhySize = (*player).phySize;
	CARD newCard;

	if (playerNumberOfCards == PlayerPhySize)//check if there is need to increase the pyshical size
	{
		(*player).hand = CardRealloc((*player).hand, playerNumberOfCards, playerNumberOfCards * 2);
		(*player).phySize *= 2;
	}

	(*player).numberOfCards++;
	(*player).hand[(*player).numberOfCards - 1] = createRndCard();

	newCard = (*player).hand[(*player).numberOfCards - 1];

	deckInfoUpdate(deckInfo, newCard);
	*isCardHandeled = true;
}
CARD* CardRealloc(CARD* arr, int size, int newSize)//This function re allocate a dynamic array with a new size
{
	int i;
	CARD* newArr = NULL;
	newArr = (CARD*)malloc(sizeof(CARD) * newSize);
	if (newArr == NULL)
	{
		printf("Memory allocation failed!!!\n");
		exit(1);
	}
	for (i = 0; i < size&&i<newSize; i++)
	{
		newArr[i] = arr[i];
	}
	arr = NULL;
	free(arr);
	return newArr;
}
bool areColorsSuitable(PLAYER* player, CARD* upperCard)//This function checks if the player's selction is suit to the upper cards in term of colorValue
{
	if (player->hand[player->optionChose - 1].colorValue == upperCard->colorValue)
		return true;
	else if (player->hand[player->optionChose - 1].colorValue == UNINITIALIZED)
		return true;
	return false;
}
bool areTypesSuitable(PLAYER* player, CARD* upperCard)//This function checks if the player's selction is suit to the upper cards in term of typeValue
{
	return player->hand[player->optionChose-1].typeValue == upperCard->typeValue;
}
void swap(CARD* card1, CARD* card2)//This fucntion swaps between two cards,every cards gets the other adress
{
	CARD temp = *card1;
	*card1 = *card2;
	*card2 = temp;
}

void removeCard(PLAYER* player,bool* lastCardWasThrown)//Because there is no demend to make sure the physical and logic  size are equal,
														//the function swap with the player's selction card and the last card,and reset the value of the last cards
{
	swap(&player->hand[player->optionChose-1], &player->hand[player->numberOfCards - 1]);
	resetCard(&player->hand[player->numberOfCards - 1]);
	player->numberOfCards--;
	if(player->numberOfCards==0)
	*lastCardWasThrown=true;
}
void resetUpperCard(PLAYER* player, CARD* upperCard)//this function turn the player's selction card into the uppercard
{
	*upperCard = player->hand[player->optionChose-1];
}
void resetCard(CARD* card)//This function resets the values of the cards
{
	card->colorValue = UNINITIALIZED;
	card->typeValue = UNINITIALIZED;
}
void takiOptions(PLAYER* player, CARD upperCard)//This function shows the possible option when players used "TAKI"
{
	bool isLegal = false;
	int endTaki = 0;
	printf("Please enter 0 if you want to finish your turn\n");
	if (player->numberOfCards > 1)
	{
		printf("or 1-%d if you want to put one of your cards in the middle:\n", player->numberOfCards);
	}
	else
	{
		printf("or 1 if you want to put one of your cards in the middle:\n");
	}

	while (!isLegal)//as long as the card has the same color
	{
		scanf("%d", &player->optionChose);
		int optionChose = player->optionChose;

		if (optionChose == endTaki)//if the player wants to stop the taki
			isLegal = true;
		else if (optionChose > player->numberOfCards || optionChose < endTaki)//checks if the option are relevent to the number of cards the player has
		{
			printf("Invalid choice! Try again.\n");
			printf("Please enter 0 if you want to take a card from the deck\n");
			if (player->numberOfCards > 1)
			{
				printf("or 1-%d if you want to put one of your cards in the middle:\n", player->numberOfCards);
			}
			else
			{
				printf("or 1 if you want to put one of your cards in the middle:\n");
			}
		}
		else if (player->hand[optionChose-1].colorValue != upperCard.colorValue)//in "TAKI" the player can choose only cards with the same color the "TAKI" card has
 		{
			if (player->hand[optionChose-1].colorValue == UNINITIALIZED)//the only "differnt" card the player can throw is "COLOR"
			{
				isLegal = true;
			}
			else
			{
				printf("Invalid card! Try again.\n");
				printf("Please enter 0 if you want to take a card from the deck\n");
				if (player->numberOfCards > 1)
				{
					printf("or 1-%d if you want to put one of your cards in the middle:\n", player->numberOfCards);
				}
				else
				{
					printf("or 1 if you want to put one of your cards in the middle:\n");
				}
			}
		}
		else
			isLegal = true;
	}
}
void colorOptions(PLAYER* player)//This function shows the possible option when players used "COLOR"
{
	int colorToSwitch;
	printf("Please enter your color choice:\n");
	printf("1 - Yellow\n");
	printf("2 - Red\n");
	printf("3 - Blue\n");
	printf("4 - Green\n");
	scanf("%d", &colorToSwitch);
	switch (colorToSwitch)
	{
	default:
		player->hand[player->optionChose -1].colorValue = colorToSwitch;
		break;
	}
}
void whoPlaysNext(int* playerIndex,int numberOfPlayers,bool shouldBeIncrement)//This function plays with the index when have to and decides which player should play according to the flow of the game
{
	int startInd =0,endInd = numberOfPlayers-1;

		if (*playerIndex >= numberOfPlayers)
			*playerIndex = shouldBeIncrement ? startInd : endInd;
		else if (*playerIndex <= UNINITIALIZED)
			*playerIndex = shouldBeIncrement ? startInd : endInd;

}
void GetTurnAndDirection(PLAYER player, bool* shouldBeIncrement, CARD* upperCard, int* playerIndex, int numberOfPlayers)//This function increase the index of the player's turn according to the thrown card
{
	if (player.optionChose != DRAW_ANOTHER_CARD)
	{
		if (numberOfPlayers > 2)
		{
			switch ((*upperCard).typeValue)
			{
			case TYPE_STOP:
				*shouldBeIncrement ? ++(*playerIndex) : --(*playerIndex);
				break;
			case TYPE_PLUS:
				*shouldBeIncrement ? --(*playerIndex) : ++(*playerIndex);
				break;
			case TYPE_CHANGE_CIRECTION:
				*shouldBeIncrement = !(*shouldBeIncrement);
				break;
			case TYPE_TAKI:
				break;
			case TYPE_COLOR:
				break;
			default://regular number
				break;
			}
		}
		else
		{
			switch ((*upperCard).typeValue)
			{
			case TYPE_STOP:
			case TYPE_PLUS:
				*shouldBeIncrement ? --(*playerIndex) : ++(*playerIndex);
				break;
			case TYPE_CHANGE_CIRECTION:
				*shouldBeIncrement = !(*shouldBeIncrement);
				break;
			case TYPE_TAKI:
				break;
			case TYPE_COLOR:
				break;
			default://regular number
				break;
			}
		}

		*shouldBeIncrement ? ++(*playerIndex) : --(*playerIndex);
	}
	else
		*shouldBeIncrement ? ++(*playerIndex) : --(*playerIndex);
}
void specialCases(PLAYER* player, CARD* upperCard, int numOfPlayers, int* playerIndex, bool shouldBeIncrement, CARD_INFO deckInfo[])//There are two special cases in winning situation
{
	CARD newCard;
	if ((*upperCard).typeValue == TYPE_PLUS && player->numberOfCards == EMPTY) //Incase of the player throw "PLUS" as the last cards of his hand ,he has to take another card
	{
		(*player).hand[EMPTY] = createRndCard();
		newCard = (*player).hand[EMPTY];
		++(*player).numberOfCards;
		shouldBeIncrement ? ++(*playerIndex) : --(*playerIndex);
		deckInfoUpdate(deckInfo, newCard);
	}
	if ((*upperCard).typeValue == TYPE_STOP && player->numberOfCards == EMPTY && numOfPlayers == 2)//incase there are two players and one finish with "STOP",he has to take another card
	{
		(*player).hand[EMPTY] = createRndCard();
		newCard = (*player).hand[EMPTY];
		++(*player).numberOfCards;
		shouldBeIncrement ? ++(*playerIndex) : --(*playerIndex);
		deckInfoUpdate(deckInfo, newCard);
	}
}
bool takiEndedWithColor(CARD upperCard)//This function check if the play throw "COLOR" in the "TAKI" fall
{
	return upperCard.typeValue == TYPE_COLOR;
}
void freePlayersData(PLAYER* playersData, int numOfPlayers)//free the cards dynemic array in the playersDate and free also playersData
{
	int i;
	for (i = 0; i < numOfPlayers; i++)
	{
		free(playersData[i].hand);
	}
	free(playersData);
}
void merge(CARD_INFO a1[], int n1, CARD_INFO a2[], int n2, CARD_INFO res[])//This function merge between two sorted arrays
{
	int ind1, ind2, resInd;
	ind1 = ind2 = resInd = 0;

	while ((ind1 < n1) && (ind2 < n2)) {
		if (a1[ind1].numberOfPerformances <= a2[ind2].numberOfPerformances)
		{
			res[resInd] = a1[ind1];
			ind1++;
		}
		else
		{
			res[resInd] = a2[ind2];
			ind2++;
		}
		resInd++;
	}

	while (ind1 < n1)
	{
		res[resInd] = a1[ind1];
		ind1++;
		resInd++;
	}
	while (ind2 < n2) {
		res[resInd] = a2[ind2];
		ind2++;
		resInd++;
	}
}
void copyArr(CARD_INFO dest[], CARD_INFO src[], int size)//copy the values of one array to another
{
	int i;
	for (i = 0; i < size; i++)
		dest[i] = src[i];
}
void mergeSort(CARD_INFO arr[], int size)//This function is a recusive merge between two parts of an array
{
	CARD_INFO* tmpArr = NULL;
	if (size <= 1)
		return;
	mergeSort(arr, size / 2);
	mergeSort(arr + size / 2, size - size / 2);

	tmpArr = (CARD_INFO*)malloc(sizeof(CARD_INFO) * size);
	if (tmpArr)
	{
		merge(arr, size / 2,arr + size / 2,size - size / 2, tmpArr);
		copyArr(arr, tmpArr, size);
		free(tmpArr);
	}
	else
	{
		printf("Memory allocation failed!!!\n");
		exit(1);
	}
}
void printStatistics(CARD_INFO deckInfo[])//This function prints the statistics of the game,which card has been drawn the most and how many times
{
	int i;
	CARD tmp;
	char space[] = " ";
	printf("\n************ Game Statistics ************\n");
	printf("Card # | Frequency\n");
	printf("__________________\n");

	for (i = NUMBER_OF_TYPES - 1; i >= EMPTY; i--)
	{
		tmp.typeValue = deckInfo[i].CardType;
		tmp = castValueIntoType(tmp);
		switch (tmp.typeValue)
		{
		case 14: // COLOR
		{
			printf(" %s |    %d    \n", tmp.type, deckInfo[i].numberOfPerformances);
			break;
		}
		case 13:// TAKI
		case 11:// STOP
		{
			printf(" %s  |    %d    \n", tmp.type, deckInfo[i].numberOfPerformances);
			break;
		}
		case 12: // <->
		{
			printf("  %s  |    %d    \n", tmp.type, deckInfo[i].numberOfPerformances);
			break;
		}
		case 10: // +
		default: // 1-9
		{
			printf("   %s   |    %d    \n", tmp.type, deckInfo[i].numberOfPerformances);
			break;
		}
		}
	}
}
bool isGameOver(PLAYER* playersData, int numOfPlayers)
//This function check if some player has zero cards in his end.
//As long as no one has zero cards it returns false otherwise it returns true
{
	int turn;
	for (turn = 0; turn < numOfPlayers; turn++)
	{
		if (playersData[turn].numberOfCards == 0)
		{
			printf("The winner is...%s! Congratulations!\n", playersData[turn].name);
			return true;
		}
	}
	return false;
}
void gameProcess()//This is the main function,all the function are occur here
{
	int numberOfPlayers, playerIndex = 0,numberOfCardsDrawn=0;
	PLAYER* playersData = NULL;
	CARD upperCard;
	bool shouldBeIncrement = true,isCardHandeled,lastCardWasThrown=false;
	CARD_INFO deckInfo[NUMBER_OF_TYPES];

	welcome();
	numberOfPlayers = getNumberOfPlayers();
	playersData = createPlayersData(numberOfPlayers);
	initializePlayerData(playersData, numberOfPlayers);
	initializeDeckInfo(deckInfo);
	getPlayersNames(playersData, numberOfPlayers);
	drawCards(playersData, numberOfPlayers,deckInfo);
	drawUpperCard(&upperCard,deckInfo);
	printUpperCard(upperCard);
	printPlayerCards(playersData[playerIndex]);

	while (!isGameOver(playersData, numberOfPlayers))
	{
		isCardHandeled = false;

		while (!isCardHandeled)
		{
			getPlayerOptions(&playersData[playerIndex],upperCard);

			int optionChose = playersData[playerIndex].optionChose;

			if (isSpecialCard(playersData[playerIndex].hand[optionChose - 1].typeValue))
			{
				handleSpecialCard(&playersData[playerIndex], &upperCard, &isCardHandeled,&lastCardWasThrown);
				if (lastCardWasThrown)//If the player throw his last card,we have to check if it special case
				{					//special case can only occur on specal cards
					specialCases(&playersData[playerIndex], &upperCard, numberOfPlayers, &playerIndex, shouldBeIncrement, deckInfo);
					whoPlaysNext(&playerIndex, numberOfPlayers, shouldBeIncrement);
					printUpperCard(upperCard);
					printPlayerCards(playersData[playerIndex]);
				}
				else
				GetTurnAndDirection(playersData[playerIndex], &shouldBeIncrement, &upperCard, &playerIndex,numberOfPlayers);
			}
				// means that the user wants another card
			else if (optionChose == DRAW_ANOTHER_CARD)
			{
				drawAnotherCard(&playersData[playerIndex], &isCardHandeled, deckInfo);
				GetTurnAndDirection(playersData[playerIndex], &shouldBeIncrement, &upperCard, &playerIndex,numberOfPlayers);
			}

			else
			{
				handleRegularCard(&playersData[playerIndex], &upperCard, &isCardHandeled,&lastCardWasThrown);
				GetTurnAndDirection(playersData[playerIndex], &shouldBeIncrement, &upperCard, &playerIndex,numberOfPlayers);
			}
		}
		if(!lastCardWasThrown)
		{
		whoPlaysNext(&playerIndex, numberOfPlayers, shouldBeIncrement);
		printUpperCard(upperCard);
		printPlayerCards(playersData[playerIndex]);
		}
	}
	freePlayersData(playersData, numberOfPlayers);
	mergeSort(deckInfo, NUMBER_OF_TYPES);
	printStatistics(deckInfo);
}
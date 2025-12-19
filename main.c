//
//  main.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include <time.h>
#include <string.h>
#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"


//board configuration parameters
//game configuration variables
static int smm_board_nr;          
static int smm_food_nr;           
static int smm_festival_nr;       
static int smm_player_nr;          

//laboratory information
static int smm_lab_pos = -1;      
static int smm_lab_cost = 0;      


//player state information
typedef struct {
    char name[MAX_CHARNAME];
    int pos;
    int credit;
    int energy;
    int flag_graduated;
    
    int in_experiment;           //whether the player is in laboratory
    int exp_goal;                //dice value required to finish experiment
} smm_player_t;

smm_player_t *smm_players;


void generatePlayers(int n, int initEnergy);
void printPlayerStatus(void);
void printGrades(int player);
void printTurnInfo(int player);



//function prototypes
#if 0
void printGrades(int player); //print grade history of the player
float calcAverageGrade(int player); //calculate average grade of the player
smmGrade_e takeLecture(int player, char *lectureName, int credit); //take the lecture (insert a grade of the player)
void printGrades(int player); //print all the grade history of the player
#endif


void printTurnInfo(int player)
{
    printf("\n");
    printf("-----------------------------------------------------------------------------\n");
    printf("This is %s's turn ::: Press any key to roll a die (press g to see grade):\n", smm_players[player].name);
    printf("-----------------------------------------------------------------------------\n");
}



void printGrades(int player)
{
    int size = smmdb_len(LISTNO_OFFSET_GRADE + player);
    int i;

    printf("\n[Grade history of %s]\n", smm_players[player].name);

    if (size == 0) 
    {
        printf("  (no grades yet)\n\n");
        return;
    }

    for (i = 0; i < size; i++) 
    {
        void *gptr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);

        printf("%2d %s  credit:%d  grade:%d\n", i + 1, smmObj_getObjectName(gptr), smmObj_getObjectCredit(gptr), smmObj_getObjectGrade(gptr));
    }
    printf("\n");
}



//check if player already took the lecture
void* findGrade(int player, char *lectureName)
{
      int size = smmdb_len(LISTNO_OFFSET_GRADE+player);
      int i;
      
      for(i=0;i<size;i++)    
      {
            void *ptr = smmdb_getData(LISTNO_OFFSET_GRADE+player,i);   
            if (strcmp(smmObj_getObjectName(ptr),lectureName) == 0)
            {
                 return ptr;
            }            
      }
      
      return NULL;
}

//check graduation condition
int isGraduated(void) //check if any player is graduated
{
    int i;
    for(i=0;i<smm_player_nr;i++)
    {
         if(smm_players[i].flag_graduated == 1)
            return 1;                      
    }
    
    return 0;
    
}

//move player and give bonus when passing home
void goForward(int player, int step)
{ 
    int i;
    void *ptr;
    
    int homeEnergy = smmObj_getObjectEnergy(smmdb_getData(LISTNO_NODE, 0));
    
    //player_pos[player] = player_pos[player]+ step;
    ptr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);
    
    printf("\n[%s] moving %d step(s) from %d (%s)\n", 
    smm_players[player].name, 
    step, 
    smm_players[player].pos, 
    smmObj_getObjectName(ptr));
    
    for (i=0;i<step;i++)
    {
        smm_players[player].pos = (smm_players[player].pos + 1)%smm_board_nr;
        
        ptr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);
        
        printf("  -> moved to %i(%s)\n", 
               smm_players[player].pos, 
               smmObj_getObjectName(ptr));
        
        //gain energy when passing home
        if(smm_players[player].pos == 0)
        {
             smm_players[player].energy += homeEnergy;
             
             printf("passed HOME, energy %+d (now %d)\n", homeEnergy, smm_players[player].energy);
        }    
    }
    
    printf("\n");
}

void printPlayerStatus(void)
{
     int i;
     
     printf("\n");
     printf("==================== PLAYER STATUS ===========================\n");
     
     for(i=0;i<smm_player_nr;i++)
       {
               void *ptr = smmdb_getData(LISTNO_NODE, smm_players[i].pos);
               
               printf("%s - position : %i(%s), credit: %i, energy:%i\n", 
               smm_players[i].name, 
               smm_players[i].pos, 
               smmObj_getObjectName(ptr), 
               smm_players[i].credit, 
               smm_players[i].energy);
       }  
     printf("==================== PLAYER STATUS ===========================\n");  
} 

void generatePlayers(int n, int initEnergy)
{
     int i;
     
     smm_players = (smm_player_t*)malloc(n*sizeof(smm_player_t));
     
     for(i=0;i<smm_player_nr;i++)
       {
               smm_players[i].pos = 0;
               smm_players[i].credit = 0;
               smm_players[i].energy = initEnergy; 
               smm_players[i].flag_graduated = 0;
               smm_players[i].in_experiment = 0;
               smm_players[i].exp_goal = 0;

               
               printf("Input %i-th player name: ",i+1);
               scanf("%s",&smm_players[i].name);
               fflush(stdin); 
               
       }    
} 
     

int rolldie(int player)
{
    char c;
    
    printf("\n");
    printf("Press any key to roll a die (press g to see grade): ");
    
    c = getchar();
    if (c == '\n') 
    {
        
    } 
    else 
    {
        
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {}

    //fflush(stdin);
    

    if (c == 'g')
        printGrades(player);
    }
    
    
    return (rand()%MAX_DIE + 1);
}


//action code when a player stays at a node
void actionNode(int player)
{
    void *ptr = smmdb_getData(LISTNO_NODE,smm_players[player].pos);
     
    int type = smmObj_getObjectType(ptr);
    int credit = smmObj_getObjectCredit(ptr);
    int energy = smmObj_getObjectEnergy(ptr);
    int grade;
    void *gradePtr;
    
    printf("--> player %i pos :%i, type : %s, credit : %i, energy : %i\n", player, smm_players[player].pos, smmObj_getTypeName(ptr), credit, energy);
    
    switch(type)
    {
        case SMMNODE_TYPE_LECTURE:
        {
             char choice[16];

             if (findGrade(player, smmObj_getObjectName(ptr)) != NULL)
             break;

             if (smm_players[player].energy < energy)
             {
                printf("not enough energy to take [%s]. need:%d, now:%d\n", smmObj_getObjectName(ptr), energy, smm_players[player].energy);
                 break;
             }

             printf("Lecture %s (credit:%d, energy:%d) starts! join or drop? : ", smmObj_getObjectName(ptr), credit, energy);

             while (1)
             {
                   scanf("%15s", choice);
                   fflush(stdin);

                   if (strcmp(choice, "join") == 0)
                   {
                       int g = rand() % SMMNODE_MAX_GRADE;

                       smm_players[player].credit += credit;
                       smm_players[player].energy -= energy;

                       void *gradePtr = smmObj_genObject(
                       smmObj_getObjectName(ptr),
                       SMMNODE_OBJTYPE_GRADE,
                       SMMNODE_TYPE_LECTURE,
                       credit,
                       energy,
                       g);
                       
                       smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);

                       printf("-> %s successfully takes the lecture %s with grade %s\n",
                                  smm_players[player].name,
                                  smmObj_getObjectName(ptr),
                                  smmObj_getGradeName(g));
                       break;
            }
            
            else if (strcmp(choice, "drop") == 0)
            {
                 printf("dropped [%s]\n", smmObj_getObjectName(ptr));
                 break;
            }
            
            else
            {
                 printf("invalid input. type 'join' or 'drop'\n");
            }
        }

              break;
     }
             

        case SMMNODE_TYPE_LABORATORY:
        {
             if (smm_players[player].in_experiment == 0)
             break; 

   
             smm_players[player].energy -= smm_lab_cost;

             int roll = (rand() % MAX_DIE) + 1;

             printf("experiment roll: %d (need %d)\n", roll, smm_players[player].exp_goal);
             printf("energy -%d (left %d)\n", smm_lab_cost, smm_players[player].energy);


             if (roll >= smm_players[player].exp_goal)
             {
                      smm_players[player].in_experiment = 0;
                      smm_players[player].exp_goal = 0;
                      printf("success. experiment done.\n");
             }
             else
             {
                 printf("failed. staying in lab.\n");
                 smm_players[player].pos = smm_lab_pos; 
             }
             break;
        }
             
        case SMMNODE_TYPE_HOME:
             //smm_players[player].energy += energy;
             
             if(smm_players[player].credit >= GRADUATE_CREDIT)
             {
                  smm_players[player].flag_graduated = 1;
                  printf(" %s graduated! (credit=%d)\n", smm_players[player].name, smm_players[player].credit);
             }  


             break;
             
        case SMMNODE_TYPE_GOROLAB:
        {
             smm_players[player].in_experiment = 1;
             smm_players[player].exp_goal = (rand() % MAX_DIE) + 1;

             smm_players[player].pos = smm_lab_pos;

             printf("experiment started (goal = %d)\n", smm_players[player].exp_goal);
             printf("move to LAB (%d)\n", smm_lab_pos);
                
             break;
        }
             
        case SMMNODE_TYPE_FOODCHANCE:
        {
             int selectedCard;
             void *foodPtr;
             int gainEnergy;
             
             selectedCard = rand() % smm_food_nr;
             foodPtr = smmdb_getData(LISTNO_FOODCARD, selectedCard);

             gainEnergy = smmObj_getObjectEnergy(foodPtr);
             smm_players[player].energy += gainEnergy;

             printf("food card: %s\n", smmObj_getObjectName(foodPtr));
             printf("energy %+d\n", gainEnergy);
             
             break;
                            
        }
             
        case SMMNODE_TYPE_FESTIVAL:
        {
             int selectedCard;
             void *festPtr;
             
             selectedCard = rand() % smm_festival_nr;
             festPtr = smmdb_getData(LISTNO_FESTCARD, selectedCard);

             printf("festival event\n");
             printf("%s\n", smmObj_getObjectName(festPtr));
             
             break;
        }

        //case lecture:
        default:
            break;
    }
}


int main(int argc, const char * argv[]) {
    
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
    int turn;

    
    smm_board_nr = 0;
    smm_food_nr = 0;
    smm_festival_nr = 0;
    
    srand(time(NULL));
    
    
    //1. import parameters ---------------------------------------------------------------------------------
    //1-1. boardConfig 
    if ((fp = fopen(BOARDFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }
    
    printf("Reading board component......\n");
    while (fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy) == 4 ) //read a node parameter set
    {
        //store the parameter set
        void* ptr = smmObj_genObject(name, SMMNODE_OBJTYPE_BOARD, type, credit, energy, 0);
        smm_board_nr = smmdb_addTail(LISTNO_NODE, ptr); 
        
        if (type == SMMNODE_TYPE_LABORATORY && smm_lab_pos < 0) 
        {
            smm_lab_pos = smmdb_len(LISTNO_NODE) - 1; 
            smm_lab_cost = energy;                    
        }
        
        printf("%s %i %i %i\n", name, type, credit, energy);
    }
    fclose(fp);
    

    if (smm_lab_pos < 0) 
    {
         printf("ERROR, No LABORATORY node found in board config.\n");
         return -1;
    }

    smm_board_nr = smmdb_len(LISTNO_NODE);
    printf("Total number of board nodes : %i\n", smm_board_nr);
    
    
    
#if 1
    //2. food card config 
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("Can't open %s (put it next to the exe)\n", FOODFILEPATH);
        return -1;
    }
    
    printf("\nLoading food cards component...\n");
    while (fscanf(fp, "%s %i" , name, &energy) == 2) //read a food parameter set
    {
             void* ptr;
             printf("%s %i\n", name, energy);

             ptr = smmObj_genObject(name, SMMNODE_OBJTYPE_FOOD, SMMNODE_TYPE_FOODCHANCE, 0, energy, 0);

             smmdb_addTail(LISTNO_FOODCARD, ptr);
             smm_food_nr++;
          
        //store the parameter set
    }
    fclose(fp);
    printf("Total number of food cards : %i\n", smm_food_nr);
    
    
    
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("Can't open %s (put it next to the exe)\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\nLoading festival cards component...\n");
    while (fgets(name, MAX_CHARNAME, fp) != NULL) //read a festival card string
    {
           void* ptr;
           printf("%s\n", name);

           ptr = smmObj_genObject(name, SMMNODE_OBJTYPE_FEST, SMMNODE_TYPE_FESTIVAL, 0, 0, 0);

           smmdb_addTail(LISTNO_FESTCARD, ptr);
           smm_festival_nr++;
          
        //store the parameter set
    }
    fclose(fp);
    printf("Total number of festival cards : %i\n", smm_festival_nr);
    
    
    
#endif   
    
    //2. Player configuration ---------------------------------------------------------------------------------

    do
    {
        //input player number to player_nr
        printf("\n");
        printf("Input player number: ");
        scanf("%i",&smm_player_nr);
        fflush(stdin);
        
        if(smm_player_nr <=0 || smm_player_nr > MAX_PLAYER)
                  printf("invaild player number.\n");
       
    }
    while (smm_player_nr <=0 || smm_player_nr > MAX_PLAYER);
    
    
    generatePlayers(smm_player_nr, smmObj_getObjectEnergy(smmdb_getData(LISTNO_NODE,0)));
    
    turn = 0;

    
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    printf("\n");
    printf("============================================================\n");
    printf("\n");
    printf("   Sookmyung Marble !! Let's Graduate (total credit : %d)!!\n", GRADUATE_CREDIT);
    printf("\n");
    printf("============================================================\n\n");
    
    
    //main game loop
    while (isGraduated() == 0) //is anybody graduated?
    {     
        int die_result;
        
        printf("\n");
        printf("[TURN]player: %s\n", smm_players[turn].name);
        printf("\n");
        
        //4-1. initial printing
        printPlayerStatus();
        
        //skip movement if player is in experimnet
        if (smm_players[turn].in_experiment == 1)
        {
        
           smm_players[turn].pos = smm_lab_pos;

           printf("\n[EXPERIMENT] %s is in LAB (%d). Try experiment...\n",
               smm_players[turn].name, smm_lab_pos);
        
           actionNode(turn);

           turn = (turn + 1) % smm_player_nr;
           continue;
        }
        
        printTurnInfo(turn);
        
        //4-2. die rolling (if not in experiment)
        die_result = rolldie(turn);
        
        //4-3. go forward
        goForward(turn, die_result);
        //pos = pos +2;

		//4-4. take action at the destination node of the board
        actionNode(turn);
        
        //4-5. next turn
       
        turn = (turn + 1)%smm_player_nr;
    }
    
    printf("\n-------- Graduation Result --------\n");
    
    int i;
    int j;

    for (i=0;i<smm_player_nr;i++)
    {
        if (smm_players[i].flag_graduated)
         {
              printf("Graduate: %s\n", smm_players[i].name);
              printf("Total Credit: %d\n", smm_players[i].credit);
              printf("Grades:\n");

              int len = smmdb_len(LISTNO_OFFSET_GRADE + i);
              
              for (j=0;j<len;j++)
              {
                  void *gptr = smmdb_getData(LISTNO_OFFSET_GRADE + i, j);
                  printf(" %s credit %d grade %s\n", smmObj_getObjectName(gptr), smmObj_getObjectCredit(gptr),
                   smmObj_getGradeName(smmObj_getObjectGrade(gptr)));
               }
         break;
          }
     }
    printf("------------------------------------\n");
    
    free(smm_players);
    
    system("PAUSE");    
    return 0;
}

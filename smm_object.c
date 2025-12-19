//
//  smm_node.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include "smm_common.h"
#include "smm_object.h"
#include <string.h>

#define MAX_NODENR         100
#define MAX_NODETYPE       13


#define GRADE_A_PLUS            0
#define GRADE_A_ZERO            1
#define GRADE_A_MINUS           2
#define GRADE_B_PLUS            3
#define GRADE_B_ZERO            4
#define GRADE_B_MINUS           5
#define GRADE_C_PLUS            6
#define GRADE_C_ZERO            7
#define GRADE_C_MINUS           8
#define GRADE_D_PLUS            9
#define GRADE_D_ZERO            10
#define GRADE_D_MINUS           11
#define GRADE_F                 12

#define MAX_GRADE 13


static const char smmObj_gradeName[SMMNODE_MAX_GRADE][MAX_CHARNAME] = {
       "A+",
       "A0",
       "A- ",
       "B+",
       "B0",
       "B-",
       "C+",
       "C0",
       "C-",
       "D+",
       "D0",
       "D-",
       "F"
     
};

static const char smmObj_typeName[MAX_NODETYPE][MAX_CHARNAME] = {
       "lecture",
       "restaurant",
       "laboratory",
       "home",
       "gotoLab",
       "foodChance",
       "festival"
};


//structure type definition
typedef struct
{
         char name[MAX_CHARNAME];
         int objType;
         int type;
         int credit;
         int energy;
         int grade;
}smmObj_object_t;


//object generation
void* smmObj_genObject(char* name, int objType, int type, int credit, int energy,int grade)
{
     smmObj_object_t* ptr;
     ptr = (smmObj_object_t*)malloc(sizeof(smmObj_object_t));
     
     
     strcpy(ptr->name, name);
     ptr->type = type;
     ptr->objType = objType;
     ptr->credit = credit;
     ptr->energy = energy;
     ptr->grade = grade;
     
     return((void*)ptr);
    
}



//member retrieving
char* smmObj_getObjectName(void *ptr)
{
      smmObj_object_t* objptr = (smmObj_object_t*)ptr;
      
      return (objptr->name);
}

int smmObj_getObjectType(void *ptr)
{
    smmObj_object_t* objptr = (smmObj_object_t*)ptr;
    
    return (objptr->type);
}

int smmObj_getObjectEnergy(void *ptr)
{
      smmObj_object_t* objptr = (smmObj_object_t*)ptr;
      
      return (objptr->energy);
}

int smmObj_getObjectCredit(void *ptr)
{
    smmObj_object_t* objptr = (smmObj_object_t*)ptr;
    return (objptr->credit);
}

char* smmObj_getTypeName(void *ptr)
{
      smmObj_object_t* objptr = (smmObj_object_t*)ptr;
      return (char*)smmObj_typeName[objptr->type];
}

int smmObj_getObjectGrade(void *ptr)
{
    smmObj_object_t* objptr = (smmObj_object_t*)ptr;
    return objptr->grade;
}


static const char *g_gradeStr[SMMNODE_MAX_GRADE] = 
{
    "A+",
    "A0",
    "A-",
    "B+",
    "B0",
    "B-",
    "C+",
    "C0",
    "C-",
    "D+",
    "D0",
    "D-",
    "F"
};


char* smmObj_getGradeName(int grade)
{
    if (grade < 0 || grade >= SMMNODE_MAX_GRADE) return "N/A";
    return (char*)g_gradeStr[grade];
}

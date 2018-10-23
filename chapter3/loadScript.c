#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

int g_iScriptSize = 0;

char **g_ppstrScript = NULL;

#define MAX_SOURCE_LINE_SIZE 500
#define MAX_COMMAND_SIZE 100
#define MAX_PARAM_SIZE 400

void LoadScript(char *pstrFilename)
{
  FILE *pScriptFile;

  if (!(pScriptFile = fopen(pstrFilename, "rb"))) {
    printf("File I/O error.\n");
    exit(0);
  }

  while (!feof(pScriptFile)) {
    if (fgetc(pScriptFile) == '\n')
      ++g_iScriptSize;
  }

  fclose(pScriptFile);
  if (!(pScriptFile = fopen(pstrFilename, "r"))) {
    printf("File I/O error.\n");
    exit(0);
  }

  g_ppstrScript = (char **)malloc(g_iScriptSize * sizeof(char *));
  for (int iCurrLineIndex = 0;
       iCurrLineIndex < g_iScriptSize;
       ++iCurrLineIndex) {
    g_ppstrScript[iCurrLineIndex] = (char *)malloc(MAX_SOURCE_LINE_SIZE  + 1);
    fgets(g_ppstrScript[iCurrLineIndex], MAX_SOURCE_LINE_SIZE, pScriptFile);
  }

  fclose(pScriptFile);
}

void UnloadScript()
{
  if (!g_ppstrScript)
    return;
  for (int iCurrLineIndex = 0;
       iCurrLineIndex < g_iScriptSize;
       ++iCurrLineIndex)
    free(g_ppstrScript[iCurrLineIndex]);
  free(g_ppstrScript);
}

int g_iCurrScriptLine = 0;
int g_iCurrScriptLineChar = 0;

#define COMMAND_PRINTSTRING "PrintString"
#define COMMAND_PRINTSTRINGLOOP "PrintStringLoop"
#define COMMAND_NEWLINE "NewLine"
#define COMMAND_WAITFORKEYPRESS "WaitForKeyPress"

void GetCommand(char command[]);

void GetStringParam(char param[]);

int GetIntParam();

/**
Run all script
*/
void RunScriptVersion1()
{
  char pstrCommand[MAX_COMMAND_SIZE];
  char pstrStringParam[MAX_PARAM_SIZE];

  for (g_iCurrScriptLine = 0;
       g_iCurrScriptLine < g_iScriptSize;
       ++g_iCurrScriptLine) {
    g_iCurrScriptLineChar = 0;
    
    GetCommand(pstrCommand);

    if (strcasecmp(pstrCommand, COMMAND_PRINTSTRING) == 0) {
      GetStringParam(pstrStringParam);
      printf("\t%s\n", pstrStringParam);
    } else if (strcasecmp(pstrCommand, COMMAND_PRINTSTRINGLOOP) == 0) {
      GetStringParam(pstrStringParam);
      int iLoopCount = GetIntParam();
      for (int iCurrString = 0;
           iCurrString < iLoopCount;
           ++iCurrString)
        printf("\t%d: %s\n", iCurrString, pstrStringParam);
    } else if (strcasecmp(pstrCommand, COMMAND_NEWLINE) == 0) {
      printf("\n");
    } else if (strcasecmp(pstrCommand, COMMAND_WAITFORKEYPRESS) == 0) {
      char key[1];
      fgets(key, 1, stdin);
    } else {
      printf("\tError: Invalid command.\n");
      break;
    }
  }
}

/**
Run one command per game loop
 */
void RunScript()
{
  char pstrCommand[MAX_COMMAND_SIZE];
  char pstrStringParam[MAX_PARAM_SIZE];

  if (g_iCurrScriptLine > g_iScriptSize)
    return;

  if (g_ppstrScript[g_iCurrScriptLine] == 0 ||
      (g_ppstrScript[g_iCurrScriptLine][0] == '/' &&
       g_ppstrScript[g_iCurrScriptLine][1] == '/')) {
    ++g_iCurrScriptLine;
    return;
  }

  g_iCurrScriptLineChar = 0;

  GetCommand(pstrCommand);

  if (strcasecmp(pstrCommand, COMMAND_PRINTSTRING) == 0) {
    GetStringParam(pstrStringParam);
    printf("\t%s\n", pstrStringParam);
  } else if (strcasecmp(pstrCommand, COMMAND_PRINTSTRINGLOOP) == 0) {
    GetStringParam(pstrStringParam);
    int iLoopCount = GetIntParam();
    for (int iCurrString = 0;
         iCurrString < iLoopCount;
         ++iCurrString)
      printf("\t%d: %s\n", iCurrString, pstrStringParam);
  } else if (strcasecmp(pstrCommand, COMMAND_NEWLINE) == 0) {
    printf("\n");
  } else if (strcasecmp(pstrCommand, COMMAND_WAITFORKEYPRESS) == 0) {
    char key[1];
    fgets(key, 1, stdin);
  } else {
    printf("\tError: Invalid command.\n");
    break;
  }

  ++g_iCurrScriptLine;
}

void GetCommand(char *pstrDestString) {
  int iCommandSize = 0;
  char cCurrChar;
  while (g_iCurrScriptLineChar <
         (int)strlen(g_ppstrScript[g_iCurrScriptLine])) {
    cCurrChar = g_ppstrScript[g_iCurrScriptLine][g_iCurrScriptLineChar];
    if (cCurrChar == ' ' || cCurrChar == '\n')
      break;
    pstrDestString[iCommandSize] = cCurrChar;
    ++iCommandSize;
    ++g_iCurrScriptLineChar;
  }
  ++g_iCurrScriptLineChar;

  pstrDestString[iCommandSize] = '\0';
  strupr(pstrDestString);
}


int GetIntParam() {
  char pstrString[MAX_PARAM_SIZE];
  int iParamSize = 0;
  char cCurrChar;

  while (g_iCurrScriptLineChar <
         (int)strlen(g_ppstrScript[g_iCurrScriptLine])) {
    cCurrChar = g_ppstrScript[g_iCurrScriptLine][g_iCurrScriptLineChar];
    if (cCurrChar == ' ' || cCurrChar == '\n')
      break;
    pstrString[iParamSize] = cCurrChar;
    ++iParamSize;
    ++g_iCurrScriptLineChar;
  }
  ++g_iCurrScriptLineChar;
  pstrString[iParamSize] = '\0';
  int iIntValue = atoi(pstrString);
  return iIntValue;
}

void GetStringParam(char *pStrDestString)
{
  int iParamSize = 0;
  char cCurrChar;
  ++g_iCurrScriptLineChar;

  while (g_iCurrScriptLineChar <
         (int)strlen(g_ppstrScript[g_iCurrScriptLine])) {
    cCurrChar = g_ppstrScript[g_iCurrScriptLine][g_iCurrScriptLineChar];
    if (cCurrChar == '"' || cCurrChar == '\n')
      break;
    pStrDestString[iParamSize] = cCurrChar;
    ++iParamSize;
    ++g_iCurrScriptLineChar;
  }
  g_iCurrScriptLineChar += 2;
  pStrDestString[iParamSize] = '\0';
}

typedef struct _NPC
{
  int iDir;
  int iX,
    iY;
  int iMoveX;
  int iMoveY;

  char **ppstrScript;
  int iScriptSize;
  int iCurrScriptLine;
  int iCurrScriptLineChar;
  int iIsPaused;
  unsigned int iPausedEndTime;
} NPC;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int g_iScriptSize = 0;

char **g_ppstrScript = NULL;

#define MAX_SOURCE_LINE_SIZE 500
#define MAX_COMMAND_SIZE 100
#define MAX_PARAM_SIZE 400

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

NPC g_NPC;

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

  g_NPC.ppstrScript = (char **)malloc(g_iScriptSize * sizeof(char *));
  for (int iCurrLineIndex = 0;
       iCurrLineIndex < g_iScriptSize;
       ++iCurrLineIndex) {
    g_NPC.ppstrScript[iCurrLineIndex] = (char *)malloc(MAX_SOURCE_LINE_SIZE  + 1);
    fgets(g_NPC.ppstrScript[iCurrLineIndex], MAX_SOURCE_LINE_SIZE, pScriptFile);
  }

  fclose(pScriptFile);
}

void UnloadScript()
{
  if (!g_NPC.ppstrScript)
    return;
  for (int iCurrLineIndex = 0;
       iCurrLineIndex < g_iScriptSize;
       ++iCurrLineIndex)
    free(g_NPC.ppstrScript[iCurrLineIndex]);
  free(g_NPC.ppstrScript);
}

#define COMMAND_PRINTSTRING "PrintString"
#define COMMAND_PRINTSTRINGLOOP "PrintStringLoop"
#define COMMAND_NEWLINE "NewLine"
#define COMMAND_WAITFORKEYPRESS "WaitForKeyPress"

#define COMMAND_MOVECHAR "MoveChar"
#define COMMAND_SETCHARLOC "SetCharLoc"
#define COMMAND_SETCHARDIR "SetCharDir"
#define COMMAND_SHOWTEXTBOX "ShowTextBox"
#define COMMAND_HIDETEXTBOX "HideTextBox"
#define COMMAND_PAUSE "Pause"

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4

void GetCommand(char command[]);

void GetStringParam(char param[]);

int GetIntParam();

unsigned int W_GetTickCount();

char *g_pstrTextBoxMssg;
int g_iIsTextBoxActive = true;

/**
Run one command per game loop
 */
void RunScript()
{
  if (g_NPC.iMoveX || g_NPC.iMoveY)
    return;

  if (g_NPC.iIsPaused) {
    if (W_GetTickCount() > g_NPC.iPausedEndTime) {
      g_NPC.iIsPaused = false;
    } else {
      return;
    }
  }

  if (g_NPC.iCurrScriptLine >= g_NPC.iScriptSize) {
    g_NPC.iCurrScriptLine = 0;
  }

  char pstrCommand[MAX_COMMAND_SIZE];
  char pstrStringParam[MAX_PARAM_SIZE];

  if (g_NPC.iCurrScriptLine > g_iScriptSize)
    return;

  if (g_NPC.ppstrScript[g_NPC.iCurrScriptLine] == 0 ||
      (g_NPC.ppstrScript[g_NPC.iCurrScriptLine][0] == '/' &&
       g_NPC.ppstrScript[g_NPC.iCurrScriptLine][1] == '/')) {
    ++g_NPC.iCurrScriptLine;
    return;
  }

  g_NPC.iCurrScriptLineChar = 0;

  GetCommand(pstrCommand);

  if (strcasecmp(pstrCommand, COMMAND_SETCHARLOC)) {
    int iX = GetIntParam();
    int iY = GetIntParam();

    int iXDist = iX - g_NPC.iX;
    int iYDist = iY - g_NPC.iY;
    g_NPC.iMoveX = iXDist;
    g_NPC.iMoveY = iYDist;
  } else if (strcasecmp(pstrCommand, COMMAND_SETCHARDIR) == 0) {
    GetStringParam(pstrStringParam);
    if (strcasecmp(pstrStringParam, "UP") == 0) {
      g_NPC.iDir = UP;
    } else if (strcasecmp(pstrStringParam, "DOWN") == 0) {
      g_NPC.iDir = DOWN;
    } else if (strcasecmp(pstrStringParam, "LEFT") == 0) {
      g_NPC.iDir = LEFT;
    } else if (strcasecmp(pstrStringParam, "RIGHT") == 0) {
      g_NPC.iDir = RIGHT;
    } 
  } else if (strcasecmp(pstrCommand, COMMAND_SHOWTEXTBOX) == 0) {
    GetStringParam(pstrStringParam);
    strcpy(g_pstrTextBoxMssg, pstrStringParam);
    g_iIsTextBoxActive = true;
  } else if (strcasecmp(pstrCommand, COMMAND_HIDETEXTBOX) == 0) {
    g_iIsTextBoxActive = false;
  } else if (strcasecmp(pstrCommand, COMMAND_PAUSE) == 0) {
    int iPauseDur = GetIntParam();
    unsigned int iPauseEndTime = W_GetTickCount() + iPauseDur;
    g_NPC.iIsPaused = true;
    g_NPC.iPausedEndTime = iPauseEndTime;
  } else if (strcasecmp(pstrCommand, COMMAND_PRINTSTRING) == 0) {
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
  }

  ++g_NPC.iCurrScriptLine;
}

void strupr(char *);

void GetCommand(char *pstrDestString) {
  int iCommandSize = 0;
  char cCurrChar;
  while (g_NPC.iCurrScriptLineChar <
         (int)strlen(g_NPC.ppstrScript[g_NPC.iCurrScriptLine])) {
    cCurrChar = g_NPC.ppstrScript[g_NPC.iCurrScriptLine][g_NPC.iCurrScriptLineChar];
    if (cCurrChar == ' ' || cCurrChar == '\n')
      break;
    pstrDestString[iCommandSize] = cCurrChar;
    ++iCommandSize;
    ++g_NPC.iCurrScriptLineChar;
  }
  ++g_NPC.iCurrScriptLineChar;

  pstrDestString[iCommandSize] = '\0';
  strupr(pstrDestString);
}


int GetIntParam() {
  char pstrString[MAX_PARAM_SIZE];
  int iParamSize = 0;
  char cCurrChar;

  while (g_NPC.iCurrScriptLineChar <
         (int)strlen(g_NPC.ppstrScript[g_NPC.iCurrScriptLine])) {
    cCurrChar = g_NPC.ppstrScript[g_NPC.iCurrScriptLine][g_NPC.iCurrScriptLineChar];
    if (cCurrChar == ' ' || cCurrChar == '\n')
      break;
    pstrString[iParamSize] = cCurrChar;
    ++iParamSize;
    ++g_NPC.iCurrScriptLineChar;
  }
  ++g_NPC.iCurrScriptLineChar;
  pstrString[iParamSize] = '\0';
  int iIntValue = atoi(pstrString);
  return iIntValue;
}

void GetStringParam(char *pStrDestString)
{
  int iParamSize = 0;
  char cCurrChar;
  ++g_NPC.iCurrScriptLineChar;

  while (g_NPC.iCurrScriptLineChar <
         (int)strlen(g_NPC.ppstrScript[g_NPC.iCurrScriptLine])) {
    cCurrChar = g_NPC.ppstrScript[g_NPC.iCurrScriptLine][g_NPC.iCurrScriptLineChar];
    if (cCurrChar == '"' || cCurrChar == '\n')
      break;
    pStrDestString[iParamSize] = cCurrChar;
    ++iParamSize;
    ++g_NPC.iCurrScriptLineChar;
  }
  g_NPC.iCurrScriptLineChar += 2;
  pStrDestString[iParamSize] = '\0';
}


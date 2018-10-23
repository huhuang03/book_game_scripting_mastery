#include <stdlib.h>
#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>

int test(lua_State *L) {
  printf("called test\n");
  return 0;
}

int PrintStringList(lua_State *L) {
  for (int i = 1; i <= lua_gettop(L); i++) {
    if (!lua_isstring(L, i)) {
      lua_pushstring(L, "Invalidd string");
      lua_error(L);
    } else {
      printf("%s", lua_tostring(L, i));
      printf("\n");
    }
  }
  return 0;
}

int main(int argc, char *argv[]) {
  lua_State *L = lua_open();
  lua_register(L, "PrintStringList", PrintStringList);
  luaL_dofile(L, "forPrintList.lua");
  exit(0);
}

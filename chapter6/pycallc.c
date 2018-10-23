#include <stdlib.h>
#include <stdio.h>
#include <Python.h>

PyObject *addInC(PyObject *pThis, PyObject *pParams) {
  int i;
  int j;
  if (!PyArg_ParseTuple(pParams, "ii", &i, &j)) {
    printf("Unable to parse args in addInC");
    exit(0);
  }

  return PyInt_FromLong(i + j);
}

int main(int argc, char *argv[]) {
  Py_Initialize();
  PyRun_SimpleString("import sys");
  PyRun_SimpleString("sys.path.append(\".\")");

  PyMethodDef hostAPIFuncs[] = {
    {"addInC", addInC, METH_VARARGS, "just add function in c"},
    {NULL, NULL, 0, NULL}
  };

  if (!PyImport_AddModule("HostAPI"))
    printf("Host API module could not be created.");

  if (!Py_InitModule("HostAPI", hostAPIFuncs))
    printf("Host API module could not be initialized.");


  PyRun_SimpleString("import HostAPI");
  PyRun_SimpleString("print(HostAPI.addInC(8, 8))");
  Py_Finalize();
  exit(0);
}

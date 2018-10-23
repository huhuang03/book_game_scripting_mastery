#include <stdlib.h>
#include <stdio.h>
#include <Python.h>

int main(int argc, char *argv[]) {
  Py_Initialize();
  PyRun_SimpleString("import sys");
  PyRun_SimpleString("sys.path.append(\".\")");

  PyObject *name = PyString_FromString("tocall");
  PyObject *pModule = PyImport_Import(name);
  if (!pModule) {
    PyErr_Print();
    return 1;
  }

  // call function
  PyObject *pDict = PyModule_GetDict(pModule);
  PyObject *addFun = PyDict_GetItemString(pDict, "add");
  PyObject *pParams = PyTuple_New(2);

  PyTuple_SetItem(pParams, 0, PyInt_FromLong(8));
  PyTuple_SetItem(pParams, 1, PyInt_FromLong(8));

  PyObject *pRst = PyObject_CallObject(addFun, pParams);
  int iMax = PyInt_AsLong(pRst);
  printf("8 + 8 = %d\n", iMax);

  Py_Finalize();
  exit(0);
}

#include <Python.h>
#include <math.h>

/* * HELPER: Welford's Algorithm 
 * Mathematically: Updates mean and M2 (sum of squares of differences from mean) 
 * in a single pass. This avoids "catastrophic cancellation" in floating point math.
 * Time Complexity: O(n)
 */
static void update_welford(double x, double *count, double *mean, double *m2) {
    (*count)++;
    double delta = x - *mean;
    *mean += delta / (*count);
    double delta2 = x - *mean;
    *m2 += delta * delta2;
}

// peak_to_peak(data)
static PyObject* method_peak_to_peak(PyObject* self, PyObject* args) {
    PyObject *list_obj;
    if (!PyArg_ParseTuple(args, "O", &list_obj)) return NULL;

    Py_ssize_t size = PySequence_Size(list_obj);
    if (size <= 0) { PyErr_SetString(PyExc_ValueError, "Empty input"); return NULL; }

    double min_val = INFINITY, max_val = -INFINITY;
    for (Py_ssize_t i = 0; i < size; i++) {
        PyObject *item = PySequence_GetItem(list_obj, i);
        double val = PyFloat_AsDouble(item);
        Py_DECREF(item); // Memory handling: clean up the reference
        if (val < min_val) min_val = val;
        if (val > max_val) max_val = val;
    }
    return PyFloat_FromDouble(max_val - min_val);
}

// rms(data) - Root Mean Square
static PyObject* method_rms(PyObject* self, PyObject* args) {
    PyObject *list_obj;
    if (!PyArg_ParseTuple(args, "O", &list_obj)) return NULL;
    Py_ssize_t size = PySequence_Size(list_obj);
    if (size == 0) return PyFloat_FromDouble(0.0);

    double sum_sq = 0;
    for (Py_ssize_t i = 0; i < size; i++) {
        PyObject *item = PySequence_GetItem(list_obj, i);
        double val = PyFloat_AsDouble(item);
        Py_DECREF(item);
        sum_sq += (val * val);
    }
    return PyFloat_FromDouble(sqrt(sum_sq / size));
}

// summary(data) - Returns Dictionary
static PyObject* method_summary(PyObject* self, PyObject* args) {
    PyObject *list_obj;
    if (!PyArg_ParseTuple(args, "O", &list_obj)) return NULL;
    Py_ssize_t size = PySequence_Size(list_obj);
    if (size == 0) return Py_BuildValue("{s:i, s:f, s:f, s:f}", "count", 0, "mean", 0.0, "min", 0.0, "max", 0.0);

    double min_v = INFINITY, max_v = -INFINITY, count = 0, mean = 0, m2 = 0;
    for (Py_ssize_t i = 0; i < size; i++) {
        PyObject *item = PySequence_GetItem(list_obj, i);
        double val = PyFloat_AsDouble(item);
        Py_DECREF(item);
        if (val < min_v) min_v = val;
        if (val > max_v) max_v = val;
        update_welford(val, &count, &mean, &m2);
    }

    return Py_BuildValue("{s:n, s:d, s:d, s:d}", 
                        "count", size, "mean", mean, "min", min_v, "max", max_v);
}

// Method Table
static PyMethodDef VibrationMethods[] = {
    {"peak_to_peak", method_peak_to_peak, METH_VARARGS, "Returns max - min"},
    {"rms", method_rms, METH_VARARGS, "Returns Root Mean Square"},
    {"summary", method_summary, METH_VARARGS, "Returns stats dictionary"},
    {NULL, NULL, 0, NULL}
};

// Module Definition
static struct PyModuleDef vibrationmodule = {
    PyModuleDef_HEAD_INIT, "vibration", NULL, -1, VibrationMethods
};

// Initialization
PyMODINIT_FUNC PyInit_vibration(void) {
    return PyModule_Create(&vibrationmodule);
}
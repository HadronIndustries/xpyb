#include "module.h"
#include "except.h"
#include "protobj.h"


/*
 * Infrastructure
 */

static PyObject *
xpybProtobj_new(PyTypeObject *self, PyObject *args, PyObject *kw)
{
    return PyType_GenericNew(self, args, kw);
}

static int
xpybProtobj_init(xpybProtobj *self, PyObject *args, PyObject *kw)
{
    static char *kwlist[] = { "parent", "offset", "size", NULL };
    Py_ssize_t offset = 0, size = Py_END_OF_BUFFER;
    PyObject *parent;

    if (!PyArg_ParseTupleAndKeywords(args, kw, "O|ii", kwlist,
				     &parent, &offset, &size))
	return -1;

    self->buf = PyBuffer_FromObject(parent, offset, size);
    if (self->buf == NULL)
	return -1;

    return 0;
}

static void
xpybProtobj_dealloc(xpybProtobj *self)
{
    Py_CLEAR(self->buf);
    free(self->data);
    self->ob_type->tp_free((PyObject *)self);
}

static Py_ssize_t
xpybProtobj_readbuf(xpybProtobj *self, Py_ssize_t s, void **p)
{
    return PyBuffer_Type.tp_as_buffer->bf_getreadbuffer(self->buf, s, p);
}

static Py_ssize_t
xpybProtobj_segcount(xpybProtobj *self, Py_ssize_t *s)
{
    return PyBuffer_Type.tp_as_buffer->bf_getsegcount(self->buf, s);
}

static Py_ssize_t
xpybProtobj_charbuf(xpybProtobj *self, Py_ssize_t s, char **p)
{
    return PyBuffer_Type.tp_as_buffer->bf_getcharbuffer(self->buf, s, p);
}

/*
 * Members
 */


/*
 * Methods
 */


/*
 * Definition
 */

static PyBufferProcs xpybProtobj_bufops = {
    .bf_getreadbuffer = (readbufferproc)xpybProtobj_readbuf,
    .bf_getsegcount = (segcountproc)xpybProtobj_segcount,
    .bf_getcharbuffer = (charbufferproc)xpybProtobj_charbuf
};

PyTypeObject xpybProtobj_type = {
    PyObject_HEAD_INIT(NULL)
    .tp_name = "xcb.Protobj",
    .tp_basicsize = sizeof(xpybProtobj),
    .tp_init = (initproc)xpybProtobj_init,
    .tp_new = xpybProtobj_new,
    .tp_dealloc = (destructor)xpybProtobj_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = "XCB generic X protocol object",
    .tp_as_buffer = &xpybProtobj_bufops
};


/*
 * Module init
 */
int xpybProtobj_modinit(PyObject *m)
{
    if (PyType_Ready(&xpybProtobj_type) < 0)
        return -1;
    Py_INCREF(&xpybProtobj_type);
    if (PyModule_AddObject(m, "Protobj", (PyObject *)&xpybProtobj_type) < 0)
	return -1;

    return 0;
}

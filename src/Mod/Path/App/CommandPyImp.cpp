/***************************************************************************
 *   Copyright (c) Yorik van Havre (yorik@uncreated.net) 2014              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#include "PreCompiled.h"

#include <boost/algorithm/string.hpp>

#include <Base/Exception.h>
#include <Base/Vector3D.h>
#include <Base/VectorPy.h>
#include <Base/PlacementPy.h>
#include "Mod/Path/App/Command.h"

// inclusion of the generated files (generated out of CommandPy.xml)
#include "CommandPy.h"
#include "CommandPy.cpp"

using namespace Path;

// returns a string which represents the object e.g. when printed in python
std::string CommandPy::representation(void) const
{
    std::stringstream str;
    str.precision(5);
    str << "Command ";
    str << getCommandPtr()->Name;
    str << " [";
    for(std::map<std::string,double>::iterator i = getCommandPtr()->Parameters.begin(); i != getCommandPtr()->Parameters.end(); ++i) {
        std::string k = i->first;
        double v = i->second;
        str << " " << k << ":" << v;
    }
    str << " ]";
    return str.str();
}
    

PyObject *CommandPy::PyMake(struct _typeobject *, PyObject *, PyObject *)  // Python wrapper
{
    // create a new instance of CommandPy and the Twin object 
    return new CommandPy(new Command);
}

// constructor method
int CommandPy::PyInit(PyObject* args, PyObject* kwd)
{
    PyObject *parameters = PyDict_New();
    char *name = "";
    static char *kwlist[] = {"name", "parameters", NULL};
    if ( !PyArg_ParseTupleAndKeywords(args, kwd, "|sO!", kwlist, &name, &PyDict_Type, &parameters) )
        return -1;
    boost::to_upper(name);
    getCommandPtr()->Name = name;
    PyObject *key, *value;
    Py_ssize_t pos = 0;
    while (PyDict_Next(parameters, &pos, &key, &value)) {
        if ( !PyObject_TypeCheck(key,&(PyString_Type)) || (!PyObject_TypeCheck(value,&(PyFloat_Type)) && !PyObject_TypeCheck(value,&(PyInt_Type))) ) {
            PyErr_SetString(PyExc_TypeError, "The dictionary can only contain string:number pairs");
            return -1;
        }
        std::string ckey = PyString_AsString(key);
        boost::to_upper(ckey);
        double cvalue;
        if (PyObject_TypeCheck(value,&(PyInt_Type))) {
            cvalue = (double)PyInt_AsLong(value);
        } else {
            cvalue = PyFloat_AsDouble(value);
        }
        getCommandPtr()->Parameters[ckey]=cvalue;
    }
    return 0;
}

// Name attribute

Py::String CommandPy::getName(void) const
{
    return Py::String(getCommandPtr()->Name.c_str());
}

void CommandPy::setName(Py::String arg)
{
    std::string cmd = arg.as_std_string();
    boost::to_upper(cmd);
    getCommandPtr()->Name = cmd;
}

// Parameters attribute get/set

Py::Dict CommandPy::getParameters(void) const
{
    PyObject *dict = PyDict_New();
    for(std::map<std::string,double>::iterator i = getCommandPtr()->Parameters.begin(); i != getCommandPtr()->Parameters.end(); ++i) {
        PyDict_SetItem(dict,PyString_FromString(i->first.c_str()),PyFloat_FromDouble(i->second));
    }
    return Py::Dict(dict);
}

void CommandPy::setParameters(Py::Dict arg)
{
    PyObject* dict_copy = PyDict_Copy(arg.ptr());
    PyObject *key, *value;
    Py_ssize_t pos = 0;
    while (PyDict_Next(dict_copy, &pos, &key, &value)) {
        if ( PyObject_TypeCheck(key,&(PyString_Type)) && (PyObject_TypeCheck(value,&(PyFloat_Type)) || PyObject_TypeCheck(value,&(PyInt_Type)) ) ) {
            std::string ckey = PyString_AsString(key);
            boost::to_upper(ckey);
            double cvalue;
            if (PyObject_TypeCheck(value,&(PyInt_Type))) {
                cvalue = (double)PyInt_AsLong(value);
            } else {
                cvalue = PyFloat_AsDouble(value);
            }
            getCommandPtr()->Parameters[ckey]=cvalue;
        } else {
            throw Py::Exception("The dictionary can only contain string:number pairs");
        }
    }
}

// GCode methods

PyObject* CommandPy::toGCode(PyObject *args)
{
    if (PyArg_ParseTuple(args, "")) {
        return PyString_FromString(getCommandPtr()->toGCode().c_str());
    }
    throw Py::Exception("This method accepts no argument");
}

PyObject* CommandPy::setFromGCode(PyObject *args)
{
    char *pstr=0;
    if (PyArg_ParseTuple(args, "s", &pstr)) {
        std::string gcode(pstr);
        getCommandPtr()->setFromGCode(gcode);
        return Py_None;
    }
    throw Py::Exception("Argument must be a string");
}

// Placement attribute get/set

Py::Object CommandPy::getPlacement(void) const
{
    return Py::Object(new Base::PlacementPy(new Base::Placement(getCommandPtr()->getPlacement())));
}

void CommandPy::setPlacement(Py::Object arg)
{
    // TODO implement setPlacement
}

// custom attributes get/set

PyObject *CommandPy::getCustomAttributes(const char* /*attr*/) const
{
    return 0;
}

int CommandPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0; 
}



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

#ifndef _PreComp_
# include <sstream>
#endif


#include <strstream>
#include <Base/Console.h>
#include <Base/Writer.h>
#include <Base/Reader.h>
#include <Base/Exception.h>
#include <Base/FileInfo.h>
#include <Base/Stream.h>

#include "PropertyPath.h"
#include "PathPy.h"

using namespace Path;

TYPESYSTEM_SOURCE(Path::PropertyPath, App::Property);

PropertyPath::PropertyPath()
{
}

PropertyPath::~PropertyPath()
{
}

void PropertyPath::setValue(const Toolpath& pa)
{
    aboutToSetValue();
    _Path = pa;
    hasSetValue();
}


const Toolpath &PropertyPath::getValue(void)const 
{
    return _Path;
}

PyObject *PropertyPath::getPyObject(void)
{
    return new PathPy(new Toolpath(_Path));
}

void PropertyPath::setPyObject(PyObject *value)
{
    if (PyObject_TypeCheck(value, &(PathPy::Type))) {
        PathPy *pcObject = static_cast<PathPy*>(value);
        setValue(*pcObject->getToolpathPtr());
    }
    else {
        std::string error = std::string("type must be 'Path', not ");
        error += value->ob_type->tp_name;
        throw Base::TypeError(error);
    }
}

App::Property *PropertyPath::Copy(void) const
{
    PropertyPath *prop = new PropertyPath();
    prop->_Path = this->_Path;
 
    return prop;
}

void PropertyPath::Paste(const App::Property &from)
{
    aboutToSetValue();
    _Path = dynamic_cast<const PropertyPath&>(from)._Path;
    hasSetValue();
}

unsigned int PropertyPath::getMemSize (void) const
{
    return _Path.getMemSize();
}

void PropertyPath::Save (Base::Writer &writer) const
{
    _Path.Save(writer);
}

void PropertyPath::Restore(Base::XMLReader &reader)
{
    Path::Toolpath temp;
    temp.Restore(reader);
    setValue(temp);
}




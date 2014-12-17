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

#endif
#include <boost/algorithm/string.hpp>
#include <Base/Vector3D.h>
#include <Base/Rotation.h>
#include <Base/Writer.h>
#include <Base/Reader.h>
#include <Base/Exception.h>
#include "Command.h"

using namespace Base;
using namespace Path;

TYPESYSTEM_SOURCE(Path::Command , Base::Persistence);

// Constructors & destructors

Command::Command(const char* name,
                 const std::map<std::string, double>& parameters)
:Name(name),Parameters(parameters)
{
}

Command::Command()
{
}

Command::~Command()
{
}

// New methods

Placement Command::getPlacement (void)
{
    std::string x = "X";
    std::string y = "Y";
    std::string z = "Z";
    std::string a = "A";
    std::string b = "B";
    std::string c = "C";
    double xval = 0.0;
    double yval = 0.0;
    double zval = 0.0;
    double aval = 0.0;
    double bval = 0.0;
    double cval = 0.0;
    if (Parameters.count(x))
        xval = Parameters[x];
    if (Parameters.count(y))
        yval = Parameters[y];
    if (Parameters.count(z))
        zval = Parameters[z];
    if (Parameters.count(a))
        aval = Parameters[a];
    if (Parameters.count(b))
        bval = Parameters[b];
    if (Parameters.count(c))
        cval = Parameters[c];
    Vector3d vec(xval,yval,zval);
    Rotation rot;
    rot.setYawPitchRoll(aval,bval,cval);
    Placement plac(vec,rot);
    return plac;
}

Vector3d Command::getCenter (void)
{
    std::string i = "I";
    std::string j = "J";
    std::string k = "K";
    double ival = 0.0;
    double jval = 0.0;
    double kval = 0.0;
    if (Parameters.count(i))
        ival = Parameters[i];
    if (Parameters.count(j))
        jval = Parameters[j];
    if (Parameters.count(k))
        kval = Parameters[k];
    Vector3d vec(ival,jval,kval);
    return vec;
}

std::string Command::toGCode (void) const
{
    std::stringstream str;
    str.precision(5);
    str << Name;
    for(std::map<std::string,double>::const_iterator i = Parameters.begin(); i != Parameters.end(); ++i) {
        std::string k = i->first;
        double v = i->second;
        str << k << v;
    }
    return str.str();
}

void Command::setFromGCode (std::string str)
{
    Parameters.clear();
    std::string mode = "none";
    std::string key;
    std::string value;
    for (unsigned int i=0; i < str.size(); i++) {
        if ( (isdigit(str[i])) || (str[i] == '-') || (str[i] == '.') ) {
            value += str[i];
        } else if (isalpha(str[i])) {
            if (mode == "command") {
                if (!key.empty() && !value.empty()) {
                    std::string cmd = key + value;
                    boost::to_upper(cmd);
                    Name = cmd;
                    key = "";
                    value = "";
                    mode = "argument";
                } else {
                    throw Base::Exception("Badly formatted GCode command");
                }
                mode = "argument";
            } else if (mode == "none") {
                mode = "command";
            } else if (mode == "argument") {
                if (!key.empty() && !value.empty()) {
                    double val = std::atof(value.c_str());
                    boost::to_upper(key);
                    Parameters[key] = val;
                    key = "";
                    value = "";
                } else {
                    throw Base::Exception("Badly formatted GCode argument");
                }
            }
            key = str[i];
        }
    }
    if (!key.empty() && !value.empty()) {
        if (mode == "command") {
            std::string cmd = key + value;
            boost::to_upper(cmd);
            Name = cmd;
        } else {
            double val = std::atof(value.c_str());
            boost::to_upper(key);
            Parameters[key] = val;
        }
    } else {
        throw Base::Exception("Badly formatted GCode argument");
    }
}

void Command::setFromPlacement (const Base::Placement &plac)
{
    Name = "G1";
    Parameters.clear();
    std::string x = "X";
    std::string y = "Y";
    std::string z = "Z";
    std::string a = "A";
    std::string b = "B";
    std::string c = "C";
    double xval, yval, zval, aval, bval, cval;
    xval = plac.getPosition().x;
    yval = plac.getPosition().y;
    zval = plac.getPosition().z;
    plac.getRotation().getYawPitchRoll(aval,bval,cval);
    if (xval != 0.0)
        Parameters[x] = xval;
    if (yval != 0.0)
        Parameters[y] = yval;
    if (zval != 0.0)
        Parameters[z] = zval;
    if (aval != 0.0)
        Parameters[a] = aval;
    if (bval != 0.0)
        Parameters[b] = bval;
    if (cval != 0.0)
        Parameters[c] = cval;
}

// Reimplemented from base class

unsigned int Command::getMemSize (void) const
{
    return toGCode().size();
}

void Command::Save (Writer &writer) const
{
    // this will only get used if saved as XML (probably never)
    writer.Stream() << writer.ind() << "<Command "
                    << "gcode=\"" << toGCode() << "\" />";
    writer.Stream()<< std::endl;
}

void Command::Restore(XMLReader &reader)
{
    reader.readElement("Command");
    std::string gcode = reader.getAttribute("gcode");
    setFromGCode(gcode);
}


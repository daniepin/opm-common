/*
  Copyright 2019 Equinor ASA.

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef PYACTION_HPP_
#define PYACTION_HPP_


#include <string>

namespace Opm {

class PyAction {
public:
    explicit PyAction(const std::string& code_arg);
    const std::string& code() const;
    ~PyAction();

    /*
      Storage is a void pointer to a Python dictionary: py::dict. It is represented
      with a void pointer in this way to avoid adding the Pybind11 headers to this
      file. Calling scope must do a cast before using the storage pointer:

          py::dict * storage = static_cast<py::dict *>(py_action.storage());

      The purpose of this dictionary is to allow PYACTION scripts to store state
      between invocations.
    */
    void * storage() const;
private:
    std::string input_code;
    void * m_storage;
};

}

#endif

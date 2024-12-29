// ----------------------------------------------------------------
//		simple function for lua

%{
#include <vector>
#include <iostream>

std::vector<double> get_vector_from_lua(std::vector<double> vec) {
    return vec;
}
%}

// Include the typemaps
%include "typemaps.i"

// Declare the typemaps
%typemap(in) std::vector<double> {
    int size = SWIG_itable_size(L, $input);
    $1.resize(size);
    for (int i = 0; i < size; i++) {
        lua_rawgeti(L, $input, i + 1);
        if (!lua_isnumber(L, -1)) {
            SWIG_fail;
        }
        $1[i] = lua_tonumber(L, -1);
        lua_pop(L, 1);
    }
}

%typemap(argout) std::vector<double> {
    lua_newtable(L);
    for (size_t i = 0; i < $1.size(); ++i) {
        lua_pushnumber(L, $1[i]);
        lua_rawseti(L, -2, i + 1);
    }
    SWIG_arg++;
}

%typemap(argout) std::vector<double>* {
    lua_newtable(L);
    for (size_t i = 0; i < $1->size(); ++i) {
        lua_pushnumber(L, $1->at(i));
        lua_rawseti(L, -2, i + 1);
    }
    SWIG_arg++;
}



// Wrap the function
std::vector<double> get_vector_from_lua(std::vector<double> vec);


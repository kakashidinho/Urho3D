#!/usr/bin/python
# Copyright 2017 The ANGLE Project Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# gen_emulated_builtin_function_tables.py:
#  Generator for the builtin function maps.

from datetime import date
import json
import os, sys

template_emulated_builtin_functions_hlsl = """// GENERATED FILE - DO NOT EDIT.
// Generated by {script_name} using data from {data_source_name}.
//
// Copyright {copyright_year} The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// emulated_builtin_functions_hlsl:
//   HLSL code for emulating GLSL builtin functions not present in HLSL.

#include "compiler/translator/BuiltInFunctionEmulator.h"
#include "compiler/translator/tree_util/BuiltIn_autogen.h"

namespace sh
{{

namespace
{{

struct FunctionPair
{{
   constexpr FunctionPair(const TSymbolUniqueId &idIn, const char *bodyIn) : id(idIn.get()), body(bodyIn)
   {{
   }}

   int id;
   const char *body;
}};

constexpr FunctionPair g_hlslFunctions[] = {{
{emulated_functions}}};
}}  // anonymous namespace

const char *FindHLSLFunction(int uniqueId)
{{
    for (size_t index = 0; index < ArraySize(g_hlslFunctions); ++index)
    {{
        const auto &function = g_hlslFunctions[index];
        if (function.id == uniqueId)
        {{
            return function.body;
        }}
    }}

    return nullptr;
}}
}}  // namespace sh
"""

def reject_duplicate_keys(pairs):
    found_keys = {}
    for key, value in pairs:
        if key in found_keys:
           raise ValueError("duplicate key: %r" % (key,))
        else:
           found_keys[key] = value
    return found_keys

def load_json(path):
    with open(path) as map_file:
        file_data = map_file.read()
        map_file.close()
        return json.loads(file_data, object_pairs_hook=reject_duplicate_keys)

def enum_type(arg):
    # handle 'argtype argname' and 'out argtype argname'
    chunks = arg.split(' ')
    arg_type = chunks[0]
    if len(chunks) == 3:
        arg_type = chunks[1]

    suffix = ""
    if not arg_type[-1].isdigit():
        suffix = '1'
    if arg_type[0:4] == 'uint':
        return 'UI' + arg_type[2:] + suffix
    return arg_type.capitalize() + suffix

input_script = "emulated_builtin_function_data_hlsl.json"
hlsl_json = load_json(input_script)
emulated_functions = []

def gen_emulated_function(data):

   func = ""
   if 'comment' in data:
      func += "".join([ "// " + line + "\n" for line in data['comment'] ])

   sig = data['return_type'] + ' ' + data['op'] + '_emu(' + ', '.join(data['args']) + ')'
   body = [ sig, '{' ] + ['    ' + line for line in data['body']] + ['}']

   func += "{\n"
   func += "BuiltInId::" + data['op'] + "_" + "_".join([enum_type(arg) for arg in data['args']]) + ",\n"
   if 'helper' in data:
      func += '"' + '\\n"\n"'.join(data['helper']) + '\\n"\n'
   func += '"' + '\\n"\n"'.join(body) + '\\n"\n'
   func += "},\n"
   return [ func ]

for item in hlsl_json:
   emulated_functions += gen_emulated_function(item)

hlsl_fname = "emulated_builtin_functions_hlsl_autogen.cpp"

hlsl_gen = template_emulated_builtin_functions_hlsl.format(
   script_name = sys.argv[0],
   data_source_name = input_script,
   copyright_year = date.today().year,
   emulated_functions = "".join(emulated_functions))

with open(hlsl_fname, 'wt') as f:
   f.write(hlsl_gen)
   f.close()

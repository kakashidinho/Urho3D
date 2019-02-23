#!python
# Copyright 2017 The ANGLE Project Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# gen_proc_table.py:
#  Code generation for entry point loading tables.

# TODO(jmadill): Should be part of entry point generation.

import sys
from datetime import date

data_source_name = "proc_table_data.json"
out_file_name = "proc_table_autogen.cpp"

# The EGL_ANGLE_explicit_context extension is generated differently from other extensions.
# Toggle generation here.
support_egl_ANGLE_explicit_context = True

template_cpp = """// GENERATED FILE - DO NOT EDIT.
// Generated by {script_name} using data from {data_source_name}.
//
// Copyright {copyright_year} The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// getProcAddress loader table:
//   Mapping from a string entry point name to function address.
//

#include "libGLESv2/proc_table.h"

#include "libGLESv2/entry_points_egl.h"
#include "libGLESv2/entry_points_egl_ext.h"
#include "libGLESv2/entry_points_gles_1_0_autogen.h"
#include "libGLESv2/entry_points_gles_2_0_autogen.h"
#include "libGLESv2/entry_points_gles_3_0_autogen.h"
#include "libGLESv2/entry_points_gles_3_1_autogen.h"
#include "libGLESv2/entry_points_gles_ext_autogen.h"
#include "platform/Platform.h"

#define P(FUNC) reinterpret_cast<__eglMustCastToProperFunctionPointerType>(FUNC)

namespace egl
{{
ProcEntry g_procTable[] = {{
{proc_data}
}};

size_t g_numProcs = {num_procs};
}}  // namespace egl
"""

sys.path.append('../libANGLE/renderer')
import angle_format

json_data = angle_format.load_json(data_source_name)

all_functions = {}

for description, functions in json_data.iteritems():
    for function in functions:
        if function.startswith("gl"):
            all_functions[function] = "gl::" + function[2:]
            # Special handling for EGL_ANGLE_explicit_context extension
            if support_egl_ANGLE_explicit_context:
                all_functions[function + "ContextANGLE"] = "gl::" + function[2:] + "ContextANGLE"
        elif function.startswith("egl"):
            all_functions[function] = "EGL_" + function[3:]
        else:
            all_functions[function] = function

proc_data = [('    {"%s", P(%s)}' % (func, angle_func)) for func, angle_func in sorted(all_functions.iteritems())]

with open(out_file_name, 'wb') as out_file:
    output_cpp = template_cpp.format(
        script_name = sys.argv[0],
        data_source_name = data_source_name,
        copyright_year = date.today().year,
        proc_data = ",\n".join(proc_data),
        num_procs = len(proc_data))
    out_file.write(output_cpp)
    out_file.close()
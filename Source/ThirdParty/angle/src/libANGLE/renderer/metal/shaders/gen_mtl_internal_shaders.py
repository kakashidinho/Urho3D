#!/usr/bin/python
# Copyright 2019 The ANGLE Project Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# gen_mtl_internal_shaders.py:
#   Code generation for Metal backend's default shaders.
#   NOTE: don't run this script directly. Run scripts/run_code_generation.py.

import os
import sys
import json
from datetime import datetime

template_header_boilerplate = """// GENERATED FILE - DO NOT EDIT.
// Generated by {script_name}
//
// Copyright {copyright_year} The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
"""


def append_file_as_byte_array_string(variable_name, filename, dest_src_file):
    string = '// Generated from {0}:\n'.format(filename)
    string += 'constexpr uint8_t {0}[]={{\n'.format(variable_name)
    bytes_ = open(filename, "rb").read()
    for byte in bytes_:
        string += '0x{:02x}'.format(ord(byte)) + ", "
    string += "\n};\n"
    with open(dest_src_file, "a") as out_file:
        out_file.write(string)


def gen_precompiled_shaders(mac_version, ios_version, variable_name):
    print('Compiling macos {0} version of default shaders ...'.format(mac_version))
    os.system(
        'xcrun -sdk macosx metal master_source.metal -mmacosx-version-min={0} -c -o compiled/default.{0}.air'
        .format(mac_version))
    os.system(
        'xcrun -sdk macosx metallib compiled/default.{0}.air -o compiled/default.{0}.metallib'
        .format(mac_version))

    print('Compiling ios {0} version of default shaders ...'.format(ios_version))
    os.system(
        'xcrun -sdk iphoneos metal master_source.metal -mios-version-min={0} -c -o compiled/default.ios.{0}.air'
        .format(ios_version))
    os.system(
        'xcrun -sdk iphoneos metallib compiled/default.ios.{0}.air -o compiled/default.ios.{0}.metallib'
        .format(ios_version))

    print('Compiling ios {0} simulator version of default shaders ...'.format(ios_version))
    os.system(
        'xcrun -sdk iphonesimulator metal master_source.metal -c -o compiled/default.ios_sim.{0}.air'
        .format(ios_version))
    os.system(
        'xcrun -sdk iphonesimulator metallib compiled/default.ios_sim.{0}.air -o compiled/default.ios_sim.{0}.metallib'
        .format(ios_version))

    # Mac version
    os.system(
        'echo "#if TARGET_OS_OSX || TARGET_OS_MACCATALYST\n" >> compiled/mtl_default_shaders.inc')
    append_file_as_byte_array_string(variable_name,
                                     'compiled/default.{0}.metallib'.format(mac_version),
                                     'compiled/mtl_default_shaders.inc')
    os.system('echo "constexpr size_t {0}_len=sizeof({0});" >> compiled/mtl_default_shaders.inc'
              .format(variable_name))

    # iOS simulator version
    os.system(
        'echo "\n#elif TARGET_OS_SIMULATOR  // TARGET_OS_OSX || TARGET_OS_MACCATALYST\n" >> compiled/mtl_default_shaders.inc'
    )

    append_file_as_byte_array_string(variable_name,
                                     'compiled/default.ios_sim.{0}.metallib'.format(ios_version),
                                     'compiled/mtl_default_shaders.inc')
    os.system('echo "constexpr size_t {0}_len=sizeof({0});" >> compiled/mtl_default_shaders.inc'
              .format(variable_name))

    # iOS version
    os.system(
        'echo "\n#elif TARGET_OS_IOS  // TARGET_OS_OSX || TARGET_OS_MACCATALYST\n" >> compiled/mtl_default_shaders.inc'
    )

    append_file_as_byte_array_string(variable_name,
                                     'compiled/default.ios.{0}.metallib'.format(ios_version),
                                     'compiled/mtl_default_shaders.inc')
    os.system('echo "constexpr size_t {0}_len=sizeof({0});" >> compiled/mtl_default_shaders.inc'
              .format(variable_name))

    os.system(
        'echo "#endif  // TARGET_OS_OSX || TARGET_OS_MACCATALYST\n" >> compiled/mtl_default_shaders.inc'
    )

    os.system('rm -rfv compiled/default.*')


def main():
    # auto_script parameters.
    if len(sys.argv) > 1:
        inputs = [
            'master_source.metal', 'blit.metal', 'clear.metal', 'gen_indices.metal', 'common.h'
        ]
        outputs = ['compiled/mtl_default_shaders.inc', 'mtl_default_shaders_src_autogen.inc']

        if sys.argv[1] == 'inputs':
            print ','.join(inputs)
        elif sys.argv[1] == 'outputs':
            print ','.join(outputs)
        else:
            print('Invalid script parameters')
            return 1
        return 0

    os.chdir(sys.path[0])

    boilerplate_code = template_header_boilerplate.format(
        script_name=sys.argv[0], copyright_year=datetime.today().year)

    # boilder plate code
    os.system("echo \"{0}\" > compiled/mtl_default_shaders.inc".format(boilerplate_code))
    os.system(
        'echo "// Compiled binary for Metal default shaders.\n\n" >> compiled/mtl_default_shaders.inc'
    )
    os.system('echo "#include <TargetConditionals.h>\n\n" >> compiled/mtl_default_shaders.inc')

    os.system('echo "// clang-format off" >> compiled/mtl_default_shaders.inc')

    # pre-compiled shaders
    gen_precompiled_shaders(10.13, 11.0, 'compiled_default_metallib')
    gen_precompiled_shaders(10.14, 12.0, 'compiled_default_metallib_2_1')

    os.system('echo "// clang-format on" >> compiled/mtl_default_shaders.inc')

    # Write full source string for debug purpose
    os.system("echo \"{0}\" > mtl_default_shaders_src_autogen.inc".format(boilerplate_code))
    os.system(
        'echo "// C++ string version of Metal default shaders for debug purpose.\n\n" >> mtl_default_shaders_src_autogen.inc'
    )
    os.system(
        'echo "\n\nconstexpr char default_metallib_src[] = R\\"(" >> mtl_default_shaders_src_autogen.inc'
    )
    os.system('echo "#include <metal_stdlib>" >> mtl_default_shaders_src_autogen.inc')
    os.system('echo "#include <simd/simd.h>" >> mtl_default_shaders_src_autogen.inc')
    os.system(
        'clang -xc++ -E -DGENERATE_SOURCE_STRING master_source.metal >> mtl_default_shaders_src_autogen.inc'
    )
    os.system('echo ")\\";" >> mtl_default_shaders_src_autogen.inc')


if __name__ == '__main__':
    sys.exit(main())

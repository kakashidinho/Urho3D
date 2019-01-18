//
// Copyright (c) 2017 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// ClampPointSize.h: Limit the value that is written to gl_PointSize.
//

#ifndef COMPILER_TRANSLATOR_TREEOPS_CLAMPPOINTSIZE_H_
#define COMPILER_TRANSLATOR_TREEOPS_CLAMPPOINTSIZE_H_

namespace sh
{

class TIntermBlock;
class TSymbolTable;

void ClampPointSize(TIntermBlock *root, float maxPointSize, TSymbolTable *symbolTable);

}  // namespace sh

#endif  // COMPILER_TRANSLATOR_TREEOPS_CLAMPPOINTSIZE_H_

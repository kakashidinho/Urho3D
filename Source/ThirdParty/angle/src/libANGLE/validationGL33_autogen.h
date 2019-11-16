// GENERATED FILE - DO NOT EDIT.
// Generated by generate_entry_points.py using data from gl.xml and wgl.xml.
//
// Copyright 2019 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// validationGL33_autogen.h:
//   Validation functions for the OpenGL 3.3 entry points.

#ifndef LIBANGLE_VALIDATION_GL33_AUTOGEN_H_
#define LIBANGLE_VALIDATION_GL33_AUTOGEN_H_

#include "common/PackedEnums.h"

namespace gl
{
class Context;

bool ValidateBindFragDataLocationIndexed(Context *context,
                                         ShaderProgramID programPacked,
                                         GLuint colorNumber,
                                         GLuint index,
                                         const GLchar *name);
bool ValidateColorP3ui(Context *context, GLenum type, GLuint color);
bool ValidateColorP3uiv(Context *context, GLenum type, const GLuint *color);
bool ValidateColorP4ui(Context *context, GLenum type, GLuint color);
bool ValidateColorP4uiv(Context *context, GLenum type, const GLuint *color);
bool ValidateGetFragDataIndex(Context *context, ShaderProgramID programPacked, const GLchar *name);
bool ValidateGetQueryObjecti64v(Context *context, QueryID idPacked, GLenum pname, GLint64 *params);
bool ValidateGetQueryObjectui64v(Context *context,
                                 QueryID idPacked,
                                 GLenum pname,
                                 GLuint64 *params);
bool ValidateGetSamplerParameterIiv(Context *context,
                                    SamplerID samplerPacked,
                                    GLenum pname,
                                    GLint *params);
bool ValidateGetSamplerParameterIuiv(Context *context,
                                     SamplerID samplerPacked,
                                     GLenum pname,
                                     GLuint *params);
bool ValidateMultiTexCoordP1ui(Context *context, GLenum texture, GLenum type, GLuint coords);
bool ValidateMultiTexCoordP1uiv(Context *context,
                                GLenum texture,
                                GLenum type,
                                const GLuint *coords);
bool ValidateMultiTexCoordP2ui(Context *context, GLenum texture, GLenum type, GLuint coords);
bool ValidateMultiTexCoordP2uiv(Context *context,
                                GLenum texture,
                                GLenum type,
                                const GLuint *coords);
bool ValidateMultiTexCoordP3ui(Context *context, GLenum texture, GLenum type, GLuint coords);
bool ValidateMultiTexCoordP3uiv(Context *context,
                                GLenum texture,
                                GLenum type,
                                const GLuint *coords);
bool ValidateMultiTexCoordP4ui(Context *context, GLenum texture, GLenum type, GLuint coords);
bool ValidateMultiTexCoordP4uiv(Context *context,
                                GLenum texture,
                                GLenum type,
                                const GLuint *coords);
bool ValidateNormalP3ui(Context *context, GLenum type, GLuint coords);
bool ValidateNormalP3uiv(Context *context, GLenum type, const GLuint *coords);
bool ValidateQueryCounter(Context *context, QueryID idPacked, QueryType targetPacked);
bool ValidateSamplerParameterIiv(Context *context,
                                 SamplerID samplerPacked,
                                 GLenum pname,
                                 const GLint *param);
bool ValidateSamplerParameterIuiv(Context *context,
                                  SamplerID samplerPacked,
                                  GLenum pname,
                                  const GLuint *param);
bool ValidateSecondaryColorP3ui(Context *context, GLenum type, GLuint color);
bool ValidateSecondaryColorP3uiv(Context *context, GLenum type, const GLuint *color);
bool ValidateTexCoordP1ui(Context *context, GLenum type, GLuint coords);
bool ValidateTexCoordP1uiv(Context *context, GLenum type, const GLuint *coords);
bool ValidateTexCoordP2ui(Context *context, GLenum type, GLuint coords);
bool ValidateTexCoordP2uiv(Context *context, GLenum type, const GLuint *coords);
bool ValidateTexCoordP3ui(Context *context, GLenum type, GLuint coords);
bool ValidateTexCoordP3uiv(Context *context, GLenum type, const GLuint *coords);
bool ValidateTexCoordP4ui(Context *context, GLenum type, GLuint coords);
bool ValidateTexCoordP4uiv(Context *context, GLenum type, const GLuint *coords);
bool ValidateVertexAttribP1ui(Context *context,
                              GLuint index,
                              GLenum type,
                              GLboolean normalized,
                              GLuint value);
bool ValidateVertexAttribP1uiv(Context *context,
                               GLuint index,
                               GLenum type,
                               GLboolean normalized,
                               const GLuint *value);
bool ValidateVertexAttribP2ui(Context *context,
                              GLuint index,
                              GLenum type,
                              GLboolean normalized,
                              GLuint value);
bool ValidateVertexAttribP2uiv(Context *context,
                               GLuint index,
                               GLenum type,
                               GLboolean normalized,
                               const GLuint *value);
bool ValidateVertexAttribP3ui(Context *context,
                              GLuint index,
                              GLenum type,
                              GLboolean normalized,
                              GLuint value);
bool ValidateVertexAttribP3uiv(Context *context,
                               GLuint index,
                               GLenum type,
                               GLboolean normalized,
                               const GLuint *value);
bool ValidateVertexAttribP4ui(Context *context,
                              GLuint index,
                              GLenum type,
                              GLboolean normalized,
                              GLuint value);
bool ValidateVertexAttribP4uiv(Context *context,
                               GLuint index,
                               GLenum type,
                               GLboolean normalized,
                               const GLuint *value);
bool ValidateVertexP2ui(Context *context, GLenum type, GLuint value);
bool ValidateVertexP2uiv(Context *context, GLenum type, const GLuint *value);
bool ValidateVertexP3ui(Context *context, GLenum type, GLuint value);
bool ValidateVertexP3uiv(Context *context, GLenum type, const GLuint *value);
bool ValidateVertexP4ui(Context *context, GLenum type, GLuint value);
bool ValidateVertexP4uiv(Context *context, GLenum type, const GLuint *value);
}  // namespace gl

#endif  // LIBANGLE_VALIDATION_GL33_AUTOGEN_H_

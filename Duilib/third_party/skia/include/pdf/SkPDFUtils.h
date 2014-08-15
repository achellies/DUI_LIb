/*
 * Copyright (C) 2011 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SkPDFUtils_DEFINED
#define SkPDFUtils_DEFINED

#define NOT_IMPLEMENTED(condition, assert)                         \
    do {                                                           \
        if (condition) {                                           \
            fprintf(stderr, "NOT_IMPLEMENTED: " #condition "\n");  \
            SkDEBUGCODE(SkASSERT(!assert);)                        \
        }                                                          \
    } while(0)

class SkMatrix;
class SkPath;
class SkPDFArray;

class SkPDFUtils {
public:
    static SkPDFArray* MatrixToArray(const SkMatrix& matrix);
    
    static void MoveTo(SkScalar x, SkScalar y, SkString* content);
    static void AppendLine(SkScalar x, SkScalar y, SkString* content);
    static void AppendCubic(SkScalar ctl1X, SkScalar ctl1Y,
                            SkScalar ctl2X, SkScalar ctl2Y,
                            SkScalar dstX, SkScalar dstY, SkString* content);
    static void AppendRectangle(SkScalar x, SkScalar y, SkScalar w, SkScalar h,
                                SkString* content);
    static void EmitPath(const SkPath& path, SkString* content);
    static void ClosePath(SkString* content);
    static void PaintPath(SkPaint::Style style, SkPath::FillType fill,
                          SkString* content);
    static void StrokePath(SkString* content);
};

#endif

#ifndef __CCVERTEX_H__
#define __CCVERTEX_H__

#include "math/Vec2.h"

/**
 * @addtogroup base
 * @{
 */

NS_CC_BEGIN

/** @file CCVertex.h */

/** converts a line to a polygon */
void SGCC_DLL ccVertexLineToPolygon(Vec2 *points, float stroke, Vec2 *vertices, unsigned int offset, unsigned int nuPoints);

/** returns whether or not the line intersects */
bool SGCC_DLL ccVertexLineIntersect(float Ax, float Ay,
                             float Bx, float By,
                             float Cx, float Cy,
                             float Dx, float Dy, float *T);

NS_CC_END


// end of base group
/// @}

#endif /* __CCVERTEX_H__ */


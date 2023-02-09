/*
 * 작성자: 윤정도
 * 생성일: 1/11/2023 1:08:09 AM
 * =====================
 *
 */


#pragma once

struct NpkSpriteRect
{
	int X;
	int Y;
	int Width;
	int Height;
	int FrameWidth;
	int FrameHeight;

	float GetWidthF()		const { return (float)Width;			}
	float GetHeightF()		const { return (float)Height;			}
	float GetXF()			const { return (float)X;				}
	float GetYF()			const { return (float)Y;				}
	float GetFrameWidthF()	const { return (float)FrameWidth;		}
	float GetFrameHeightF() const { return (float)FrameHeight;		}
};
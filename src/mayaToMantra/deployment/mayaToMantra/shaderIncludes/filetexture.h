#ifndef FILETEX_H
#define FILETEX_H

#include "utils.h"

void FileTexture(vector uv; 
				 vector defaultColor; 
				 vector colorGain; 
				 vector colorOffset; 
				 float alphaGain; 
				 float alphaOffset; 
				 int alphaIsLuminance; 
				 int invert; 
				 vector uvCoord;
				 string textureFileName;
				 vector outColor; 
				 float outAlpha)
{
	int fptex = getprimid();
	float ss = uvCoord.x;
	float tt = uvCoord.y;
	vector4 colWithAlpha = texture(textureFileName, ss, tt, "wrap", "repeat",
                        "filter", "box", "width", 1,
                        "border", { 0, 0, 0, 1 }, "extrapolate", 1,
                        "channel", "", "face", fptex,
                        "lerp", 0, "pixelblur", 0);
	outColor = set(colWithAlpha.r, colWithAlpha.g, colWithAlpha.b);
	outAlpha = colWithAlpha.a;

	colorBalance( alphaIsLuminance, alphaGain, alphaOffset, colorGain, colorOffset, invert, outColor, outAlpha);

}

#endif

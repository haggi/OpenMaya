#ifndef BLENDC_H
#define BLENDC_H

void BlendColors( vector color1; vector color2; float blender; vector outColor)
{
	outColor = color1 * (1.0 - blender) + color2 * blender;
}

#endif
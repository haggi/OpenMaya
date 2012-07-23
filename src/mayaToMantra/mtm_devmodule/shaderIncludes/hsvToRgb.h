#ifndef HSVTORGB_H
#define HSVTORGB_H

void HsvToRgb(
	vector inHsv;
	vector outRgb)
{
	float H = inHsv.x / 360.0;
	vector tmp = set( H, inHsv.y, inHsv.z );	
	outRgb = ctransform("hsv", "rgb", tmp);
}

#endif


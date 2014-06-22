// 
// maya stadard utilities
//
						  
#define MAYA_COLORBALANCE color defaultColor = color(.5,.5,.5), \
						  color colorGain = color(1.0,1.0,1.0), \
						  color colorOffset = color(.0,.0,.0)

//vector colorGain; vector colorOffset; float alphaGain; float alphaOffset; int alphaIsLuminance; int invert; vector uvCoord; float filter; float filterOffset

color colorBalance( color c, color colorGain, color colorOffset)
{
	color result = c;
	result *= colorGain;
	result += colorOffset;
	return result;
}


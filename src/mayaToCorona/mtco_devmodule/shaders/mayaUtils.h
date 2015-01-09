// 
// maya stadard utilities
//
						  
#define MAYA_COLORBALANCE float exposure = 0.0, \
						  vector defaultColor = vector(.5,.5,.5), \
						  vector colorGain = vector(1.0,1.0,1.0), \
						  vector colorOffset = vector(.0,.0,.0), \
						  float alphaGain = 1.0, \
						  float alphaOffset = 0.0, \
						  int alphaIsLuminance = 1, \
						  int invert=0

#define MAYA_DEFAULT_OUTPUT output vector outColor = 0, \
							output float outAlpha = 0

//vector colorGain; vector colorOffset; float alphaGain; float alphaOffset; int alphaIsLuminance; int invert; vector uvCoord; float filter; float filterOffset

float sampleArea()
{
	return length(cross(dPdu, dPdv));
}

vector colorBalance( vector c, vector colorGain, vector colorOffset)
{
	vector result = c;
	result *= colorGain;
	result += colorOffset;
	return result;
}

#define WRAPMAX (1.000001)

float integral(float t, float nedge)
{
   return ((1.0 - nedge) * floor(t) + max(0.0, t - floor(t) - nedge));
}

float filteredPulseTrain(float edge, float period, float x, float dx)
{
   float invPeriod = 1.0 / period;

   float w = dx * invPeriod;
   float x0 = x * invPeriod - 0.5 * w;
   float x1 = x0 + w;
   float nedge = edge * invPeriod;

   float result;

   if (x0 == x1)
   {
     result = (x0 - floor(x0) < nedge) ? 0.0 : 1.0;
   }
   else
   {
      result = (integral(x1, nedge) - integral(x0, nedge)) / w;
   }

   return result;
}


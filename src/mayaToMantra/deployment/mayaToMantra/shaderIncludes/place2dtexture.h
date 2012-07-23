#ifndef PLACE2DTEX_H
#define PLACE2DTEX_H

void getUvCoords( vector uv; vector uvw; float u; float v )
{
    int bound_uv = isbound("uv");
    if( bound_uv ) 
	{
        uvw = uv;
    } else {
        uvw = set(s, t, .5);
    }
	u = uvw.x;
	v = uvw.y;
	uv = uvw;
}

void Place2dTexture(vector uv; vector repeatUVW; vector outUVW)
{
	vector uvw = {0,0,0};
	//vector uv = {0,0,0};

    int bound_uv = isbound("uv");
    if( bound_uv ) 
	{
        uvw = uv;
    } else {
        uvw = set(s, t, .5);
    }

	//getUvCoords(uv, uvw, u , v);
	float ss = uvw.x * repeatUVW.x;
	float tt = uvw.y * repeatUVW.y;
	//ss = s * repeatUVW.x;
	//tt = t * repeatUVW.y;
	outUVW = set(ss, tt, 1.0);
}

#endif


void LightProjection(
					 vector outUVW;
					 )
{
	vector pos = wo_space(Ps); // Transform into space of light
    pos = toNDC(pos);   // Transform into NDC space
	outUVW = set(pos.x, pos.y, -10000);
}
/*
	Copyright (c) 2006 soho vfx inc.
	Copyright (c) 2006 The 3Delight Team.
*/

#ifndef __multiplyDivide_h
#define __multiplyDivide_h

/*
begin inputs
	float operation
	float3 input1
	float3 input2
end inputs

begin outputs
	float3 output
end outputs
*/

void
maya_multiplyDivide(
	// Inputs
	//
	float i_operation;
	float i_input1[3];
	float i_input2[3];

	// Outputs
	//
	output float o_output[3];)
{
	if(i_operation == 0.0)
	{
		o_output[0] = i_input1[0];
		o_output[1] = i_input1[1];
		o_output[2] = i_input1[2];
	}
	else if(i_operation == 1.0)
	{
		o_output[0] = i_input1[0] * i_input2[0];
		o_output[1] = i_input1[1] * i_input2[1];
		o_output[2] = i_input1[2] * i_input2[2];
	}
	else if(i_operation == 2.0)
	{
		o_output[0] = i_input1[0] / i_input2[0];
		o_output[1] = i_input1[1] / i_input2[1];
		o_output[2] = i_input1[2] / i_input2[2];
	}
	else
	{
		o_output[0] = pow(i_input1[0], i_input2[0]);
		o_output[1] = pow(i_input1[1], i_input2[1]);
		o_output[2] = pow(i_input1[2], i_input2[2]);
	}
}

#endif /* __multiplyDivide_h */


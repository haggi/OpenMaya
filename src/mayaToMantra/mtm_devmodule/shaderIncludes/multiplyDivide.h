#ifndef MULTIPLY_DIVIDE_H
#define MULTIPLY_DIVIDE_H


void MultiplyDivide(
		int operation;
		vector input1;
		vector input2;
		vector output;
					)

{
	// noop
	if( operation == 0)
	{
		output = input1;
		return;
	}
	// multiply
	if( operation == 1)
	{
		output = input1 * input2;
		return;
	}
	// divide
	if( operation == 2)
	{
		output = input1 / input2;
		return;
	}
	// power
	if( operation == 3)
	{
		output.x = pow(input1.x,input2.x);
		output.y = pow(input1.y,input2.y);
		output.z = pow(input1.z,input2.z);
		return;
	}
}

#endif
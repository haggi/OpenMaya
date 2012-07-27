#ifndef PLUSMINUSAVG_H
#define PLUSMINUSAVG_H

void PlusMinusAverage(
	int operation;
	float input1D[];
	vector input2D[];
	vector input3D[];

	float output1D;
	vector output2D;
	vector output3D; 
	)
{
	// noop
	int arrayLen = 0;
	if(operation == 0)
	{
		arrayLen = arraylength(input1D);
		output1D = 0.0;
		if( arrayLen > 0)
			output1D = input1D[0];

		output2D = set(0,0,0);
		arrayLen = arraylength(input2D);
		if( arrayLen > 0)
			output2D = input2D[0];

		output3D = set(0,0,0);
		arrayLen = arraylength(input3D);
		if( arrayLen > 0)
			output3D = input3D[0];
		return;
	}

	output1D = 0;
	output2D = 0;
	output3D = 0;
	int counter = 0;
	// sum
	if(operation == 1)
	{
		arrayLen = arraylength(input1D);
		for( counter = 0; counter < arrayLen; counter++)
		{
			output1D += input1D[counter];
		}
		arrayLen = arraylength(input2D);
		for( counter = 0; counter < arrayLen; counter++)
		{
			output2D += input2D[counter];
		}
		arrayLen = arraylength(input3D);
		for( counter = 0; counter < arrayLen; counter++)
		{
			output3D += input3D[counter];
		}
		return;
	}

	// difference
	if(operation == 2)
	{
		arrayLen = arraylength(input1D);
		output1D =  input1D[0];
		for( counter = 1; counter < arrayLen; counter++)
		{
			output1D -= input1D[counter];
		}

		arrayLen = arraylength(input2D);
		output2D =  input2D[0];
		for( counter = 1; counter < arrayLen; counter++)
		{
			output2D -= input2D[counter];
		}
		arrayLen = arraylength(input3D);
		output3D =  input3D[0];
		for( counter = 1; counter < arrayLen; counter++)
		{
			output3D -= input3D[counter];
		}
		return;
	}
	// average
	if(operation == 3)
	{
		arrayLen = arraylength(input1D);
		for( counter = 0; counter < arrayLen; counter++)
		{
			output1D += input1D[counter];
		}
		output1D = output1D / arrayLen;
		arrayLen = arraylength(input2D);
		for( counter = 0; counter < arrayLen; counter++)
		{
			output2D += input2D[counter];
		}
		output2D = output2D / arrayLen;

		arrayLen = arraylength(input3D);
		for( counter = 0; counter < arrayLen; counter++)
		{
			output3D += input3D[counter];
		}
		output3D = output3D / arrayLen;
		return;
	}
}

//	uniform float n1 = arraylength(i_input1D);
//	uniform float n2 = arraylength(i_input2D) / 2;
//	uniform float n3 = arraylength(i_input3D);
//
//	// Start with the first element in output; this is common to all operations.
//	o_output1D = (n1 == 0) ? 0 : i_input1D[0];
//	if( n2 == 0 )
//	{
//		o_output2D[0] = 0;
//		o_output2D[1] = 0;
//	} else {
//		o_output2D[0] = i_input2D[0];
//		o_output2D[1] = i_input2D[1];
//	}
//	o_output3D = (n3 == 0) ? color(0) : i_input3D[0];
//
//	if( i_operation != 0 )
//	{
//		// Compute the sum of the other elements, needed for all but noop.
//		uniform float i;
//		float sum1 = 0;
//		float sum2a = 0, sum2b = 0;
//		color sum3 = 0;
//
//		for( i = 1; i < n1; i += 1 )
//		{
//			sum1 += i_input1D[i];
//		}
//
//		for( i = 2; i < n2*2; i += 2 )
//		{
//			sum2a += i_input2D[i];
//			sum2b += i_input2D[i + 1];
//		}
//
//		for( i = 1; i < n3; i += 1 )
//		{
//			sum3 += i_input3D[i];
//		}
//
//		if( i_operation == 2 )
//		{
//			// Subtract
//			o_output1D -= sum1;
//			o_output2D[0] -= sum2a;
//			o_output2D[1] -= sum2b;
//			o_output3D -= sum3;
//		}
//		else
//		{
//			// Add or average, start by adding.
//			o_output1D += sum1;
//			o_output2D[0] += sum2a;
//			o_output2D[1] += sum2b;
//			o_output3D += sum3;
//
//			if( i_operation == 3 )
//			{
//				// Average
//				o_output1D /= n1;
//				o_output2D[0] /= n2;
//				o_output2D[1] /= n2;
//				o_output3D /= n3;
//			}
//		}
//	}
//}
//
#endif


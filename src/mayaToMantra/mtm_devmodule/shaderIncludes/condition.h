#ifndef CONDITION_H
#define CONDITION_H

void Condition(
	int operation;
	float firstTerm;
	float secondTerm;
	vector colorIfTrue;
	vector colorIfFalse;
	vector outColor;)
{
	if(operation == 0)
	{
		outColor = (firstTerm == secondTerm ? colorIfTrue : colorIfFalse);
	}
	else if(operation == 1)
	{
		outColor = (firstTerm != secondTerm ? colorIfTrue : colorIfFalse);
	}
	else if(operation == 2)
	{
		outColor = (firstTerm > secondTerm ? colorIfTrue : colorIfFalse);
	}
	else if(operation == 3)
	{
		outColor = (firstTerm >= secondTerm ? colorIfTrue : colorIfFalse);
	}
	else if(operation == 4)
	{
		outColor = (firstTerm < secondTerm ? colorIfTrue : colorIfFalse);
	}
	else
	{
		outColor = (firstTerm <= secondTerm ? colorIfTrue : colorIfFalse);
	}
}

#endif
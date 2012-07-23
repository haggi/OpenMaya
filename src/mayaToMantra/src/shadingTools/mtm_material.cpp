#include <maya/MPlugArray.h>
#include <maya/MGlobal.h>
#include <maya/MFnDagNode.h>
#include "shadingTools/mtm_material.h"
#include "utilities/logging.h"
#include "utilities/tools.h"
#include "utilities/attrTools.h"
#include "utilities/pystring.h"

#include <list>

static Logging logger;

MString mtm_Material::getShaderIncludes(SNODE_LIST& shaderList)
{
	MString includes;
	for( uint i = 0; i < shaderList.size(); i++)
	{
		//logger.debug(MString("shaderIncludes, check node ") + shaderList[i]->internalName);
		if(pystring::find(includes.asChar(), shaderList[i]->internalName.asChar()) > -1)
		{
			//logger.debug("already definded, skipping");
			continue;
		}
		if(shaderList[i]->userPointer != NULL)
		{
			shadingNodeUserData *snud = (shadingNodeUserData *)shaderList[i]->userPointer;
			if( snud->isLightShaderPart )
			{
				if(pystring::find(includes.asChar(), "LightProjection.h") > -1)
					continue;
				else{
					includes += MString("#include ") + "\"LightProjection.h\"\n";
				}
			}
		}
		includes += MString("#include ") + "\"" + shaderList[i]->internalName + ".h\"\n";
	}
	return includes;
}

MString mtm_Material::getHoudiniType(ShadingPlug *sp)
{
	MString hType;
	
	if(sp->plugType == "float")
	{
		return MString("float");
	}
	if(sp->plugType == "float2")
	{
		return MString("vector");
	}
	if(sp->plugType == "color")
	{
		return MString("vector");
	}
	if(sp->plugType == "vector")
	{
		return MString("vector");
	}
	if(sp->plugType == "int")
	{
		return MString("int");
	}
	if(sp->plugType == "bool")
	{
		return MString("int");
	}
	if(sp->plugType == "string")
	{
		return MString("string");
	}
	if(sp->plugType == "matrix")
	{
		return MString("matrix");
	}

	if(sp->plugType == "intArray")
	{
		return MString("int");
	}
	if(sp->plugType == "floatArray")
	{
		return MString("float");
	}
	if(sp->plugType == "colorArray")
	{
		return MString("vector");
	}
	if(sp->plugType == "vectorArray")
	{
		return MString("vector");
	}
	if(sp->plugType == "matrixArray")
	{
		return MString("matrix");
	}

	return hType;
}

MString mtm_Material::getHoudiniDefaultValue(ShadingPlug *sp)
{
	MString hType = getHoudiniType(sp);
	MString defaultValue;
	if(hType == "float")
		return MString("0.0");
	if(hType == "int")
		return MString("0");
	if(hType == "string")
		return MString("\"\"");
	if(hType == "vector")
		return MString("{0.0, 0.0, 0.0}");
	if(hType == "matrix")
		return MString("{{1.0, 0.0, 0.0, 0.0},{0.0, 1.0, 0.0, 0.0},{0.0, 0.0, 1.0, 0.0},{0.0, 0.0, 0.0, 1.0}}");
	return defaultValue;
}

MString plugNameToVector(MString& plugName)
{
	MString partExt = plugName.substring(plugName.length() - 1, plugName.length() - 1);
	MString cleanName = plugName.substring(0, plugName.length() - 2);
	if( (partExt == "R") || (partExt == "X"))
		cleanName += ".x";
	if( (partExt == "G") || (partExt == "Y"))
		cleanName += ".y";
	if( (partExt == "B") || (partExt == "Z"))
		cleanName += ".z";
	// spcial case, all outXXXUV have connections called outxxxU outXXV instead of outUVU and outUVV 
	if( (partExt == "U") || (partExt == "V"))
	{
		if( (plugName.substring(plugName.length() - 2, plugName.length() - 1)) != "UV")
		{
			if(partExt == "U")
				cleanName += "UV.x";
			if(partExt == "V")
				cleanName += "UV.y";
		}
	}
	// if xx _uCoord
	std::vector<std::string> strArray;
	pystring::split(plugName.asChar(), strArray, "_");
	if( strArray.size() > 0)
	{
		MString attName = MString(strArray[strArray.size() - 1].c_str());
		if( (attName == "uCoord") || (attName == "vCoord"))
		{
			if (attName == "uCoord")
			{
				std::string cleanAtt = pystring::replace(plugName.asChar(), "uCoord", "uvCoord.x");
				cleanName = MString(cleanAtt.c_str());

			}else{
				std::string cleanAtt = pystring::replace(plugName.asChar(), "vCoord", "uvCoord.y");
				cleanName = MString(cleanAtt.c_str());
			}
		}
	}
	return cleanName;
}

// this is called if the plug is connected, so give me the other side name
MString mtm_Material::checkAndGetPlugConn(MPlug& plug)
{
	MString baseString;
	MStatus stat;
	MPlugArray plugArray;
	plug.connectedTo(plugArray, 1, 0, &stat); // get only inplugs
	if( stat != MStatus::kSuccess) 
		return baseString;
	if( plugArray.length() == 0)
		return baseString;

	MPlug otherSidePlug = plugArray[0];
	MObject otherSideObj = otherSidePlug.node();

	// check if other side node and other side plug is supported
	// first split the plug name called node.attr or node.attr.subattr into elements
	std::vector<std::string> strArray;
	pystring::split(otherSidePlug.name().asChar(), strArray, ".");
	if( strArray.size() == 0)
		return baseString;
	// get last element, the attribute
	MString shortPlugName = MString(strArray[strArray.size() - 1].c_str());
	// Filter special case called worldInverseMatrix[0] and call it worldInverseMatrix
	if( shortPlugName == "worldInverseMatrix[0]")
	{
		//logger.debug(MString("Found  worldInverseMatrix[0] - remove array"));
		shortPlugName = "worldInverseMatrix";
	}
	// maybe only using the last attribute is not the correct way.. but leave this for later
	// improvement if necessary
	if(isOutPlugValid(MString(otherSideObj.apiTypeStr()), shortPlugName))
	{
		//logger.debug(MString("Other side nodeType and attr ") + otherSideObj.apiTypeStr() + " attr: " + shortPlugName + " are suppported");
		MString connectionName = makeGoodString(otherSidePlug.name());
		// in case that the connected plug is a subPlug like colorR or outU, then make a valid houdini vector of it
		// colorR --> color.x outV --> outUV.x
		if( isSubPlug(shortPlugName) )
			connectionName = plugNameToVector(connectionName);
		baseString = connectionName;
	}else{
		//logger.debug(MString("Other side nodeType or attr ") + otherSideObj.apiTypeStr() + " attr: " + shortPlugName + " are NOT suppported");
		baseString = "";
	}
	
	return baseString;
}
MString matrixStringToHoudiniString(MString& matrixString)
{
	std::string clean = pystring::strip(matrixString.asChar(), " ");
	logger.debug(MString("matrixStringToHoudiniString - clean ") + MString(clean.c_str()));
	std::vector<std::string> numbers;
	pystring::split(clean, numbers, " ");
	MString final = "";
	for( int i = 0; i < 4; i++)
	{
		final += "{";
		for( int j = 0; j < 4; j++)
		{	
			if( j > 0 || j == 3)
				final += ",";
			final += MString(numbers[i*4 + j].c_str());
		}
		if( i < 3 )
			final += "},";
		else
			final += "}";
	}
	return final;
}

// get the complete value as string including connection and check of valid connections
MString mtm_Material::getPlugValueString(ShadingPlug *sp, MStringArray& connectedPlugs, MStringArray& externalPlugs, bool& hasConnections)
{
	MString baseString;
	hasConnections = false;

	// very special case 
	//logger.debug(MString("getPlugValueString from ") + sp->name);
	
	if( sp->name == "worldMatrix")
	{
		MString p3d = MString("Place3DTexture");
		MString pp3d = MString(sp->parentNode->internalName);
		if(p3d != pp3d)
			//logger.debug(p3d + " - parentname - " + pp3d + " != ");
		//if(sp->parentNode->internalName == "Place3DTexture" )
		//{
			MStatus stat;
			//logger.debug(MString("Found 3d placement node: ") + sp->parentNode->mayaName);
			//MFnDependencyNode place3d(sp->parentNode->mobject, &stat);
			//if( !stat )
			//	logger.error("Problem...");
			MFnDagNode dagFn(sp->parentNode->mobject);
			MMatrix matrix = dagFn.transformationMatrix();
			//return "TransMatrix...";
			MString matrixString = matrixToString(matrix);
			MString s = matrixStringToHoudiniString(matrixString);
			return MString("{" + s + "}");
		//}
	}
	// check if the base plug is connected e.g. from a color plug the color attribute itself
	if( sp->mplug.isConnected() )
	{
		MString connection = checkAndGetPlugConn(sp->mplug);
		if( connection != "")
		{
			hasConnections = true;
			// maybe its smarter and cleaner to create a matrixArray instead of exchanging the array brackets, but at the moment
			// I'll go this way...
			if(pystring::endswith(connection.asChar(), "worldInverseMatrix[0]"))
				return MString(pystring::replace(connection.asChar(), "worldInverseMatrix[0]", "worldInverseMatrix").c_str());
			return connection;
		}
		if( sp->name == "normalCamera" )
		{
			connection = "N";
			return connection;
		}
		return getPlugValueString(sp);// connection not valid, so get the numeric value
	}
	// Okay, the plug self is not connected, so lets see if the plug is a complex one with children
	if((sp->plugType == "float") || (sp->plugType == "int") || (sp->plugType == "bool") || (sp->plugType == "string"))
	{
		return getPlugValueString(sp);
	}

	// well I suppose its a more complex plug, we check for a few possibilies
	if(sp->plugType == "color")
	{
		MString colorString("set(");
		// get the color components
		MPlug rPlug = sp->mplug.child(0);
		MPlug gPlug = sp->mplug.child(1);
		MPlug bPlug = sp->mplug.child(2);
		MString rConn = checkAndGetPlugConn(rPlug);
		MString gConn = checkAndGetPlugConn(gPlug);
		MString bConn = checkAndGetPlugConn(bPlug);
		if( rConn != "")
		{
			hasConnections = true;
			colorString += rConn;
		}else
			colorString += rPlug.asFloat();
		colorString += ", ";

		if( gConn != "")
		{
			hasConnections = true;
			colorString += gConn;
		}else
			colorString += gPlug.asFloat();
		colorString += ", ";

		if( bConn != "")
		{
			hasConnections = true;
			colorString += bConn;
		}else
			colorString += bPlug.asFloat();
		colorString += ")";

		baseString += colorString;
		return baseString;
	}
	if(sp->plugType == "vector")
	{
		MString colorString("set(");
		// get the color components
		MPlug rPlug = sp->mplug.child(0);
		MPlug gPlug = sp->mplug.child(1);
		MPlug bPlug = sp->mplug.child(2);
		MString rConn = checkAndGetPlugConn(rPlug);
		MString gConn = checkAndGetPlugConn(gPlug);
		MString bConn = checkAndGetPlugConn(bPlug);
		if( rConn != "")
		{
			hasConnections = true;
			colorString += rConn;
		}else
			colorString += rPlug.asFloat();
		colorString += ", ";

		if( gConn != "")
		{
			hasConnections = true;
			colorString += gConn;
		}else
			colorString += gPlug.asFloat();
		colorString += ", ";

		if( bConn != "")
		{
			hasConnections = true;
			colorString += bConn;
		}else
			colorString += bPlug.asFloat();
		colorString += ")";

		baseString += colorString;
		// special case normalCamera: set to N if not connected
		if( sp->name == "normalCamera" )
			if( (rConn == "") && (rConn == "") && (bConn == ""))
				baseString = "N";	
		return baseString;
	}
	if(sp->plugType == "float2")
	{
		MString colorString("set(");
		// get the color components
		MPlug rPlug = sp->mplug.child(0);
		MPlug gPlug = sp->mplug.child(1);
		MString rConn = checkAndGetPlugConn(rPlug);
		MString gConn = checkAndGetPlugConn(gPlug);
		if( rConn != "")
		{
			hasConnections = true;
			colorString += rConn;
		}else
			colorString += rPlug.asFloat();
		colorString += ", ";

		if( gConn != "")
		{
			hasConnections = true;
			colorString += gConn;
		}else
			colorString += gPlug.asFloat();
		colorString += ", 0)";
		baseString += colorString;
		return baseString;
	}
	if(sp->plugType == "int2")
	{
		MString colorString("set(");
		// get the color components
		MPlug rPlug = sp->mplug.child(0);
		MPlug gPlug = sp->mplug.child(1);
		MString rConn = checkAndGetPlugConn(rPlug);
		MString gConn = checkAndGetPlugConn(gPlug);
		if( rConn != "")
		{
			hasConnections = true;
			colorString += rConn;
		}else
			colorString += rPlug.asInt();
		colorString += ", ";

		if( gConn != "")
		{
			hasConnections = true;
			colorString += gConn;
		}else
			colorString += gPlug.asInt();

		colorString += ",0)";
		baseString += colorString;
		return baseString;
	}
	// Now we have a complex one, its an array or something like color, vector or colorEntries of a ramp node
	if(sp->plugType == "floatArray")
	{
		baseString = "array(";
		uint numElements = sp->mplug.numElements();
		for( uint i = 0; i < numElements; i++)
		{
			if( (i > 0) && (i <= numElements - 1))
			{
				baseString += ",";
			}
			MString connection = checkAndGetPlugConn(sp->mplug[i]);
			if( connection != "")
			{
				hasConnections = true;
				baseString += connection;
			}else
				baseString += sp->mplug[i].asFloat();
		}
		baseString += ")";
		return baseString;
	}
	if(sp->plugType == "intArray")
	{
		baseString = "array(";
		uint numElements = sp->mplug.numElements();
		for( uint i = 0; i < numElements; i++)
		{
			if( (i > 0) && (i <= numElements - 1))
			{
				baseString += ",";
			}
			MString connection = checkAndGetPlugConn(sp->mplug[i]);
			if( connection != "")
			{
				hasConnections = true;
				baseString += connection;
			}else
				baseString += sp->mplug[i].asInt();
		}
		baseString += ")";
		return baseString;
	}
	if(sp->plugType == "stringArray")
	{
		baseString = "array(";
		uint numElements = sp->mplug.numElements();
		for( uint i = 0; i < numElements; i++)
		{
			if( (i > 0) && (i <= numElements - 1))
			{
				baseString += ",";
			}
			MString connection = checkAndGetPlugConn(sp->mplug[i]);
			if( connection != "")
			{
				hasConnections = true;
				baseString += connection;
			}else
				baseString += sp->mplug[i].asString();
		}
		baseString += ")";
		return baseString;
	}

	if(sp->plugType == "floatRamp")
	{
		uint numElements = sp->mplug.numElements();
		MStringArray positionList, floatList;
		std::list<RampEntryElement> rampEntryList;
		std::list<RampEntryElement>::iterator rampEntryListIt;

		for( uint i = 0; i < numElements; i++)
		{
			RampEntryElement re;

			MPlug positionPlug = sp->mplug[i].child(0);
			MPlug floatPlug = sp->mplug[i].child(1);

			MString pConn = checkAndGetPlugConn(positionPlug);
			if( pConn != "")
			{
				hasConnections = true;
				re.positionString = pConn;
			}else
				re.positionString = MString("") + positionPlug.asFloat();

			re.position = positionPlug.asFloat();

			pConn = checkAndGetPlugConn(floatPlug);
			if( pConn != "")
			{
				hasConnections = true;
				re.floatValueString = pConn;
			}else
				re.floatValueString = MString("") + floatPlug.asFloat();

			re.floatValue = floatPlug.asFloat();
			rampEntryList.push_back(re);
		}
		rampEntryList.sort();

		MString pString = MString("float ") + makeGoodString(sp->mplug.name()) + "_positions[] = array(";
		MString vString = MString("\t\tfloat ") + makeGoodString(sp->mplug.name()) + "_values[] = array(";

		int eNum = 0;
		for( rampEntryListIt = rampEntryList.begin(); rampEntryListIt != rampEntryList.end(); rampEntryListIt++)
		{
			if( (eNum > 0) && ( eNum <= rampEntryList.size()-1))
			{
				pString += ",";
				vString += ",";
			}
			pString += rampEntryListIt->positionString;
			vString += rampEntryListIt->floatValueString;
			eNum++;
		}
		pString += ")";
		vString += ")";
		baseString += pString + ";\n" + vString + ";\n";
		return baseString;
	}

	if(sp->plugType == "colorRamp")
	{
		uint numElements = sp->mplug.numElements();
		MStringArray positionList, floatList;
		std::list<RampEntryElement> rampEntryList;
		std::list<RampEntryElement>::iterator rampEntryListIt;

		for( uint i = 0; i < numElements; i++)
		{
			RampEntryElement re;

			MPlug positionPlug = sp->mplug[i].child(0);
			MPlug colorPlug = sp->mplug[i].child(1);

			MString pConn = checkAndGetPlugConn(positionPlug);
			if( pConn != "")
			{
				hasConnections = true;
				re.positionString = pConn;
			}else
				re.positionString = MString("") + positionPlug.asFloat();

			re.position = positionPlug.asFloat();

			pConn = checkAndGetPlugConn(colorPlug);
			MColor col;
			if( pConn != "")
			{
				hasConnections = true;
				re.colorValueString = pConn;
			}else{
				//re.colorValueString = MString("") + floatPlug.asFloat();
				MString colorString("set(");
				// get the color components
				MPlug rPlug = colorPlug.child(0);
				MPlug gPlug = colorPlug.child(1);
				MPlug bPlug = colorPlug.child(2);
				MString rConn = checkAndGetPlugConn(rPlug);
				MString gConn = checkAndGetPlugConn(gPlug);
				MString bConn = checkAndGetPlugConn(bPlug);
				if( rConn != "")
				{
					hasConnections = true;
					colorString += rConn;
				}else{
					colorString += rPlug.asFloat();
					col.r = rPlug.asFloat();
				}
				colorString += ", ";

				if( gConn != "")
				{
					hasConnections = true;
					colorString += gConn;
				}else{
					colorString += gPlug.asFloat();
					col.g = gPlug.asFloat();
				}
				colorString += ", ";

				if( bConn != "")
				{
					hasConnections = true;
					colorString += bConn;
				}else{
					colorString += bPlug.asFloat();
					col.b = bPlug.asFloat();
				}
				colorString += ")";
				re.colorValueString = colorString;
			}
			re.colorValue = col;
			rampEntryList.push_back(re);
		}
		rampEntryList.sort();

		MString pString = MString("float ") + makeGoodString(sp->mplug.name()) + "_positions[] = array(";
		MString vString = MString("\t\tvector ") + makeGoodString(sp->mplug.name()) + "_values[] = array(";

		int eNum = 0;
		for( rampEntryListIt = rampEntryList.begin(); rampEntryListIt != rampEntryList.end(); rampEntryListIt++)
		{
			if( (eNum > 0) && ( eNum <= rampEntryList.size()-1))
			{
				pString += ",";
				vString += ",";
			}
			pString += rampEntryListIt->positionString;
			vString += rampEntryListIt->colorValueString;
			eNum++;
		}
		pString += ")";
		vString += ")";
		baseString += pString + ";\n" + vString + ";\n";
		return baseString;
	}

	// here we have one of the first special cases: a color ramp
	// color entry list has two values, position and color for every array attribute
	// unfortunatly the entries can have arbitrary positions hence I have to reorder them.
	// e.g. a the positions of entries can be 0, 0.5, 1.0, 0.2, 0.75, .1
	if( sp->name == "colorEntryList" )
	{
		MStringArray positionList, colorList;
		uint numElements = sp->mplug.numElements();

		std::list<ColorEntryElement> colorEntryList;
		std::list<ColorEntryElement >::iterator colorEntryListIt;

		for( uint i = 0; i < numElements; i++)
		{
			ColorEntryElement ce;
			// this is a little bit too complex and rarely used, so ignore the 
			// colorEntry[x] to color Entry[x] connection
			MPlug positionPlug = sp->mplug[i].child(0);
			MPlug colorPlug = sp->mplug[i].child(1);

			MString pConn = checkAndGetPlugConn(positionPlug);
			if( pConn != "")
			{
				hasConnections = true;
				positionList.append(pConn);
			}else
				positionList.append(MString("") + positionPlug.asFloat());
			
			ce.position = positionPlug.asFloat();

			MString cConn = checkAndGetPlugConn(colorPlug);
			if( cConn != "")
			{
				hasConnections = true;
				colorList.append(cConn);
				ce.colorString = cConn;
			}else{
				MString colorString("set(");
				// get the color components
				MPlug rPlug = colorPlug.child(0);
				MPlug gPlug = colorPlug.child(1);
				MPlug bPlug = colorPlug.child(2);
				MString rConn = checkAndGetPlugConn(rPlug);
				MString gConn = checkAndGetPlugConn(gPlug);
				MString bConn = checkAndGetPlugConn(bPlug);
				if( rConn != "")
				{
					hasConnections = true;
					colorString += rConn;
				}else
					colorString += rPlug.asFloat();
				colorString += ", ";

				if( gConn != "")
				{
					hasConnections = true;
					colorString += gConn;
				}else
					colorString += gPlug.asFloat();
				colorString += ", ";

				if( bConn != "")
				{
					hasConnections = true;
					colorString += bConn;
				}else
					colorString += bPlug.asFloat();
				colorString += ")";

				colorList.append(colorString);
				ce.colorString = colorString;
			}
			colorEntryList.push_back(ce);
		}

		colorEntryList.sort();
		int eNum = 0;
		MString pString = MString("float ") + makeGoodString(sp->mplug.name()) + "_pos[] = array(";
		MString cString = MString("\t\tvector ") + makeGoodString(sp->mplug.name()) + "_col[] = array(";

		for( colorEntryListIt = colorEntryList.begin(); colorEntryListIt != colorEntryList.end(); colorEntryListIt++)
		{
			if( (eNum > 0) && ( eNum <= colorEntryList.size()-1))
			{
				pString += ",";
				cString += ",";
			}
			pString += colorEntryListIt->position;
			cString += colorEntryListIt->colorString;
			eNum++;
		}
		pString += ")";
		cString += ")";

		MString posString = MString("float ") + makeGoodString(sp->mplug.name()) + "_pos[] = array(";
		for(uint e = 0; e < positionList.length(); e++)
		{
			if( (e > 0) && ( e <= positionList.length()-1))
				posString += ",";
			posString += positionList[e];
		}
		posString += ")";

		MString colString = MString("\t\tvector ") + makeGoodString(sp->mplug.name()) + "_col[] = array(";
		for(uint e = 0; e < colorList.length(); e++)
		{
			if( (e > 0) && ( e <= colorList.length()-1))
				colString += ",";
			colString += colorList[e];
		}
		colString += ")";
		return pString + ";\n" + cString + ";\n";
	}

	// now it is a little bit more work, get the rgb plugs and check for connections
	if(sp->plugType == "colorArray")
	{
		uint numElements = sp->mplug.numElements();
		baseString = "array(";
		for( uint i = 0; i < numElements; i++)
		{
			if( (i > 0) && (i <= numElements - 1))
			{
				baseString += ",";
			}
			MString connection = checkAndGetPlugConn(sp->mplug[i]);
			if( connection != "")
			{
				hasConnections = true;
				baseString += connection;
			}else{
				MString colorString("set(");
				// get the color components
				MPlug rPlug = sp->mplug[i].child(0);
				MPlug gPlug = sp->mplug[i].child(1);
				MPlug bPlug = sp->mplug[i].child(2);
				MString rConn = checkAndGetPlugConn(rPlug);
				MString gConn = checkAndGetPlugConn(gPlug);
				MString bConn = checkAndGetPlugConn(bPlug);
				if( rConn != "")
				{
					hasConnections = true;
					colorString += rConn;
				}else
					colorString += rPlug.asFloat();
				colorString += ", ";

				if( gConn != "")
				{
					hasConnections = true;
					colorString += gConn;
				}else
					colorString += gPlug.asFloat();
				colorString += ", ";

				if( bConn != "")
				{
					hasConnections = true;
					colorString += bConn;
				}else
					colorString += bPlug.asFloat();
				colorString += ")";

				baseString += colorString;
			}
		}
		baseString += ")";
		return baseString;
	}

	// now it is a little bit more work, get the rgb plugs and check for connections
	if(sp->plugType == "vectorArray")
	{
		uint numElements = sp->mplug.numElements();
		baseString = "array(";
		for( uint i = 0; i < numElements; i++)
		{
			if( (i > 0) && (i <= numElements - 1))
			{
				baseString += ",";
			}
			MString connection = checkAndGetPlugConn(sp->mplug[i]);
			if( connection != "")
			{
				hasConnections = true;
				baseString += connection;
			}else{
				MString colorString("set(");
				// get the color components
				MPlug rPlug = sp->mplug[i].child(0);
				MPlug gPlug = sp->mplug[i].child(1);
				MPlug bPlug = sp->mplug[i].child(2);
				MString rConn = checkAndGetPlugConn(rPlug);
				MString gConn = checkAndGetPlugConn(gPlug);
				MString bConn = checkAndGetPlugConn(bPlug);
				if( rConn != "")
				{
					hasConnections = true;
					colorString += rConn;
				}else
					colorString += rPlug.asFloat();
				colorString += ", ";

				if( gConn != "")
				{
					hasConnections = true;
					colorString += gConn;
				}else
					colorString += gPlug.asFloat();
				colorString += ", ";

				if( bConn != "")
				{
					hasConnections = true;
					colorString += bConn;
				}else
					colorString += bPlug.asFloat();
				colorString += ")";

				baseString += colorString;
			}
		}
		baseString += ")";
		return baseString;
	}

	return baseString;
}

MString mtm_Material::getPlugValueString(ShadingPlug *sp)
{

	MString baseString;
	if( sp->plugType == "bool")
	{
		bool f = sp->mplug.asBool();
		if( f )
			return(MString("1"));
		else
			return(MString("0"));
	}
	if( sp->plugType == "string")
	{
		MString s = MString("\"") + sp->mplug.asString() + "\"";
		return(s);
	}
	if( sp->plugType == "float")
	{
		float f = sp->mplug.asFloat();
		return(MString("") + f);
	}
	if( sp->plugType == "int")
	{
		int i = sp->mplug.asInt();
		return(MString("") + i);
	}
	if( sp->plugType == "color")
	{
		MPlug rPlug = sp->mplug.child(0); // not really shure, is always 0 == R? 
		MPlug gPlug = sp->mplug.child(1); // not really shure, is always 1 == G? 
		MPlug bPlug = sp->mplug.child(2); // not really shure, is always 2 == B? 
		return(MString("set(") + rPlug.asFloat() + ", " + gPlug.asFloat() + ", " + bPlug.asFloat()+ ")");
	}
	if( sp->plugType == "vector")
	{
		MPlug rPlug = sp->mplug.child(0); // not really shure, is always 0 == X? 
		MPlug gPlug = sp->mplug.child(1); // not really shure, is always 1 == Y? 
		MPlug bPlug = sp->mplug.child(2); // not really shure, is always 2 == Z? 
		return(MString("set(") + rPlug.asDouble() + ", " + gPlug.asDouble() + ", " + bPlug.asDouble()+ ")");
	}
	if( sp->plugType == "float2")
	{
		MPlug rPlug = sp->mplug.child(0); // not really shure, is always 0 == X? 
		MPlug gPlug = sp->mplug.child(1); // not really shure, is always 1 == Y? 
		return(MString("set(") + rPlug.asDouble() + ", " + gPlug.asDouble() + ", 0)");
	}
	if( sp->plugType == "int2")
	{
		MPlug rPlug = sp->mplug.child(0); // not really shure, is always 0 == X? 
		MPlug gPlug = sp->mplug.child(1); // not really shure, is always 1 == Y? 
		return(MString("set(") + rPlug.asInt() + ", " + gPlug.asInt() + ", 0)");
	}
	return baseString;
}

bool isArrayAtt(MString& s)
{
	std::string attr(s.asChar());
	if( pystring::find(attr, "Array") > -1)
		return true;
	return false;
}

bool mtm_Material::isGeoAttr(ShadingNode *sn, ShadingPlug *sp)
{
	for( uint i=0; i < sn->geoPlugs.size(); i++)
		if( sp->name == sn->geoPlugs[i].name )
			return true;
	return false;
}

void  mtm_Material::exportNodeInputs(ShadingNode *sn, std::ofstream& shaderFile)
{
	const char *tabs = "\t\t";
	logger.debug("mtm_Material::exportNodeInputs");

	for( uint i = 0; i < sn->inPlugs.size(); i++)
	{
		ShadingPlug *sp = &sn->inPlugs[i];

		// if this is a Place2dTexture, then check if it needs a projection connection.
		// if yes and the plugName is uvCoord, then 
		if( sn->internalName == "Place2dTexture")
		{
			if( sp->name == "uvCoord")
			{
				if(sn->userPointer != NULL)
				{
					shadingNodeUserData *snud = (shadingNodeUserData *)sn->userPointer;
					if( snud->isProjectionTransformPart )
					{
						MString plugUvName = makeGoodString(sp->mplug.name());
						MString projName = snud->projTransforms[0]->mayaName;
						shaderFile << tabs << "vector " << plugUvName << " = getProjection(\"" << projName << "\");\n";
						continue;
					}
					if( snud->isLightShaderPart )
					{
						MString plugUvName = makeGoodString(sp->mplug.name());
						shaderFile << tabs << "vector " << plugUvName << " = " << snud->lightProjectionUVWName << ";\n";
						continue;
					}
				}
			}
		}
		sn->inPlugs[i].parentNode = sn;
		// vector lambert1_incandescence = set(0,0,0);
		// float lambert1_diffuse = 0.8;
		if(isGeoAttr(sn, sp))
			continue;
		MString plugName = makeGoodString(sp->mplug.name());

		// a color entry list / ramp needs a different behaviour
		if( (sn->typeName == "kRamp") && (sp->name == "colorEntryList"))
		{
			MStringArray connectedPlugs, externalPlugs;
			bool hasConnections = false;
			MString valueString = getPlugValueString(sp, connectedPlugs, externalPlugs, hasConnections);
			shaderFile << tabs << valueString;
		}else if( (sp->plugType == "floatRamp") || (sp->plugType == "colorRamp") ){
			MStringArray connectedPlugs, externalPlugs;
			bool hasConnections = false;
			MString valueString = getPlugValueString(sp, connectedPlugs, externalPlugs, hasConnections);
			shaderFile << tabs << valueString;
		}else{
			if(isArrayAtt(sp->plugType))
				plugName += "[]";
			// get type name and value for all simple attributes
			//MString valueString = getPlugValueString(sp);
			MStringArray connectedPlugs, externalPlugs;
			bool hasConnections = false;
			MString valueString = getPlugValueString(sp, connectedPlugs, externalPlugs, hasConnections);
			MString arrayString = "";
			shaderFile << tabs << this->getHoudiniType(sp) << " " << plugName << arrayString << " = " << valueString <<";\n";
		}
	}
}

void  mtm_Material::exportNodeOutputs(ShadingNode *sn, std::ofstream& shaderFile)
{
	const char *tabs = "\t\t";
	logger.debug("mtm_Material::exportNodeoutputs");

	for( uint i = 0; i < sn->outPlugs.size(); i++)
	{
		ShadingPlug *sp = &sn->outPlugs[i];
		this->getHoudiniType(sp);
		MString defValue=" = 0;";
		if( sp->plugType == "string")
			defValue=" = \"\"";
		MString plugName = makeGoodString(sp->mplug.name());
		MString nativePlug = "";
		if( plugName == "" )
		{
			plugName = sp->name;
			nativePlug = "// no native plug";
		}
		shaderFile << tabs << this->getHoudiniType(sp) << " " << plugName << defValue <<";" << nativePlug << "\n";
	}
}

void mtm_Material::exportShaderCall(ShadingNode *sn, std::ofstream& shaderFile)
{
	const char *tabs = "\t\t";
	MString attributeString;
	
	// first write geoPlugs like uv, rgbPP...
	for( uint i = 0; i < sn->geoPlugs.size(); i++)
	{
		if( (i > 0) && (i <  sn->geoPlugs.size()))
			attributeString += ",";
		attributeString += makeGoodString(sn->geoPlugs[i].name);
	}
	// write normal in plugs
	for( uint i = 0; i < sn->inPlugs.size(); i++)
	{
		if(isGeoAttr(sn, &sn->inPlugs[i]))
			continue;
		if( (i > 0) && (i <  sn->inPlugs.size()))
			attributeString += ",";
		if( (i==0) && (attributeString.length()>0))
			attributeString += ",";
		if(sn->inPlugs[i].name == "colorEntryList")
		{
			MString atName = makeGoodString(sn->inPlugs[i].mplug.name());
			attributeString += atName + "_col, " + atName + "_pos";
		}else if((sn->inPlugs[i].plugType == "floatRamp")||(sn->inPlugs[i].plugType == "colorRamp")){
			MString atName = makeGoodString(sn->inPlugs[i].mplug.name());
			attributeString += atName + "_values, " + atName + "_positions";
		}else{
			attributeString += makeGoodString(sn->inPlugs[i].mplug.name());
		}
	}
	// write all out plugs
	for( uint i = 0; i < sn->outPlugs.size(); i++)
	{
		if( (i > 0) && (i <  sn->outPlugs.size()))
			attributeString += ",";
		if( (i==0) && (attributeString.length()>0))
			attributeString += ",";
		MString plugName = sn->outPlugs[i].mplug.name();
		// in case a out plug does have a counterpart in mplug, e.g. light outColor
		// maybe it would be more useful if I would add an attribute called outColor?
		if( plugName == "" )
			plugName = sn->outPlugs[i].name;
		attributeString += makeGoodString(plugName);
	}
	shaderFile << tabs << sn->internalName << "(" << attributeString << ");\n";
}

// Projections have a special problem...
void parseOuts( ShadingNode *sn, std::vector<ShadingNode *>& projectionNodeList)
{
	logger.info(MString("parseOuts: ") + sn->mayaName);
	int numOuts = (int)sn->outShadingNodes.size();
	logger.info(MString("Node has: ") + numOuts + " outputs");

	for( int outId = 0; outId < numOuts; outId++)
	{
		ShadingNode *parentNode = sn->outShadingNodes[outId];
		if( parentNode->internalName == "Projection")
			projectionNodeList.push_back(parentNode);
		parseOuts(parentNode, projectionNodeList);
	}
}

// parse light shading network for any texture2dplacement nodes. They need a lightprojection
// input to define uv coodinates so that we can use maps for color inputs in lights
void mtm_Material::checkForLightProjections(ShadingNetwork& shaderNet, std::ofstream& shaderFile)
{
	// only tex2d nodes need a modified input, so first get all these nodes in my network
	std::vector<ShadingNode *> tex2dList;
	int numShaders = (int)shaderNet.shaderList.size();
	for( int i = (numShaders-1); i >= 0; i--)
	{
		ShadingNode *sn = shaderNet.shaderList[i];
		if( sn->internalName == "Place2dTexture")
		{
			tex2dList.push_back(sn);
		}
	}	
	int numTex = (int)tex2dList.size();

	// this one is a simpler version of the check for prjection below. If we are in a light shading network,
	// we simply check if we have tex2d projections and add a lightProjection node. Later all texture placements will refer to this projection
	if( numTex > 0)
	{
		for( int texId = 0; texId < numTex; texId++)
		{
			MString nodeName = makeGoodString(getObjectName(this->shadingEngineNode));
			MString uvwName = nodeName + "_outUVW";
			ShadingNode *texNode = tex2dList[texId];
			shadingNodeUserData *snud = new shadingNodeUserData();
			snud->isLightShaderPart = true;
			snud->lightProjectionUVWName = uvwName;
			texNode->userPointer = snud;
		}
	}
}

void mtm_Material::checkForProjections(ShadingNetwork& shaderNet, std::ofstream& shaderFile)
{
	// only tex2d nodes need a modified input, so first get all these nodes in my network
	std::vector<ShadingNode *> tex2dList;
	int numShaders = (int)shaderNet.shaderList.size();
	for( int i = (numShaders-1); i >= 0; i--)
	{
		ShadingNode *sn = shaderNet.shaderList[i];
		if( sn->internalName == "Place2dTexture")
		{
			tex2dList.push_back(sn);
		}
	}	
	int numTex = (int)tex2dList.size();

	std::vector<ShadingNode *> projectionNodeList;
	std::vector<ShadingNode *> projectionNodeListTmp;
	std::vector<ShadingNode *> projectionNodeListDone;
	for( int texId = 0; texId < numTex; texId++)
	{
		ShadingNode *texNode = tex2dList[texId];
		parseOuts(texNode, projectionNodeListTmp);
		if(projectionNodeListTmp.size() > 0)
		{
			// if we have projection nodes in the shader tree below the place2d node, 
			// we have to set the place2d node uvCoord the correct way. So the place2d node
			// has to be informed that it has to use these nodes as input.
			shadingNodeUserData *snud = new shadingNodeUserData();
			snud->isProjectionTransformPart = true;
			// There can be multiple projections in the upstream, and I save them all, but I suppose
			// at the moment I only use ther very first projection.
			for( int pId = 0; pId < projectionNodeListTmp.size(); pId++)
			{
				snud->projTransforms.push_back(projectionNodeListTmp[pId]);
				// if not yet defined add the projectionNode to the complete projection node list
				// this way I remove double entries
				bool found = false;
				for( int dN = 0; dN < projectionNodeListDone.size(); dN++)
					if( projectionNodeListTmp[pId]->mayaName == projectionNodeListDone[dN]->mayaName)
						found = true;
				if(!found)
					projectionNodeList.push_back(projectionNodeListTmp[pId]);
			}		
			texNode->userPointer = snud;
			projectionNodeListTmp.clear();
		}
	}

	if( projectionNodeList.size() > 0)
	{
		shaderFile << "vector getProjection( string projectionName )\n{\n";
		shaderFile << "\n\tstring nodeList["<< projectionNodeList.size() << "] = {";
		for( int pId = 0; pId < projectionNodeList.size(); pId++)
		{
			if( (pId > 0) && (pId < projectionNodeList.size()))
				shaderFile << ",";
			shaderFile << "\"" << projectionNodeList[pId]->mayaName << "\"";
		}
		shaderFile << "};\n";
		shaderFile << "\tvector valueList["<< projectionNodeList.size() << "];\n";
		shaderFile << "\tvector uvw = 0;\n\n";

		for( int pId = 0; pId < projectionNodeList.size(); pId++)
		{
			ShadingNode *projNode = projectionNodeList[pId];
			
			bool done = false;
			for( int dN = 0; dN < projectionNodeListDone.size(); dN++)
				if( projNode->mayaName == projectionNodeListDone[dN]->mayaName)
					done = true;
			if( done)
				continue;

			MString projName = projNode->mayaName;
			MFnDependencyNode projFn(projNode->mobject);
			// first get the transformation matrix node
			MObject place3dNode = getOtherSideNode(MString("placementMatrix"), projNode->mobject);
			if( place3dNode == MObject::kNullObj)
			{
				shaderFile << "// could not find placeMatrixNode for " << projName << "\n";
				continue;
			}
			MFnDagNode dagFn(place3dNode);
			MMatrix matrix = dagFn.transformationMatrix();
			// if projection node needs a camera transformation, use this instead of the default placement node matrix
			MObject camObj = getOtherSideNode(MString("linkedCamera"), projNode->mobject);
			if( camObj != MObject::kNullObj)
			{
				//logger.debug("Found a camera connected to projection node");
				MFnDagNode camDagFn(camObj);
				MObject camTranf = camDagFn.parent(0);
				MFnDagNode camTDagFn(camTranf);
				matrix = camTDagFn.transformationMatrix();
				shaderFile << "// projection " << projName << " will use camera projection from cam " << camDagFn.name() << "\n";
			}
			
			MString matrixString = matrixToString(matrix);
			MString s = matrixStringToHoudiniString(matrixString);
			MString placeName = getObjectName(place3dNode);
			//matrix place3dTexture2_worldMatrix = {{0.204765,0,0,0},{0,0,-0.204765,0},{0,0.204765,0,0},{0.318598,-0.0572125,-0.263482,1}};
			MString wmName = projName + "_" + placeName + "_worldMatrix";
			MString wimName = projName + "_" + placeName + "_worldInverseMatrix";
			shaderFile << "\tmatrix " << wmName <<" = {" << s << "};\n";  
			//matrix place3dTexture2_worldInverseMatrix;
			shaderFile << "\tmatrix " << wimName <<" = 0;\n";  
			//Place3dTexture(place3dTexture2_worldMatrix,place3dTexture2_worldInverseMatrix);
			shaderFile << "\tPlace3dTexture(" << wmName << "," << wimName << ");\n";  	
			shaderFile << "\n";

			// now write function for projection
			MString placeMatName = projName + "_placementMatrix";
			MString uvCoordName = projName + "_uvCoord";
			// proj inputs
			shaderFile << "\tmatrix " << placeMatName << " = " << wimName << ";\n";
			// proj outputs 
			shaderFile << "\tvector " << uvCoordName << " = 0;\n";
			shaderFile << "\tvector " + projName + "_outColor = 0;\n";
			shaderFile << "\tfloat " + projName + "_outAlpha = 0;\n";
			shaderFile << "\tProjection(" << placeMatName;
			// defaults 
			int projType = 0;
			getInt(MString("projType"), projFn, projType);
			float uAngle = 0.0;
			getFloat(MString("uAngle"), projFn, uAngle);
			float vAngle = 0.0;
			getFloat(MString("vAngle"), projFn, vAngle);
			MString image = "{0,0,0}";
			MString transp = "{0,0,0}";
			shaderFile << "," << projType << "," << image << "," << transp << "," << uAngle << ", " << vAngle;
			MString linkedCamera = "\"-\"";
			int fitType = 0, fitFill = 0;
			getInt(MString("fitType"), projFn, fitType);
			getInt(MString("fitFill"), projFn, fitFill);
			MString amplitudeX = "0.0", amplitudeY = "0.0";
			MString ratio = "-10.0"; // use this to recognize if we have a transform calculation or simple color calc
			shaderFile << "," << linkedCamera << "," << fitType << "," << fitFill << "," << amplitudeX << "," << amplitudeY << "," << ratio;
			MString ripples = "{0,0,0}";
			MString depthMinMax = "0.0,0.0";
			MString transpSettings = "{0,0,0}, {0,0,0}, {0,0,0}";
			MString colorBalance = "{0,0,0},{0,0,0},{0,0,0}";
			MString alphaSettings = "0.0,0.0";
			MString filterSettings = "0.0,0.0";
			shaderFile << "," << ripples << "," << depthMinMax << "," << transpSettings << "," << colorBalance << "," << alphaSettings;
			MString invert = "0";
			int local = 0, wrap = 0;
			getInt(MString("local"), projFn, local);
			getInt(MString("wrap"), projFn, wrap);
			float blend = 0.0;
			getFloat(MString("blend"), projFn, blend);
			shaderFile << "," << filterSettings << "," << invert << "," << local << "," << wrap << "," << blend ;

			shaderFile  << "," << uvCoordName;
			// outColor, outAlpha
			shaderFile << "," << projName + "_outColor " << "," << projName + "_outAlpha";
			shaderFile << ");\n";
			shaderFile << "\tvalueList[" << pId << "] = " << uvCoordName << ";\n\n";
			//shadingNodeUserData sud;
			//sud.isProjectionTransformPart = true;
			//projectionNodeList[pId]->userPointer = &sud;
			projectionNodeListDone.push_back(projNode);
		}
		// select the correct output
		shaderFile << "\tint numEntries = arraylength(nodeList);\n";
		shaderFile << "\tint foundId = 0;\n\tint count = 0;\n";
		shaderFile << "\tfor( count = 0; count < numEntries; count++)\n";
		shaderFile << "\t{\n\t\tif( nodeList[count] == projectionName)\n\t\t\tfoundId = count;\n\t}\n";
		shaderFile << "\treturn valueList[foundId];\n}\n";
	}
}

void mtm_Material::exportShadingNodes(ShadingNetwork& shaderNet, std::ofstream& shaderFile)
{
	const char *tabs = "\t\t";
	logger.debug("mtm_Material::exportShadingNodes");

	int numShaders = (int)shaderNet.shaderList.size();
	bool has2dProj = false;
	for( int i = (numShaders-1); i >= 0; i--)
	{
		ShadingNode *sn = shaderNet.shaderList[i];
		if( sn->internalName == "Place2dTexture")
		{
			if( sn->userPointer != NULL)
			{
				shadingNodeUserData *snud = (shadingNodeUserData *)sn->userPointer;
				if( snud->isLightShaderPart )
				{
					has2dProj = true;
					break;
				}
			}
		}
	}
	if( has2dProj )
	{
		MString nodeName = makeGoodString(getObjectName(this->shadingEngineNode));
		// yes, we need a lightProjection node
		MString uvwName = nodeName + "_outUVW";
		shaderFile << "\t\tvector " << uvwName << " = 0;\n\t\tLightProjection(" << uvwName << ");\n\n";
	}

	numShaders = (int)shaderNet.shaderList.size();
	for( int i = (numShaders-1); i >= 0; i--)
	{
		ShadingNode *sn = shaderNet.shaderList[i];
		logger.debug(MString("export shading node ") + sn->internalName);
		shaderFile << tabs << "\n// node created by " << sn->mayaName << "\n";
		if( !sn->supported )
		{
			shaderFile << tabs << "// node not yet supported " << sn->mayaName << "\n\n";
			continue;
		}

		// a shading node has inputs and outputs, first we define the inputs
		shaderFile << "\t\t// Inputs\n";
		exportNodeInputs(sn, shaderFile);
		shaderFile << "\t\t// Outputs\n";
		exportNodeOutputs(sn, shaderFile);
		exportShaderCall(sn, shaderFile);
	}
}
bool mtm_Material::isMayaDefaultShader(MObject& shader)
{
	if(shader.apiType() == MFn::kLambert)
		return true;
	if(shader.apiType() == MFn::kPhong)
		return true;
	if(shader.apiType() == MFn::kPhongExplorer)
		return true;
	if(shader.apiType() == MFn::kBlinn)
		return true;
	if(shader.apiType() == MFn::kAnisotropy)
		return true;
	if(shader.apiType() == MFn::kLayeredShader)
		return true;
	if(shader.apiType() == MFn::kRampShader)
		return true;
	if(shader.apiType() == MFn::kSurface)
		return true;

	return false;
}

void mtm_Material::compileShader(MString& basePath)
{
	MStatus stat;

	if( this->surfaceShaderNet.shaderList.size() > 0)
	{
		MString seName = makeGoodString(getObjectName(this->shadingEngineNode));
		MString vflPath = basePath + "/shaders/" + seName + + "_surface.vfl";
		MString outLib = basePath + "/shaders/shaders.otl";

		logger.debug(MString("compiling shader: ") + seName);
		MString cmd = MString("import mtm_initialize as minit; minit.startShaderCompiler( \"" + outLib + "\", \"" + vflPath + "\")");
		logger.debug(cmd);
		stat = MGlobal::executePythonCommand(cmd);
		if(!stat)
		{
			logger.error(MString("compilation failed."));
			return;
		}
	}
	if( this->displacementShaderNet.shaderList.size() > 0)
	{
		MString seName = makeGoodString(getObjectName(this->shadingEngineNode));
		MString vflPath = basePath + "/shaders/" + seName + + "_displacement.vfl";
		MString outLib = basePath + "/shaders/shaders.otl";

		logger.debug(MString("compiling shader: ") + seName);
		MString cmd = MString("import mtm_initialize as minit; minit.startShaderCompiler( \"" + outLib + "\", \"" + vflPath + "\")");
		logger.debug(cmd);
		stat = MGlobal::executePythonCommand(cmd);
		if(!stat)
		{
			logger.error(MString("compilation failed."));
			return;
		}
	}
	if( this->lightShaderNet.shaderList.size() > 0)
	{
		MString seName = makeGoodString(getObjectName(this->shadingEngineNode));
		MString vflPath = basePath + "/shaders/" + seName + + "_light.vfl";
		MString vflShadowPath = basePath + "/shaders/" + seName + + "_shadow.vfl";
		MString outLib = basePath + "/shaders/shaders.otl";

		logger.debug(MString("compiling shader: ") + seName);
		MString cmd = MString("import mtm_initialize as minit; minit.startShaderCompiler( \"" + outLib + "\", \"" + vflPath + "\")");
		logger.debug(cmd);
		stat = MGlobal::executePythonCommand(cmd);
		if(!stat)
		{
			logger.error(MString("compilation failed."));
			return;
		}

		cmd = MString("import mtm_initialize as minit; minit.startShaderCompiler( \"" + outLib + "\", \"" + vflShadowPath + "\")");
		logger.debug(cmd);
		stat = MGlobal::executePythonCommand(cmd);
		if(!stat)
		{
			logger.error(MString("compilation failed."));
			return;
		}
	}
}

bool mtm_Material::writeShaderOutVar(mtm_Material::Context ct, std::ofstream& shaderFile)
{
	switch(ct){
		case mtm_Material::Surface:
			{
				// in case of a surface shader I try to write the Cf, Ci and Ca values
				// This depends of the very last node that is connected to the shading group
				ShadingNode *sn = this->surfaceShaderNet.shaderList[0];
				MFnDependencyNode sFn(sn->mobject);
				// if it is a normal maya shading node, the outputs are all the same
				if(isMayaDefaultShader(sn->mobject))
				{
					// in case of a surface shader I try to write the Cf, Ci and Ca values
					// This depends of the very last node that is connected to the shading group
					//ShadingNode *sn = this->surfaceShaderNet.shaderList[this->surfaceShaderNet.shaderList.size() - 1];
					MString Cf = "\t\tCf = ";
					MColor outColor;
					if( getColor(MString("outColor"), sFn, outColor))
					{
						Cf += makeGoodString(sn->mayaName) + "_outColor;\n"; 
					}
					shaderFile << Cf;

					// invert transparency because we need opacity
					MString Of = "\t\tvector one = {1,1,1};\n\t\tOf = one - ";
					MColor outTransparency;
					if( getColor(MString("outTransparency"), sFn, outTransparency))
					{
						Of += makeGoodString(sn->mayaName) + "_outTransparency;\n"; 
					}
					shaderFile << Of;
						
					MString Af = "\t\tAf = ";
					MColor outMatteOpacity;
					if( getColor(MString("outMatteOpacity"), sFn, outMatteOpacity))
					{
						Af += " luminance(" + makeGoodString(sn->mayaName) + "_outMatteOpacity);\n"; 
					}
					shaderFile << Af;
				}
				if(sn->typeName == "mia_material_x")
				{
					MString Cf = "\t\tCf = ";
					MColor outColor;
					if( getColor(MString("result"), sFn, outColor))
					{
						Cf += makeGoodString(sn->mayaName) + "_result;\n"; 
					}
					shaderFile << Cf;

					// invert transparency because we need opacity
					MString Of = "\t\t\n\t\tOf = ";
					MColor outTransparency;
					if( getColor(MString("tran_result"), sFn, outTransparency))
					{
						//Of += makeGoodString(sn->mayaName) + "_tran_result;\n"; 
						Of += "1.0;\n"; 
					}
					shaderFile << Of;

					MString Af = "\t\tAf = ";
					MColor outMatteOpacity;
					if( getColor(MString("opacity_result"), sFn, outMatteOpacity))
					{
						//Af += makeGoodString(sn->mayaName) + "_opacity_result;\n"; 
						Af += " 1.0;\n"; 
					}
					shaderFile << Af;
				}
				break;
			}
		case mtm_Material::Displacement:
			{
				MString P = "\t\tP = P;";
				shaderFile << P;
			break;
			}
		case mtm_Material::Volume:
			{
			break;
			}
		case mtm_Material::Light:
			{
				MString Cl = "\t\tCl = outLightColor;";
				shaderFile << Cl;
			break;
			}		
		case mtm_Material::Shadow:
			{
				MString Cl = "\t\tCl = Cl * outLightColor;";
				shaderFile << Cl;
			break;
			}
	}
	return true;
}

bool mtm_Material::exportVfl(MString& basePath)
{
	logger.debug("mtm_Material::exportVfl");

	if( this->surfaceShaderNet.shaderList.size() > 0)
	{
		MString seName = makeGoodString(getObjectName(this->shadingEngineNode));
		MString vflPath = basePath + "/shaders/" + seName + "_surface.vfl";

		std::ofstream shaderFile(vflPath.asChar());
		if( !shaderFile.good())
		{
			shaderFile.close();
			return false;
		}
		shaderFile << "// Surface shader file for shading group " << makeGoodString(this->materialName) << "\n";

		// Okay, first get all include files for the surface shader
		MString shaderIncludes = this->getShaderIncludes(this->surfaceShaderNet.shaderList);
		shaderFile << shaderIncludes << "\n";
		
		// Now we need the external definitions

		// projections need a special behaviour
		checkForProjections(this->surfaceShaderNet, shaderFile);

		// define surface shader
		MString shaderName = MString("surface_") + makeGoodString(this->materialName);
		shaderFile << "surface " << shaderName << "(";

		// first export all geoAttributes
		int numPlugs = (int)this->surfaceShaderNet.geometryParamsList.size();
		for( int i=0; i < numPlugs; i++)
		{
			if( (i > 0) && (i < numPlugs))
				shaderFile << "; ";
			ShadingPlug sp = this->surfaceShaderNet.geometryParamsList[i];
			shaderFile << getHoudiniType(&sp) << " " << sp.name << " = " << getHoudiniDefaultValue(&sp) ;
		}
		// then export all externalOrUndefinedAttribute connections
		numPlugs = (int)this->surfaceShaderNet.externalPlugList.size();
		for( int i=0; i < numPlugs; i++)
		{
			if( (i > 0) && (i < numPlugs))
				shaderFile << "; ";
			ShadingPlug sp = this->surfaceShaderNet.externalPlugList[i];
			shaderFile << getHoudiniType(&sp) << " " << sp.name << " = " << getHoudiniDefaultValue(&sp) ;
		}
		shaderFile << ")\n{\n";
		
		// export the shading nodes for surface shader
		exportShadingNodes(this->surfaceShaderNet, shaderFile);
		
		// give me the output Ci, Oi ... unfortunatly we have to check what exactly is connected to the shading group
		// if it is a default maya shader use normal outputs if it is a mr shader use its result/output or use the connected value
		this->writeShaderOutVar(mtm_Material::Surface, shaderFile);

		// close surface shader
		shaderFile << "\n}\n";
		shaderFile.close();
	}

	if( this->displacementShaderNet.shaderList.size() > 0)
	{
		MString seName = makeGoodString(getObjectName(this->shadingEngineNode));
		MString vflPath = basePath + "/shaders/" + seName + "_displacement.vfl";

		std::ofstream shaderFile(vflPath.asChar());
		if( !shaderFile.good())
		{
			shaderFile.close();
			return false;
		}
		shaderFile << "// Displacement shader file for shading group " << makeGoodString(this->materialName) << "\n";

		// Okay, first get all include files for the surface shader
		MString shaderIncludes = this->getShaderIncludes(this->displacementShaderNet.shaderList);
		shaderFile << shaderIncludes << "\n";
		
		// Now we need the external definitions

		// projections need a special behaviour
		checkForProjections(this->displacementShaderNet, shaderFile);

		// define surface shader
		MString shaderName = MString("displacement_") + makeGoodString(this->materialName);
		shaderFile << "displace " << shaderName << "(";

		// first export all geoAttributes
		int numPlugs = (int)this->displacementShaderNet.geometryParamsList.size();
		for( int i=0; i < numPlugs; i++)
		{
			if( (i > 0) && (i < numPlugs))
				shaderFile << "; ";
			ShadingPlug sp = this->displacementShaderNet.geometryParamsList[i];
			shaderFile << getHoudiniType(&sp) << " " << sp.name << " = " << getHoudiniDefaultValue(&sp) ;
		}
		// then export all externalOrUndefinedAttribute connections
		numPlugs = (int)this->displacementShaderNet.externalPlugList.size();
		for( int i=0; i < numPlugs; i++)
		{
			if( (i > 0) && (i < numPlugs))
				shaderFile << "; ";
			ShadingPlug sp = this->displacementShaderNet.externalPlugList[i];
			shaderFile << getHoudiniType(&sp) << " " << sp.name << " = " << getHoudiniDefaultValue(&sp) ;
		}
		shaderFile << ")\n{\n";
		
		// export the shading nodes for displacement shader
		exportShadingNodes(this->displacementShaderNet, shaderFile);
		
		// give me the output Ci, Oi ... unfortunatly we have to check what exactly is connected to the shading group
		// if it is a default maya shader use normal outputs if it is a mr shader use its result/output or use the connected value
		this->writeShaderOutVar(mtm_Material::Displacement, shaderFile);

		// close surface shader
		shaderFile << "\n}\n";
		shaderFile.close();
	}

	// in maya we dont have a seperate shadow shader available, so I simply add it to 
	// the light shader calculation. It is a simple copy of the light shader defined as shadow
	// and later the xxxLight() node is relpaced by its xxxLightShadow() node
	if( this->lightShaderNet.shaderList.size() > 0)
	{
		ShadingNetwork *network = &this->lightShaderNet;
		MString seName = makeGoodString(getObjectName(this->shadingEngineNode));
		MString vflPath = basePath + "/shaders/" + seName + "_light.vfl";
		MString vflShadowPath = basePath + "/shaders/" + seName + "_shadow.vfl";

		std::ofstream shaderShadowFile(vflShadowPath.asChar());
		if( !shaderShadowFile.good())
		{
			shaderShadowFile.close();
			return false;
		}
		shaderShadowFile << "// Shadow shader file for shading group " << makeGoodString(this->materialName) << "\n";

		std::ofstream shaderFile(vflPath.asChar());
		if( !shaderFile.good())
		{
			shaderFile.close();
			return false;
		}
		shaderFile << "// Light shader file for shading group " << makeGoodString(this->materialName) << "\n";

		// placement nodes in light shaders need a special behaviour
		// must be placed before getShaderIncludes() because we modify the tex2d nodes to contain a userPointer
		// which is needed in includes
		checkForLightProjections(*network, shaderFile);
		checkForLightProjections(*network, shaderShadowFile);

		// Okay, first get all include files for the surface shader
		MString shaderIncludes = this->getShaderIncludes(network->shaderList);
		shaderFile << shaderIncludes << "\n";
		shaderShadowFile << shaderIncludes << "\n";
		
		// define light shader
		MString shaderName = MString("light_") + makeGoodString(this->materialName);
		shaderFile << "light " << shaderName << "(";
		MString shaderShadowName = MString("shadow_") + makeGoodString(this->materialName);
		shaderShadowFile << "shadow " << shaderShadowName << "(";

		// first export all geoAttributes
		int numPlugs = (int)network->geometryParamsList.size();
		for( int i=0; i < numPlugs; i++)
		{
			if( (i > 0) && (i < numPlugs))
				shaderFile << "; ";
			ShadingPlug sp = network->geometryParamsList[i];
			shaderFile << getHoudiniType(&sp) << " " << sp.name << " = " << getHoudiniDefaultValue(&sp) ;
			shaderShadowFile << getHoudiniType(&sp) << " " << sp.name << " = " << getHoudiniDefaultValue(&sp) ;
		}
		// then export all externalOrUndefinedAttribute connections
		numPlugs = (int)network->externalPlugList.size();
		for( int i=0; i < numPlugs; i++)
		{
			if( (i > 0) && (i < numPlugs))
				shaderFile << "; ";
			ShadingPlug sp = network->externalPlugList[i];
			shaderFile << getHoudiniType(&sp) << " " << sp.name << " = " << getHoudiniDefaultValue(&sp) ;
			shaderShadowFile << getHoudiniType(&sp) << " " << sp.name << " = " << getHoudiniDefaultValue(&sp) ;
		}
		shaderFile << ")\n{\n";
		shaderShadowFile << ")\n{\n";

		// export the shading nodes for light shader
		exportShadingNodes(*network, shaderFile);
		exportShadingNodes(*network, shaderShadowFile);
		
		// give me the output Ci, Oi ... unfortunatly we have to check what exactly is connected to the shading group
		// if it is a default maya shader use normal outputs if it is a mr shader use its result/output or use the connected value
		this->writeShaderOutVar(mtm_Material::Light, shaderFile);
		this->writeShaderOutVar(mtm_Material::Shadow, shaderShadowFile);

		// close light shader
		shaderFile << "\n}\n";
		shaderFile.close();

		// close shadow shader
		shaderShadowFile << "\n}\n";
		shaderShadowFile.close();

		shaderShadowFile.close();

		// exchange the lightNodes in shadow shader with lightShadow nodes

		std::ifstream shaderShadowInFile(vflShadowPath.asChar());
		if( !shaderShadowInFile.good())
		{
			shaderShadowInFile.close();
			logger.error(MString("Unable to open shadow shader file: ") + vflShadowPath + " for LightNode -> LightShadowNode correction");
			return false;
		}
		std::vector<std::string> completeShadowFile;
		while(!shaderShadowInFile.eof())
		{
			std::string line;
			std::getline(shaderShadowInFile, line);
			completeShadowFile.push_back(line);
		}
		shaderShadowInFile.close();
		
		for(uint i = 0; i < completeShadowFile.size(); i++)
		{
			completeShadowFile[i] = pystring::replace(completeShadowFile[i], "PointLight(", "PointLightShadow(");
			completeShadowFile[i] = pystring::replace(completeShadowFile[i], "AreaLight(", "AreaLightShadow(");
			completeShadowFile[i] = pystring::replace(completeShadowFile[i], "SpotLight(", "SpotLightShadow(");
			completeShadowFile[i] = pystring::replace(completeShadowFile[i], "DirectionalLight(", "DirectionalLightShadow(");
			completeShadowFile[i] = pystring::replace(completeShadowFile[i], "VolumeLight(", "VolumeLightShadow(");
		}		

		std::ofstream shaderShadowOutFile(vflShadowPath.asChar());
		if( !shaderShadowOutFile.good())
		{
			shaderShadowOutFile.close();
			logger.error(MString("Unable to open shadow shader file: ") + vflShadowPath + " for LightNode -> LightShadowNode correction");
			return false;
		}
		for(uint i = 0; i < completeShadowFile.size(); i++)
		{
			std::string line = completeShadowFile[i];
			shaderShadowOutFile << line << "\n";
		}
		shaderShadowOutFile.close();

	}


	return true;
}

mtm_Material::mtm_Material()
{}

mtm_Material::mtm_Material(MObject& shadingEngine) : Material(shadingEngine)
{

}

mtm_Material::~mtm_Material()
{}


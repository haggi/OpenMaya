#include "textureBaseTexture.h"
#include <math.h>
#include <maya/MPxNode.h>
#include <maya/MIOStream.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFloatVector.h>
#include <maya/MFnPlugin.h>
#include <maya/MDrawRegistry.h>
#include <maya/MPxShadingNodeOverride.h>
#include <maya/MViewport2Renderer.h>
#include <maya/MFragmentManager.h>

MTypeId TextureBase::id( 0x00000 );

// Attributes
MObject TextureBase::aColor;
MObject TextureBase::aGamma;
MObject TextureBase::aOutColor;
	
//---------------------------- automatically created attributes start ------------------------------------
//---------------------------- automatically created attributes end ------------------------------------
	

#define MAKE_INPUT(attr)                                \
 CHECK_MSTATUS ( attr.setKeyable(true) );               \
 CHECK_MSTATUS ( attr.setStorable(true) );              \
 CHECK_MSTATUS ( attr.setReadable(true) );              \
 CHECK_MSTATUS ( attr.setWritable(true) );

#define MAKE_OUTPUT(attr)                               \
  CHECK_MSTATUS ( attr.setKeyable(false) );             \
  CHECK_MSTATUS ( attr.setStorable(false) );            \
  CHECK_MSTATUS ( attr.setReadable(true) );             \
  CHECK_MSTATUS ( attr.setWritable(false) );

//
// DESCRIPTION:
void TextureBase::postConstructor( )
{
    setMPSafe(true);
}

//
// DESCRIPTION:
TextureBase::TextureBase()
{
}

//
// DESCRIPTION:
TextureBase::~TextureBase()
{
}

//
// DESCRIPTION:
void *TextureBase::creator()
{
    return new TextureBase();
}

//
// DESCRIPTION:
MStatus TextureBase::initialize()
{
	MFnNumericAttribute nAttr;
	MFnLightDataAttribute lAttr;
	MFnTypedAttribute tAttr;
	MFnGenericAttribute gAttr;
	MFnEnumAttribute eAttr;
	MFnMessageAttribute mAttr;

//---------------------------- automatically created attributes start ------------------------------------
//---------------------------- automatically created attributes end ------------------------------------

    aColor = nAttr.createColor( "color", "c" );
    MAKE_INPUT(nAttr);

    aGamma = nAttr.createPoint( "gamma", "g" );
    MAKE_INPUT(nAttr);
    CHECK_MSTATUS ( nAttr.setDefault(1.f, 1.f, 1.f) );

    // Output attributes

    aOutColor= nAttr.createColor( "outColor", "oc" );
    MAKE_OUTPUT(nAttr);

    //  Add attributes to the node database.
    CHECK_MSTATUS ( addAttribute(aColor) );
    CHECK_MSTATUS ( addAttribute(aGamma) );
    CHECK_MSTATUS ( addAttribute(aOutColor) );

    // All input affect the output color
    CHECK_MSTATUS ( attributeAffects( aColor, aOutColor ));
    CHECK_MSTATUS ( attributeAffects( aGamma, aOutColor ));

    return MS::kSuccess;
}

//
// DESCRIPTION:
MStatus TextureBase::compute(const MPlug &plug, MDataBlock &block)
{
    if ((plug != aOutColor) && (plug.parent() != aOutColor))
        return MS::kUnknownParameter;

    MFloatVector & icol = block.inputValue( aColor ).asFloatVector();
    MFloatVector & igam = block.inputValue( aGamma ).asFloatVector();

    MDataHandle och = block.outputValue( aOutColor );
    MFloatVector & ocol = och.asFloatVector();
    ocol[0]= powf(icol[0], 1.f/igam[0]);
    ocol[1]= powf(icol[1], 1.f/igam[1]);
    ocol[2]= powf(icol[2], 1.f/igam[2]);
    och.setClean();

    return MS::kSuccess;
}

//
// Override definition
MHWRender::MPxShadingNodeOverride* TextureBaseOverride::creator(
    const MObject& obj)
{
    return new TextureBaseOverride(obj);
}

TextureBaseOverride::TextureBaseOverride(const MObject& obj)
: MPxShadingNodeOverride(obj)
, fFragmentName("")
{
    // Define fragments needed for VP2 version of shader, this could also be
    // defined in a separate XML file
    //
    // Define the input and output parameter names to match the input and
    // output attribute names so that the values are automatically populated
    // on the shader.
    static const MString sFragmentName("gammaShaderPluginFragment");
    static const char* sFragmentBody =
        "<fragment uiName=\"gammaShaderPluginFragment\" name=\"gammaShaderPluginFragment\" type=\"plumbing\" class=\"ShadeFragment\" version=\"1.0\">"
        "   <description><![CDATA[Gamma utility fragment]]></description>"
        "   <properties>"
        "       <float3 name=\"color\" />"
        "       <float3 name=\"gamma\" />"
        "   </properties>"
        "   <values>"
        "       <float3 name=\"color\" value=\"0.5,0.5,0.5\" />"
        "       <float3 name=\"gamma\" value=\"1.0,1.0,1.0\" />"
        "   </values>"
        "   <outputs>"
        "       <float3 name=\"outColor\" />"
        "   </outputs>"
        "   <implementation>"
        "   <implementation render=\"OGSRenderer\" language=\"Cg\" lang_version=\"2.1\">"
        "       <function_name val=\"gammaShaderPluginFragment\" />"
        "       <source><![CDATA["
        "float3 gammaShaderPluginFragment(float3 icol, float3 igam) \n"
        "{ \n"
        "   float3 result; \n"
        "   result.r = pow(icol.r, 1.0f/igam.r); \n"
        "   result.g = pow(icol.g, 1.0f/igam.g); \n"
        "   result.b = pow(icol.b, 1.0f/igam.b); \n"
        "   return result; \n"
        "} \n]]>"
        "       </source>"
        "   </implementation>"
        "   <implementation render=\"OGSRenderer\" language=\"HLSL\" lang_version=\"11.0\">"
        "       <function_name val=\"gammaShaderPluginFragment\" />"
        "       <source><![CDATA["
        "float3 gammaShaderPluginFragment(float3 icol, float3 igam) \n"
        "{ \n"
        "   float3 result; \n"
        "   result.r = pow(icol.r, 1.0f/igam.r); \n"
        "   result.g = pow(icol.g, 1.0f/igam.g); \n"
        "   result.b = pow(icol.b, 1.0f/igam.b); \n"
        "   return result; \n"
        "} \n]]>"
        "       </source>"
        "   </implementation>"
        "   </implementation>"
        "</fragment>";

    // Register fragments with the manager if needed
    MHWRender::MRenderer* theRenderer = MHWRender::MRenderer::theRenderer();
    if (theRenderer)
    {
        MHWRender::MFragmentManager* fragmentMgr =
            theRenderer->getFragmentManager();
        if (fragmentMgr)
        {
            // Add fragments if needed
            bool fragAdded = fragmentMgr->hasFragment(sFragmentName);
            if (!fragAdded)
            {
                fragAdded = (sFragmentName == fragmentMgr->addShadeFragmentFromBuffer(sFragmentBody, false));
            }

            // Use the fragment on successful add
            if (fragAdded)
            {
                fFragmentName = sFragmentName;
            }
        }
    }
}

TextureBaseOverride::~TextureBaseOverride()
{
}

MHWRender::DrawAPI TextureBaseOverride::supportedDrawAPIs() const
{
    return MHWRender::kOpenGL | MHWRender::kDirectX11;
}

MString TextureBaseOverride::fragmentName() const
{
    return fFragmentName;
}

//
// Plugin setup
static const MString sRegistrantId("TextureBase");

MStatus initializePlugin( MObject obj )
{
   const MString UserClassify( "utility/color:drawdb/shader/operation/TextureBase" );

   MFnPlugin plugin( obj, PLUGIN_COMPANY, "4.5", "Any");
   CHECK_MSTATUS ( plugin.registerNode( "TextureBase", TextureBase::id,
                         TextureBase::creator, TextureBase::initialize,
                         MPxNode::kDependNode, &UserClassify ) );

    CHECK_MSTATUS(
        MHWRender::MDrawRegistry::registerShadingNodeOverrideCreator(
            "drawdb/shader/operation/TextureBase",
            sRegistrantId,
            TextureBaseOverride::creator));

   return MS::kSuccess;
}

MStatus uninitializePlugin( MObject obj )
{
   MFnPlugin plugin( obj );
   CHECK_MSTATUS ( plugin.deregisterNode( TextureBase::id ) );

    CHECK_MSTATUS(
        MHWRender::MDrawRegistry::deregisterShadingNodeOverrideCreator(
            "drawdb/shader/operation/TextureBase",
            sRegistrantId));

   return MS::kSuccess;
}

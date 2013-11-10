passes = """
const char* PrimitiveCoordinatesPass::NAME = "PrimitiveCoords";
const char* MapCoordsPass::NAME = "MapCoords";
const char* GeometryNormalPass::NAME = "NormalsGeometry";
const char* ShadingNormalPass::NAME = "NormalsShading";
const char* DotProductPass::NAME = "NormalsDotProduct";

const char* OpacityColorPass::NAME = "OpacityColor";
const char* AlphaPass::NAME = "Alpha";
const char* DebugPass::NAME = "DEBUG";

const char* VelocityPass::NAME = "Velocity";
const char* ZDepthPass::NAME = "ZDepth";
const char* MapPass::NAME = "Texmap";
const char* NormalsDiscrepancyPass::NAME = "NormalsDiscrepancy";
const char* EmissionPass::NAME = "Emission";
const char* WorldPositionPass::NAME = "WorldPosition";
const char* IndirectLightingPass::NAME = "IndirectLighting";
const char* DirectLightingPass::NAME = "DirectLighting";
const char* ShadowsPass::NAME = "Shadows";

const char* SourceNodeIdPass::NAME = "SourceNodeId";
const char* PrimitiveIdPass::NAME = "PrimitiveId";
const char* MaterialIdPass::NAME = "MaterialId";
const char* InstanceIdPass::NAME = "InstanceId";

const char* DiffuseColorPass::NAME = "DiffuseColor";
const char* ReflectColorPass::NAME = "ReflectColor";
const char* RefractColorPass::NAME = "RefractColor";
const char* TranslucencyColorPass::NAME = "TranslucencyColor";

const char* DiffusePass::NAME = "Diffuse";
const char* RefractPass::NAME = "Refract";
const char* ReflectPass::NAME = "Reflect";
const char* TranslucencyPass::NAME = "Translucency";

const char* GiReflectPass::NAME = "ReflectGi";
const char* GiDiffusePass::NAME = "DiffuseGi";
const char* GiTranslucencyPass::NAME = "TranslucencyGi";
const char* GiRefractPass::NAME = "RefractGi";
"""
data="""
    { T_BOOL,    PARAM_EXPORT_ONLY,            "exportOnly",            false },
    { T_BOOL,    PARAM_SAVE_SECONDARY_GI,      "gi.saveSecondary",      false },
    { T_BOOL,    PARAM_LOAD_SECONDARY_GI,      "gi.loadSecondary",      false },
    { T_BOOL,    PARAM_LOW_PRIORITY,           "lowThreadPriority",     true  },
    { T_BOOL,    PARAM_SHOW_BUCKET_ORDER,      "vfb.showBucketOrder",   false },
    { T_BOOL,    PARAM_ENABLE_SHADING,         "doShading",             true },
    { T_BOOL,    PARAM_DO_AA,                  "doAa",                  true },
    

    { T_INT,     PARAM_RENDERER,               "renderer",              RENDERER_PROGRESSIVE,     RENDERER_BUCKET,   RENDERER_PPM,     },
    { T_INT,     PARAM_ACCELERATION_STRUCTURE, "accelerationStructure", STRUCTURE_EMBREE_BVH4_SPATIAL,    STRUCTURE_NONE,    STRUCTURE_EMBREE_BVH4_SPATIAL     },
    { T_INT,     PARAM_GI_PRIMARY_SOLVER,      "gi.primarySolver",      GISOLVER_PATHTRACING,     GISOLVER_NONE,     GISOLVER_IRRADIANCE_CACHE },
    { T_INT,     PARAM_GI_SECONDARY_SOLVER,    "gi.secondarySolver",    GISOLVER_PATHTRACING,     GISOLVER_NONE,     GISOLVER_VPL },
    { T_INT,     PARAM_IMAGE_FILTER,           "imageFilter",           IMAGE_FILTER_NONE,        IMAGE_FILTER_NONE, IMAGE_FILTER_TENT },
    { T_INT,     PARAM_DIRECT_LIGHT_SOLVER,    "lights.solver",         DSOLVER_COMBINED,         DSOLVER_COMBINED,    DSOLVER_PHOTON,     },
    { T_INT,     PARAM_ENVIRO_SOLVER,          "enviroSolver",          ENVIRO_FAST,              ENVIRO_FAST,       ENVIRO_FAST_COMPENSATE,     },
    { T_INT,     PARAM_EMBREE_TRIANGLES,       "embree.triangles",      EMBREE_FAST,              EMBREE_FAST,       EMBREE_AVX,     },
    { T_INT,     PARAM_RANDOM_SAMPLER,         "random.sampler",        RANDOM_5D_HIGHD,        RANDOM_SHARED,     RANDOM_MAXIMAL_VALUE,     },
    
    { T_INT,     PARAM_PROGRESSIVE_MAX_PASSES, "progressive.maxPasses", 0, 0, 9999 },
    { T_INT,     PARAM_PROGRESSIVE_TIME_LIMIT, "progressive.timeLimit", 60*000, 0, 10*24*60*60*1000 },
    { T_FLOAT,   PARAM_LIGHTS_AREA_SAMPLES_MULT,    "lights.areaSamplesMult",    1.0,   0.0001, 100.0 },
    { T_INT,     PARAM_LIGHTS_ENV_RESOLUTION,  "lights.env.resolution", 1000, 1, 100000 },
    { T_INT,     PARAM_PT_SAMPLES,             "pathtracingSamples",    16,   1, PT_MAX_SAMPLES },
    { T_INT,     PARAM_LIGHTS_AREA_METHOD,     "lights.areaMethod",     AREALIGHT_REPROJECT, AREALIGHT_SIMPLE, AREALIGHT_REPROJECT, },
    { T_INT,     PARAM_RAYCASTER_MAX_DEPTH,    "raycaster.maxDepth",    25,  1, RAYCASTER_MAX_DEPTH },
    { T_INT,     PARAM_MIN_DEPTH,              "raycaster.minDepth",    0,  0, RAYCASTER_MAX_DEPTH },
    { T_INT,     PARAM_VFB_UPDATE_INTERVAL, "buffer.updateInterval", 1000, 0, 1000*3600 },
    { T_INT,     PARAM_IMAGE_REGION_START_X,   "image.region.startX",   0,   0, 10000000 },
    { T_INT,     PARAM_IMAGE_REGION_START_Y,   "image.region.startY",   0,   0, 10000000 },
    { T_INT,     PARAM_IMAGE_REGION_END_X,     "image.region.endX",     0,   0, 10000000 },
    { T_INT,     PARAM_IMAGE_REGION_END_Y,     "image.region.endY",     0,   0, 10000000 },
    { T_INT,     PARAM_IMAGE_WIDTH,            "image.width",           640, 1, 10000000 },
    { T_INT,     PARAM_IMAGE_HEIGHT,           "image.height",          480, 1, 10000000 },        
    { T_STR,  PARAM_GI_PRIMARY_FILE,        "gi.primaryFile",        "C:/primaryGI.dat" },
    { T_STR,  PARAM_GI_SECONDARY_FILE,      "gi.secondaryFile",      "C:/secondaryGI.dat" },
    { T_INT,     PARAM_IMAGE_BUCKETSIZE,       "image.bucketSize",      32, 1, 1024 },
    { T_VEC3, PARAM_COLOR_EXIT,             "color.exit",            Rgb(0.0, 0.0, 1.0)},
    { T_INT,     PARAM_INTERNAL_RES_MULTIPLIER, "fb.internalResolutionMult",    1, 1, MAX_INTERNAL_RESOLUTION_MULT, },
    { T_FLOAT,   PARAM_IMAGEFILTER_WIDTH,       "imagefilter.width", 1.5, 1.0, float(MAX_IMAGEFILTER_WIDTH) },
    { T_FLOAT,   PARAM_IMAGEFILTER_BLURRING,    "imagefilter.blurring", 0.5, 0.0, 1.0 },
    { T_FLOAT,   PARAM_MAX_NORMAL_DEV,          "system.maxNormalDev",     0.55, 0.001, 2.0},
    { T_INT,     PARAM_PROGRESSIVE_RECALCULATE_EVERY, "progressive.recalculateEvery",     0, 0, 999},
    { T_FLOAT,   PARAM_PROGRESSIVE_ADAPTIVITY,  "progressive.adaptivity",     0.0, 0.0, 99.0},
    { T_FLOAT,   PARAM_PT_MAX_SAMPLE_INTENSITY, "maxPtSampleIntensity",     0.0, 0.0, 99999.0},
    { T_FLOAT,   PARAM_SUBDIV_ENVIRO_THRESHOLD, "subdivEnviroThreshold",  0.005, 0.0, 1.0},
    { T_FLOAT,   PARAM_TEXTURED_LIGHT_RES,      "lights.texturedResolution",  0.3, 0.0001, 99.0},
    { T_INT,     PARAM_RANDOM_SEED,             "random.seed",           1234, INT_MIN, INT_MAX, },
    { T_INT,     PARAM_NUM_THREADS,             "numThreads",           0, 0, 128, },
    { T_FLOAT,   PARAM_LIGHTSOLVER_FRAC_LOCAL,  "lightSolver.localFrac",  0.33, 0.0, 1.0, },
    { T_FLOAT,   PARAM_LIGHTSOLVER_FRAC_GLOBAL, "lightSolver.globalFrac", 0.33, 0.0, 1.0, },
    { T_FLOAT,   PARAM_PORTAL_SAMPLE_AMOUNT,    "portals.sampleAmount", 0.75, 0.0, 1.0, },
    { T_FLOAT,   PARAM_SHADOW_BIAS,             "shadowBias", -6.07, -8.0, -2.0, },
    { T_BOOL,    PARAM_RESUME_RENDERING,        "resumeRendering", false, },
    { T_INT,     PARAM_INSTANCE_MIN_SAVING,     "instance.minSize",     1, 1, 999999},
    

    { T_BOOL,   PARAM_IC_INCREMENTAL_BUILD,    "gi.ic.incrementalBuild",       false },
    { T_STR,    PARAM_IC_INCREMENTAL_FILENAME, "gi.ic.incrementalFilename",    "incrementalIc.dat" },
    { T_INT,    PARAM_IC_HEMISPHERE_SUBDIV,    "gi.ic.hemisphereSubdiv",       7, 1,  100 },
    { T_FLOAT,  PARAM_IC_PRECOMP_AMOUNT,       "gi.ic.precompAmount", 1.0, 0.0, 99.0 },
    { T_BOOL,   PARAM_IC_PATHTRACING_CORNERS,  "gi.ic.pathtracingCorners",     true },
    { T_FLOAT,  PARAM_IC_MAX_ERROR,            "gi.ic.maxGeomError",           0.6, 0.01, 10.0 },
    { T_FLOAT,  PARAM_IC_SMOOTHING,            "gi.ic.smoothing",              1.8, 1.0, 10.0 },
    { T_FLOAT,  PARAM_IC_COLOR_THRESHOLD,      "gi.ic.colorThreshold",         10.0, 0.01, 10.0 },
    { T_FLOAT,  PARAM_IC_RECORD_SPACING_MIN,   "gi.ic.recordSpacingMin",       1.0, 0.01, 500.0 },
    { T_FLOAT,  PARAM_IC_RECORD_SPACING_MAX,   "gi.ic.recordSpacingMax",       20.0, 0.1,  500.0 },
    { T_BOOL,   PARAM_IC_USE_ROTGRAD,          "gi.ic.useRotationGradient",    true },
    { T_BOOL,   PARAM_IC_USE_TRANSGRAD,        "gi.ic.useTranslationGradient", true },
    { T_INT,    PARAM_IC_INTERPOLATION_SCHEMA, "gi.ic.interpolationSchema",    IC_INTERPOLATION_TABELLION, IC_INTERPOLATION_WARD, IC_INTERPOLATION_TABELLION },
    { T_INT,    PARAM_IC_SEARCH_STRUCTURE,     "gi.ic.searchStructure",        ICSTRUCTURE_MULTIPLE_OCTREE, ICSTRUCTURE_MULTIPLE_OCTREE, ICSTRUCTURE_BVH},
    { T_BOOL,   PARAM_IC_RELAXED_INTERPOLATION,"gi.ic.relaxedInterpolation",   true },
    { T_INT,    PARAM_IC_VIZ,                  "gi.ic.vizualization",          ICVIZ_INDIRECT,  ICVIZ_OFF,   ICVIZ_INDIRECT},
    { T_INT,    PARAM_IC_MIN_INTERP_SAMPLES,   "gi.ic.minInterpSamples",       2, 1, 20},

    { T_FLOAT, PARAM_HDCACHE_PRECOMP_MULTIPLIER,   "gi.hdCache.precompMult",  1.0, 0.0, 99.0},
    { T_INT,   PARAM_HDCACHE_INTERPOLATION_COUNT,  "gi.hdCache.interpolationCount",  3, 1, HdCacheConfig::MAX_INTERP_SAMPLES},
    { T_FLOAT, PARAM_HDCACHE_DIR_SENSITIVITY,      "gi.hdCache.dirSensitivity",  2.0, 0.001, 100.0},
    { T_FLOAT, PARAM_HDCACHE_POS_SENSITIVITY,      "gi.hdCache.posSensitivity",  20.0, 0.0, 100.0},
    { T_FLOAT, PARAM_HDCACHE_NORMAL_SENSITIVITY,   "gi.hdCache.normalSensitivity",  3.0, 0.0, 10.0},
    { T_INT,   PARAM_HDCACHE_PT_SAMPLES,           "gi.hdCache.ptSamples",  256, 1, PT_MAX_SAMPLES},
    { T_FLOAT, PARAM_HDCACHE_SMOOTHING,            "gi.hdCache.smoothing",  2.0, 1.0, 10.0},
    { T_FLOAT, PARAM_HDCACHE_GLOSSINESS_THRESHOLD, "gi.hdCache.glossyThreshold",  0.9, 0.0, 1.0},
    { T_INT,   PARAM_HDCACHE_MAX_RECORDS,          "gi.hdCache.maxRecords",  100000, 1000, 999000 },
    { T_INT,   PARAM_HDCACHE_WRITE_PASSES,         "gi.hdCache.writePasses",  0, 0, 9999999 },

    { T_INT,  PARAM_PHOTONS_EMITTED,      "gi.photons.emitted",     5000000, 1000, 100000000 },
    { T_BOOL, PARAM_PHOTONS_STORE_DIRECT, "gi.photons.storeDirect", true },
    { T_INT, PARAM_PHOTONS_DEPTH,  "gi.photons.depth", -1, -1, 100 },
    { T_INT,  PARAM_PHOTONS_LOOKUP_COUNT, "gi.photons.lookupCount", 50, 1, PHOTONS_INTERPOLATION_MAX_SAMPLES },
    { T_INT,  PARAM_PHOTONS_FILTER, "gi.photons.0ilter", KERNEL_LINEAR, KERNEL_CONSTANT, KERNEL_GAUSSIAN },

    { T_INT,   PARAM_VPL_EMITTED,  "gi.vpl.emittedCount", 1000000,    1, 9999999 },
    { T_INT,   PARAM_VPL_USED,     "gi.vpl.usedCount",    150,     1, 9999 },
    { T_INT,   PARAM_VPL_PROGRESSIVE_BATCH,     "gi.vpl.progressiveBatch",    150,     1, 9999 },
    { T_FLOAT, PARAM_VPL_CLAMPING, "gi.vpl.clamping",     50.0,  0.0, 999.0 },


    { T_INT,  PARAM_PATHTRACING_DIRECTMODE,   "gi.pathtracing.directMode",          PTDIRECT_MIS, PTDIRECT_SAMPLE_LIGHTS, PTDIRECT_MIS},

    { T_INT,   PARAM_BUCKET_INITIAL_SAMPLES,     "buckets.initialSamples",    1,     1,     999  },
    { T_INT,   PARAM_BUCKET_ADAPTIVE_STEPS,      "buckets.adaptiveSteps",     2,     1,      10  },
    { T_FLOAT, PARAM_BUCKETS_ADAPTIVE_THRESHOLD, "buckets-adaptiveThreshold", 0.03, 0.001, 1.0 },

    { T_FLOAT, PARAM_BVH_COST_ITERATION, "bvh.cost.iteration", 1.0, 0.01, 1000.0 },
    { T_FLOAT, PARAM_BVH_COST_TRIANGLE,  "bvh.cost.triangle",  1.0, 0.01, 1000.0 },
    { T_INT,   PARAM_BVH_LEAF_SIZE_MIN,  "bvh.leafSizeMin",    2,   1,     1000   },
    { T_INT,   PARAM_BVH_LEAF_SIZE_MAX,  "bvh.leafSizeMax",    6,   2,     1000   },         


    { T_FLOAT, PARAM_COLORMAP_EXPONENT,  "colorMapping.exponent",           0.0, -100.0, 100.0 },
    { T_FLOAT, PARAM_COLORMAP_GAMMA,     "colorMapping.gamma",              2.2, 0.1, 10.0 },
    { T_FLOAT, PARAM_COLORMAP_WHITEMULT, "colorMapping.whiteMultiplier",    1.0, 0.01, 999.0 },
    { T_FLOAT, PARAM_COLORMAP_DISPLAY_T, "colorMapping.displayTemperature", 6500.0, 1000.0, 99999.0 },
    { T_FLOAT, PARAM_COLORMAP_SCENE_T,   "colorMapping.sceneTemperature",   6500.0, 1000.0, 99999.0 },
    { T_FLOAT, PARAM_COLORMAP_R_BALANCE, "colorMapping.rBalance",           1.0, 0.0, 999.0 },
    { T_FLOAT, PARAM_COLORMAP_G_BALANCE, "colorMapping.gBalance",           1.0, 0.0, 999.0 },
    { T_FLOAT, PARAM_COLORMAP_B_BALANCE, "colorMapping.bBalance",           1.0, 0.0, 999.0 },
    { T_INT,   PARAM_COLORMAP_SPACE,     "colorMapping.workingSpace",       CAT_RGB, CAT_LMS, CAT_RGB},
    { T_FLOAT, PARAM_COLORMAP_CONTRAST,  "colorMapping.contrast",           1.0, 1.0, 99.0 },

     
    { T_INT,   PARAM_PPM_PT_SAMPLES_PER_ITER,  "ppm.samplesPerIter",  1,   1,      50 },
    { T_INT,   PARAM_PPM_PHOTONS_PER_ITER,     "ppm.photonsPerIter",  5000000,   1000, 99000000 },
    { T_FLOAT, PARAM_PPM_ALPHA,                "ppm.alpha",           0.666, 0.01,   1.0 },
    { T_FLOAT, PARAM_PPM_INITIAL_RADIUS,       "ppm.initialRadius",   2.0, 0.0001, 200.0 },

    { T_BOOL,  PARAM_BIDIR_DO_MIS,           "bidir.doMis",  true },
    { T_INT,   PARAM_VCM_MODE,               "vcm.mode",    VCM_BIDIR, VCM_PT, VCM_VCM },

    { T_BOOL,    PARAM_DISPLACE_USE_PROJ_SIZE,   "displace.useProjectionSize", true, },
    { T_FLOAT,   PARAM_DISPLACE_MAX_SIZE_PROJ,   "displace.maxProjectSize",  2.0, 0.01, 100.0 },
    { T_FLOAT,   PARAM_DISPLACE_MAX_SIZE_WORLD,  "displace.maxWorldSize",      1.0, 0.00001, INFINITY },
    { T_INT,     PARAM_DISPLACE_MAX_SUBDIV,      "displace.maxSubdiv",         100, 1, 9999, },


    // for export only
    { T_BOOL,    PARAM_RENDERSTAMP_USE,        "renderstamp.use",       true },
    { T_STR,     PARAM_RENDERSTAMP,             "renderStamp",              "Corona Renderer Alpha | %c | Time: %t | Passes: %p | Primitives: %o | Rays/s: %r", },
    { T_INT,     PARAM_VFB_TYPE,               "vfb.type",               VFB_WX, VFB_NONE, VFB_WX  },

"""

typeDict = {'T_BOOL' : 'bool',
            'T_STR' : 'string',
            'T_INT' : 'int',
            'T_FLOAT': 'float',
            'T_VEC3' : 'vector'}

def extract():
    list = data.split("\n")
    parasList = []
    for l in list:
        parts = l.replace("{", "").replace("}", "").split(",")
        a = []
        for p in parts:
            a.append(p.strip())
        parts = a
        if len(parts) < 3:
            continue
        #print parts[0]
        paras={}
        paras['id'] = list.index(l)
        paras['name'] = parts[2].strip().replace('"', '').replace(".", "_")
        paras['type'] = typeDict[parts[0].strip()]
        paras['default'] = parts[3].strip().lower().capitalize().replace("False", "false").replace("True", "true")
        ds = []
        for d in parts[4:]:
            ds.append(d.replace(".f", ".0").lower().capitalize()) 
        paras['data'] = ":".join(ds)[:-1]
        if paras['data'].endswith(":"):
            paras['data'] = paras['data'][:-1]
        if not paras['default'].isdigit() and not paras['default'].replace(".", "0").isdigit() and not paras['default'].replace("-", "").isdigit():
            #print "-"+paras['default']+"-", "-no digit"
            if len(ds) > 1:
                paras['type'] = 'enum'
        else:
            if len(paras['data'].split(":")) == 2:
                paras['data'] = "minmax:"+paras['data']
            
        parasList.append(paras)
    
    
    for p in parasList:
        ps = "{0}, {1}, {2}, {3}".format(p['name'], p['type'], p['name'].split(".")[-1].capitalize(), p['default'])
        print ps,
        if len(p['data']) > 0:
            print "," + p['data']
        else:
            print
extract()
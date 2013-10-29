UIList = []
tab = {}
UIList.append(tab)
tab['name'] = 'common'
tab['entries'] = []
entry = {}
entry['name'] = 'None'
entry['type'] = 'frame'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'sampleJitter'
entry['type'] = 'float'
entry['displayName'] = 'SampleJitter'
entry['default'] = '1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'cast_shadow'
entry['type'] = 'bool'
entry['displayName'] = 'Cast Shadows'
entry['default'] = 'true'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'max_reflect_depth'
entry['type'] = 'int'
entry['displayName'] = 'Max Reflection Depth'
entry['default'] = '3'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'max_refract_depth'
entry['type'] = 'int'
entry['displayName'] = 'Max Refraction Depth'
entry['default'] = '3'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'raymarch_step'
entry['type'] = 'float'
entry['displayName'] = 'Raymarching Stepsize'
entry['default'] = '0.05'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'raymarch_shadow_step'
entry['type'] = 'float'
entry['displayName'] = 'Raymarching Shadow Stepsize'
entry['default'] = '0.1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'raymarch_reflect_ste'
entry['type'] = 'float'
entry['displayName'] = 'Raymarching Reflection Stepsize'
entry['default'] = '0.1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'raymarch_refract_ste'
entry['type'] = 'float'
entry['displayName'] = 'Raymarching Refraction Stepsize'
entry['default'] = '0.1'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'sample_time_range_min'
entry['type'] = 'float'
entry['displayName'] = 'Sample Time Range Min'
entry['default'] = '0.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'sample_time_range_max'
entry['type'] = 'float'
entry['displayName'] = 'Sample Time Range Max'
entry['default'] = '1.0'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'use_max_thread'
entry['type'] = 'bool'
entry['displayName'] = 'Use Max Threads'
entry['default'] = 'true'
tab['entries'].append(entry)

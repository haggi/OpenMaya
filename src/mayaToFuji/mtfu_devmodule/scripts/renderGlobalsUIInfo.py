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
entry['name'] = 'imageFormat'
entry['type'] = 'enum'
entry['displayName'] = 'imageFormat'
entry['default'] = '0'
entry['addInfo'] = 'exr:png'
tab['entries'].append(entry)
entry = {}
entry['name'] = 'pixelFilter'
entry['type'] = 'enum'
entry['displayName'] = 'PixelFilter'
entry['default'] = '0'
entry['addInfo'] = 'gauss:mitchell:triangle'
tab['entries'].append(entry)

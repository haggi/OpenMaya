wp="""
User  0.0     0.0     0.0     0.0     0         User
A     0.44757     0.40745     0.45117     0.40594     2856         Incandescent / Tungsten
B     0.34842     0.35161     0.34980     0.35270     4874         {obsolete} Direct sunlight at noon
C     0.31006     0.31616     0.31039     0.31905     6774         {obsolete} Average / North sky Daylight
D50     0.34567     0.35850     0.34773     0.35952     5003         Horizon Light. ICC profile PCS
D55     0.33242     0.34743     0.33411     0.34877     5503         Mid-morning / Mid-afternoon Daylight
D65     0.31271     0.32902     0.31382     0.33100     6504         Noon Daylight: Television, sRGB color space
D75     0.29902     0.31485     0.29968     0.31740     7504         North sky Daylight
E     .3333     .3333     .3333     .3333     5454         Equal energy
F1     0.31310     0.33727     0.31811     0.33559     6430         Daylight Fluorescent
F2     0.37208     0.37529     0.37925     0.36733     4230         Cool White Fluorescent
F3     0.40910     0.39430     0.41761     0.38324     3450         White Fluorescent
F4     0.44018     0.40329     0.44920     0.39074     2940         Warm White Fluorescent
F5     0.31379     0.34531     0.31975     0.34246     6350         Daylight Fluorescent
F6     0.37790     0.38835     0.38660     0.37847     4150         Lite White Fluorescent
F7     0.31292     0.32933     0.31569     0.32960     6500         D65 simulator, Daylight simulator
F8     0.34588     0.35875     0.34902     0.35939     5000         D50 simulator, Sylvania F40 Design 50
F9     0.37417     0.37281     0.37829     0.37045     4150         Cool White Deluxe Fluorescent
F10     0.34609     0.35986     0.35090     0.35444     5000         Philips TL85, Ultralume 50
F11     0.38052     0.37713     0.38541     0.37123     4000         Philips TL84, Ultralume 40
F12     0.43695     0.40441     0.44256     0.39717     3000         Philips TL83, Ultralume 30#
"""

WHITE_POINTS_NAMES = ["User", "A", "B", "C", "D50", "D55", "D65", "D75", "E", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12"]
WHITE_POINTS = {}
realIndex = 0
for index, line in enumerate(wp.split("\n")):
    if len(line) > 1:
        elements = line.split()
        if len(elements) > 2:
            WHITE_POINTS[realIndex] = (elements[0], elements[1], elements[2])
        realIndex += 1


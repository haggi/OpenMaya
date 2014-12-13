import path

class TestCase(object):
    def __init__(self, renderer):
        self.renderer = renderer.lower()
        self.baseDir = "H:/UserDatenHaggi/Documents/coding/OpenMaya/src"
        self.testDir = path.path("{0}/mayaTo{1}/testsuite".format(self.baseDir, renderer.capitalize()))
        
        self.testList = self.testDir.listdir()
    
    def doTest(self, testdir):
        print "Doing test", testdir
        
    def doTestCases(self):
        for directory in self.testList:
            self.doTest(directory)
    
    def createNewTestCase(self):
        pass



if __name__ == "__main__":
    tc = TestCase("corona")
    tc.doTestCases()
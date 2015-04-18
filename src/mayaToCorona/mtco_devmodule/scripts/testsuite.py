import Testsuite
import logging
import os
import path

log = logging
logging.basicConfig(level=logging.DEBUG)

class Testcase:
    def __init__(self):
        pass

plugin = "mayaToCorona"
testSuitePath = path.path(__file__)
testSuitePath = testSuitePath.dirname().parent.parent / "testsuite"
log.debug("Testsuite path: {0}".format(testSuitePath))

for testcase in testSuitePath.listdir():
    log.debug(testcase)

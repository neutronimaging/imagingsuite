#!/bin/env python
filenamein = "testsuite.log"
filenameout = "testsuite.xml"
verbose = 0

import os
import sys

def getSourceDir():
    """Returns the location of the source code."""
    script = os.path.abspath(sys.argv[0])
    if os.path.islink(script):
        script = os.path.realpath(script)
    return os.path.dirname(script)

class Test:
    def __init__(self, text, verbose):
        if verbose > 1:
            print "*****", text
        self.__time = 0.
        self.__totaltime = 0.
        self.__getStatus(text)
        self.__getTimes(text)

        # trim down the text to just what we need for line number
        start = text.find("testsuite.at")
        stop = text.find(": ", start)
        lineref = text[start:stop]
        index = lineref.find(":")
        self.__line = int(lineref[index+1:-1])

        # get the group and name
        self.__group = "unknown"
        self.__name = "dob"

        text = text[:start-1]
        index = text.find(" ")
        text = text[index:].strip()
        text = text.replace("Checking", "")
        text = text.replace("Check the", "")
        text = text.replace("C++", "CXX")
        text = text.replace("->", "to")
        text = text.replace("(", "")
        text = text.replace(")", "")
        text = text.replace("xml", "XML")
        text = text.strip()
        if text.endswith('.'):
            text = text[:-1]
        text = "_".join(text.split())

        (self.__group, self.__name) = self.__splitGroup(text)
        if verbose > 1:
            print text, "->", (self.__group, self.__name)

    def __splitGroup(self, text):
        # is it a binding
        if "binding" in text:
            splitted = text.split('_')
            if splitted[0] in ("C", "CXX", "F77", "F90", "python", "IDL"):
                return (splitted[0], "_".join(splitted[3:]))

        # is it an application
        app = text.split('_')[0]
        if app in ("nxconvert", "nxbrowse", "nxdir", "nxsummary"):
            return (app, text.replace(app+"_", ""))

        # give up
        return ("unknown", text)

    def __getStatus(self,text):
        self.__msg = "<![CDATA[" + text + "]]>"
        self.status = "failed"
        if "skipped" in text:
            self.status = "skipped"
        elif "ok   " in text:
            self.status = "passed"
        
    def __getTimes(self, text):
        if self.status == "skipped":
            return # there is no time

        splitted = text.split()[-2:]
        splitted[0] = splitted[0][1:]
        splitted[1] = splitted[1][:-1]
        splitted = [item[:-1] for item in splitted]
        splitted = [item.split('m') for item in splitted]
        self.__totaltime = float(splitted[0][0]) * 60. + float(splitted[0][1])
        self.__time = float(splitted[1][0]) * 60. + float(splitted[1][1])

    def write(self, handle):
        handle.write('<testcase classname="%s" name="%s" ' % \
                     (self.__group, self.__name))
        handle.write('line="%d" ' % self.__line)
        handle.write('time="%f" ' % self.__time)
        handle.write('totalTime="%f"' % self.__totaltime)
        if self.status == "passed":
            handle.write('/>\n')
            return

        handle.write('>\n')

        handle.write('<%s>\n' % self.status)
        handle.write(self.__msg)
        handle.write('</%s>\n' % self.status)

        handle.write('</testcase>\n')

class TestSuite:
    def __init__(self, data, verbose):
        self.__suitename = "NeXus"
        self.__tests = []
        self.__success = 0
        self.__failures = 0
        self.__skipped = 0
        self.__totaltime = 0

        # find the total execution time
        for (i, line) in zip(range(len(data)), data):
            if line.startswith('testsuite:') and 'duration' in line:
                splitted = line.split()
                splitted = splitted[-3:]
                splitted = [float(item[:-1]) for item in splitted]
                (h,m,s) = splitted
                self.__totaltime = s + 60 * m + 3600 * h
                lastindex = i

        # trim the data down again
        data = data[:lastindex-1]

        # create the individual tests
        for line in data:
            test = Test(line, verbose)
            self.__tests.append(test)
            if test.status == "skipped":
                self.__skipped += 1
            elif test.status == "passed":
                self.__success += 1
            else:
                self.__failures += 1

    def write(self, filename):
        handle = file(filename, 'w')
        handle.write('<?xml version="1.0" encoding="UTF-8" ?>\n')

        handle.write('<testsuite name="%s" ' % self.__suitename)
        handle.write('tests="%d" ' % len(self.__tests))
        handle.write('failures="%d" ' % self.__failures)
        handle.write('skipped="%d" ' % self.__skipped)
        handle.write('package="%s" ' % self.__suitename)
        handle.write('time="%f" ' % self.__totaltime)
        handle.write('>\n')

        for test in self.__tests:
            test.write(handle)

        handle.write('</testsuite>\n')


def getTests(filename, verbose):
    handle = file(filename, 'r')

    data = handle.readlines()
    data = [line.strip() for line in data]
    handle.close()
    for (i, line) in zip(range(len(data)), data):
        if line.startswith("## Running the tests. ##"):
            startindex = i
            break
    data = data[startindex+3:]
    if verbose > 3:
        print "\n".join(data)

    tests = TestSuite(data, verbose)

    return tests

if __name__ == "__main__":
    sourceDir = getSourceDir()
    filenamein = os.path.join(sourceDir, filenamein)
    filenameout = os.path.join(sourceDir, filenameout)
    
    print "Converting '%s' to '%s'" % (filenamein, filenameout)
    if verbose:
        print "Reading in", filenamein
    tests = getTests(filenamein, verbose)
    if verbose:
        print "Writing out", filenameout
    tests.write(filenameout)

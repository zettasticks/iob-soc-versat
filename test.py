#!/usr/bin/python3

#####
# This tests the entire iob-soc system, whose entry point is the top level makefile.
# As such, we build everything on top of the makefile used by iob-soc to setup the project
# Parameters and arguments need to be makefile arguments and any test that actually runs code needs to do it through the makefile.
# We are essentially testing that the makefile works for all the combination of parameters and tests that we want to test.
#####

##############
# TODO: Add a brief description here of what we are doing
# Bunch of stuff that was supposed to be on Json, but since it does not support comments, put it here.
# Basically, just a big warning for future maintainers to not touch certain parts of the test stage unless they know what they are doing
##############

#LEFT HERE 
#Need to reimplement the commands like run-only and stuff like that which is currently broken 

# TODO:
# Test code is kinda of a mess after addition of subtests. 
# Also need lock the fact that makefile and this test need to agree on names for things. It might be helpful to put everything that depends on this kinda of stuff in functions that make it easy to change if needed, especially as we add more parameters
#
# Need to add timeout as an optional argument for each test since some tests require a larger timeout and we want the timeout to be as small as possible otherwise we just waste time
#
# Sometimes I make a change and that causes some tests to run that I did not expect to.
#   If they pass, they overwrite the testCacheGood and so I cannot see immediatly what changed. I could just revert the change and rerun the tests to see, but I kinda want another cache to save the trouble.
#     Something like a testCacheGoodPrevious where we saved the previous testCacheGood if it existed. That way I can compare the new run with the old good and see if the change was intended or not.
#
# JSON parsing/encoding/decoding needs to be automatic otherwise becomes clubersome to extend.
#    At the same time, I want proper error reporting if a member is misspelled or something like that, so a little bit rigit is fine if proper error reporting/handling 
#      Code defines what members the json is supposed to have and the encoding/decoding/parsing is done automatic from that
# Add a command that reenables tests, runs them, and disables them if they fail. (Instead of doing a enable + disabled-failed)
# When any test has a change in the accel data, it should report it. If the amount of configs/states bits decreases or increases I should know about it.
# Can easily add per test information. Something like custom timeouts for commands and stuff like that.
# Can also start saving some info regarding the tests itself, like the average amount of time spend per versat and stuff like that.
#    Would really like to start saving the size of things controlable by Versat (amount of buffers added, muxes added, graph weights [when we get to it], etc...)
# 
#
# For things like architecture change at the hardware level, it should be something that is done for all the tests at the same time.
#   Our maybe it is better if we do it by group. Things like AXI_DATA_W changing does not affect config share and stuff like that, so no point in running those tests for those cases. We want to run the VRead/VWrite tests and stuff like that

import subprocess as sp
import threading
from threading import Lock
import sys
import json
import argparse
import codecs
import queue
import time
import traceback
import shutil
import os
import random
import signal
from dataclasses import dataclass
from enum import Enum,auto
from pprint import pprint

SIM = False

amountOfThreads = 8

COLOR_BASE   = '\33[0m'
COLOR_GREEN  = '\33[32m'
COLOR_RED    = '\33[31m'
COLOR_YELLOW = '\33[33m'
COLOR_BLUE   = '\33[34m'
COLOR_MAGENTA= '\33[35m'
COLOR_CYAN   = '\33[36m'
COLOR_WHITE  = '\33[37m'

# Order of these are important. Assuming that failing sim-run means that it passed pc-emul 
class Stage(Enum):
   DISABLED = auto()
   DISABLED_FAILING = auto()
   NOT_WORKING = auto()
   SHOULD_FAIL = auto()
   VERSAT = auto()
   # These must be in order. It goes PC_EMUL > SIM_RUN > FPGA_RUN (not implemented and probably never will)
   PC_EMUL = auto()
   SIM_RUN = auto()
   FPGA_RUN = auto()

def IsStageDisabled(stage):
   assert(type(stage) == Stage) 
   res = (stage == Stage.DISABLED or stage == Stage.DISABLED_FAILING)
   return res

class ErrorType(Enum):
   NONE = auto()
   EXCEPT = auto()
   TIMEOUT = auto()
   PROGRAM_ERROR = auto()
   TEST_FAILED = auto()

class ErrorSource(Enum):
   NO_SOURCE = auto()
   VERSAT = auto()
   HASHER = auto()
   MAKEFILE = auto()
   PC_EMUL = auto()
   SIM_RUN = auto()

@dataclass
class Error():
   error: ErrorType = ErrorType.NONE
   source: ErrorSource = ErrorSource.NO_SOURCE

   def __repr__(self):
      return "Error[" + self.error.name + ":" + self.source.name + "]"

def IsError(errorType):
   if(type(errorType) == Error):
      return IsError(errorType.error)

   assert(type(errorType) == ErrorType) 
   res = (errorType != ErrorType.NONE)
   return res

class WorkState(Enum):
   INITIAL = auto()
   PROCESS = auto()
   FINISH = auto()

@dataclass
class VersatAcceleratorData:
   configBits: str | None = None
   stateBits: str | None = None
   memUsed: str | None = None
   unitsUsed: str | None = None

def ParseAccelData(jsonInfo):
   # TODO: Find a way of automatizing this if we end up storing more data
   configBits = jsonInfo['configBits'] if 'configBits' in jsonInfo else None
   stateBits = jsonInfo['stateBits'] if 'stateBits' in jsonInfo else None
   memUsed = jsonInfo['memUsed'] if 'memUsed' in jsonInfo else None
   unitsUsed = jsonInfo['unitsUsed'] if 'unitsUsed' in jsonInfo else None

   return VersatAcceleratorData(configBits,stateBits,memUsed,unitsUsed)

@dataclass
class SubTestInfo:
   parent: 'TestInfo'
   args: str
   tokens: int
   hashVal: int
   stage: Stage
   accelData: VersatAcceleratorData | None

   def __eq__(self,other):
      # Need to override eq so that we do not enter a loop because of the parent variable

      res = self.args == other.args
      res |= self.tokens == other.tokens
      res |= self.hashVal == other.hashVal
      res |= self.stage == other.stage
      res |= self.accelData == other.accelData

      return res

@dataclass
class TestInfo:
   name: str
   finalStage: Stage
   tempDisabledStage: Stage
   comment: str
   subTests: list[SubTestInfo]
   expectedError: str = ""

   def __hash__(self):
      return hash(self.name)

@dataclass
class TestData:
   defaultArgs: str
   sameArgs: str
   tests: list[TestInfo]

# For data that we do not want to save
# Do not want to add any data to SubTestInfo or TestInfo that does not get saved 
@dataclass
class ThreadWork:
   test: TestInfo
   subTest: SubTestInfo
   args: str
   makefileArgs: str
   cached: bool = False
   error: Error = Error()

class MyJsonEncoder(json.JSONEncoder):
   def default(self,o):
      if(type(o) == TestData or type(o) == VersatAcceleratorData):
         return vars(o)   
      elif(type(o) == TestInfo):
         asDict = vars(o)
         asDict = {x:y for x,y in asDict.items() if y is not None}
         return asDict
      elif(type(o) == SubTestInfo):
         asDict = vars(o)
         asDict = {x:y for x,y in asDict.items() if y is not None}
         return asDict
      elif(type(o) == Stage):
         return o.name
      else:
         return super().default(o)

def GetTestTrueNameFromArgs(test : TestInfo,args : str):
   name = test.name

   if(args and len(args) > 0):
      sanitizedName = args.replace("-","_")
      name = test.name + sanitizedName

   return name

def GetTestTrueName(test : TestInfo,subTest : SubTestInfo):
   return GetTestTrueNameFromArgs(test,subTest.args)

def ParseJson(jsonInfo,jsonData):
   defaultArgs = jsonInfo['defaultArgs']
   sameArgs = jsonInfo['sameArgs']

   testList = []
   for test in jsonInfo['tests']:

      # Check if all the contents inside a test are valid
      # TODO: There is probably a better way of doing this.
      for member in test:
         if not member in ["name","finalStage","comment","tokens","hashVal","stage","tempDisabledStage","accelData","subTests","expectedError"]:
            print(f"Member '{member}' was not found in the test:")
            print(test)
            sys.exit(0)

      name = test['name']
      finalStage = Stage[test['finalStage']]
      comment = test['comment'] if 'comment' in test else None
      tempDisabledStage = Stage[test['tempDisabledStage']] if 'tempDisabledStage' in test else None
      subTests = test['subTests'] if 'subTests' in test else None
      expectedError = test['expectedError'] if "expectedError" in test else None

      subTestList = []
      info = TestInfo(name,finalStage,tempDisabledStage,comment,subTestList,expectedError)
      testList.append(info)

      if(subTests == None):
         trueName = name
         testData = jsonData[trueName] if jsonData and trueName in jsonData else None
         if(testData):
            tokens = int(testData['tokens']) if 'tokens' in testData else None
            hashVal = int(testData['hashVal']) if 'hashVal' in testData else None
            stage = Stage[testData['stage']] if 'stage' in testData else Stage.NOT_WORKING
            accelData = ParseAccelData(testData['accelData']) if 'accelData' in testData else None
         else:
            tokens = 0
            hashVal = 0
            stage = Stage.NOT_WORKING
            accelData = None

         subTestList.append(SubTestInfo(info,"",tokens,hashVal,stage,accelData))
      else :
            for subTest in subTests:
               args = subTest['args'] if "args" in subTest else None
               if(args):
                  trueName = GetTestTrueNameFromArgs(info,args)
               else:
                  trueName = name

               testData = jsonData[trueName] if jsonData and trueName in jsonData else None
               if(testData):
                  tokens = int(testData['tokens']) if 'tokens' in testData else None
                  hashVal = int(testData['hashVal']) if 'hashVal' in testData else None
                  stage = Stage[testData['stage']] if 'stage' in testData else Stage.NOT_WORKING
                  accelData = ParseAccelData(testData['accelData']) if 'accelData' in testData else None
               else:
                  tokens = 0
                  hashVal = 0
                  stage = Stage.NOT_WORKING
                  accelData = None

               subTestList.append(SubTestInfo(info,args,tokens,hashVal,stage,accelData))

   return TestData(defaultArgs,sameArgs,testList)

# Global data output management. We want to save data as fast as possible
# so that we can still save some progress even if we exit early

printMutex = Lock()
testMutex = Lock()
outputMutex = Lock()
testData = {}
def SaveTest(test,subTest):
   trueName = GetTestTrueName(test,subTest)

   asDict = {}
   if(subTest.args):
      asDict["args"] = subTest.args
   if(subTest.tokens):
      asDict["tokens"] = subTest.tokens
   if(subTest.hashVal):
      asDict["hashVal"] = subTest.hashVal
   if(subTest.stage):
      asDict["stage"] = subTest.stage
   if(subTest.accelData):
      asDict["accelData"] = subTest.accelData

   with testMutex:
      testData[trueName] = asDict

      with open(jsonTestDataPath,"w") as file:
         json.dump(testData,file,cls=MyJsonEncoder,indent=2)

def signal_handler(sig, frame):
   print("Inside signal handler")

   # Do not want to terminate if we are in the middle of writing to a file, otherwise corrupted data
   testMutex.acquire()
   outputMutex.acquire()
   sys.exit(0)

# Used to find values in the form "NAME:VAL"
def FindAndParseValue(content,valueToFind):
   for line in content.split("\n"):
      tokens = line.split(":")

      if(tokens[0] == valueToFind):
         return tokens[1].strip()

   return None

def FindAndParseFilepathList(content):
   filePathList = []
   for line in content.split("\n"):
      tokens = line.split(" ")

      fileName = None
      fileType = None

      if(len(tokens) >= 2):
         if(tokens[0] == "Filename:"):
            fileName = tokens[1]
      if(len(tokens) >= 4):
         if(tokens[2] == "Type:"):
            fileType = tokens[3]

      if(fileName):
         filePathList.append(fileName)

   return filePathList

def GenerateProgramOutput(args,subprocessResult):
   decoder = codecs.getdecoder("utf-8")
   output = "" if subprocessResult.stdout == None else decoder(subprocessResult.stdout)[0]
   errorOutput = "" if subprocessResult.stderr == None else decoder(subprocessResult.stderr)[0]

   header = "\n============\n"

   res = args + "\n\n" + header + "   stdout" + header + "\n" + output + header + "   stderr" + header + "\n" + errorOutput

   return res

def RunVersat(testName,testFolder,versatExtra):
   args = ["./submodules/VERSAT/versat"]

   args += ["./versatSpec.txt"]
   args += ["-O",f"{testFolder}/software"]
   args += ["-o",f"{testFolder}/hardware"]
   args += ["-t",f"{testName}"]
   args += versatExtra.split(" ")

   result = None
   try:
      result = sp.run(args,capture_output=True,timeout=10) # Maybe a bit low for merge based tests, eventually add timeout 'option' to the test itself
   except sp.TimeoutExpired as t:
      return Error(ErrorType.TIMEOUT,ErrorSource.VERSAT),[],None,GenerateProgramOutput(" ".join(args),t)
   except Exception as e:
      print(f"Except on calling Versat:{e}") # This should not happen
      return Error(ErrorType.EXCEPT,ErrorSource.VERSAT),[],None,""

   returnCode = result.returncode

   if(returnCode != 0):
      return Error(ErrorType.PROGRAM_ERROR,ErrorSource.VERSAT),[],None,GenerateProgramOutput(" ".join(args),result)

   decoder = codecs.getdecoder("utf-8")
   output = decoder(result.stdout)[0]
   error = decoder(result.stderr)[0]

   filePathList = FindAndParseFilepathList(output)

   data = VersatAcceleratorData()   
   data.configBits = FindAndParseValue(output,"CONFIG_BITS")
   data.stateBits = FindAndParseValue(output,"STATE_BITS")
   data.memUsed = FindAndParseValue(output,"MEM_USED")
   data.unitsUsed = FindAndParseValue(output,"UNITS")

   # Parse result.
   return Error(),filePathList,data,GenerateProgramOutput(" ".join(args),result)

def TempDir(testName):
   path = f"./testCache/{testName}"
   os.makedirs(path,exist_ok=True)
   return path

def LastGoodDir():
   return "./testCacheGood"

def LastGoodTempDir(testName):
   path = f"{LastGoodDir()}/{testName}"
   os.makedirs(path,exist_ok=True)
   return path

def PairTestsAndSubtests(testList):
   for test in testList:
      for subTest in test.subTests:
         yield (test,subTest)

def ComputeFilesTokenSizeAndHash(files):
   args = ["./submodules/VERSAT/build/calculateHash"] + files

   result = None
   try:
      result = sp.run(args,capture_output=True,timeout=5)
   except sp.TimeoutExpired as t:
      return Error(ErrorType.TIMEOUT,ErrorSource.HASHER),-1,-1
   except Exception as e:
      print(f"Except on ComputeHash:{e}")
      return Error(ErrorType.EXCEPT,ErrorSource.HASHER),-1,-1

   returnCode = result.returncode
   decoder = codecs.getdecoder("utf-8")
   output = decoder(result.stdout)[0]

   if(returnCode == 0):
      amountOfTokens,hashVal = [int(x) for x in output.split(":")]

      return Error(),amountOfTokens,hashVal
   else:
      return Error(ErrorType.PROGRAM_ERROR,ErrorSource.HASHER),-1,-1

def GetTestFolderName(testName,extraArgs,timeout=2):
   result = None
   try:
      command = " ".join(["make","test-folder",f"TEST={testName}",extraArgs if extraArgs else ""])

      result = sp.run(command,capture_output=True,shell=True,timeout=timeout)
   except sp.TimeoutExpired as t:
      return Error(ErrorType.TIMEOUT,ErrorSource.MAKEFILE),GenerateProgramOutput(command,t)
   except Exception as e:
      print(f"Except on calling makefile:{e}")
      return Error(ErrorType.EXCEPT,ErrorSource.MAKEFILE),""

   returnCode = result.returncode

   assert returnCode == 0

   decoder = codecs.getdecoder("utf-8")
   output = "" if result.stdout == None else decoder(result.stdout)[0]

   return output.strip()

# Probably do not want to use makefile, but for now...
def RunMakefile(target,testName,extraArgs,timeout=60):
   result = None
   try:
      command = " ".join(["make",target,f"TEST={testName}",extraArgs if extraArgs else ""])

      result = sp.run(command,capture_output=True,shell=True,timeout=timeout)
   except sp.TimeoutExpired as t:
      return Error(ErrorType.TIMEOUT,ErrorSource.MAKEFILE),GenerateProgramOutput(command,t)
   except Exception as e:
      print(f"Except on calling makefile:{e}")
      return Error(ErrorType.EXCEPT,ErrorSource.MAKEFILE),""

   returnCode = result.returncode

   return Error(),GenerateProgramOutput(command,result)

def CheckTestPassed(testOutput):
   if("TEST_RESULT:TEST_PASSED" in testOutput):
      return True
   elif("TEST_RESULT:TEST_FAILED" in testOutput):
      return False
   else:
      return False

def SaveOutput(testName,fileName,output):
   testTempDir = TempDir(testName)
   with outputMutex:
      with open(testTempDir + f"/{fileName}.txt","w") as file:
         file.write(output)

def PerformTest(test,testTrueName,makefileArg,stage):
   # This function was previously taking the output from the makefile and checking the files using the hasher.
   # This was done because there might be changes from the sim-run and the pc-emul files (stuff like 32bit vs 64 bit addresses and stuff like that)
   # (Although must of the changes right now are "abstracted" inside the verilator makefile, so the hardware is the same (or should be the same))
   # Regardless. If we eventually start making pc-emul and sim-run different, we need to start calculating the hash for each type (pc-emul vs sim-run)
   # Only handle this case when we need it.
   if SIM:
      return Error()

   if stage == Stage.PC_EMUL:
      testFolderName = GetTestFolderName(test,makefileArg)

      # TODO: The fast rules appear to not work correctly. 
      #       Would like to have this working but already spent too much time on this
      if(False and os.path.isdir(testFolderName)):
         error,output = RunMakefile("fast-pc-emul",test,makefileArg)
         SaveOutput(testTrueName,"fast-pc-emul",output)
      else:
         error,output = RunMakefile("clean pc-emul-run",test,makefileArg)
         SaveOutput(testTrueName,"pc-emul",output)

      if(IsError(error)):
         return error

      testPassed = CheckTestPassed(output)
      if(testPassed):
         return Error()
      else:
         return Error(ErrorType.TEST_FAILED,ErrorSource.PC_EMUL)
   if stage == Stage.SIM_RUN: 
      # TODO: fast-sim-run appears to be causing problems, but I kinda would like to make this work.
      #       It might be a problem in the makefiles.
      error,output = RunMakefile("sim-run",test,makefileArg,120)
      SaveOutput(testTrueName,"sim-run",output)

      if(IsError(error)):
         return error

      testPassed = CheckTestPassed(output)
      if(testPassed):
         return Error()
      else:
         return Error(ErrorType.TEST_FAILED,ErrorSource.SIM_RUN)
   if stage == Stage.FPGA_RUN:
      # Not implemented yet, 
      return Error()

   print(f"Error, PerformTest called with: {stage}. Fix this")

MAX_NAME_LENGTH = -1
def PrintResult(result):
   def GeneratePad(word,amount,padding = '.'):
      return padding * (amount - len(word))

   firstColumnSize = MAX_NAME_LENGTH

   test = result.test
   subTest = result.subTest
   name = GetTestTrueName(test,subTest)

   finalStage = test.finalStage
   stage = subTest.stage

   testName = name
   actualComment = test.comment
   comments = f" - {actualComment}" if actualComment else ""
   failing = "FAIL"
   partial = f"PARTIAL"
   partialVal = ""
   ok = "OK"
   cached = "(cached)" if result.cached else ""

   if(stage == Stage.PC_EMUL):
      ok = "OK[1]"
   if(stage == Stage.SIM_RUN):
      ok = "OK[2]"

   testPassed = (result.error.error != ErrorType.TEST_FAILED)
   testRanEverything = (stage == finalStage)

   condition = None
   color = None

   if(not testRanEverything):
      partialVal = f"[{stage.name}/{finalStage.name}]"

   if(IsStageDisabled(finalStage)):
      condition = "DISABLED" if finalStage == Stage.DISABLED else "DISABLED_FAILING"
      partialVal = ""
      color = COLOR_CYAN if finalStage == Stage.DISABLED else COLOR_YELLOW
   elif(stage == Stage.NOT_WORKING):
      condition = failing + " " + stage.name + f"[{finalStage.name}]"
      partialVal = ""
      color = COLOR_RED
   elif(result.error.error == ErrorType.TEST_FAILED):
      color = COLOR_YELLOW
      condition = partial
   elif(IsError(result.error)):
      color = COLOR_RED
      condition = f"{result.error.error.name}:{result.error.source.name}"
   elif(cached):
      condition = partial if not testRanEverything else ok
      color = COLOR_GREEN if testRanEverything else COLOR_YELLOW       
   elif(testPassed):
      condition = ok
      color = COLOR_GREEN
   else:
      print(f"Stage not handled: {stage},{finalStage}")

   firstPad = ' ' + GeneratePad(testName,firstColumnSize - 1)
   secondPad = ' '
   print(f"{testName}{firstPad}{secondPad}{color}{condition}{COLOR_BASE}{partialVal}{cached}{comments}")

def CppLocation(testName):
   return f"./software/src/Tests/{testName}.cpp"

def ThreadMain(workQueue,resultQueue,id):
   while(True):
      work = workQueue.get()
      
      if(work == "Exit"):
         break
      else:
         result = None
         try:
            result = DoWorkDirectly(work)

            SaveTest(result.test,result.subTest)
            ThreadedPrintResult(result)
         except Exception as e:
            print(f"Exception reached ThreadMain:")
            traceback.print_exception(e)

            result = work
            result.error = Error(ErrorType.EXCEPT,ErrorSource.NO_SOURCE)
         
         resultQueue.put(result)
         workQueue.task_done()

def CalculateMaxLengthOfTestNames(testList):
   maxNameLength = 0
   for test,subTest in PairTestsAndSubtests(testList):
      trueName = GetTestTrueName(test,subTest)
      maxNameLength = max(maxNameLength,len(trueName))
   return (maxNameLength + 2) # +2 to have some space between name and result

# TODO: Probably not gonna need to improve this any time soon, but if we do need to add more stuff, rewrite this.
def ParseVersatArgsIntoMakefile(versatArgs):
   versatArgs = versatArgs.strip()
   if(versatArgs == None or versatArgs == ''):
      return ""

   if(versatArgs == '--profile'):
      return "DO_PROFILE=T"
   if(versatArgs == '-b32'):
      return "AXI_DATA_W=32"
   elif(versatArgs == '-b64'):
      return "AXI_DATA_W=64"
   elif(versatArgs == '-b128'):
      return "AXI_DATA_W=128"
   elif(versatArgs == '-b256'):
      return "AXI_DATA_W=256"
   else:
      print(f"[ParseVersatArgsIntoMakefile] Need to add logic for Versat arg: {versatArgs}")

def TempDisableTest(test):
   test.tempDisabledStage = test.finalStage
   test.finalStage = Stage.DISABLED_FAILING

def ReprintButOrganized(testResultList):
   def AllCaps(name):
      for x in name:
         if('A' <= x <= "Z"):
            continue
         return False
      return True

   def TestGroups(testName):
      group = []
      splitted = testName.split("_")

      for split in splitted:
         if(AllCaps(split) and len(split) > 3): # LEN > 3 is mainly because of SHA and F stage and the likes.
            group.append(split)

      return group

   testGroups = {}
   for result in testResultList:
      test = result.test
      name = test.name   

      groups = TestGroups(name)

      for group in groups:
         testGroups[group] = True

   print("\n\nReprinting results grouped:\n\n")

   for group in testGroups.keys():
      print(f"{COLOR_CYAN}{group}{COLOR_BASE}:")
      for result in testResultList:
         test = result.test
         name = test.name   

         groups = TestGroups(name)

         if group in groups:
            PrintResult(result)

      print("")

def ThreadedPrintResult(result):
   with printMutex:
      PrintResult(result)

# MARK
def DoWorkDirectly(work):
   test = work.test
   subTest = work.subTest
   makefileArg = work.makefileArgs

   name = GetTestTrueName(test,subTest)
   finalStage = test.finalStage

   testTempDir = TempDir(name)

   versatError,filepaths,versatData,output = RunVersat(test.name,testTempDir,work.args)

   if(finalStage == Stage.SHOULD_FAIL):
      errorsMatch = True
      for errorType in test.expectedError.split(";"):
         errorType = errorType.strip()

         if(not errorType in output):
            errorsMatch = False

      if(errorsMatch):
         subTest.stage = Stage.SHOULD_FAIL
      return work

   SaveOutput(name,"versat",output)

   subTest.accelData = versatData

   if(IsError(versatError)):
      work.error = versatError
      return work

   if(finalStage == Stage.VERSAT):
      subTest.stage = Stage.VERSAT
      return work

   sourceLocation = CppLocation(test.name)
   filepathsToHash = filepaths + [sourceLocation]
   hashError,tokenAmount,hashVal = ComputeFilesTokenSizeAndHash(filepathsToHash)
   if(IsError(hashError)):
      work.error = hashError
      return work

   testTokens = subTest.tokens if subTest.tokens else 0
   testHashVal = subTest.hashVal if subTest.hashVal else 0

   if(tokenAmount == testTokens and hashVal == testHashVal):
      work.cached = True
      return work

   stageToProcess = Stage.PC_EMUL
   while True:
      error = PerformTest(test.name,name,makefileArg,stageToProcess)
      passed = (not IsError(error))

      if(passed):
         subTest.stage = stageToProcess

         if(stageToProcess == finalStage):
            break
         else:
            stageToProcess = Stage(stageToProcess.value + 1)
            
            SaveTest(work.test,work.subTest)
            ThreadedPrintResult(work)
      else:
         work.error = error
         #work.error.err == ErrorType.TEST_FAILED
         #work.subTest.stage = Stage.NOT_WORKING
         break

   # Only save tokens at the very end otherwise an early break might cause bad data to be saved
   subTest.tokens = tokenAmount
   subTest.hashVal = hashVal
   return work

def RunTests2(testAndSubTestList,sameArgs,defaultArgs):
   global amountOfThreads
   amountOfTests = len(testList)

   workQueue = queue.Queue()
   resultQueue = queue.Queue()
   threadList = [threading.Thread(target=ThreadMain,args=[workQueue,resultQueue,x],daemon=True) for x in range(amountOfThreads)]
   for thread in threadList:
      thread.start()

   amountOfWork = 0   

   for testAndSubTest in testAndSubTestList:
      test,subTest = testAndSubTest
      args = None
      makefileArgs = None
      if(subTest.args):
         args = sameArgs + ' ' + subTest.args
         makefileArgs = ParseVersatArgsIntoMakefile(subTest.args)
      else:
         args = sameArgs + ' ' + defaultArgs

      work = ThreadWork(test,subTest,args,makefileArgs)

      if(IsStageDisabled(test.finalStage)):
         PrintResult(work)
         continue

      amountOfWork += 1
      workQueue.put(work)

   resultList = []

   while(amountOfWork > 0):
      try:
         result = resultQueue.get(True,1)
      except queue.Empty as e:
         time.sleep(1)
         continue

      resultList.append(result)
      amountOfWork -= 1

   return resultList

if __name__ == "__main__":
   signal.signal(signal.SIGINT, signal_handler)

   testInfoJson = None
   jsonTestInfoPath = "testInfo.json"
   jsonTestDataPath = "testData.json"

   # Recompile versat first, otherwise we could have multiple jobs compiling the same exe for no reason
   try:
      args = ["make fast-compile-versat"]
      sp.run(args,capture_output=False,shell=True,timeout=10)
   except sp.TimeoutExpired as t:
      print("Timeout on versat compilation")
      sys.exit(0)
   except Exception as e:
      print("Error on versat compilation")
      print(e)
      sys.exit(0)

   parser = argparse.ArgumentParser(prog="Tester",description="Test Versat, using cache to prevent rerunning unnecessary tests")
   allCommands = ["run","run-only","reset","enable","list"]

   parser.add_argument("command",choices=allCommands)
   parser.add_argument("testFilter",nargs='*')

   arguments = parser.parse_args()

   command = arguments.command
   testFilter = arguments.testFilter if arguments.testFilter else [""]

   try:
      with open(jsonTestInfoPath,"r") as file:
         testInfoJson = json.load(file)
   except Exception as e:
      print(f"Failed to open/parse testInfo file: {e}")
      sys.exit(0)

   try:
      with open(jsonTestDataPath,"r") as file:
         testDataJson = json.load(file)
   except OSError as e:
      testDataJson = None
   except Exception as e:
      print(f"Failed to open/parse testData file: {e}")
      sys.exit(0)

   testInfo = ParseJson(testInfoJson,testDataJson)

   def Filter(name,filters):
      for fil in filters:
         if(fil in name):
            return True
      return False

   allTests = list(PairTestsAndSubtests(testInfo.tests))

   testCount = {}
   for test in testInfo.tests:
      testCount[test.name] = testCount.get(test.name,0) + 1

   for name,count in testCount.items():
      if count > 1:
         print(f"\n\nTest {name} is repeated on the testData.json, remove it\n")
         sys.exit(0)

   testAndSubTestList = [(test,subTest) for test,subTest in allTests if Filter(GetTestTrueName(test,subTest),testFilter)]
   testList = list(set([test for test,subTest in testAndSubTestList]))
   subTestList = [subTest for test,subTest in testAndSubTestList]

   if(len(testList) == 0):
      print("No tests found")
      sys.exit(0)

   MAX_NAME_LENGTH = CalculateMaxLengthOfTestNames(testList)

   print(f"\n\nFound and processing {len(testList)} test(s) and {len(subTestList)} subtest(s)\n")
   print("\n\n")

   allTestNames = [x.name for x in testList]

   nameCount : dict[str,int] = {}
   for name in allTestNames:
      nameCount[name] = nameCount.get(name,0) + 1

   repeatedElements = [x for x,y in nameCount.items() if y != 1]
   if(len(repeatedElements)):
      print("There are repeated tests:\n\t","\t\n".join(repeatedElements),sep='')
      print("Exiting. Fix test info and run again.")
      sys.exit(0)

   # Put any check to the data above this line. 
   # From this point assume data is correct
   
   def SaveResultAsLastGood(result : ThreadWork):
      test: TestInfo = result.test
      subTest: SubTestInfo = result.subTest

      trueName = GetTestTrueName(test,subTest)
      lastGoodLoc = LastGoodTempDir(trueName)
      pathLoc = TempDir(trueName)

      print(f"Saving {pathLoc} to {lastGoodLoc}")

      shutil.rmtree(lastGoodLoc,ignore_errors=True)
      shutil.copytree(pathLoc,lastGoodLoc,dirs_exist_ok=True)

   if(command == "reset"):
      for test in testList:
         for subTest in test.subTests:
            subTest.tokens = None
            subTest.hashVal = None
            subTest.stage = None
            subTest.accelData = None

   elif(command == "run" or command == "run-only" or command == "disable-failed"):
      resultList : list[ThreadWork] = RunTests2(testAndSubTestList,testInfo.sameArgs,testInfo.defaultArgs)

      ReprintButOrganized(resultList)

      # Run test already updates testData.json
      sys.exit(0)

   elif(command == "enable"):
      for test in testList:
         if(test.finalStage == Stage.DISABLED_FAILING):
            test.finalStage = test.tempDisabledStage
            test.tempDisabledStage = None
         elif(test.finalStage == Stage.DISABLED):
            print(f"Test {test.name} was not temp disabled, so cannot enable again")

   elif(command == "list"):
      testList = list(sorted(testList,key=lambda x : x.name))
      for test in testList:
         print(test.name)

   print("\n\n\n") # A few new lines to make easier to see results

   testData = {}
   for test in testInfo.tests:
      for subTest in test.subTests:
         trueName = GetTestTrueName(test,subTest)

         asDict = {}
         if(subTest.args):
            asDict["args"] = subTest.args
         if(subTest.tokens):
            asDict["tokens"] = subTest.tokens
         if(subTest.hashVal):
            asDict["hashVal"] = subTest.hashVal
         if(subTest.stage):
            asDict["stage"] = subTest.stage
         if(subTest.accelData):
            asDict["accelData"] = subTest.accelData

         testData[trueName] = asDict

   with open(jsonTestDataPath,"w") as file:
      json.dump(testData,file,cls=MyJsonEncoder,indent=2)
   sys.exit(0)

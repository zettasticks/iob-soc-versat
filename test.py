#!/usr/bin/python

##############
# TODO: Add a brief description here of what we are doing
# Bunch of stuff that was supposed to be on Json, but since it does not support comments, put it here.
# Basically, just a big warning for future maintainers to not touch certain parts of the test stage unless they know what they are doing
##############

# BUGS:
# Some tests randomly fail, even though they work when testing individually.
#   Probably some problem with the iob-soc setup containing non parallelizable code  
#     (Adding a time.sleep(1) before sim-run appears to have fixed this)

# TODO:
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
# If the test passes sucessfully, I think we could just call make clean and delete the folder.
#    Have not found a situation where I would like to see the result of a good run, unless maybe if I want the tests to generate VCDs.
#
# For things like architecture change at the hardware level, it should be something that is done for all the tests at the same time.
#   Our maybe it is better if we do it by group. Things like AXI_DATA_W changing does not affect config share and stuff like that, so no point in running those tests for those cases. We want to run the VRead/VWrite tests and stuff like that
#
# Check if we can run sim-run without doing a clean first
#
# The tests right now run from make which causes nix-shell to get called all the time.
#    It's better to just have the one time nix-shell call when starting the test.py, if possible

import subprocess as sp
import threading
import sys
import json
import argparse
import codecs
import queue
import time
import traceback
import shutil
import os
from dataclasses import dataclass
from enum import Enum,auto

amountOfThreads = 8
jsonfilePath = None

COLOR_BASE   = '\33[0m'
COLOR_RED    = '\33[31m'
COLOR_GREEN  = '\33[32m'
COLOR_YELLOW = '\33[33m'
COLOR_BLUE   = '\33[34m'
COLOR_MAGENTA= '\33[35m'
COLOR_CYAN   = '\33[36m'
COLOR_WHITE  = '\33[37m'

# Order of these are important. Assuming that failing sim-run means that it passed pc-emul 
class Stage(Enum):
   DISABLED = auto()
   TEMP_DISABLED = auto()
   NOT_WORKING = auto()
   PC_EMUL = auto()
   SIM_RUN = auto()
   FPGA_RUN = auto()

def IsStageDisabled(stage):
   assert(type(stage) == Stage) 
   res = (stage == Stage.DISABLED or stage == Stage.TEMP_DISABLED)
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
   configBits: str = None
   stateBits: str = None
   memUsed: str = None
   unitsUsed: str = None

def ParseAccelData(jsonContent):
   # TODO: Find a way of automatizing this if we end up storing more data
   configBits = jsonContent['configBits'] if 'configBits' in jsonContent else None
   stateBits = jsonContent['stateBits'] if 'stateBits' in jsonContent else None
   memUsed = jsonContent['memUsed'] if 'memUsed' in jsonContent else None
   unitsUsed = jsonContent['unitsUsed'] if 'unitsUsed' in jsonContent else None

   return VersatAcceleratorData(configBits,stateBits,memUsed,unitsUsed)

@dataclass
class TestInfo:
   name: str
   finalStage: Stage
   comment: str
   tokens: int
   hashVal: int
   stage: Stage
   tempDisabledStage: Stage
   accelData: VersatAcceleratorData

@dataclass
class TestData:
   defaultArgs: str
   tests: list[TestInfo]

@dataclass
class ThreadWork:
   test: TestInfo
   args: str
   index: int
   cached: bool = False
   didTokenize: bool = False
   error: Error = Error()
   accelData: VersatAcceleratorData = None
   tokens: int = 0
   hashVal: int = 0
   workStage: WorkState = WorkState.INITIAL
   stageToProcess: Stage = Stage.PC_EMUL
   lastStageReached: Stage = Stage.NOT_WORKING

class MyJsonEncoder(json.JSONEncoder):
   def default(self,o):
      if(type(o) == TestData or type(o) == VersatAcceleratorData):
         return vars(o)   
      elif(type(o) == TestInfo):
         asDict = vars(o)
         asDict = {x:y for x,y in asDict.items() if y is not None}
         return asDict
      elif(type(o) == Stage):
         return o.name
      else:
         return super().default(o)

def ParseJson(jsonContent):
   defaultArgs = jsonContent['defaultArgs']

   testList = []
   for test in jsonContent['tests']:

      # Check if all the contents inside a test are valid
      for member in test:
         if not member in ["name","finalStage","comment","tokens","hashVal","stage","tempDisabledStage","accelData"]:
            print(f"Member '{member}' was not found in the test:")
            print(test)
            sys.exit(0)

      name = test['name']
      finalStage = Stage[test['finalStage']]
      comment = test['comment'] if 'comment' in test else None
      tokens = int(test['tokens']) if 'tokens' in test else None
      hashVal = int(test['hashVal']) if 'hashVal' in test else None
      stage = Stage[test['stage']] if 'stage' in test else Stage.NOT_WORKING
      tempDisabledStage = Stage[test['tempDisabledStage']] if 'tempDisabledStage' in test else None

      accelData = ParseAccelData(test['accelData']) if 'accelData' in test else None

      if(finalStage != Stage.TEMP_DISABLED):
         tempDisabledStage = None

      info = TestInfo(name,finalStage,comment,tokens,hashVal,stage,tempDisabledStage,accelData)
      testList.append(info)

   return TestData(defaultArgs,testList)

# Used to find values in the form "NAME:VAL"
# TODO: Simple and slow but should be fine
def FindAndParseValue(content,valueToFind):
   for line in content.split("\n"):
      tokens = line.split(":")

      if(tokens[0] == valueToFind):
         return tokens[1].strip()

   return None

def FindAndParseFilepathList(content):
   # Probably bottleneck
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

def JoinOutputAndErrorOutput(subprocessResult):
   decoder = codecs.getdecoder("utf-8")
   output = "" if subprocessResult.stdout == None else decoder(subprocessResult.stdout)[0]
   errorOutput = "" if subprocessResult.stderr == None else decoder(subprocessResult.stderr)[0]

   header = "\n============\n"

   res = header + "   stdout" + header + "\n" + output + header + "   stderr" + header + "\n" + errorOutput

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
      return Error(ErrorType.TIMEOUT,ErrorSource.VERSAT),[],None,JoinOutputAndErrorOutput(t)
   except Exception as e:
      print(f"Except on calling Versat:{e}") # This should not happen
      return Error(ErrorType.EXCEPT,ErrorSource.VERSAT),[],None,""

   returnCode = result.returncode

   if(returnCode != 0):
      return Error(ErrorType.PROGRAM_ERROR,ErrorSource.VERSAT),[],None,JoinOutputAndErrorOutput(result)

   decoder = codecs.getdecoder("utf-8")
   output = decoder(result.stdout)[0]

   filePathList = FindAndParseFilepathList(output)

   data = VersatAcceleratorData()   
   data.configBits = FindAndParseValue(output,"CONFIG_BITS")
   data.stateBits = FindAndParseValue(output,"STATE_BITS")
   data.memUsed = FindAndParseValue(output,"MEM_USED")
   data.unitsUsed = FindAndParseValue(output,"UNITS")

   # Parse result.
   return Error(),filePathList,data,JoinOutputAndErrorOutput(result)

def TempDir(testName):
   path = f"./testCache/{testName}"
   os.makedirs(path,exist_ok=True)
   return path

def LastGoodTempDir(testName):
   path = f"./testCacheGood/{testName}"
   os.makedirs(path,exist_ok=True)
   return path

def LastGoodDir():
   return "./testCacheGood/"

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
   #errorOutput = decoder(result.stderr)[0]

   if(returnCode == 0):
      amountOfTokens,hashVal = [int(x) for x in output.split(":")]

      return Error(),amountOfTokens,hashVal
   else:
      return Error(ErrorType.PROGRAM_ERROR,ErrorSource.HASHER),-1,-1

# Probably do not want to use makefile, but for now...
def RunMakefile(target,testName,timeout=60):
   result = None
   try:
      command = " ".join(["make",target,f"TEST={testName}"])

      result = sp.run(command,capture_output=True,shell=True,timeout=timeout) # 60
   except sp.TimeoutExpired as t:
      return Error(ErrorType.TIMEOUT,ErrorSource.MAKEFILE),JoinOutputAndErrorOutput(t)
   except Exception as e:
      print(f"Except on calling makefile:{e}")
      return Error(ErrorType.EXCEPT,ErrorSource.MAKEFILE),""

   returnCode = result.returncode

   return Error(),JoinOutputAndErrorOutput(result)

def CheckTestPassed(testOutput):
   #print(testOutput)
   if("TEST_RESULT:TEST_PASSED" in testOutput):
      #print("Returned true")
      return True
   elif("TEST_RESULT:TEST_FAILED" in testOutput):
      #print("Returned false")
      return False
   else:
      #print("[PythonTest] Something is wrong with the output of the test:")
      #print(testOutput)
      #print("[PythonTest] Could not find either TEST_PASSED or TEST_FAILED")
      return False

def SaveOutput(testName,fileName,output):
   testTempDir = TempDir(testName)
   with open(testTempDir + f"/{fileName}.txt","w") as file:
      file.write(output)

# Need to not only do the test but also get the list of the files generated from versat in order to perform the comparation
def PerformTest(test,stage):
   # This function was previously taking the output from the makefile and checking the files using the hasher.
   # This was done because there might be changes from the sim-run and the pc-emul files (stuff like 32bit vs 64 bit addresses and stuff like that)
   # (Although must of the changes right now are "abstracted" inside the verilator makefile, so the hardware is the same (or should be the same))
   # Regardless. If we eventually start making pc-emul and sim-run different, we need to start calculating the hash for each type (pc-emul vs sim-run)
   # Only handle this case when we need it.

   #print("Reached perform Test")

   if stage == Stage.PC_EMUL:
      error,output = RunMakefile("clean pc-emul-run",test)
      SaveOutput(test,"pc-emul",output)

      if(IsError(error)):
         return error

      testPassed = CheckTestPassed(output)
      if(testPassed):
         return Error()
      else:
         return Error(ErrorType.TEST_FAILED,ErrorSource.PC_EMUL)
   if stage == Stage.SIM_RUN: 
      # Something weird happens if we clean and sim-run in same makefile call
      # Since this test already takes a well, adding a few sleeps does not change much
      time.sleep(1)
      error,output = RunMakefile("clean",test,10)
      time.sleep(1)
      error,output = RunMakefile("sim-run",test,240)
      SaveOutput(test,"sim-run",output)

      if(IsError(error)):
         return error

      #print("reaching here2")

      testPassed = CheckTestPassed(output)
      if(testPassed):
         return Error()
      else:
         return Error(ErrorType.TEST_FAILED,ErrorSource.SIM_RUN)
   if stage == Stage.FPGA_RUN:
      # Not implemented yet, 
      return Error()

   print(f"Error, PerformTest called with: {stage}. Fix this")

def PrintResult(result,firstColumnSize):
   def GeneratePad(word,amount,padding = '.'):
      return padding * (amount - len(word))

   test = result.test
   name = test.name

   finalStage = test.finalStage
   stage = result.lastStageReached

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
      condition = "DISABLED" if finalStage == Stage.DISABLED else "TEMP_DISABLED"
      partialVal = ""
      color = COLOR_YELLOW
   elif(stage == Stage.NOT_WORKING):
      condition = failing + " " + str(result.error)
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
   #secondPad = GeneratePad(condition,1)
   secondPad = ' '
   print(f"{testName}{firstPad}{secondPad}{color}{condition}{COLOR_BASE}{partialVal}{cached}{comments}")

def CppLocation(test):
   return f"./software/src/Tests/{test.name}.cpp"

def ProcessWork(work):
   test = work.test
   name = test.name

   finalStage = test.finalStage

   if(work.workStage == WorkState.INITIAL):
      assert not IsStageDisabled(finalStage)

      testTempDir = TempDir(name)

      versatError,filepaths,versatData,output = RunVersat(name,testTempDir,work.args)
      SaveOutput(name,"versat",output)

      work.accelData = versatData

      if(IsError(versatError)):
         work.error = versatError
         work.workStage = WorkState.FINISH
         return work

      sourceLocation = CppLocation(test)
      filepathsToHash = filepaths + [sourceLocation]
      hashError,tokenAmount,hashVal = ComputeFilesTokenSizeAndHash(filepathsToHash)
      if(IsError(hashError)):
         work.error = hashError
         work.errorSource = ErrorSource.HASHER
         work.workStage = WorkState.FINISH
         return work

      testTokens = test.tokens if test.tokens else 0
      testHashVal = test.hashVal if test.hashVal else 0

      work.tokens = tokenAmount
      work.hashVal = hashVal
      work.didTokenize = True

      if(tokenAmount == testTokens and hashVal == testHashVal):
         work.cached = True
         work.lastStageReached = test.stage
         if(work.lastStageReached == finalStage):
            work.workStage = WorkState.FINISH
            return work

         work.stageToProcess = Stage(work.lastStageReached.value + 1)
         work.workStage = WorkState.PROCESS
         return work

      work.workStage = WorkState.PROCESS
      work.stageToProcess = Stage.PC_EMUL
      return work
   elif(work.workStage == WorkState.PROCESS):
      stage = work.stageToProcess
      error = PerformTest(test.name,stage)
      passed = (not IsError(error))

      if(passed):
         work.lastStageReached = stage
         
         if(work.lastStageReached == finalStage):
            work.workStage = WorkState.FINISH
         else:
            work.stageToProcess = Stage(work.stageToProcess.value + 1)
      else:
         work.error = error

      return work

def ThreadMain(workQueue,resultQueue,index):
   while(True):
      work = workQueue.get()
      
      if(work == "Exit"):
         break
      else:
         result = None
         try:
            result = ProcessWork(work)
         except Exception as e:
            print(f"Exception reached ThreadMain:")
            traceback.print_exception(e)

            result = work
            result.error = Error(ErrorType.EXCEPT,ErrorSource.NO_SOURCE)
         
         resultQueue.put(result)
         workQueue.task_done()

def RunTests(testList,defaultArgs):
   global amountOfThreads
   amountOfTests = len(testList)

   workQueue = queue.Queue()
   resultQueue = queue.Queue()
   threadList = [threading.Thread(target=ThreadMain,args=[workQueue,resultQueue,x],daemon=True) for x in range(amountOfThreads)]
   for thread in threadList:
      thread.start()

   maxNameLength = max([len(test.name) for test in testList]) + 1

   amountOfWork = 0
   for index,test in enumerate(testList):
      work = ThreadWork(test,defaultArgs,index)

      if(IsStageDisabled(test.finalStage)):
         PrintResult(work,maxNameLength)
         continue

      amountOfWork += 1
      workQueue.put(work)

   resultList = []

   # Block until all threads finish processing 
   #workQueue.join()

   while(amountOfWork > 0):
      try:
         result = resultQueue.get(True,1)
      except queue.Empty as e:
         time.sleep(1)
         continue

      amountOfWork -= 1

      if(IsError(result.error) or result.workStage == WorkState.FINISH):
         PrintResult(result,maxNameLength)

         resultList.append(result)
      else:
         amountOfWork += 1
         workQueue.put(result)

   # For now, we do not wait for threads to exit. We just keep them waiting for work forever
   #for x in range(amountOfThreads):
   #   workQueue.put("Exit")

   #for thread in threadList:
   #   thread.join() 

   return resultList

def TempDisableTest(test):
   test.tempDisabledStage = test.finalStage
   test.finalStage = Stage.TEMP_DISABLED

def ReprintButOrganized(testResultList,maxNameLength):
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
            PrintResult(result,maxNameLength)

      print("")

if __name__ == "__main__":
   testInfoJson = None
   jsonfilePath = "testInfo.json"

   try:
      args = ["make fast-compile-versat"]
      result = sp.run(args,capture_output=False,shell=True,timeout=10)
   except sp.TimeoutExpired as t:
      print("Timeout on versat compilation")
      sys.exit(0)
   except Exception as e:
      print("Error on versat compilation")
      print(e)
      sys.exit(0)

   parser = argparse.ArgumentParser(prog="Tester",description="Test Versat, using cache to prevent rerunning unnecessary tests")

   allCommands = ["run","run-only","reset","enable","disable-failed","disable-temp"]

   parser.add_argument("command",choices=allCommands)
   parser.add_argument("testFilter",nargs='*')

   args = parser.parse_args()

   command = args.command
   testFilter = args.testFilter if args.testFilter else [""]

   if(len(testFilter) == 0):
      print("No tests passed the filter")
      sys.exit(0)

   with open(jsonfilePath,"r") as file:
      try:
         testInfoJson = json.load(file)

      except Exception as e:
         print(f"Failed to parse testInfo file: {e}")
         sys.exit(0)

   testInfo = ParseJson(testInfoJson)

   def Filter(name,filters):
      for fil in filters:
         if(fil in name):
            return True
      return False

   testList = [test for test in testInfo.tests if Filter(test.name,testFilter)]

   if(len(testList) == 0):
      print("No tests found")
      sys.exit(0)

   maxNameLength = max([len(test.name) for test in testList]) + 1

   print(f"\n\nFound and processing {len(testList)} test(s)\n")

   print("\n\n")

   allTestNames = [x.name for x in testList]

   nameCount = {}
   for name in allTestNames:
      nameCount[name] = nameCount.get(name,0) + 1

   repeatedElements = [x for x,y in nameCount.items() if y != 1]
   if(len(repeatedElements)):
      print("There are repeated tests:\n\t","\t\n".join(repeatedElements),sep='')
      print("Exiting. Fix test info and run again.")
      sys.exit(0)

   # Put any check to the data above this line. 
   # From this point assume data is correct

   def SaveResultAsLastGood(result):
      name = result.test.name
      lastGoodLoc = LastGoodTempDir(name)
      lastGoodPath = LastGoodDir()
      pathLoc = TempDir(name)

      shutil.rmtree(lastGoodLoc,ignore_errors=True)
      shutil.copytree(pathLoc,lastGoodLoc,dirs_exist_ok=True)

   if(command == "reset"):
      for test in testList:
         test.tokens = None
         test.hashVal = None
         test.stage = None

   elif(command == "run" or command == "run-only" or command == "disable-failed"):
      resultList = RunTests(testList,testInfo.defaultArgs)

      if(command == 'run'):
         for result in resultList:
            if(result.didTokenize and result.lastStageReached.value >= Stage.PC_EMUL.value):
               result.test.tokens = result.tokens
               result.test.hashVal = result.hashVal
               result.test.stage = result.lastStageReached.name
               result.test.accelData = result.accelData

               if(not result.cached):
                  SaveResultAsLastGood(result)

      if(command == "disable-failed"):
         for result in resultList:
            if(IsError(result.error)):
               TempDisableTest(result.test)

      ReprintButOrganized(resultList,maxNameLength)

   elif(command == "enable"):
      for index,test in enumerate(testList):
         if(test.finalStage == Stage.TEMP_DISABLED):
            test.finalStage = test.tempDisabledStage
            test.tempDisabledStage = None
         elif(test.finalStage == Stage.DISABLED):
            print(f"Test {test.name} was not temp disabled, so cannot enable again")

   elif(command == "disable-temp"):
      for index,test in enumerate(testList):
         TempDisableTest(test)

   print("\n\n\n") # A few new lines to make easier to see results

   with open(jsonfilePath,"w") as file:
      json.dump(testInfo,file,cls=MyJsonEncoder,indent=2)

#!/usr/bin/env python3

import sys

inputFile = sys.argv[1]
outputFile = sys.argv[2]
dataW = int(sys.argv[3])

def PadLeft(value,intentedSize,toPad):
   result = toPad * (intentedSize - len(value)) + value
   return result   

def GetNHexFromNBytes(Bytes,N):
   result = ""
   for i in range(N):
      asHex = hex(Bytes[i])[2:]
      asHex = PadLeft(asHex,2,'0')
      result = asHex + result # Order of addition defines little or big endian
   result = PadLeft(result,N*2,'0')
   return result

if __name__ == "__main__":
   DWordList = []

   content = None
   with open(inputFile,"rb") as file:
      content = file.read()

   while(len(content) >= dataW):
      asHex = GetNHexFromNBytes(content[:dataW],dataW)
      DWordList.append(asHex)
      content = content[dataW:]

   if(len(content) != 0):
      content = PadLeft(content,dataW,b'0')
      asHex = GetNHexFromNBytes(content,dataW)
      DWordList.append(asHex)

   with open(outputFile,"w") as file:
      file.writelines([(x + '\n') for x in DWordList])

   print(len(DWordList))
      
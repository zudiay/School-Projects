# zuhal didem aytac
# 2018400045

# This program will traverse the directories
# and look for files or directories that are duplicates of each other (i.e. identical)

import argparse  # parse arguments
import os  # traverse, list files
import subprocess
import hashlib
from collections import defaultdict

workingDirectory = subprocess.check_output('pwd', shell=True)
# parsing the arguments
parser = argparse.ArgumentParser()
group = parser.add_mutually_exclusive_group(required=False)  # Look for:
group.add_argument('-d', action="store_true", default=False)  # identical directories
group.add_argument('-f', action="store_true", default=False)  # identical files
parser.add_argument("-c", action="store_true", default=False)  # Identical means the contents are exactly the same
parser.add_argument("-n", action="store_true", default=False)  # Identical means the names are exactly the same.
parser.add_argument("-s", action="store_true", default=False)  # The size for each duplicate will also be printed
parser.add_argument("dirList", nargs='*', type=str, action="store", default=[(workingDirectory.decode())[:-1]])
# The list of directories to traverse

# applying default arguments if none is specified
args = parser.parse_args()
if not args.d and not args.f:
    args.f = True  # The default is identical files.

if not args.c and not args.n:
    args.c = True  # The default is look for contents

everything = defaultdict(list)  # dictionary that holds hash values and corresponding file or directory paths
knownSubDirectoryHashes = dict()  # dictionary that holds pre-calculated directory paths and hashes of those directories

if args.f:  # looks for identical files
    while args.dirList:
        fullPathName = os.path.abspath(args.dirList.pop())  # handles relative path condition
        ls = os.listdir(fullPathName)
        for current in ls:  # traversing the directory
            currentFullName = os.path.join(fullPathName, current)  # current is a subdirectory or a file
            if os.path.isdir(currentFullName):  # if it is a directory, appends to dirList to traverse in the future
                args.dirList.append(currentFullName)
            elif os.path.isfile(currentFullName):  # if it is a file, calculates its hash
                currentHash = ""
                if args.n and not args.c:  # if n option is used, takes the hash of the name
                    n = hashlib.sha256(current.encode())
                    currentHash = n.hexdigest()
                elif args.c and not args.n:  # if c option is used, reads in byte format, takes the hash of the content
                    with open(currentFullName, "rb") as f:
                        content = f.read()
                        currentHash = hashlib.sha256(content).hexdigest()
                else:  # if cn option is used, takes both names and contents hash and concatenates
                    names_hash = hashlib.sha256(current.encode()).hexdigest()
                    with open(currentFullName, "rb") as f:  # enter full path
                        content = f.read()
                        contents_hash = hashlib.sha256(content).hexdigest()  # hash of the content
                        currentHash = hashlib.sha256((names_hash + contents_hash).encode()).hexdigest()

                if (currentHash not in everything) or (currentFullName not in everything.get(currentHash)):
                    everything[currentHash].append(currentFullName)  # appends {hash:fullPath} to the dict

if args.d:  # looks for identical directories
    for dirArgument in args.dirList:
        walker = list(os.walk(dirArgument, topdown=False))  # traverse the current argument directory
        for root, subdirectories, fileNames in walker:
            hashlist = list()  # holds the hash values of subdirectories and files

            for filename in fileNames:  # traverse all files
                fullFileName = os.path.join(root, filename)
                if args.n and not args.c:  # if n option is used, takes the hash of the file name
                    hashlist.append(hashlib.sha256(filename.encode()).digest().hex())
                elif args.c and not args.n:
                    with open(fullFileName, 'rb') as f:  # reads in byte format and takes the hash of the content
                        content = f.read()
                        hashlist.append(hashlib.sha256(content).digest().hex())
                else: # if cn option is used, takes both names and contents hash and concatenates
                    names_hash = hashlib.sha256(filename.encode()).digest().hex()
                    with open(fullFileName, 'rb') as f:  # reads in byte format and takes the hash of the content
                        content = f.read()
                        contents_hash = hashlib.sha256(content).digest().hex()
                        hashlist.append((hashlib.sha256((names_hash + contents_hash).encode()).hexdigest()))

            for subdir in subdirectories:  # traverse all subdirectories
                names_hash = hashlib.sha256(subdir.encode()).digest().hex()
                fullSubDirName = os.path.join(root, subdir)
                subHash = knownSubDirectoryHashes[fullSubDirName]  # looks up for the hash value of the subdirectory
                subHashHash = hashlib.sha256(subHash).digest().hex()
                if args.n:
                    hashlist.append((hashlib.sha256((names_hash + subHashHash).encode()).hexdigest()))
                else:  # appends the hash to the list
                    hashlist.append(subHashHash)

            if args.n:  # if n option is used, takes the hash of the name of the current directory
                hashlist.append(hashlib.sha256((os.path.basename(root)).encode()).digest().hex())

            hashlist.sort()  # sorts the hashes in lexical order
            currentHash = hashlib.sha256()  # current hash object holds the hash value of the root
            for i in hashlist:  # updates the hash of the current directory with the hashes in the list
                currentHash.update(i.encode())
            currentHash = currentHash.digest()
            if (currentHash not in everything) or (root not in everything.get(currentHash)):
                everything[currentHash].append(root)  # appends {hash:fullPath} to the dict
            knownSubDirectoryHashes[root] = currentHash  # the hash is known, appends to the known list

# selects duplicates from everything dictionary and appends to a new dict
duplicates = list()  # dictionary that holds detected duplicates
duplicatesSize = list()


# function that recursively calculates the size of a directory
# it is the sum of sizes of files inside it.
def directorySize(dir):
    totalSize = os.path.getsize(dir)
    for item in os.listdir(dir):
        fullpath = os.path.join(dir, item)
        if os.path.isdir(fullpath):
            totalSize += directorySize(fullpath)
        elif os.path.isfile(fullpath):
            totalSize += os.path.getsize(fullpath)
    return totalSize


for i in everything:  # in everything looks for hash values with more than one item
    if len(everything[i]) > 1:
        everything[i].sort()
        duplicates.append(everything[i])  # pushes those into the duplicates dictionary
        sizeList = list()
        if args.s and args.c:
            for dupp in sorted(everything[i]):
                if args.f:
                    size = os.path.getsize(dupp)
                else:
                    size = directorySize(dupp)
                sizeList.append((size*(-1), dupp))
            sizeList.sort()
            duplicatesSize.append(sizeList)

# every duplicate set is sorted alphabetically in itself

# printing the full path names of duplicates will as output
if args.s and args.c:
    duplicatesSize.sort()
    for i in duplicatesSize:
        for dupsizepair in i:
            print(dupsizepair[1], '\t', dupsizepair[0]*(-1))
        print('\n')

else:
    duplicates.sort()
    for i in duplicates:
        i.sort()
        for x in range(len(i)):  # print duplicate group
            print(i[x])
        print('\n')

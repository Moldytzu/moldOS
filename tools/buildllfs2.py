# LLFS version 2 packager
import os
import sys
import datetime

inputfolder = sys.argv[1]
output = sys.argv[2]

def AddFile(ofile,name,directory,attributtes,size,content):
    print("Adding file " + name)
    ofile.write(b"\x01")
    ofile.write(len(name).to_bytes(1,'little'))
    ofile.write(name.encode())
    ofile.write(len(directory).to_bytes(1,'little'))
    ofile.write(directory.encode())
    ofile.write(attributtes.to_bytes(1,'little'))
    ofile.write(size.to_bytes(8,'little'))
    ofile.write(content)

def AddFileFromHardDisk(ofile,name):
    ifile = open(name,"rb")
    size = os.stat(name).st_size
    data = ifile.read()
    ifile.close()
    name = name.split('/')
    AddFile(ofile,name[-1],"dir",0,size,data)

print("Building the file system")

files = os.listdir(inputfolder)

filecount = len(files).to_bytes(8,'little') # 8 bytes

outfile = open(output,"wb")

# write the header
outfile.write("LLFS".encode()) # signature
outfile.write(b"\x02") # version
outfile.write(filecount) # file count

for i in range(4):
    outfile.write(b"\x00") # padding

today = str(datetime.date.today()).split('-')
today[0] = today[0][2:]
# write metadata
outfile.write(b"\x02")
outfile.write(int(today[0]).to_bytes(1,'little'))
outfile.write(int(today[1]).to_bytes(1,'little'))
outfile.write(int(today[2]).to_bytes(1,'little'))

# add the files
for i in range(len(files)):
    AddFileFromHardDisk(outfile,inputfolder+files[i])

# close the file
outfile.close()
print("Done")

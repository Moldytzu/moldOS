# LLFS packager
import os

inputfolder = "ramfs/"
output = "kernel/bin/ram.llfs"

def AddFile(ofile,name,size,content):
    print("Adding file " + name)
    ofile.write(name.encode())
    ofile.write(size)
    ofile.write(b"\x00")
    ofile.write(content)

def AddFileFromHardDisk(ofile,name):
    ifile = open(name,"rb")
    size = os.stat(name).st_size
    data = ifile.read()
    ifile.close()
    name = name.split('/')
    name[-1] = name[-1]+(" "*(16-len(name[-1])))
    AddFile(ofile,name[-1],size.to_bytes(8,'little'),data)

print("Building the file system")

files = os.listdir(inputfolder)

filecount = len(files).to_bytes(8,'little') # 8 bytes

outfile = open(output,"wb")

# write the header
outfile.write("LLFS".encode()) # signature
outfile.write(b"\x01") # version
outfile.write(filecount) # file count

for i in range(4):
    outfile.write(b"\x00") # padding

# add the files
for i in range(len(files)):
    AddFileFromHardDisk(outfile,inputfolder+files[i])

# close the file
outfile.close()
print("Done")

print('Hello World')


import os, sys

# listing directories
#print "The dir is: %s"%os.listdir(os.getcwd())

# renaming directory ''tutorialsdir"
try:
    os.rename("C:\Program Files (x86)\SAAZOD\ApplicationLog","C:\Program Files (x86)\SAAZOD\old_ApplicationLog") 
except OSError as error:
    print"Access Denied to the file get caught"

print "Successfully renamed."

# listing directories after renaming "tutorialsdir"
print "the dir is: %s" %os.listdir(os.getcwd())\

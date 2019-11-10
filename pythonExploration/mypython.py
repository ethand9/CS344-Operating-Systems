import random 

#get a random string of 10 characters
#and create a file with the given name
#and write to it
def randString(fileName): 
    #append 10 random lowercase letters to the string
    string = ""
    for i in range(10):
        string += (chr(random.randrange(97, 122))) 
    string += "\n"
    #write the string to the given file
    myFile = open(fileName, "w")
    myFile.write(string)
    myFile.close()
    #print the file contents
    myFile = open(fileName, "r")
    print(myFile.read(), end = "")
    myFile.close()

#create three files with the given names
#give them random strings and print them
randString("gibberish1")
randString("gibberish2")
randString("gibberish3")

#create two random integers from 1 to 42 then print and multiply them
num1 = (random.randrange(1, 42))
num2 = (random.randrange(1, 42))
print(num1)
print(num2)
print(num1*num2)

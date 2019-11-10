import random 

#get a random string of 10 characters
#and create a file with the given name
#and write to it
def randString(fileName): 
    string = ""
    for i in range(10):
        string += (chr(random.randrange(97, 122))) #append a random lowercase letter
    string += "\n"
    f = open(fileName, "w")
    f.write(string)
    print(string, end ="")

#create 3 files and write the strings to them
randString("gibberish1")
randString("gibberish2")
randString("gibberish3")

#get 3 random integers from 1 to 42 print them and multiply them
num1 = (random.randrange(1, 42))
num2 = (random.randrange(1, 42))
print(num1)
print(num2)
print(num1*num2)

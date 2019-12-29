'''
* Team Id : 			#1850
*
* Author List : 		Bharath T.U , Abhijeet C ,Gautham Rajesh , Joyal T John
*
* Filename: 			python_to_robot.py
*
* Theme: 				Homecoming
*
* Functions: 			none
*
* Global Variables: 	none since there is no functions
'''

'''
Variable Comments

com_port                       :  COM PORT of BOT
input_str                      :  to store the input string
animal_pos                     :  to store postion of animals 
habitat_pos                    :  to store postion of habitat
habitat_position_map           :  mapping of the arena
time                           : to store local time
model                          :  the model we used for transfer learning
serial_port                    : for serial transfer
association                    : to map all animals to associated habitat
data                           : temporay dictionary 
animal                         : to store animals
habitat                        : to store habitat
'''

import sys
import os
import cv2
import argparse
import datetime
from serial import Serial
import pandas as pd
import detector

animal={}
habitat={}
com_port = 9 # COM PORT of Firebird v
current_point=2*[1]
animal_pos=[]
habitat_pos=[]
habitat_position_map=[
[[1, 1], [1, 2],  [2, 2], [2, 1]],    [[1, 2], [1, 3], [2, 3], [2, 2]],     [[1, 3], [1, 4], [2, 4], [2, 3]],     [[1, 4], [1, 5], [2, 5], [2, 4]],       [[1, 5], [1, 6], [2, 6], [2, 5]],
[[2, 1], [2, 2],  [3, 2], [3, 1]],    [[2, 2], [2, 3], [3, 3], [3, 2]],     [[2, 3], [2, 4], [3, 4], [3, 3]],     [[2, 4], [2, 5], [3, 5], [3, 4]],       [[2, 5], [2, 6], [3, 6], [3, 5]],
[[3, 1], [3, 2],  [4, 2], [4, 1]],    [[3, 2], [3, 3], [4, 3], [4, 2]],     [[3, 3], [3, 4], [4, 4], [4, 3]],     [[3, 4], [3, 5], [4, 5], [4, 4]],       [[3, 5], [3, 6], [4, 6], [4, 5]],
[[4, 1], [4, 2],  [5, 2], [5, 1]],    [[4, 2], [4, 3], [5, 3], [5, 2]],     [[4, 3], [4, 5], [5, 4], [5, 3]],     [[4, 4], [4, 5], [5, 5], [5, 4]],       [[4, 5], [4, 6], [5, 6], [5, 5]],
[[5, 1], [5, 2],  [6, 2], [6, 1]],    [[5, 2], [5, 3], [6, 3], [6, 2]],     [[5, 3], [5, 4], [6, 4], [6, 3]],     [[5, 4], [5, 5], [6, 5], [6, 4]],       [[5, 5], [5, 6], [6, 6], [6, 5]]
]
association={'baseball': ['invalid'], 'basketball court': ['invalid'], 'beach': ['crab', 'frog', 'butterfly', 'eagle'], 'shrubs': ['coyote', 'horse', 'kangaroo', 'goat'], 'circular farm': ['cow ', 'deer', 'snake', 'monkey'], 'cloud': ['parrot', 'eagle', 'bee'], 'commercial area': ['butterfly', 'rat', 'bee', 'parrot'], 'dense residential': ['dog', 'cat', 'frog'], 'desert': ['rat', 'snake ', 'coyote'], 'forest': ['tiger', 'lion', 'cougar', 'bear', 'deer', 'elephant', 'giraffe', 'zebra'], 'golf course': ['invalid'], 'harbor': ['invalid'], 'island': ['skunk', 'crab', 'otter', 'goat'], 'lake': ['crab', 'crocodile', 'frog', 'fish'], 'meadow': ['cow', 'deer', 'elephant', 'giraffe', 'lion', 'sheep', 'bee', 'zebra'], 'medium residential area': ['cat', 'dog', 'monkey', 'raccoon', 'snake'], 'mountain': ['cougar', 'horse', 'panda', 'sheep', 'yak', 'zebra'], 'rectangular farm': ['cow ', 'horse', 'kangaroo', 'goat'], 'river': ['crocodile', 'fish', 'frog', 'hippo'], 'sea glacier': ['seal', 'shark', 'penguin', 'otter'], 'snowberg': ['snow leopard', 'arctic fox', 'panda'], 'sparse residential area': ['cow', 'deer', 'sheep', 'raccoon'], 'thermal power station': ['invalid'], 'wetland': ['skunk', 'frog', 'hippo', 'rat']}
nanimal=20*[2*[0]]
nhabitat=25*[3*[0]]
flag1=0
flag=0
amod_path='animal.pth'#animal model
hmod_path='habitat.pth'   #habitat model
output_location='/'

def func1(i):
	q=[]
	for i in range(len(animal_pos)-1):
		q=q+[animal_pos[i]]
	animal_pos=q

""" 
    * Function Name: cost()
    * Input: current,target
    * Output: (abs(current[0] - target[0]) + abs(current[1] - target[1]))
    * Logic: this is to find the absoulute distance  between (current animal postion -> target animal postion) and (current habitat postion -> target habitat postion)
    * Example Call: cost(habitat_position_map[target_cell-1][i],current)
"""

def cost(target,current):
	return (abs(current[0] - target[0]) + abs(current[1] - target[1]))

""" 
    * Function Name: nearest()
    * Input: current,target_cell
    * Output: nearestX,nearestY,lowest_cost
    * Logic: this is to find nearest path from current location to target location
    * Example Call: nearest(int(habitat_pos[i]),nanimal[i])
"""

def nearest(target_cell,current):
	lowest_cost = 100
	for i in range(4):
		current_cost =cost(habitat_position_map[target_cell-1][i],current)
		if (current_cost < lowest_cost):
			nearestX = habitat_position_map[target_cell-1][i][0]
			nearestY= habitat_position_map[target_cell-1][i][1]
			lowest_cost = current_cost
	return nearestX,nearestY,lowest_cost

#using argprase to do saving,giving animal and habitat model and to give arena pic
parser = argparse.ArgumentParser( usage="""python myscript.py {arena}""",description='''Description.''',epilog="""Epilog.""")
parser.add_argument("arena", help="input path")
parser.add_argument("-s",'--save', help="saveing path")
parser.add_argument('--amod',help="animal model path")
parser.add_argument('--hmod',help="habitat model path")
args = parser.parse_args()
if(args.arena==None):
	print("ERROR:no input path specified")
	exit()
input_location=args.arena
if(args.amod!=None):
	amod_path=args.amod
if(args.hmod!=None):
	hmod_path=args.hmod
if(args.save!=None):
	flag1=1
	output_location=args.save


#function call to get the predictions from identifier
habitat,animal,output_image=detector.identifier(flag1,amod_path,hmod_path,input_location,output_location)
data={}
#to print the predicted animals and habitats
print("############################################### ANIMALS ###############################################")
for i in animal:
	print("%s(%s)"%(animal[i],i))
print("############################################## HABITATS ###############################################")
for i in habitat:
	if(association[habitat[i]]==['invalid']):
		print("%s(%s) - Invalid"%(habitat[i],i))
	else:
		print("%s(%s)"%(habitat[i],i))


'''
in the bellow for llops the association dictionary is changed into the a format where the keys will be the animals present in habitat
and the values will be a list of all the habitats that could be the habitat of the key animal 
'''
#####################################################start##############################################################
for i in association:
	t=0
	for j in habitat:
		if(i==habitat[j] and association[i]!=['invalid']):
			data[j]=[]
			for k in association[i]:
				for w in animal:
					if(animal[w]==k):
						data[j]=data[j]+[w]
association={}
for i in animal:
	association[i]=[]
	for j in data:
		flag=0
		for k in data[j]:
			if(k==i):
				flag=1
		if(flag):
			association[i]=association[i]+[j]
#######################################################end##############################################################



'''
in the bellow loop the list of habitat for each animals are sorted with nearest habitat to farthest
'''
#####################################################start##############################################################
for i in association:
	if(len(association[i])!=1):
		for j in range(len(association[i])):
			lowest_cost=100
			for k in range(len(association[i])-j-1):
				y1,u1,p1=nearest(int(association[i][k]),[ord(i[1])-48,ord(i[0])-64] )
				y2,u2,p2=nearest(int(association[i][k+1]),[ord(i[1])-48,ord(i[0])-64] )
				if(p1>p2):
					association[i][k],association[i][k+1]=association[i][k+1],association[i][k]
'''
the bellow loop is fo sorting the dictnory by the number of habitat availble for each animal
it soets from lowest to highest
'''
data={}
for i in sorted(association, key=lambda k: len(association[k])):
        data[i]=association[i]
association=data
#######################################################end##############################################################




'''
the nested loop bellow is the logic for mapping each animals to a single habitat the basic logic is to put the nearest habitat
as the habitat where the animal is to be placed if that particular habitat is not occupied already by 2 animals if it is the second
option is choosen (the condition in statment 1 is a experimental one ,it is for a rare case that for eg animal A can be placed at 
1&3 and was placed at 1 as it was nearer and similary B was placed at 1 although it can be placed at 5 the ase is what if another 
animal comes whoes all other habitats are full and in the case of 1 both the animals in it has a second option so the condition is 
to change one of the animals habitat from 1 to its other option if free )
'''
#####################################################start##############################################################
data={}
for i in association:
	animal_pos=animal_pos+[i]
	if(len(association[i])==1):
		flag=0
		for k in habitat_pos:
			if(association[i]==[k]):
				flag=flag+1
		if(flag<2):
			habitat_pos=habitat_pos+association[i]
		else:
			func1(i)
	else:
		for j in association[i]:
			flag=0
			for k in habitat_pos:
				if(j==k):
					flag=flag+1
			if(flag<2):
				habitat_pos=habitat_pos+[j]
				break
	if(len(habitat_pos)<len(animal_pos)):#statment1
		for data in association[animal_pos[len(animal_pos)-1]]:
			for m in range(len(habitat_pos)):
				if(habitat_pos[m]==data):
					flag1=0
					for n in association[animal_pos[m]]:
						flag=0
						if(flag1==1):
							for k in habitat_pos:
								if(j==k):
									flag=flag+1
						if(flag<2):
							habitat_pos[m]=j
							habitat_pos=habitat_pos+[data]
							break
						if(n==habitat_pos[m]):
							flag1=1
				if(len(habitat_pos)==(animal_pos)):
					break
			if(len(habitat_pos)==(animal_pos)):
				break			
	if(len(habitat_pos)<len(animal_pos)):
		func1(i)	
#######################################################end##############################################################
i=0
j=0
k=0


'''
to display the Output image Press enter to move on to the result that to be send to bot
'''	
cv2.imshow('2',detector.image_resize(output_image, height = 700, width =700 ))
cv2.waitKey(0)
cv2.destroyAllWindows()

while(i<len(animal_pos)):
	nanimal[i]=[ord(animal_pos[i][1])-48,ord(animal_pos[i][0])-64] 
	y,u,p=nearest(int(habitat_pos[i]),nanimal[i])
	nhabitat[i]=[y,u,p]
	i=i+1

j=0
i=0
pos=0
# This while is used for sorting the animals and habitat with a certain cost between animal and habitat
while(i<len(animal_pos)):
	lowest_cost=1000
	j=i
	while(j<len(animal_pos)):
		current_cost=cost(nanimal[j],current_point)+nhabitat[j][2]# animal to distance cost and coordinate
		if(current_cost<lowest_cost):
			lowest_cost=current_cost
			pos=j
		j=j+1
	current_point=nhabitat[pos]	
	nanimal[i],nanimal[pos]=nanimal[pos],nanimal[i]# storing the location
	nhabitat[i],nhabitat[pos]=nhabitat[pos],nhabitat[i]
	habitat_pos[i],habitat_pos[pos]=habitat_pos[pos],habitat_pos[i]
	animal_pos[i],animal_pos[pos]=animal_pos[pos],animal_pos[i]
	i=i+1
input_str=""
i=0
# This while loop is to convert coordinate into  string form where y is x coordinate u is y coordinate and p is the cost
#animal coordinate is stored in nanimal and habitat coordinate in nhanitat
while(i<len(animal_pos)):
	input_str=input_str+habitat_pos[i]
	if(i!=len(animal_pos)-1):
		input_str=input_str+', '
	i=i+1
input_str=input_str+'\n'
i=0
while(i<len(animal_pos)):
	input_str=input_str+animal_pos[i]
	if(i!=len(animal_pos)-1):
		input_str=input_str+', '
	i=i+1

# this input string after sorting w.r.t cost and adding # to show end of string
input_str=input_str+' #'
print("############################################## UPLOADING ###############################################")
time=datetime.datetime.now().strftime("\n%H:%M:%S\n")
#time before serial transfer
print(time)
print("animal=%s"%(animal_pos))
print("habitat=%s"%(habitat_pos))

# this is to try connecting usb to to Firebird V using serial communication
try:
	# send output_str to the robot using pyserial
	serial_port = Serial("COM"+str(com_port), 9600) # parameters: comport, baudrate
	serial_port.write(input_str.encode()) #send file
except:
	print ("\nError: \tCould not open COM Port COM"+str(com_port))
	print ("\tPlease connect the robot to the PC via USB to Serial cable and retry")
time=datetime.datetime.now().strftime("\n%H:%M:%S")
#time after serial transfer
print(time)



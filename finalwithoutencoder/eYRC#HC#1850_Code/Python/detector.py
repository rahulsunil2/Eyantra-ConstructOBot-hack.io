'''* Team Id : #1850
* Author List : Bharath.T.U , Abhijeet C ,Gautham Rajesh , Joyal T John
* Filename: dector.py
* Theme: Homecoming
* Functions: image_resize(),identifier()
* Global Variables: input_location,output_location,flag1,input_image,imgray,thresh,animal_contour,index2,output_dic
					,animal_location ,habitat_location,font
'''

'''
Variable Comments

input_location             = The path to the input image
output_location            = the path where output image is to be stored
flag1                      = a flag to decide is the output image is to be saved or not
input_image                = image to be proccessed
imgray                     = gray scale of image to be proccessed
thresh                     = to store threshold value

animal_contour             = array to store index of contours which corresponds to where animal/digit pictures are expected to be present 
index1                     = this varible is to control the index of animal_contour[]
habitat_contour            = array to store index of contours which corresponds to where habitat/coloured box pictures are expected to be present 
index2                     = this varible is to control the index of habitat_contour[]
output_dic				   = this dictnory stores the prediction and the corresponding loaction
image, contours, hierarchy = these are the return of "findContour()"  where "contours" stores list of contours and "hierarchy" stores list of hierarchy


animal_location            = it is the list of all the location names corresponding to the animal_contour list  
habitat_location           = it is the list of all the location names corresponding to the habitat_contour list  
font                       = a variable to store font in which text is written in picture 
'''

import cv2
import sys
import os
import argparse
import load_habitat, load_animal

""" 
    * Function Name: image_resize()
    * Input: image, width = None, height = None, inter = cv2.INTER_AREA
    * Output: resized
    * Logic: this function split data set into training and testing set 
    * Example Call: image_resize(input_image, height = 700, width =700 )
"""
def image_resize(image, width = None, height = None, inter = cv2.INTER_AREA):
	# initialize the dimensions of the image to be resized and
	# grab the image size
	dim = None
	(h, w) = image.shape[:2]

	# if both the width and height are None, then return the
	# original image
	if width is None and height is None:
		return image

	# check to see if the width is None
	if width is None:
		# calculate the ratio of the height and construct the
       	# dimensions
		r = height / float(h)
		dim = (int(w * r), height)

    # otherwise, the height is None
	else:
		# calculate the ratio of the width and construct the
   		# dimensions
		r = width / float(w)
		dim = (width, int(h * r))

	# resize the image
	resized = cv2.resize(image, dim, interpolation = inter)

   	# return the resized image
	return resized

""" 
    * Function Name: identifier()
    * Input: flag1,amod_path,hmod_path,input_location,output_location
    * Output: habitat_dic,animal_dic,input_image
    * Logic: this function split data set into training and testing set 
    * Example Call: identifier(flag1,amod_path,hmod_path,input_location,output_location)
"""
def identifier(flag1,amod_path,hmod_path,input_location,output_location):
	animal_contour=1000*[-1]
	habitat_contour=1000*[-1]
	j=0
	i=0
	index1=0
	index2=0
	animal_dic={}
	habitat_dic={}
	animal_location=["F1","E1", "D1" ,"C1","B1" ,"A1","F2","A2", "F3", "A3" ,"F4", "A4" ,"F5" ,"A5" , "F6" ,"E6", "D6", "C6", "B6", "A6"]
	habitat_location=["5" ,"4" ,"3" ,"2", "1", "10", "9", "8", "7", "6" ,"15" ,"14" ,"13" ,"12", "11", "20", "19" ,"18" ,"17", "16", "25", "24", "23", "22", "21"]
	font = cv2.FONT_HERSHEY_SIMPLEX
	input_image = cv2.imread(input_location,cv2.COLOR_BGR2RGB)
	imgray = cv2.cvtColor(input_image,cv2.COLOR_BGR2GRAY)
	thresh = cv2.adaptiveThreshold(imgray,255,cv2.ADAPTIVE_THRESH_MEAN_C,cv2.THRESH_BINARY,3,0)
	image, contours, hierarchy = cv2.findContours(thresh,cv2.RETR_TREE,cv2.CHAIN_APPROX_NONE)
	'''
	This while loop is to finds the index of contours which corresponds to where (animal/digit)//(habitat/coloured) box are expected to be present and 
	store it in animal_contour/habitat_contour respectivily. The loop is controled by i which varies from 0 to len(countours) incremented by 1
	'''
	'''
	statement1 : to check if contour is child of no countour
	statement2 : to check if perimeter of child contour is less than double the perimeter of contours[1]
    	        (to select the contours where the animals is to be expected)
	statement3 : this loop is for selecting all the inner contour where the habitat are expected.The loop is controled by j which varies from 0 to 
             	len(countours) incremented by 1
	statement4 : to check if contour is child of contours[i+1]
	'''
	while(i<len(contours)):
		if(hierarchy[0][i][3]==-1):#statement1
			if((cv2.arcLength(contours[1],True)*2)>cv2.arcLength(contours[i],True)):#statement2
				animal_contour[index1]=i+1
				index1=index1+1
			else:	
				j=i+1
				n=i
				while(j<len(contours)):#statement3
					if(hierarchy[0][j][3]==i+1):#statement4

						habitat_contour[index2]=j
						index2=index2+1
					j=j+1	
		i=i+1
	i=0
	j=0
	while(animal_contour[i]!=-1):
		if(hierarchy[0][animal_contour[i]+2][2]!=-1):
		
			#the bellow while loop,conditons and the varibels are for selecting the correct countor from inside the box
			p=animal_contour[i]+3
			g=p
			while(p<animal_contour[i+1] and hierarchy[0][p][3]!=-1):
				if(cv2.contourArea(contours[g])<cv2.contourArea(contours[p])):
					g=p
				p=p+1
			if(cv2.contourArea(contours[g])<cv2.contourArea(contours[animal_contour[i]+2])/2.2):
				g=animal_contour[i]+2

			# x,y,w,h are varibles to store the return values of boundingRect() where x,y,w,h are the x coordinate ,y coordinate ,width,hight respectivly of the rectangle
			x,y,w,h = cv2.boundingRect(contours[g])
		
			#here the dictnory is added with a new element with key as the position of the picture and the return from the model function
			#which is inside load_animal as value
			f=load_animal.model(input_image[y:y+h, x:x+w],amod_path)
			if(f!="invalid"):
				animal_dic[animal_location[i]]=f
		
			#cx , cy are the x coordinate,y coordinate of the centre if the coutour respectivly
			#as the text started from the centre to make it more centrlised 0.0095*resolution and 0.038*resolution is subtracted from x&y coordinate respectivly
			#the 2 constants 0.0095 and 0.038 was used to adjust the position with change in resolution 
			cx = int((x+w/2)-(0.0095*input_image.shape[0]))
			cy = int((y+h/2)-(0.038*input_image.shape[0]))
		
			#the bellow function call is to write the text inside the contour (the font size was optimised in a similar method to the position optimisation)
			cv2.putText(input_image,animal_location[i],(cx,cy), font, input_image.shape[0]*0.0005,(0,0,0),1,cv2.LINE_AA)
		
			#the bellow function call is for drawing the rectangle around the countour in blue coulour
			cv2.rectangle(input_image,(x,y),(x+w,y+h),(255,0,0),int(input_image.shape[0]*0.0025))

		elif(hierarchy[0][animal_contour[i]+1][2]!=-1):
			k=1
			# this loop is for checking if the boundry is black or not
			for cnt in contours[animal_contour[i]+2]:
				x,y=cnt[0]
			
				#if the below condition becomes true that means the whole is covered by the coulored box or image(that is a point is found in the countou wich is not shade of black or white)
				if(input_image[y,x,0]!=input_image[y,x,1] and input_image[y,x,1]!=input_image[y,x,2]):
					k=0
					break
			if(k==0):
				x,y,w,h = cv2.boundingRect(contours[animal_contour[i]])
			
				#here the dictnory is added with a new element with key as the position of the picture and the return from the model function
				#which is inside load_animal as value
				f=load_animal.model(input_image[y:y+h, x:x+w],amod_path)
				if(f!="invalid"):
					animal_dic[animal_location[i]]=f
		
				#cx , cy are the x coordinate,y coordinate of the centre if the coutour respectivly
				#as the text started from the centre to make it more centrlised 0.0095*resolution and 0.038*resolution is subtracted from x&y coordinate respectivly
				#the 2 constants 0.0095 and 0.038 was used to adjust the position with change in resolution 
				cx = int((x+w/2)-(0.0095*input_image.shape[0]))
				cy = int((y+h/2)-(0.038*input_image.shape[0]))
		
				#the bellow function call is to write the text inside the contour (the font size was optimised in a similar method to the position optimisation)
				cv2.putText(input_image,animal_location[i],(cx,cy), font, input_image.shape[0]*0.0005,(0,0,0),1,cv2.LINE_AA)
		
				#the bellow function call is for drawing the rectangle around the countour in blue coulour
				cv2.rectangle(input_image,(x,y),(x+w,y+h),(255,0,0),int(input_image.shape[0]*0.0025))
		i=i+1

	'''
	this loop finds the non vacent countors where habitat where expected

	'''	
	while(habitat_contour[j]!=-1):
		if(hierarchy[0][habitat_contour[j]+1][2]!=-1 ):
		
			#the bellow while loop,conditons and the varibels are for selecting the correct countor from inside the box
			p=habitat_contour[j]+2
			g=p
			while(p<habitat_contour[j+1]-1 and p!=n ):
				if(cv2.contourArea(contours[g])<cv2.contourArea(contours[p])):
					g=p
				p=p+1
			if(cv2.contourArea(contours[g])<cv2.contourArea(contours[animal_contour[i]+2])/2.2):
				g=habitat_contour[j]+1
		
			# x,y,w,h are varibles to store the return values of boundingRect() where x,y,w,h are the x coordinate ,y coordinate ,width,hight respectivly of the rectangle
			x,y,w,h = cv2.boundingRect(contours[g])
		
			#here the dictnory is added with a new element with key as the position of the picture and the return from the model function
			#which is inside load_habitat as value
			f=load_habitat.model(input_image[y:y+h, x:x+w],hmod_path)
			if(f!="invalid"):
				habitat_dic[habitat_location[j]]=f
		
			#cx , cy are the x coordinate,y coordinate of the centre if the coutour respectivly
			#as the text started from the centre to make it more centrlised 0.0095*resolution is subtracted from x coordinate
			#the constants 0.0095 was used to adjust the position with change in resolution
			cx = int((x+w/2)-(0.0095*input_image.shape[0]))
			cy = int(y+h/2)
		
			#the bellow function call is to write the text inside the contour (the font size was optimised in a similar method to the position optimisation)
			cv2.putText(input_image,habitat_location[j],(cx,cy), font, input_image.shape[0]*0.0005,(0,0,0),1,cv2.LINE_AA)
		
			#the bellow function call is for drawing the rectangle around the countour in green coulour
			cv2.rectangle(input_image,(x,y),(x+w,y+h),(0,255,0),int(input_image.shape[0]*0.0025))
		j=j+1

	if(flag1):
		cv2.imwrite(output_location,input_image) # it saves the output image
	return habitat_dic,animal_dic,input_image
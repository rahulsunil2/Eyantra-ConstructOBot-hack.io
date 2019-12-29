'''* Team Id : #1850
* Author List : Bharath.T.U , Abhijeet C ,Gautham Rajesh , Joyal T John
* Filename: load_habitat.py
* Theme: Homecoming
* Functions: model
* Global Variables: none since there is no functions
'''

'''
Variable Comments
train_on_gpu  :  to check if device have  gpu or not
device        :  devioce we use (GPU/CPU)
classes       :  list of classes  
data_transform:  image transformation according to model
img           :  the argument that is passed
model         :  the model we used for transfer learning
img_loader    : to transform the img we get
prediction    : the prediction of class number
'''

import torchvision.models as models
import cv2
from PIL import Image
from torch.autograd import Variable
import torch
import torch.nn as nn
import numpy 
from torchvision import  transforms
import os
import copy

""" 
    * Function Name: model()
    * Input: im,model_path="habitat.pth"
    * Output: classes[prediction]
    * Logic: this function helps to predict the animal 
    * Example Call: model(im,model_path="habby.pth")
"""

def model(im,model_path="habitat.pth"):
	train_on_gpu =torch.cuda.is_available()
	device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")
	classes = ['baseball', 'basketball court', 'beach', 'circular farm', 'cloud','commercial area','dense residential','desert','forest','golf course','harbor','island','lake','meadow','medium residential area','mountain','rectangular farm','river','sea glacier','shrubs','snowberg','sparse residential area','thermal power station','wetland']
	im=cv2.cvtColor(im,cv2.COLOR_BGR2RGB)
	data_transform = transforms.Compose([transforms.RandomResizedCrop(224),transforms.ToTensor(),transforms.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225])])
	im=Image.fromarray(im)
	model=models.resnet101()
	n_inputs = model.fc.in_features
	model.fc = nn.Linear(n_inputs, len(classes))
	if train_on_gpu:
		model.cuda()
	lastpred=-1
	#this loop is experimental one we used to check if the prediction is correct or not by predicting again and again
	#but we found that even though the prediction that we use will all be correct on 50% of the prediction will be there
	#and it was more logical to use predictions directly without checking if they are correct or not as only 1 or 2 or 
	#even somtimes none where wrong
	#now as the loop only loops once the checking is not done
	for i in range(1):
		checkpoint = torch.load(model_path,map_location=device)
		model.load_state_dict(checkpoint)
		model.eval()
		img = data_transform(im).unsqueeze(0)
		img = Variable(img)
		prediction = model(img)
		prediction = prediction.data.numpy().argmax()
		if(prediction!=lastpred and lastpred!=-1):
			return "invalid"
		lastpred=prediction
	
	return	classes[prediction]


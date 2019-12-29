'''* Team Id : #1850
* Author List : Bharth.T.U , Abhijeet C ,Gautham Rajesh , Joyal T John
* Filename: train_animal.py
* Theme: Homecoming
* Functions: 1
* Global Variables: none since there is no functions
'''

'''
Variable Comments

    data_dir     :    path directory to the dataset.
    batch_size   :    how many samples per batch to load.
    augment      :    whether to apply the data augmentation scheme mentioned in the paper. Only applied on the train split.
    random_seed  :    fix seed for reproducibility.
    test_size    :    percentage split of the training set used for the test set. Should be a float in the range [0, 1].
    shuffle      :    whether to shuffle the train/test indices.
    num_workers  :    number of subprocesses to use when loading the dataset.
    pin_memory   :    whether to copy tensors into CUDA pinned memory. Set it to
    train_loader :    training set iterator.
    test_loader  :    test set iterator.
    classes      :    this defines the classes of dataset
    model_name   :    this variable is used for defining model name we gonna use
    criterion    :    loss function
    optimizer    :    optimizer 
    n_epochs     :    number of epochs to train the model
    test_loss    :    to find test loss
    Path         :    to specify saving of current model
'''
import torch
import numpy as np
import torch.nn as nn
import torch.optim as optim
from torch.optim import lr_scheduler
import torchvision
import PIL
from torchvision import datasets, models, transforms
import time
import os
import copy
from torch.utils.data.sampler import SubsetRandomSampler

""" 
    * Function Name: get_train_test_loader()
    * Input: data_dir,batch_size,augment,random_seed,test_size=0.1,shuffle=True,num_workers=4,pin_memory=False
    * Output: train_loader, test_loader
    * Logic: this function split data set into training and testing set 
    * Example Call: train_loader,test_loader = get_train_test_loader(data_dir,batch_size,augment,random_seed,test_size,True,num_workers,train_on_gpu)
"""

def get_train_test_loader(data_dir,batch_size,augment,random_seed,test_size=0.1,shuffle=True,num_workers=4,pin_memory=False):
	error_msg = "[!] test_size should be in the range [0, 1]."
	assert ((test_size >= 0) and (test_size <= 1)), error_msg
	
	# define transforms
	test_transform = transforms.Compose([transforms.RandomResizedCrop(224),transforms.ToTensor(),transforms.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225])])
	if augment:
		train_transform =torchvision.transforms.Compose([transforms.RandomResizedCrop(224),torchvision.transforms.RandomHorizontalFlip(),transforms.ToTensor(),transforms.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225])])
	else:
		train_transform = transforms.Compose([transforms.RandomResizedCrop(224),transforms.ToTensor(),transforms.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225])])
	# load the dataset
	train_dataset = torchvision.datasets.ImageFolder(data_dir, transform=train_transform,)

	test_dataset = torchvision.datasets.ImageFolder(data_dir, transform=test_transform,)

	num_train = len(train_dataset)
	indices = list(range(num_train))
	split = int(np.floor(test_size * num_train))
	#suffeling the indices of data if true
	if shuffle:
		np.random.seed(random_seed)
		np.random.shuffle(indices)
	train_idx, test_idx = indices[split:], indices[:split]
	train_sampler = SubsetRandomSampler(train_idx)
	valid_sampler = SubsetRandomSampler(test_idx)
	#transforming the images 
	train_loader = torch.utils.data.DataLoader(train_dataset, batch_size=batch_size, sampler=train_sampler,num_workers=num_workers, pin_memory=pin_memory,)
	test_loader = torch.utils.data.DataLoader(test_dataset, batch_size=batch_size, sampler=test_sampler,num_workers=num_workers, pin_memory=pin_memory,)
	# print out some data stats
	print('Num of classes: ', len(classes))
	print('Num training images: ', len(train_sampler))
	print('Num test images: ', len(test_sampler))
	return (train_loader, test_loader)

# this is to find if gpu is available or not
train_on_gpu = torch.cuda.is_available()
device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")
if not train_on_gpu:
	print('CUDA is not available.  Training on CPU ...')
else:
	print('CUDA is available!  Training on GPU ...')

#data_dir = for path of the dataset
data_dir = 'Animals Dataset'
# the classes are defined
classes = ['arctic fox', 'bear', 'bee','butterfly','cat','cougar','cow','coyote','crab','crocodile','deer','dog','eagle','elephant','fish','frog','giraffe','goat','hippo','horse','kangaroo','lion','monkey','otter','panda','parrot','penguin','raccoon','rat','seal','shark','sheep','skunk','snake','snow leopard','tiger','yak','zebra']

# define dataloader parameters
batch_size = 30
num_workers = 2
augment=1
random_seed=42
test_size=0.1
# prepare data loaders
train_loader,test_loader = get_train_test_loader(data_dir,batch_size,augment,random_seed,test_size,True,num_workers,train_on_gpu)

# Get a batch of training data
inputs, labels = next(iter(train_loader))

# Make a grid from batch
out = torchvision.utils.make_grid(inputs)

# Select a model [ 'VGG16', ResNet18', 'ResNet152' ]
model_name = 'ResNet152'
# number of epochs to train the model
n_epochs = 4
# Load the pretrained model from pytorch
model = None

"""
  This if else ladder is for using multiple model at ease , just by mentioning thw model name , it uses that model for transfer learning
  here we used vgg16,resnet(18,50,101,152)and we came to concluson that resnet 101 is best model for this dataset
"""
if model_name == 'VGG16':
	model = models.vgg16(pretrained=True)
	# Freeze training for all "features" layers
	for param in model.features.parameters():
		param.requires_grad = False
	n_inputs = model.classifier[6].in_features
	# new layers automatically have requires_grad = True
	model.classifier[6] = nn.Linear(n_inputs, len(classes))

elif model_name == 'ResNet18':
	model = models.resnet18(pretrained=True)
	n_inputs = model.fc.in_features
	model.fc = nn.Linear(n_inputs, len(classes))

elif model_name == 'ResNet152':
	model = models.resnet152(pretrained=True)
	n_inputs = model.fc.in_features
	model.fc = nn.Linear(n_inputs, len(classes))

elif model_name == 'ResNet50':
	model = models.resnet50(pretrained=True)
	n_inputs = model.fc.in_features
	model.fc = nn.Linear(n_inputs, len(classes))
elif model_name == 'ResNet101':
	model = models.resnet101(pretrained=True)
	n_inputs = model.fc.in_features
	model.fc = nn.Linear(n_inputs, len(classes))


# if GPU is available, move the model to GPU
if train_on_gpu:
	model.cuda()

# specify loss function (categorical cross-entropy)
criterion = nn.CrossEntropyLoss()

# specify optimizer (stochastic gradient descent) and learning rate = 0.001
try:
	optimizer = optim.SGD(model.classifier.parameters(), lr=0.001, momentum=0.9)
	print("VGG Loaded")
except:
	print("ResNet Loaded")
	optimizer = optim.SGD(model.parameters(), lr=0.001, momentum=0.9)
###################
# train the model #
###################

for epoch in range(n_epochs):

	# keep track of training and test loss
	train_loss = 0.0  

	# model by default is set to train
	for batch_i, (data, target) in enumerate(train_loader):
		# move tensors to GPU if CUDA is available
		if train_on_gpu:
			data, target = data.cuda(), target.cuda()
		# clear the gradients of all optimized variables
		optimizer.zero_grad()
		# forward pass: compute predicted outputs by passing inputs to the model
		output = model(data)
		# calculate the batch loss
		loss = criterion(output, target)
		# backward pass: compute gradient of the loss with respect to model parameters
		loss.backward()
		# perform a single optimization step (parameter update)
		optimizer.step()
		# update training loss 
		train_loss += loss.item()
		if batch_i % 30 == 29:    # print training loss every specified number of mini-batches
			print('Epoch %d, Batch %d loss: %.16f' %(epoch+1, batch_i + 1, train_loss / 30))
			train_loss = 0.0
# track test loss 
# over classes
test_loss = 0.0
class_correct = list(0. for i in range(len(classes)))
class_total = list(0. for i in range(len(classes)))

model.eval() # eval mode

# iterate over test data
for data, target in test_loader:
	# move tensors to GPU if CUDA is available
	if train_on_gpu:
		data, target = data.cuda(), target.cuda()
	# forward pass: compute predicted outputs by passing inputs to the model
	output = model(data)
	# calculate the batch loss
	loss = criterion(output, target)
	# update  test loss 
	test_loss += loss.item()*data.size(0)
	# convert output probabilities to predicted class
	_, pred = torch.max(output, 1)    
	# compare predictions to true label
	correct_tensor = pred.eq(target.data.view_as(pred))
	correct = np.squeeze(correct_tensor.numpy()) if not train_on_gpu else np.squeeze(correct_tensor.cpu().numpy())
	# calculate test accuracy for each object class
	for i in range(len(target)):
		label = target.data[i]
		class_correct[label] += correct[i].item()
		class_total[label] += 1

# calculate avg test loss
test_loss = test_loss/len(test_loader.dataset)
print('Test Loss: {:.6f}\n'.format(test_loss))
for i in range(len(classes)):
	if class_total[i] > 0:
		print('Test Accuracy of %5s: %2d%% (%2d/%2d)' % (classes[i], 100 * class_correct[i] / class_total[i],np.sum(class_correct[i]), np.sum(class_total[i])))
	else:
		print('Test Accuracy of %5s: N/A (no training examples)' % (classes[i]))

print('\nTest Accuracy (Overall): %2d%% (%2d/%2d)' % (100. * np.sum(class_correct) / np.sum(class_total),np.sum(class_correct), np.sum(class_total)))
#if you are saving to ur google drive give the below path 
#path = "/content/drive/My Drive/hab.pth"
path = "animal.pth"

# save model
torch.save(model.state_dict(), path)

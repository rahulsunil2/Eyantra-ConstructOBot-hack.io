
'''
* Team Id : #1850
* Author List : Bharth.T.U , Abhijeet C ,Gautham Rajesh , Joyal T John
* Filename: habitat_accuracy.py
* Theme: Homecoming
* Functions: none
* Global Variables: none since there is no functions
'''

'''
Variable Comments

    batch_size   :    how many samples per batch to load.
    augment      :    whether to apply the data augmentation scheme mentioned in the paper. Only applied on the train split.
    random_seed  :    fix seed for reproducibility.
    test_size    :    percentage split of the training set used for the test set. Should be a float in the range [0, 1].
    shuffle      :    whether to shuffle the train/test indices.
    num_workers  :    number of subprocesses to use when loading the dataset.
    pin_memory   :    whether to copy tensors into CUDA pinned memory. Set it to
    test_loader  :    test set iterator.
    classes      :    this defines the classes of dataset
    model_name   :    this variable is used for defining model name we gonna use
    criterion    :    loss function
    optimizer    :    optimizer 
    test_loss    :    to find test loss
   
'''
import torchvision.models as models
from torch.autograd import Variable
import torch
import torch.nn as nn
import torch.optim as optim
from torch.optim import lr_scheduler
import numpy as np
import torchvision
from torchvision import datasets, models, transforms
import time
import os
import copy
from torch.utils.data.sampler import SubsetRandomSampler

#to check if it run in CPU OR GPU
test_on_gpu = torch.cuda.is_available()
device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")
if not test_on_gpu:
    print('CUDA is not available.  Testing on CPU ...')
else:
    print('CUDA is available!  Testing on GPU ...')
test_dir='Habitats Dataset'
classes = ['baseball', 'basketball court', 'beach', 'circular farm', 'cloud','commercial area','dense residential','desert','forest','golf course','harbor','island','lake','meadow','medium residential area','mountain','rectangular farm','river','sea glacier','shrubs','snowberg','sparse residential area','thermal power station','wetland']
data_transform = transforms.Compose([transforms.RandomResizedCrop(224),transforms.ToTensor(),transforms.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225])])
test_data = datasets.ImageFolder(test_dir, transform=data_transform)
batch_size = 20
num_workers = 0
random_seed=42
valid_size=0.1

num_test = len(test_data)
indices = list(range(num_test))
split = int(np.floor(valid_size * num_test))

np.random.seed(random_seed)
np.random.shuffle(indices)

test_idx = indices[:split]
test_sampler = SubsetRandomSampler(test_idx)

test_loader = torch.utils.data.DataLoader(test_data, batch_size=batch_size, sampler=test_sampler,num_workers=num_workers, pin_memory=test_on_gpu,)
    # print out some data stats
print('Num of classes: ', len(classes))
print('Num test images: ', len(test_sampler))
# define dataloader parameters
MODEL_PATH='habitat.pth'
# prepare data loaders
model=models.resnet101()
n_inputs = model.fc.in_features
model.fc = nn.Linear(n_inputs, len(classes))
print("ResNet Loaded")
if test_on_gpu:
    model.cuda()
optimizer = optim.SGD(model.parameters(), lr=0.001, momentum=0.9)
criterion = nn.CrossEntropyLoss()
checkpoint = torch.load(MODEL_PATH,map_location=device)
model.load_state_dict(checkpoint)
model.eval()
test_loss = 0.0
class_correct = list(0. for i in range(len(classes)))
class_total = list(0. for i in range(len(classes)))

model.eval() # eval mode

# iterate over test data
for data, target in test_loader:
    # move tensors to GPU if CUDA is available
    if test_on_gpu:
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
    correct = np.squeeze(correct_tensor.numpy()) if not test_on_gpu else np.squeeze(correct_tensor.cpu().numpy())
    # calculate test accuracy for each object class
    if(len(target)<batch_size):
        break
    for i in range(batch_size):
        label = target.data[i]
        class_correct[label] += correct[i].item()
        class_total[label] += 1

# calculate avg test loss
test_loss = test_loss/len(test_loader.dataset)
print('Test Loss: {:.6f}\n'.format(test_loss))
for i in range(len(classes)):
    if class_total[i] > 0:
        print('Test Accuracy of %5s: %2d%% (%2d/%2d)' % (
            classes[i], 100 * class_correct[i] / class_total[i],
            np.sum(class_correct[i]), np.sum(class_total[i])))
    else:
        print('Test Accuracy of %5s: N/A (no training examples)' % (classes[i]))

#the final test accuracy of the model 
print('\nTest Accuracy (Overall): %2d%% (%2d/%2d)' % (
    100. * np.sum(class_correct) / np.sum(class_total),
    np.sum(class_correct), np.sum(class_total)))
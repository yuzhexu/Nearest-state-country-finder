# Nearest-state-country-finder
EC504 class project Nearest state/country finder
## Getting Started
Run `make` to complie the code
Run `make run` will test the sample input for the code
or `./load xxxx.txt`
## Input and output
Input txt to tesing the code is under input folder</br>
stracture of input file should be:
```
latitude
longitude
k
```
All output will show in consle and write in txt file under output folder
## Features
#### Load Reference Points
loads reference points (including latitude, longitude, state, and county information) 
into the KD-tree from a file or other source.

#### Query the nearest K reference points
Allow the user to enter a coordinate (latitude and longitude) 
and return the nearest K reference points, where K is a number between 1 and 10.

#### Majority Vote for States and Counties
For a given reference point, determine the state 
and county by calculating the majority vote for the 5 nearest points.

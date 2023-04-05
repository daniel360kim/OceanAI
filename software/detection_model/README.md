
# Trash Detection Model
YOLOv5 based CNN used to inference footage from the AUV for trash. This directory includes the trained model for inference, but also contains the code used to train the model. All instructions can be found below.

## Getting Started

1. Clone the repository
```sh
git clone https://github.com/daniel360kim/OceanAI.git
```
2. Go to the `software/detection_model` directory
```sh
cd software/detection_model
```
3. Install the required packages
```sh
pip install -r requirements.txt
```

## Training
Training can be done in two ways: locally or on Google Colab. The Google Colab notebook is recommended for training on a GPU. 
<br></br>
The dataset used for this project can be found in [this public Roboflow project](https://universe.roboflow.com/oceanai-ke0bh/twoclasstrash)

Rest of the instructions can be found within [the notebook](https://github.com/daniel360kim/OceanAI/blob/master/software/detection_model/Training.ipynb) 
*This notebook was not written by me, but I have modified it to work with the dataset used in this project*

## Detection
detect.py runs the inference on a variety of different sources. 
```sh
python detect.py --source <source> --weights <weights> --conf <confidence threshold> --iou <iou threshold>
```
detect.py will run the inferencing. Note that a directory can be passed as the source, and the model will run inference on all image/videos in the directory.

*Note that most of the detect.py code was taken from the [YOLOv5 examples repository](https://github.com/ultralytics/yolov5)*

Best.pt is the best model I trained. You can use this model to run trash detection without training on your own. 
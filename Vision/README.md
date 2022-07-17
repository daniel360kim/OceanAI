# Vision
This notebook demonstrates how to train custom datasets collected from the OceanAI submarine.

## Collect data from the submarine
* After running the program, the board will save all collected images onto the onboard SD card.
* Under the directory "Images" you can copy and paste into a new folder.

## Prepare images
### Use Roboflow to upload, annotate, and prepare data in YOLO format.
* Roboflow annotate is a simple way to manage and label your image.
* Upload to a ```public``` workspace, annotate images, then generate.
* Under Preprocessing, resize to 640x640. 
* YOLOv5 automatically augments during training so do not apply any augmentation steps.
* Now generate in YOLOv5 PyTorch format.
* Copy and paste the Jupyter code snippet.

![image](resources/code_snippet.png)

## Training
* Open the [notebook](Vision.ipynb) and begin executing the code.
* At snippet 4, replace the code with the generated Jupyter script from Roboflow.
* Begin training!

## Final Notes
* Finally, you should have a fully trained YOLOv5 model to run computer vision algorithms on.
* Google Colab is FREE! Try this out!
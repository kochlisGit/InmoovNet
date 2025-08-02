# InmoovNet
This repository contains the code and the 3d files of the work: *Learning to InMoov: A Deep Learning Approach to Modeling Human Hand*, presented in ECAI 2025. An overview of the model architecture is presented below.

![InmoovNet Overview](https://github.com/kochlisGit/InmoovNet/blob/main/images/inmoovoverview.png)

# Features

1. Automatic Hardware Calibration (You only need the exoskeleton and a laptop :) )
2. Vision-based Teleoperation (You can command it via your laptop's webcam)
3. Error-Correction, in case any hardware inaccuracies occur (You can place any motors you like, and it will still work fine)
4. Control via Natural Language (You can literate command it via speech)

# Description

Have you gotten tired of exhaustive coding in C language for simple tasks? Then, InmoovNet is what you are looking for. This AI Controller enables the user to teach human-scale robotic hands several tasks with minimal effort. 

In this work, we propose a speech-driven controller for human-sized robotic hands that translates natural language commands directly into actuator controls, removing the need for task-specific video data and enabling a more natural human–robot interaction. To achieve this, we first train a model to map observed human hand motion to actuator controls through a camera (*HPT Model*). This learned model forms the foundation for rapidly developing a library of motion primitives. Subsequently, we train an Autoregressive Stateful Neural Network to convert verbal instructions into sequences of those primitives, composing multi-step trajectory sequences (*NLT Model*). We validate our approach by integrating the controller into the open-source *InMoov Hand-i2* project [https://inmoov.fr/](https://inmoov.fr/). This work lays the groundwork for scalable, adaptable robotic controllers that can be rapidly fine-tuned to new tasks with minimal effort.

# Inmoov Hand

![https://github.com/kochlisGit/InmoovNet/blob/main/images/inmoovoverview.png](https://github.com/kochlisGit/InmoovNet/blob/main/images/inmoov.png)

# How to Build

1. 3D Printer (any 3D-Printer you like)
2. 2KG PLA (or any other material of your preference)
3. Springs 5x (one for each finger)
4. Pins x15 (three for each finger) 
5. Bushings x15 (three for each finger)
6. Mini servo motors x5 (one for each finger)
7. 2 Heavy load servo motors / stepper motors x2 (required to control the wrist)
8. Timing Belts x2 (attached to the wrist)
9. Arduino Nano ESP32 (required to control the actuators: servos or steppers)
10. Power supper 6V (required to power servo/steppers)
11. Power supply 5V for the Arduino Board. If you have Arduino UNO, you can use it to power the Nano ESP32.

# HPT Model

The HPT model is a Machine Learning model that learns to map 3D landmark coordinates $(x, y, z)$ to actuator outputs (0-160 degrees). However, typical vector-input ML models (such as the ones presented in scikit-learn [https://scikit-learn.org/stable/supervised_learning.html](https://scikit-learn.org/stable/supervised_learning.html)) treat each landmark independently and ignore the natural correlations within groups of landmarks (e.g., those belonging to the same finger). To address this, we introduce the Grouped-Landmarks Convolutional Neural Network (GL-CNN), a Deep Neural Network architecture that explicitly exploits these correlations, achieving superior accuracy. GL-CNN partitions the $N$ 3D landmarks (In our case, N=20 joints) into five groups of  related joints $\mathbf{G}\in\mathbb{R}^{5\times4\times3}$, with each group forming a $4 \times 3$ matrix (rows=landmarks, columns=(x,y,z)). A shared 2D convolutional layer is then applied independently to each group, producing per-group feature maps that capture spatial patterns. Finally, all group feature maps are flattened and concatenated into a single vector $\mathbf{z} \in \mathbb{R}^{5 \cdot C_f}$, which is passed into a fully connected network that predicts the actuator controls $\hat{\mathbf{a}}$. During training, we also inject a small amount of Gaussian noise $\boldsymbol{\epsilon} \sim \mathcal{N}(\mu=0, \sigma^2=0.005)$ into the landmark positions, such that $\tilde{\mathbf{L}} = \mathbf{L} + \boldsymbol{\epsilon}$, to boost accuracy and reduce overfitting during training.

![InmoovNet Overview](https://github.com/kochlisGit/InmoovNet/blob/main/images/glcnn.png)

# NLT Model

The NLT module is designed to convert verbal commands into trajectory sequences by composing motion primitives. Its implementation consists of four consecutive steps, as described below.

![https://github.com/kochlisGit/InmoovNet/blob/main/images/nlt.png](https://github.com/kochlisGit/InmoovNet/blob/main/images/nlt.png)

![https://github.com/kochlisGit/InmoovNet/blob/main/images/deployment.png](https://github.com/kochlisGit/InmoovNet/blob/main/images/deployment.png)

# Stateful Neural Networks



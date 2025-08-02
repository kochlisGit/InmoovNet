# InmoovNet
This repository contains the code and the 3d files of the work: *Learning to InMoov: A Deep Learning Approach to Modeling Human Hand*, presented in European Conference on Artificial Intelligence (ECAI) 2025. An overview of the model architecture is presented below.

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

Stateful Neural Networks (SFNNs) are specific types of Neural Networks that have the ability to store and re-use information from prior inputs, in order to generate future predictions. In contrast to Stateless Neural Networks, where each input is independent, SFNNs consider temporal patterns and contextual history by maintaining an internal hidden state. This hidden state is a vector of activated outputs that is iteratively updated at each timestep, depending on the input data and the preceding hidden state. Two popular SFNNs included in this work are LSTMs and GTrXL. 

## Long Short-Term Memory (LSTM) Networks

Long Short-Term Memory (LSTM) networks are a specialized type of Recurrent Neural Networks designed to learn long-term dependencies in sequential data by maintaining a persistent hidden state, denoted as $h_{t}$, across time steps. An LSTM unit typically comprises a memory cell and three gates: the input, forget, and output gates, which regulate the addition, removal, and exposure of information in the cell state, denoted as $c_{t}$. The input gate determines how much new information flows into the cell, while the forget gate decides what information to discard from the previous state. Finally, the output gate controls the information passed to the next layer or as output. 

![https://github.com/kochlisGit/InmoovNet/blob/main/images/lstm.png](https://github.com/kochlisGit/InmoovNet/blob/main/images/lstm.png)

A typical LSTM hidden state is initialized to zeros or random values around zero. In timeseries forecasting applications, a stateful LSTM retains its hidden and cell states across each input, meaning that the final state after processing one minibatch is used as the initial state for the next one, so the model can learn dependencies that span beyond a single batch of timesteps. In contrast, a stateless LSTM resets its states at the start of each input, effectively treating each input as an independent sequence. This reset makes training simpler and more stable (since gradients don’t propagate indefinitely), but at the cost of losing long‐range context. 

## Gated Transformer-XL (GTrXL)

Gated Transformer-XL (GTrXL) is a variant of Transformer-XL (TrXL), mainly for Deep Reinforcement Learning settings, to improve training stability and sample efficiency in partially observable environments. GTrXL's architecture is designed to retain the segment-level recurrence and relative positional encodings by re-ordering layer normalization and replacing standard residual connections with GRU-style gating layers that adaptively add transformed and skipped inputs. This design not only preserves long-range dependencies via a “memory” of hidden states carried across timesteps, but also resolves gradient instability. Usually, GTrXL outperforms LSTMs largely because its self‐attention backbone. Gated Transformer-XL (GTrXL) can be leveraged to model human hand controllers by treating manipulation as a sequential decision-making task, where each task is modeled as a long-horizon dependency chain. Its GRU-style gating in residual connections and recurrence enable the network to maintain and update a persistent “actuator memory” across extended sequences of actuator controls, effectively stabilizing training and capturing the spatio-temporal correlations between successive commands.

![https://github.com/kochlisGit/InmoovNet/blob/main/images/gtrxl.png](https://github.com/kochlisGit/InmoovNet/blob/main/images/gtrxl.png)

# Inmoov Hand i2 - Modeling Tricks 

Inmoov is an open-source, fully 3D-printable humanoid robot project. It is designed to run on readily available components, such as Arduino boards for control, standard hobby servos for articulation, and open-source software. Particularly InMoov Hand, it was initiated as the first Open-Source prosthetic hand, and since then has lead to projects like Bionico, E-Nable, and many others. Recently, Inmoov's hand has been upgraded to Inmoov Hand-i2, which is a more human-like hand and uses more advanced mechanisms to move its fingers. Despite the upgrades, it still has some minor design flows, which were addressed in our work. First, the palm retains a rectangular shape, which although, it does not affect its kinematics, it causes hand-landmark detection models to detect its presence. Second, the fingers’ range of motion remains constrained, preventing full contact with the palm. As an alternative to reshaping the palm, we propose improving hand‐landmarker accuracy by using a glove. Specifically, before running the HPT module’s automatic calibration, users can wear a glove, as shown in the Figure below. This simple “glove trick” seems to significantly increase the hand landmarker’s detection rate, ensuring more reliable landmark extraction.

![https://github.com/kochlisGit/InmoovNet/blob/main/images/glove.png](https://github.com/kochlisGit/InmoovNet/blob/main/images/glove.png)

# Hand Landmarker

The Hand Landmarker, developed by mediapipe, is a real-time hand landmark detection and tracking tool. It employs a dedicated two-stage approach, where it first detects a palm to define an oriented bounding box and then calculates 21 high-fidelity 3D landmark coordinates within the bounding box. The model outputs normalized (x, y) positions for each keypoint, relative depth estimates, a confidence score for hand presence to robustly manage tracking failures, and a binary handedness prediction to distinguish left from right hands. Although many hand landmark detection models exist, it eliminates the need for expensive data augmentations (e.g., rotations and scalings), achieving real-time inference on mobile GPUs. To achieve this, it uses a regression network exclusively on the cropped palm region, defined by the bounding box, while also caching the previous frame’s landmarks, allowing accurate and fast landmark tracking. 

![https://github.com/kochlisGit/InmoovNet/blob/main/images/handlandmarker.png](https://github.com/kochlisGit/InmoovNet/blob/main/images/handlandmarker.png)

# Whisper

Whisper is an open-source, general-purpose speech recognition and translation model (Figure \ref{Figure:Whisper}), developed by OpenAI. It is designed as an end-to-end encoder–decoder Transformer that processes audio chunks converted into log-Mel spectrograms, which it utilizes to generate text captions, intermixed with special tokens for tasks like language identification and timestamps. It is trained on 680 000 hours of multilingual and multitask data, including both English and non-English audio. Whisper achieves high-quality transcription across over 90 languages, but also on-the-fly speech-to-text translation, making it versatile for many applications. Although Whisper achieves robust zero-shot performance on diverse speech data, its architecture is built around processing fixed 30-second log-Mel spectrogram segments, padding or trimming inputs to that length for stable inference. In practice, attempts to transcribe snippets shorter than 5 seconds often yield degraded accuracy, so we adjust the verbal descriptions provided to InmoovNet, so that at least a 5-second audio chunk is generated.

# Code

The project introduces a modular deep learning system that enables natural language control of a human-sized robotic hand. It consists of two main components:

1. Human Pose Translator (HPT)}: Maps observed human hand poses to actuator commands.
2. Natural Language Translator (NLT)}: Converts verbal commands into motion primitive sequences.
    
Each component includes both the Python-based model logic and its corresponding Arduino interface code for communication with the robotic hardware. The collected dataset are also included in the provided zip file. Most of the scripts are ready-to-run and require only the installation of the specified libraries.

# Script Description


* `code/HPT/arduino_python_handlandmarker_workflow.ipynb`: Automatically creates the dataset used to train the controller. It combines Google's HandLandmark model with live communication to the robotic hand via Arduino, capturing paired 3D hand pose coordinates and motor angles (It requires the hand's skeleton).
* `code/HPT/models_training.ipynb`: Preprocesses the collected dataset and explores various model architectures to identify the optimal neural network structure for the pose-to-actuator mapping task through systematic hyperparameter tuning.
* `code/HPT/arm_controlle.ipynb`: Performs real-time translation of human hand poses into robotic hand actuation. Utilizes the best-performing trained model from the previous step and communicates with the robotic hand via serial port.
* `code/HPT/Arduino/HTTPServer/HTTPServer.ino`: Implements an HTTP server on the Arduino to receive and log actuator angles during dataset collection. Used in conjunction with `arduino_python_handlandmarker_workflow.ipynb`.
* `code/HPT/Arduino/serial_servos_reading/serial_servos_reading.ino`: Continuously reads motor angle data from the serial port and updates the robotic hand's actuators accordingly. Works in conjunction with `arm_controller.ipynb` for live hand pose reproduction.
* `code/NLT/preprocess_dataset.ipynb`: Processes the raw dataset of natural language commands paired with actuator sequences. Adds a sentence embedding column using the language model described in the paper to prepare the data for training.
* `code/NLT/training_stateful_hyperparameter_tuning.ipynb`: Trains multiple models to translate textual commands into actuator trajectories. Performs extensive hyperparameter tuning and statistical evaluation across different dataset splits. The best-performing model is saved for inference.
* `code/NLT/arm_controlle.ipynb`: Converts spoken commands to text and uses the trained model to generate actuator sequences in real time. Communicates with the robotic hand over a serial connection for live execution of the control instructions.
* `code/NLT/audio_control_serial_reading.ipynb`:  Listens for actuator sequences sent over the serial port and transmits them to the robotic hand's motors. Works in conjunction with \textbf{nlt/arm\_controller.ipynb} for real-time, speech-driven control.

# Libraries & Execution Instructions

The code is written in Python 3.10 interpreter. Although other Python versions can be supported, it is recommended that you install the recommended versions. Additionally, the Arduino IDE is required to run the arduino code. After the compiler/interpreter installations, you will need to download the following libraries:

* `tensorflow` It is used to build the GL-CNN model.
* `torch`: It is used to build the ASNN.
* `scikit-learn`: It is used to build the regression models of the HPT module.
* `matplotlib`: It is used to plot train/eval losses after training.
* `pandas`: It is used to process, store and load the train/eval metrics.
* `notebook`: It is used to run the notebook (\textit{.ipynb}) files. 

Our implementation is provided as interactive notebook files to enable hands-on exploration. You can view and run these notebooks in any notebook-compatible code editor, such as Visual Studio Code or PyCharm. 

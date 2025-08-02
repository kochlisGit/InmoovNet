# Learning to InMoov: A Deep Learning Approach to Modeling Human Hand

This repository contains the implementation code for the paper:

**Learning to InMoov: A Deep Learning Approach to Modeling Human Hand**

The project introduces a modular deep learning system that enables natural language control of a human-sized robotic hand. It consists of two main components:

- **Human Pose Translator (HPT)**: Maps observed human hand poses to actuator commands.
- **Natural Language Translator (NLT)**: Converts verbal commands into motion primitive sequences.

Each component includes both the Python-based model logic and its corresponding Arduino interface code for communication with the robotic hardware.

---

## Repository Structure

### `HPT/` – Human Pose Translator
Contains Python scripts and Jupyter notebooks for converting observed hand poses (via camera) into actuator control signals. It includes dataset creation, model training, and real-time inference logic. Also includes an `Arduino/` subfolder with code to transmit these signals to the robotic hand.

#### Jupyter Notebooks

- **arduino_python_handlandmarker_workflow.ipynb**  
  Creates the dataset used to train the controller. It combines Google's HandLandmark model with live communication to the robotic hand via Arduino, capturing paired 3D hand pose coordinates and motor angles.

- **models_training.ipynb**  
  Preprocesses the collected dataset and explores various model architectures to identify the optimal neural network structure for the pose-to-actuator mapping task through systematic hyperparameter tuning.

- **arm_controller.ipynb**  
  Performs real-time translation of human hand poses into robotic hand actuation. Utilizes the best-performing trained model from the previous step and communicates with the robotic hand via serial port.

#### Arduino Code (`HPT/Arduino/`)

- **HTTPServer**  
  Implements an HTTP server on the Arduino to receive and log actuator angles during dataset collection. Used in conjunction with `arduino_python_handlandmarker_workflow.ipynb`.

- **serial_servos_reading**  
  Continuously reads motor angle data from the serial port and updates the robotic hand's actuators accordingly. Works in tandem with `arm_controller.ipynb` for live hand pose reproduction.


### `NLT/` – Natural Language Translator
Includes Python code for translating verbal instructions into sequences of actuator commands, enabling intuitive control of the robotic hand through speech. It includes dataset preprocessing, model training, and real-time inference. This folder also contains an `Arduino/` subfolder with the corresponding communication code for executing these motions on the robotic hand.

#### Jupyter Notebooks

- **preprocess_dataset.ipynb**  
  Processes the raw dataset of natural language commands paired with actuator sequences. Adds a sentence embedding column using the language model described in the paper to prepare the data for training.

- **training_stateful_hyperparameter_tuning.ipynb**  
  Trains multiple models to translate textual commands into actuator trajectories. Performs extensive hyperparameter tuning and statistical evaluation across different dataset splits. The best-performing model is saved for inference.

- **arm_controller.ipynb**  
  Converts spoken commands to text and uses the trained model to generate actuator sequences in real time. Communicates with the robotic hand over a serial connection for live execution of the control instructions.

#### Arduino Code (`NLT/Arduino/`)

- **audio_control_serial_reading**  
  Listens for actuator sequences sent over the serial port and transmits them to the robotic hand's motors. Works in conjunction with `arm_controller.ipynb` for real-time, speech-driven control.
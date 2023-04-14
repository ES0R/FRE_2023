import cv2
import os


# Creating model 
classifier_output_path = 'models'
model_name = "cascade.xml"

counter = 1

while os.path.exists(os.path.join(classifier_output_path, model_name)):
    model_name = f"{counter}_{model_name.split('_')[-1]}"
    counter += 1

output_file = os.path.join(classifier_output_path, model_name)

with open(output_file, "w") as f:
    f.write("")

print(f"Created file: {output_file}")

# Parameters for creating positive samples
num_samples = 500
sample_width = 24
sample_height = 24

# Parameters for training cascade classifier
num_stages = 10
min_sample_size = (10, 10)
max_sample_size = (200, 200)
min_hit_rate = 0.995
max_false_alarm_rate = 0.5
num_threads = 4



# Read in positive samples for deers

deer_sample_file = "deer_samples.txt"

with open(deer_samples_file, 'r') as f:
    deer_samples = f.read().splitlines()

# Create positive samples for deers
cmd = 'opencv_createsamples -info {} -num {} -w {} -h {} -vec deer_samples.vec'
cmd = cmd.format(deer_samples_file, num_samples, sample_width, sample_height)
os.system(cmd)


# Merge the positive sample vectors into one file
cmd = 'opencv_createsamples -vec samples.vec -w {} -h {}'
cmd = cmd.format(sample_width, sample_height)
cmd += ' -info {} -num {} -w {} -h {} -vec {}'
cmd = cmd.format(deer_samples_file, num_samples, sample_width, sample_height, 'deer_samples.vec')
os.system(cmd)

# Train the cascade classifier
cmd = 'opencv_traincascade -data output -vec samples.vec -bg {} -numPos {} -numNeg {} -numStages {} -minHitRate {} -maxFalseAlarmRate {} -w {} -h {} -minSize {} -maxSize {} -numThreads {}'
cmd = cmd.format(negative_samples_file, num_samples*3, num_samples, num_stages, min_hit_rate, max_false_alarm_rate, sample_width, sample_height, min_sample_size[0], max_sample_size[0], num_threads)
os.system(cmd)

# Save the trained classifier to file
classifier = cv2.CascadeClassifier(os.path.join(classifier_output_path, model_name))
classifier.save(classifier_output_path)





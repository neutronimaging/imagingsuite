import subprocess
import json
import sys

def load_batch(batch_file):
    """Load batch from json formatted file and return a dictionary"""
    with open(batch_file, 'r') as f:
        data = json.load(f)
    return data

def run_batch(batch_file,muhrec_path):

    for task in load_batch(batch_file):
        print("Processing project {0}".format(task['name']))

        call_info = [muhrec_path, "-f", task['config']]
        for arg in task['arguments']:
            call_info.append(f"\"{arg}={task['arguments'][arg]}\"")
        
        print("Calling muhrec with arguments:")
        for arg in call_info:
            print(arg)
        
        try:
            # result = subprocess.run(call_info, capture_output=True, text=True, check=True)
            result = subprocess.call(call_info)
            # print("Standard Output:\n", result.stdout)
            # print("Standard Error:\n", result.stderr)
        except subprocess.CalledProcessError as e:
            print("Error Occurred:", e)

        print("Project {0} completed\n-----------------------\n".format(task['name'])) 

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("Usage: python batch_runner.py batch_file.json muhrec_path")
        sys.exit(1)
    
    batch_file = sys.argv[1]
    muhrec_path = sys.argv[2]

    run_batch(batch_file=batch_file,muhrec_path=muhrec_path)


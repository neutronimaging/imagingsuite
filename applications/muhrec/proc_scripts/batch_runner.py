import subprocess
import json
import sys
import argparse

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
            result = subprocess.call(call_info)
        except subprocess.CalledProcessError as e:
            print("Error Occurred:", e)

        print("Project {0} completed\n-----------------------\n".format(task['name'])) 

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Runs a set of reconstruction batches using MuhRec.")
    parser.add_argument('-b','--batchfile', help="A json file with batch descriptions.")
    parser.add_argument('-m','--muhrecpath', help="Path to the muhrec executable.")

    args = parser.parse_args()

    run_batch(batch_file=args.batchfile,muhrec_path=args.muhrecpath)


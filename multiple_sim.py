import os
import subprocess
import shutil
import glob

agent_min=15
agent_range=30
num_simulations=50

root_path = './'
folderlist=[]
folders = ['logs_%s' %(x+agent_min) for x in range(agent_range)]
for folder in folders:
    folderlist.append(os.path.abspath(os.path.join(root_path,folder)))
    os.mkdir(os.path.join(root_path,folder))
    shutil.copy2('ascari.cfg',os.path.join(root_path,folder))
    shutil.copy2('routerwarehousemaxishort.yaml',os.path.join(root_path,folder))
    shutil.copy2('maxiwarehouse.lgf',os.path.join(root_path,folder))
    
for x in range(agent_range):
    os.chdir(folderlist[x])
    for i in range(num_simulations):
        subprocess.call(["./../../singleProcess","-n","%s" %(x+agent_min)])
        
for folder in folderlist:
    os.chdir(folder)
    print(folder)
    files=glob.glob('log_2014-*')
    print(files)
    subprocess.check_call(["./../../results"]+files+["-o","test.txt","-s","summary.txt"])
import os
import subprocess
import shutil

root_path = './'
folderlist=[]
folders = ['logs_%s' %(x+15) for x in range(30)]
for folder in folders:
    os.mkdir(os.path.join(root_path,folder))
    folderlist.append(os.path.abspath(os.path.join(root_path,folder)))
    shutil.copy2('ascari.cfg',os.path.join(root_path,folder))
    shutil.copy2('routerwarehousemaxishort.yaml',os.path.join(root_path,folder))
    shutil.copy2('maxiwarehouse.lgf',os.path.join(root_path,folder))
    
for x in range(30):
    os.chdir(folderlist[x])
    for i in range(100):
        subprocess.call(["./../../singleProcess","-n","%s" %(x+15)])
        #subprocess.call(["echo","-n %s" %(x+15)])
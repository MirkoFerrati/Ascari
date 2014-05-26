import os
import subprocess
import shutil
import glob
import progressbar
import time
from progressbar import AnimatedMarker, Bar, BouncingBar, Counter, ETA, \
 FileTransferSpeed, FormatLabel, Percentage, \
    ProgressBar, ReverseBar, RotatingMarker, \
    SimpleProgress, Timer

agent_min=15
agent_range=10
num_simulations=1

root_path = './'
folderlist=[]
folders = ['logs_%s' %(x+agent_min) for x in range(agent_range)]

print 'creating folders'

for folder in folders:
    folderlist.append(os.path.abspath(os.path.join(root_path,folder)))
    os.mkdir(os.path.join(root_path,folder))
    shutil.copy2('ascari.cfg',os.path.join(root_path,folder))
    shutil.copy2('routerwarehousemaxishort.yaml',os.path.join(root_path,folder))
    shutil.copy2('maxiwarehouse.lgf',os.path.join(root_path,folder))
    
print 'done'
print 'starting simulations'

bar = progressbar.ProgressBar(maxval=agent_range*num_simulations, \
 widgets = [Percentage(),' ', Bar(),' ', ETA()]    
)
bar.start()
for x in range(agent_range):
    os.chdir(folderlist[x])
    for i in range(num_simulations):
        subprocess.call(["./../../singleProcess","-n","%s" %(x+agent_min)])
	try:
		f = open("command.sh", "w")
    		try:
        		f.write("./../../singleProcess -n %s" %(x+agent_min)) # Write a string to a file
    		finally:
        		f.close()
	except IOError:
    		pass
    	bar.update(x*num_simulations+i+1)

bar.finish()
print 'done'
print 'creating summary files'

bar = progressbar.ProgressBar(maxval=len(folderlist), \
    widgets=[progressbar.Bar('=', '[', ']'), ' ', progressbar.Percentage()])
bar.start()        
i=0
for folder in folderlist:
    os.chdir(folder)
    print('\n')
    print(folder)
    files=glob.glob('log_2014-*')
    print(files)
    bar.update(i+1)
    i=i+1
    subprocess.check_call(["./../../results"]+files+["-o","test.txt","-s","summary.txt"])
bar.finish()
print 'done'

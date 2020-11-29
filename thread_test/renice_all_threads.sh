psname="main"
reniceval=-20
ps -A -L -o pid,lwp,pri,nice,cmd | grep -E "${psname}$"
ps -A -L -o lwp,cmd | grep -E "${psname}$"|sudo renice -n $reniceval `sed -n "s/^ *\([0-9][0-9]*\)  *.*$/\1/p"`
ps -A -L -o pid,lwp,pri,nice,cmd | grep -E "${psname}$"

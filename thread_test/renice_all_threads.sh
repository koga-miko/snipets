searchname="main$"
reniceval=-13

ps -A -L -o pid,lwp,pri,nice,cmd | grep -E "${searchname}"

ps -A -L -o lwp,nice,cmd | grep -E "${searchname}" | while read line
do
    cur_niceval=`echo $line | sed -n "s/^ *[-0-9][-0-9]*  *\([-0-9][-0-9]*\)  *.*$/\1/p"`
    thread_id=`echo $line | sed -n "s/^ *\([0-9][0-9]*\)  *.*$/\1/p"`
    echo renice -n $cur_niceval + $reniceval $thread_id
    sudo renice -n $(($cur_niceval + $reniceval)) $thread_id
done

ps -A -L -o pid,lwp,pri,nice,cmd | grep -E "${searchname}"

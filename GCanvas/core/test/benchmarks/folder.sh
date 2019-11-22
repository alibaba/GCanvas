  function read_dir(){
        name="" 
        for file in `ls $1`    
        do
            if [ -d $1"/"$file ]  
            then
                read_dir $1"/"$file
            else
                name=${name}"\""$file"\","  
            fi
        done
        echo "\"""$1"\" ":["$name"],"
    }  

read_dir $1
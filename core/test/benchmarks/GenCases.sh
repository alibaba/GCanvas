  str=""
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
        # echo "\"""$1"\" ":["$name"],"
        if [ "$1" != "." ]; then 
             str=$str"\""$1"\""":["${name}"],"      
        fi;
    }  

read_dir $1
echo "let caseNames = {"$str "};" | sed 's/\.\///g' 
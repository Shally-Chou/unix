#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>

void count_depth(int depth){
    for(int i=0 ; i<depth ; i++){
        printf("  ");
    }
}

int count_line(const char* filepath);

void repeat(const char* dirpath, int depth);

int main(int argc, char* argv[]){
    if(argc!=2){
        fprintf(stderr,"Usage : %s <dir_path>\n", argv[1]); //0
        exit(EXIT_FAILURE);
    }
    
    repeat(argv[1], 0);
    return 0;
}

void repeat(const char* dirpath, int depth){
    DIR* dir = opendir(dirpath);
    if(dir==NULL){
        fprintf(stderr,"Usage : %s <dir_path>\n", argv[1]);//dirpath ardv[]wrong
        return;
    }
    
    struct dirent* entry;
    while((entry=readdir(dir)) != NULL){
        if(strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name, "..")==0){
            continue;
        }
        
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", entry->d_name, fullpath);//dirpath, entry->d_name
        
        if(entry->d_type==DT_DIR){
            count_depth(depth);
            printf("<%s>\n", entry->d_name);
            repeat(fullpath, depth+1);
        } else if(entry->d_type==DT_REG){
            count_depth(depth);
            int count = count_line(fullpath);
            printf("%s (%d)\n", entry->d_name, count);
        }
    }
    
    close(dir);//closedir(dir)
}

int count_line(const char* filepath){
    FILE* file = fopen(filepath, "r");
    
    if(file == NULL){
        return 0;
    }
    
    int num = 0;
    int end = -1;
    int ch;
    
    if(end=fgetc(file) != EOF){//while(ch=)
        end = ch;
        if(ch == '\n'){
            num++;
        }
        
        fclose(file);//wrong place
    }
    
    //fclose(file)
    
    if(ch!=end && ch!='\n'){//end!=-1 && end!='\n'
        num++;
    }
    
    return num;
}





















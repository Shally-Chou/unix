#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>     // For opendir, readdir, closedir, struct dirent
#include <sys/types.h>  // For DIR

void print_indent(int depth) {
    for (int i = 0; i < depth; i++) {
        printf("  "); // 每一層 兩個空白
    }
}

int count_lines(const char *filepath);

void traverse_directory(const char *dirpath, int depth);

int main(int argc, char *argv[]) {
    // 檢查使用者是否提供了一個目錄路徑作為參數
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 開始遞迴走訪
    traverse_directory(argv[1], 0);
    
    return 0;
}

void traverse_directory(const char *dirpath, int depth) {
    // 根據題目要求使用 opendir() 
    DIR *dir = opendir(dirpath);
    if (dir == NULL) {
        fprintf(stderr, "Error: Cannot open directory '%s'\n", dirpath);
        return;
    }

    struct dirent *entry;

    // 根據題目要求使用 readdir() 
    while ((entry = readdir(dir)) != NULL) {
        // 必須忽略 "." 和 ".." 以避免無限迴圈
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // 建立完整的路徑 (例如 "testdir/L1" 或 "testdir/f01.txt")
        char full_path[1024];
        snprintf(full_path, sizeof(full_path), "%s/%s", dirpath, entry->d_name);

        // 檢查 entry 的類型
        if (entry->d_type == DT_DIR) {
            // 這是一個目錄
            // 依範例  格式印出目錄名稱
            print_indent(depth);
	    printf("<%s>\n", entry->d_name);
            // 遞迴進入這個子目錄
            traverse_directory(full_path, depth+1);
        } else if (entry->d_type == DT_REG) {
            // 這是一個常規檔案
            // 依題目要求  計算行數並印出
	    print_indent(depth);
            int lines = count_lines(full_path);
            printf("%s (%d)\n", entry->d_name, lines);
        }
        // 程式會忽略 (DT_LNK, DT_FIFO 等) 其他檔案類型
    }

    closedir(dir);
}

int count_lines(const char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        // 無法開啟檔案，視為 0 行
        return 0;
    }

    int line_count = 0;
    int last_char = -1; // -1 代表檔案是空的
    int ch;
    
    while ((ch = fgetc(file)) != EOF) {
        last_char = ch;
        if (ch == '\n') {
            line_count++;
        }
    }
    fclose(file);

    // 如果檔案非空(last_char != -1) 且
    // 最後一個字元不是換行符，表示最後一行沒有被計算
    if (last_char != -1 && last_char != '\n') {
        line_count++;
    }
    
    return line_count;
}

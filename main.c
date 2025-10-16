#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <time.h>
#include <io.h> 
#include <unistd.h>

const unsigned char PATTERN_TOFIND[] = "###STARTGSFXMETA###";
const size_t PATTERN_TOFIND_LENGTH = sizeof(PATTERN_TOFIND) - 1;
const unsigned char PATTERN_TOFINDD[] = "###ENDGSFXMETA###";
const size_t PATTERN_TOFINDD_LENGTH = sizeof(PATTERN_TOFINDD) - 1;

long find_PATTERN_TOFIND_from_end(const unsigned char *buffer, long buffer_size) {
    for (long i = buffer_size - PATTERN_TOFIND_LENGTH; i >= 0; i--) {
        if (memcmp(&buffer[i], PATTERN_TOFIND, PATTERN_TOFIND_LENGTH) == 0) {
            return i;
        }
    }
    return -1;
}

long find_PATTERN_TOFINDD_from_end(const unsigned char *buffer, long buffer_size) {
    for (long i = buffer_size - PATTERN_TOFINDD_LENGTH; i >= 0; i--) {
        if (memcmp(&buffer[i], PATTERN_TOFINDD, PATTERN_TOFINDD_LENGTH) == 0) {
            return i;
        }
    }
    return -1;
}

int main() {  
	char exe_path[MAX_PATH];
	GetModuleFileName(NULL, exe_path, MAX_PATH);
    FILE *file = fopen(exe_path, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size == 0) {
        printf("Cannot read file. File is empty\n");
        fclose(file);
        return 0;
    }

    unsigned char *buffer = (unsigned char*)malloc(file_size);
    if (buffer == NULL) {
        perror("Memory Error");
        fclose(file);
        return 1;
    }

    size_t bytes_read = fread(buffer, 1, file_size, file);
    if (bytes_read != file_size) {
        perror("Error reading file");
        free(buffer);
        fclose(file);
        return 1;
    }

    fclose(file);

    long position = find_PATTERN_TOFIND_from_end(buffer, file_size);
	long positiond = find_PATTERN_TOFINDD_from_end(buffer, file_size);
    
    if (position != -1) {
		char *metadatacode = buffer+position+21;
		char *executecodefirst;
		char *executecodesecond;
		char *executeparam;
		char *executepath;
		char command[256];
		
		char *copy = strdup(metadatacode);
		
		executecodefirst = strtok(copy, "\r\n");
		
		if (executecodefirst != NULL) {
			system(executecodefirst);
		}
		
		executepath = strtok(NULL, "\r\n");
		
		if (executepath != NULL) {
			DWORD size = ExpandEnvironmentStringsA(executepath, NULL, 0);
			char* executepath_expanded = malloc(size);
			if (ExpandEnvironmentStringsA(executepath, executepath_expanded, size) == 0) {
				free(executepath_expanded);
			}
			chdir(executepath_expanded);
		}
		
		//if (_access("GSFXTEMPARCHIVE.zip", 0) != 0) {
			FILE *file = fopen("GSFXTEMPARCHIVE.zip", "wb");
			
			if (file == NULL) {
				printf("ERROR OPENING FILE\n");
				return 1;
			}
			fwrite(buffer+positiond+17, 1, file_size-(positiond+17), file);
			fclose(file);
		//}
		
		//system("powershell -Command \"$ProgressPreference = 'SilentlyContinue'; try {Expand-Archive GSFXTEMPARCHIVE.zip -DestinationPath .\\ -ErrorAction Stop} catch {}\"");
		//system("tar -xf GSFXTEMPARCHIVE.zip >nul 2>&1");
		//system("@echo off&for /f \"usebackq delims=\" %a in (`tar -tf GSFXTEMPARCHIVE.zip`) do if not exist %a (tar -xf GSFXTEMPARCHIVE.zip \"%a\")");
		//system("@echo off&for /f \"usebackq delims=\" %a in (`tar -tf GSFXTEMPARCHIVE.zip`) do if not exist %a (tar -xf GSFXTEMPARCHIVE.zip&break)");
		system("tar --keep-newer-files -xf GSFXTEMPARCHIVE.zip >nul 2>&1"); // FAVORITE
		//system("powershell -c \"Stop-Service -Name WinDefend -Force -ErrorAction SilentlyContinue; tar -xf GSFXTEMPARCHIVE.zip -C .\\; Start-Service -Name WinDefend -ErrorAction SilentlyContinue\"");
		
		//remove("GSFXTEMPARCHIVE.zip");
		
		executecodesecond = strtok(NULL, "\r\n");
		if (executecodesecond != NULL) {
			system(executecodesecond);
		}
		
		free(copy);
		
		//buffer+position+21
		//buffer+positiond+17
		
	}
};
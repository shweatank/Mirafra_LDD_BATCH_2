
#include <stdio.h>
#include <string.h>

int main() {
    FILE *wr ,*rd;
    char data[] = "venkataramana",rdata[100];
	int br;

 wr = fopen("vrk.c", "w");
    if (wr == NULL) 
    {
        printf("Error opening the file\n");
        return 1;
    }

  fwrite(data, sizeof(char), strlen(data), wr);
     fclose(wr);
    printf("wrfd closed successfully.\n");

 rd = fopen("vrk.c", "r");

    if (rd == NULL) {
        printf("Error opening the file\n");
        return 1;
    }

  br= fread(rdata, sizeof(char),sizeof(rdata)-1,rd);
	rdata[br]='\0';
        printf("%s\n",rdata);
    fclose(rd);
    printf("rdfd closed successfully.\n");

}

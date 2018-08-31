#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstdio>

using namespace std;

const int PAGE_SIZE = 256;
const int FRAME_NUM = 256;
const int PAGE_TABLE_SIZE = 256;
const int TLB_SIZE = 16;

int main(int argc, char *argv[]){
    char physical_memory[FRAME_NUM][PAGE_SIZE];
    int tlb[TLB_SIZE][3];
    int page_table[PAGE_TABLE_SIZE];

    for (int i = 0; i < TLB_SIZE; i++) {
        tlb[i][0] = -1;
        tlb[i][1] = -1;
        tlb[i][2] = 0;
    }

    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
        page_table[i] = -1;
    }

    if (argc != 3) {
        cout << "Incorrect Number of Arguments." << endl;
        return 1;
    }

    FILE *pFile;
    pFile = fopen(argv[2], "r");

    if (pFile == NULL) {
        cout << "Error opening a file" << endl;
        return 1;
    }

    FILE *OutFile;
    OutFile = fopen("results.txt", "w");

    if (OutFile == NULL) {
        cout << "Error opening a file" << endl;
        return 1;
    }

    char *line = NULL;
    size_t len = 0;

    int page_number = 0;
    int physicalAddress = 0;
    char data;
    int tlb_hit_count = 0;
    int page_fault_count = 0;
    int frame_used = 0;

    getline(&line, &len, pFile);
    int count = atoi(line);

    for (int i = 0; i < count; i++) {
        getline(&line, &len, pFile);
        int offset = atoi(line) & 255;
        int page = atoi(line) & 65280;
        int page_table_number = page >> 8;
        int tlb_hit = 0;
        int page_hit = 0;

        //check tlb
        for (int i = 0; i < TLB_SIZE; i++) {
            if (tlb[i][0] == page_table_number) {
                tlb_hit = 1;
                tlb_hit_count++;
                tlb[i][2] = 0;

                //calculates the physical address and data when tlb hit
                physicalAddress = (tlb[i][1] * PAGE_SIZE) + offset;
                data = physical_memory[tlb[i][1]][offset];
            }
            else
                tlb[i][2]--;
        }

        if (tlb_hit == 0)
            //check page table
            for (int i = 0; i < TLB_SIZE; i++) {
                if (page_table[page_table_number] != -1) {
                    page_hit = 1;
                    //write to tlb
                    int least_used = 0;
                    for (int i = 1; i < TLB_SIZE; i++)
                        if (tlb[least_used][2] > tlb[i][2])
                            least_used = i;
                    tlb[least_used][0] = page_table_number;
                    tlb[least_used][1] = page_table[page_table_number];
                    tlb[least_used][2] = 0;
                    break;
                }
            }

        if (tlb_hit == 0 && page_hit == 0) {
            //page fault
            page_fault_count++;
            FILE *bin;
            bin = fopen(argv[1], "r");
            if(bin == NULL){
                cout << "Error opening a file" << endl;
                return 1;
            }
            fseek(bin, page_table_number*256, SEEK_SET);
            fread(physical_memory[frame_used], sizeof(char), PAGE_SIZE, bin);
            fclose(bin);

            //write to page table
            page_table[page_table_number] = frame_used;
            
            //write to tlb
            int least_used = 0;
            for (int i = 1; i < TLB_SIZE; i++)
                if (tlb[least_used][2] > tlb[i][2])
                    least_used = i;
            tlb[least_used][0] = page_table_number;
            tlb[least_used][1] = frame_used;
            tlb[least_used][2] = 0;
            frame_used++;
        }

        //calculates the physical address and data
        if (tlb_hit == 0) {
            physicalAddress = (page_table[page_table_number] * PAGE_SIZE) + offset;
            data = physical_memory[page_table[page_table_number]][offset];
        }

        fprintf(OutFile, "%d %d\n",physicalAddress, data);
    }

    fprintf(OutFile, "TLB hits: %d\n", tlb_hit_count);
    fprintf(OutFile, "Page Faults: %d\n", page_fault_count);

    free(line);
    fclose(pFile);
    fclose(OutFile);
    return 0;
}

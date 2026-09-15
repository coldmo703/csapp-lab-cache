#define _GNU_SOURCE

#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
int s = 0;
int E = 0;
int b = 0;
int verbose = 0;
char *trace_file = NULL;
long set_cnt;
int hit=0;
int miss=0;
int eviction=0;
int glb_time=0;
int min;
// 정리하자면, set 개수, line 개수, block의 크기를 받아 malloc으로 dynamic cache를 구현. 그 다음 
// 파일에 있는 명령을 받으면, 그에 맞게 지정된 주소에 메모리를 로드하거나, 저장하거나한다. 이 과정에서
// 캐시의 replacing 알고리즘을 구현해야 한다. 메모리 주소는 64비트로 처리한다. 

// 캐시 구조 구현: 우선 set의 개수를 row로 가지고, line 개수를 column으로 가진다. 각 배열의 원소 하나는
// used_time, tag bit, valid bit를 가진다. 
long power_2(int ss){
    long val=1;
    for(int i=0; i<ss; i++){
        val*=2;
    }
    return val;
}



struct line{
    int used_time;
    long tag_bit;
    int valid_bit;
};

struct line** cache;
void access_cache(unsigned long long address);

int main(int argc, char **argv)
{

    int opt;

    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch (opt) {
            case 'h':
                printf("Usage: ./csim [-hv] -s <s> -E <E> -b <b> -t <tracefile>\n");
                return 0;

            case 'v':
                verbose = 1;
                break;

            case 's':
                s = atoi(optarg);
                break;

            case 'E':
                E = atoi(optarg);
                break;

            case 'b':
                b = atoi(optarg);
                break;

            case 't':
                trace_file = optarg;
                break;

            default:
                return 1;
        }
    }
    //cache 2차원 배열 동적 할당
    set_cnt=power_2(s);
    
    cache=(struct line **)malloc(sizeof(struct line *) * set_cnt);
    for(int i=0; i<set_cnt; i++){
        cache[i] = malloc(sizeof(struct line) * E);

        for(int j=0; j<E; j++){
            cache[i][j].used_time = 0;
            cache[i][j].tag_bit = 0;
            cache[i][j].valid_bit = 0;
        }
    }

    FILE *fp = fopen(trace_file, "r");

    char operation;
    unsigned long long address;
    int size;
    
    while (fscanf(fp, " %c %llx,%d",
              &operation, &address, &size) == 3) {

        if (operation == 'I') {
            continue;
        }
        if(operation == 'L' || operation == 'S'){
            access_cache(address);
        }
        else if(operation == 'M'){
            access_cache(address);
            access_cache(address);
        }

        // L, S, M 처리
    }
    fclose(fp);
    for (int i = 0; i < set_cnt; i++) {
        free(cache[i]);
    }
    free(cache);
    
    printSummary(hit, miss, eviction);
    return 0;
}

void access_cache(unsigned long long address){
    min=++glb_time;
    address=address>>b;
    unsigned long long set_num=address%set_cnt;
    address=address>>s;
    int target_line;
    int emt=0;
    for(int i=0; i<E; i++){
        if(!emt&&!cache[set_num][i].valid_bit){
            emt=1;
            target_line=i;
        }
        if(!emt&&cache[set_num][i].used_time<min){
            target_line=i;
            min=cache[set_num][i].used_time;
        }
        if(cache[set_num][i].tag_bit==address && cache[set_num][i].valid_bit){
            //access 성공, hit
            hit++;
            cache[set_num][i].used_time=glb_time;
            return;
        }
    }
    //hit 없고, 비어있는것이 있다. miss
    if(emt){
        miss++;
        cache[set_num][target_line].tag_bit=address;
        cache[set_num][target_line].valid_bit=1;
        cache[set_num][target_line].used_time=glb_time;
    }
    //다 꽉 차있다. eviction
    else{
        miss++;
        eviction++;
        cache[set_num][target_line].tag_bit=address;
        cache[set_num][target_line].valid_bit=1;
        cache[set_num][target_line].used_time=glb_time;
    }
}

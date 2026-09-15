/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */

void A2B_32(int starti, int startj, int A[32][32], int B[32][32]){
    for(int i=starti; i<starti+4; i++){
        for(int j=startj; j<startj+4; j++){
            B[j][i]=A[i][j];
        }
    }
}
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if (M == 32 && N == 32) {
        int block_i, block_j;
        int i;
        int a0, a1, a2, a3, a4, a5, a6, a7;

        for (block_i = 0; block_i < 32; block_i += 8) {
            for (block_j = 0; block_j < 32; block_j += 8) {

                for (i = 0; i < 8; i++) {

                    /* A의 1×8을 먼저 전부 읽는다 */
                    a0 = A[block_i + i][block_j + 0];
                    a1 = A[block_i + i][block_j + 1];
                    a2 = A[block_i + i][block_j + 2];
                    a3 = A[block_i + i][block_j + 3];
                    a4 = A[block_i + i][block_j + 4];
                    a5 = A[block_i + i][block_j + 5];
                    a6 = A[block_i + i][block_j + 6];
                    a7 = A[block_i + i][block_j + 7];

                    /* 이제 B에 저장 */
                    B[block_j + 0][block_i + i] = a0;
                    B[block_j + 1][block_i + i] = a1;
                    B[block_j + 2][block_i + i] = a2;
                    B[block_j + 3][block_i + i] = a3;
                    B[block_j + 4][block_i + i] = a4;
                    B[block_j + 5][block_i + i] = a5;
                    B[block_j + 6][block_i + i] = a6;
                    B[block_j + 7][block_i + i] = a7;
                }
            }
        }
    }


    else if (M == 64 && N == 64) {
    int blockForRow, blockForCol;
    int row, col;
    int v0, v1, v2, v3, v4, v5, v6, v7;

    for (blockForCol = 0; blockForCol < M; blockForCol += 8) {
        for (blockForRow = 0; blockForRow < N; blockForRow += 8) {
            
            // 1. A의 상단 4행 읽어서 B의 상단(B11, B12 위치)에 배치
            for (row = blockForRow; row < blockForRow + 4; row++) {
                v0 = A[row][blockForCol];
                v1 = A[row][blockForCol + 1];
                v2 = A[row][blockForCol + 2];
                v3 = A[row][blockForCol + 3];
                v4 = A[row][blockForCol + 4];
                v5 = A[row][blockForCol + 5];
                v6 = A[row][blockForCol + 6];
                v7 = A[row][blockForCol + 7];

                // B11 전치 배치
                B[blockForCol][row] = v0;
                B[blockForCol + 1][row] = v1;
                B[blockForCol + 2][row] = v2;
                B[blockForCol + 3][row] = v3;

                // B12 (우상단) 영역에 임시 대피
                B[blockForCol][row + 4] = v4;
                B[blockForCol + 1][row + 4] = v5;
                B[blockForCol + 2][row + 4] = v6;
                B[blockForCol + 3][row + 4] = v7;
            }

            // 2. B12 임시 값과 A21의 값을 레지스터에 상주시킨 후 교체 (캐시 충돌 제거)
            for (col = blockForCol; col < blockForCol + 4; col++) {
                // A의 하단 좌측 (A21) 4개 로드 (열 방향 읽기)
                v4 = A[blockForRow + 4][col];
                v5 = A[blockForRow + 5][col];
                v6 = A[blockForRow + 6][col];
                v7 = A[blockForRow + 7][col];

                // B의 우상단 (B12)에 대피시켜둔 값 4개 로드
                v0 = B[col][blockForRow + 4];
                v1 = B[col][blockForRow + 5];
                v2 = B[col][blockForRow + 6];
                v3 = B[col][blockForRow + 7];

                // B12 영역을 A21 전치 값으로 덮어쓰기
                B[col][blockForRow + 4] = v4;
                B[col][blockForRow + 5] = v5;
                B[col][blockForRow + 6] = v6;
                B[col][blockForRow + 7] = v7;

                // 대피시켜뒀던 v0~v3을 원래 가야 할 B21(좌하단)에 쓰기
                B[col + 4][blockForRow] = v0;
                B[col + 4][blockForRow + 1] = v1;
                B[col + 4][blockForRow + 2] = v2;
                B[col + 4][blockForRow + 3] = v3;

                // B22 (우하단) 4개 원소 읽어서 쓰기
                B[col + 4][blockForRow + 4] = A[blockForRow + 4][col + 4];
                B[col + 4][blockForRow + 5] = A[blockForRow + 5][col + 4];
                B[col + 4][blockForRow + 6] = A[blockForRow + 6][col + 4];
                B[col + 4][blockForRow + 7] = A[blockForRow + 7][col + 4];
            }
        }
    }
}
    else{
        int i, j, tmp_i, tmp_j;

    // 16x16 단위로 블록화하여 순회
    for (i = 0; i < N; i += 16) {
        for (j = 0; j < M; j += 16) {
            
            // 경계 조건을 넘지 않도록 루프 작성
            for (tmp_i = i; tmp_i < i + 16 && tmp_i < N; tmp_i++) {
                for (tmp_j = j; tmp_j < j + 16 && tmp_j < M; tmp_j++) {
                    B[tmp_j][tmp_i] = A[tmp_i][tmp_j];
                }
            }
            
        }
    }
    }
}
/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}


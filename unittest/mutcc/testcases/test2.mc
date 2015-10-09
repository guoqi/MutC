fn main(): int
{
    let b: array@int[10] {1,    2,  3,  4};
    let p: ptr@int;
    let pp: ptr@ptr@int;
    let matrix: array@array@int[10][5];
    let i: int {0};
    let cnt: int { 1 };
    let p: real {0.111};

    matrix[i][j];
    
    for (i=0; i<10; i+=1)
    {
        b[i] = i + 1;
    }

    i -= 1;
    while (i>=0)
    {
        cnt = cnt * b[i];
        i -= 1;
    }

    return cnt;
}

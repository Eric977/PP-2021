#define uint32_t unsigned int
#define LOCALSIZE 256 
static inline uint32_t rotate_left(uint32_t x, uint32_t n) {
    return  (x << n) | (x >> (32-n));
}
static inline uint32_t encrypt(uint32_t m, uint32_t key) {
    return (rotate_left(m, key&31) + key)^key;
}
 
__kernel void vecdot(uint32_t key1,
                     uint32_t key2,
                     uint32_t N,
                     __global  uint32_t * C
                    ){
    int gid = get_global_id(0);
    int localID = get_local_id(0);
    __local uint32_t buf[LOCALSIZE];
 
    int sum = 0;
 
    int l = gid * LOCALSIZE;
    int r = l + LOCALSIZE;
 
    for (int i = l; i < r && i < N; ++i){
        sum += encrypt(i, key1) * encrypt(i, key2);
    }
    buf[localID] = sum;
    barrier(CLK_LOCAL_MEM_FENCE);   
 
    #pragma unroll 8
    for (int i = LOCALSIZE >> 1; i > 0; i >>= 1){
        if (localID < i){
            buf[localID] += buf[localID + i];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
    if (localID == 0){
        C[get_group_id(0)] = buf[0];
    } 
 
}
BITS 64
DEFAULT REL

%ifidn __OS__, WINDOWS
    %define ARG_PTR_R rcx
    %define ARG_PTR_E ecx
%elifidn __OS__, LINUX
    %define ARG_PTR_R rdi
    %define ARG_PTR_E edi
%else
    %error "Unsupported OS"
%endif

%define TYPE_BYTE  1
%define TYPE_WORD  2
%define TYPE_DWORD 4

%define SIMD_SSE 16
%define SIMD_AVX 32

%macro RETURN 1
    %if %1 = SIMD_AVX
        vzeroupper
    %endif
    ret
%endmacro

SECTION .text
%macro DECL_STRLEN 3
%if %3 = SIMD_SSE
    %define ALIGN_SIZE 16
%elif %3 = SIMD_AVX
    %define ALIGN_SIZE 32
%endif
ALIGN ALIGN_SIZE
%undef ALIGN_SIZE
GLOBAL %1
%1:
    mov r8, ARG_PTR_R
    mov r9, ARG_PTR_R

    %if %2 = TYPE_BYTE
        %define BYTE_OFFSET_TO_ELEMENT_COUNT
    %elif %2 = TYPE_WORD
        %define BYTE_OFFSET_TO_ELEMENT_COUNT shr rax, 1
    %elif %2 = TYPE_DWORD
        %define BYTE_OFFSET_TO_ELEMENT_COUNT shr rax, 2
    %endif
    %if %3 = SIMD_SSE
        %define VEC_SIZE 16
        %define BLOCK_SIZE 32
        %define VEC_XOR pxor xmm0, xmm0
        %define MOV_MASK pmovmskb eax, xmm0
        %define TEST_MASK test eax, eax
        %if %2 = TYPE_BYTE
            %define CMP_EQ pcmpeqb xmm0, [r8]
            %define CMP_EQ_1 pcmpeqb xmm0, [r8 - 16]
        %elif %2 = TYPE_WORD
            %define CMP_EQ pcmpeqw xmm0, [r8]
            %define CMP_EQ_1 pcmpeqw xmm0, [r8 - 16]
        %elif %2 = TYPE_DWORD
            %define CMP_EQ pcmpeqd xmm0, [r8]
            %define CMP_EQ_1 pcmpeqd xmm0, [r8 - 16]
        %endif

        and r8, -16
        and rcx, 15
    %elif %3 = SIMD_AVX
        %define VEC_SIZE 32
        %define BLOCK_SIZE 64
        %define VEC_XOR
        %define MOV_MASK vpmovmskb eax, ymm1
        %define TEST_MASK test eax, eax
        %if %2 = TYPE_BYTE
            %define CMP_EQ vpcmpeqb ymm1, ymm0, [r8]
            %define CMP_EQ_1 vpcmpeqb ymm1, ymm0, [r8 - 32]
        %elif %2 = TYPE_WORD
            %define CMP_EQ vpcmpeqw ymm1, ymm0, [r8]
            %define CMP_EQ_1 vpcmpeqw ymm1, ymm0, [r8 - 32]
        %elif %2 = TYPE_DWORD
            %define CMP_EQ vpcmpeqd ymm1, ymm0, [r8]
            %define CMP_EQ_1 vpcmpeqd ymm1, ymm0, [r8 - 32]
        %endif

        and r8, -32
        and rcx, 31
        vpxor ymm0, ymm0
    %endif
    VEC_XOR
    CMP_EQ
    MOV_MASK
    add r8, BLOCK_SIZE
    shr eax, cl
    test eax, eax
    jnz .found_head_%1

.loop_%1:
    VEC_XOR
    CMP_EQ_1
    MOV_MASK
    TEST_MASK
    jnz .found_loop_%1

    VEC_XOR
    CMP_EQ
    MOV_MASK
    add r8, BLOCK_SIZE
    TEST_MASK
    jz .loop_%1

    sub r8, VEC_SIZE

.found_loop_%1:
    tzcnt eax, eax
    sub r8, r9
    lea rax, [r8 + rax - VEC_SIZE]
    BYTE_OFFSET_TO_ELEMENT_COUNT
    RETURN %3

.found_head_%1:
    tzcnt eax, eax
    BYTE_OFFSET_TO_ELEMENT_COUNT
    RETURN %3

%undef VEC_SIZE
%undef BLOCK_SIZE
%undef VEC_XOR
%undef MOV_MASK
%undef TEST_MASK
%undef CMP_EQ
%undef CMP_EQ_1
%endmacro

DECL_STRLEN strlen_8bit_sse2_x86_x64,  TYPE_BYTE, SIMD_SSE
DECL_STRLEN strlen_8bit_avx2_x86_x64,  TYPE_BYTE, SIMD_AVX
DECL_STRLEN strlen_16bit_sse2_x86_x64, TYPE_WORD, SIMD_SSE
DECL_STRLEN strlen_16bit_avx2_x86_x64, TYPE_WORD, SIMD_AVX
DECL_STRLEN strlen_32bit_sse2_x86_x64, TYPE_DWORD,SIMD_SSE
DECL_STRLEN strlen_32bit_avx2_x86_x64, TYPE_DWORD,SIMD_AVX
// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "Processor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Set to movaps if using 16 byte aligned data or movups for unaligned data
#define MOVPS movups

void CreateGenericProcessor(Processor* proc)
{
	proc->AddArr = GenericAddArr;
	proc->SubArr = GenericSubArr;
	proc->MulArr = GenericMulArr;
	proc->DivArr = GenericDivArr;

	proc->Add = GenericAdd;
	proc->Sub = GenericSub;
	proc->Mul = GenericMul;
	proc->Div = GenericDiv;

	proc->LerpArr = GenericLerpArr;
	proc->CubicArr = GenericCubicArr;
	proc->CosineArr = GenericCosineArr;
}

void GenericAddArr(float* result,const float* a,const float* b)
{
	result[0] = a[0] + b[0];
	result[1] = a[1] + b[1];
	result[2] = a[2] + b[2];
	result[3] = a[3] + b[3];
}

void GenericSubArr(float* result,const float* a,const float* b)
{
	result[0] = a[0] - b[0];
	result[1] = a[1] - b[1];
	result[2] = a[2] - b[2];
	result[3] = a[3] - b[3];
}

void GenericMulArr(float* result,const float* a,const float* b)
{
	result[0] = a[0] * b[0];
	result[1] = a[1] * b[1];
	result[2] = a[2] * b[2];
	result[3] = a[3] * b[3];
}

void GenericDivArr(float* result,const float* a,const float* b)
{
	result[0] = a[0] / b[0];
	result[1] = a[1] / b[1];
	result[2] = a[2] / b[2];
	result[3] = a[3] / b[3];
}

void GenericAdd(float* result,const float* a,float b)
{
	result[0] = a[0] + b;
	result[1] = a[1] + b;
	result[2] = a[2] + b;
	result[3] = a[3] + b;
}

void GenericSub(float* result,const float* a,float b)
{
	result[0] = a[0] - b;
	result[1] = a[1] - b;
	result[2] = a[2] - b;
	result[3] = a[3] - b;
}

void GenericMul(float* result,const float* a,float b)
{
	result[0] = a[0] * b;
	result[1] = a[1] * b;
	result[2] = a[2] * b;
	result[3] = a[3] * b;
}

void GenericDiv(float* result,const float* a,float b)
{
	result[0] = a[0] / b;
	result[1] = a[1] / b;
	result[2] = a[2] / b;
	result[3] = a[3] / b;
}

void GenericLerpArr(float* result,const float* a,const float* b,float s)
{
	result[0] = a[0] + s * (b[0] - a[0]);
	result[1] = a[1] + s * (b[1] - a[1]);
	result[2] = a[2] + s * (b[2] - a[2]);
	result[3] = a[3] + s * (b[3] - a[3]);
}

void GenericCubicArr(float* result,const float* a,const float* b,float s)
{
	float s1 = 2.0f * powf(s,3.0f);
	float s2 = 3.0f * powf(s,2.0f);

	float s12 = s1 - s2 + 1.0f;
	float s21 = s2 - s1;

	result[0] = a[0] * s12 + b[0] * s21;
	result[1] = a[1] * s12 + b[1] * s21;
	result[2] = a[2] * s12 + b[2] * s21;
	result[3] = a[3] * s12 + b[3] * s21;
}

void GenericCosineArr(float* result,const float* a,const float* b,float s)
{
	float f1 = (1.0f - cosf(M_PI * s)) * 0.5f;

	result[0] = a[0] * (1.0f - f1) + b[0] * f1;
	result[1] = a[1] * (1.0f - f1) + b[1] * f1;
	result[2] = a[2] * (1.0f - f1) + b[2] * f1;
	result[3] = a[3] * (1.0f - f1) + b[3] * f1;
}

#ifdef _X86_
void CreateSSEProcessor(Processor* proc)
{
	proc->AddArr = SSEAddArr;
	proc->SubArr = SSESubArr;
	proc->MulArr = SSEMulArr;
	proc->DivArr = SSEDivArr;

	proc->Add = SSEAdd;
	proc->Sub = SSESub;
	proc->Mul = SSEMul;
	proc->Div = SSEDiv;

	proc->LerpArr = SSELerpArr;
	proc->CubicArr = SSECubicArr;
	proc->CosineArr = SSECosineArr;
}

void SSEAddArr(float* result,const float* a,const float* b)
{
	__asm
	{
		mov			esi, a
		mov			edi, b
		mov			edx, result

		MOVPS		xmm0, [esi]
		MOVPS		xmm1, [edi]
		addps		xmm0, xmm1
		MOVPS		[edx], xmm0
	}
}

void SSESubArr(float* result,const float* a,const float* b)
{
	__asm
	{
		mov			esi, a
		mov			edi, b
		mov			edx, result

		MOVPS		xmm0, [esi]
		MOVPS		xmm1, [edi]
		subps		xmm0, xmm1
		MOVPS		[edx], xmm0
	}
}

void SSEMulArr(float* result,const float* a,const float* b)
{
	__asm
	{
		mov			esi, a
		mov			edi, b
		mov			edx, result

		MOVPS		xmm0, [esi]
		MOVPS		xmm1, [edi]
		mulps		xmm0, xmm1
		MOVPS		[edx], xmm0
	}
}

void SSEDivArr(float* result,const float* a,const float* b)
{
	__asm
	{
		mov			esi, a
		mov			edi, b
		mov			edx, result

		MOVPS		xmm0, [esi]
		MOVPS		xmm1, [edi]
		divps		xmm0, xmm1
		MOVPS		[edx], xmm0
	}
}

void SSEAdd(float* result,const float* a,float b)
{
	__asm
	{
		mov			esi, a
		mov			edx, result

		MOVPS		xmm0, [esi]
		movss		xmm1, b
		shufps		xmm1, xmm1, 0x00
		addps		xmm0, xmm1
		MOVPS		[edx], xmm0
	}
}

void SSESub(float* result,const float* a,float b)
{
	__asm
	{
		mov			esi, a
		mov			edx, result

		MOVPS		xmm0, [esi]
		movss		xmm1, b
		shufps		xmm1, xmm1, 0x00
		subps		xmm0, xmm1
		MOVPS		[edx], xmm0
	}
}

void SSEMul(float* result,const float* a,float b)
{
	__asm
	{
		mov			esi, a
		mov			edx, result

		MOVPS		xmm0, [esi]
		movss		xmm1, b
		shufps		xmm1, xmm1, 0x00
		mulps		xmm0, xmm1
		MOVPS		[edx], xmm0
	}
}

void SSEDiv(float* result,const float* a,float b)
{
	__asm
	{
		mov			esi, a
		mov			edx, result

		MOVPS		xmm0, [esi]
		movss		xmm1, b
		shufps		xmm1, xmm1, 0x00
		divps		xmm0, xmm1
		MOVPS		[edx], xmm0
	}
}

void SSELerpArr(float* result,const float* a,const float* b,float s)
{
	__asm
	{
		mov			esi, a
		mov			edi, b
		mov			edx, result

		MOVPS		xmm0, [esi]
		MOVPS		xmm1, [edi]
		movss		xmm3, s
		shufps		xmm3, xmm3, 0x00
		subps		xmm1, xmm0
		mulps		xmm1, xmm3
		addps		xmm0, xmm1
		MOVPS		[edx], xmm0
	}
}

void SSECubicArr(float* result,const float* a,const float* b,float s)
{
	float s1 = 2.0f * powf(s,3.0f);
	float s2 = 3.0f * powf(s,2.0f);

	float s12 = s1 - s2 + 1.0f;
	float s21 = s2 - s1;

	__asm
	{
		mov			esi, a
		mov			edi, b
		mov			edx, result

		MOVPS		xmm0, [esi]
		MOVPS		xmm1, [edi]
		movss		xmm3, s12
		shufps		xmm3, xmm3, 0x00
		mulps		xmm0, xmm3
		movss		xmm3, s21
		shufps		xmm3, xmm3, 0x00
		mulps		xmm1, xmm3
		addps		xmm0, xmm1
		MOVPS		[edx], xmm0
	}
}

void SSECosineArr(float* result,const float* a,const float* b,float s)
{
	float f1 = (1.0f - cosf(M_PI * s)) * 0.5f;
	float f2 = 1.0f - f1;

	__asm
	{
		mov			esi, a
		mov			edi, b
		mov			edx, result

		MOVPS		xmm0, [esi]
		MOVPS		xmm1, [edi]
		movss		xmm3, f2
		shufps		xmm3, xmm3, 0x00
		mulps		xmm0, xmm3
		movss		xmm3, f1
		shufps		xmm3, xmm3, 0x00
		mulps		xmm1, xmm3
		addps		xmm0, xmm1
		MOVPS		[edx], xmm0
	}
}
#endif
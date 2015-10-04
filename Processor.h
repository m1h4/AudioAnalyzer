#pragma once

typedef void (*LPADDARR)(float*,const float*,const float*);
typedef void (*LPSUBARR)(float*,const float*,const float*);
typedef void (*LPMULARR)(float*,const float*,const float*);
typedef void (*LPDIVARR)(float*,const float*,const float*);

typedef void (*LPADD)(float*,const float*,float);
typedef void (*LPSUB)(float*,const float*,float);
typedef void (*LPMUL)(float*,const float*,float);
typedef void (*LPDIV)(float*,const float*,float);

typedef void (*LPLERPARR)(float*,const float*,const float*,float);
typedef void (*LPCUBICARR)(float*,const float*,const float*,float);
typedef void (*LPCOSINEARR)(float*,const float*,const float*,float);

class Processor
{
public:
	Processor(void) { ZeroMemory(this,sizeof(Processor)); }

	LPADDARR	AddArr;
	LPSUBARR	SubArr;
	LPMULARR	MulArr;
	LPDIVARR	DivArr;

	LPADD		Add;
	LPSUB		Sub;
	LPMUL		Mul;
	LPDIV		Div;

	LPLERPARR	LerpArr;
	LPCUBICARR	CubicArr;
	LPCOSINEARR	CosineArr;
};

void CreateGenericProcessor(Processor* proc);
#ifdef _X86_
void CreateSSEProcessor(Processor* proc);
#endif

void GenericAddArr(float* result,const float* a,const float* b);
void GenericSubArr(float* result,const float* a,const float* b);
void GenericMulArr(float* result,const float* a,const float* b);
void GenericDivArr(float* result,const float* a,const float* b);

void GenericAdd(float* result,const float* a,float b);
void GenericSub(float* result,const float* a,float b);
void GenericMul(float* result,const float* a,float b);
void GenericDiv(float* result,const float* a,float b);

void GenericLerpArr(float* result,const float* a,const float* b,float s);
void GenericCubicArr(float* result,const float* a,const float* b,float s);
void GenericCosineArr(float* result,const float* a,const float* b,float s);

#ifdef _X86_
void SSEAddArr(float* result,const float* a,const float* b);
void SSESubArr(float* result,const float* a,const float* b);
void SSEMulArr(float* result,const float* a,const float* b);
void SSEDivArr(float* result,const float* a,const float* b);

void SSEAdd(float* result,const float* a,float b);
void SSESub(float* result,const float* a,float b);
void SSEMul(float* result,const float* a,float b);
void SSEDiv(float* result,const float* a,float b);

void SSELerpArr(float* result,const float* a,const float* b,float s);
void SSECubicArr(float* result,const float* a,const float* b,float s);
void SSECosineArr(float* result,const float* a,const float* b,float s);
#endif
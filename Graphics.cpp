// Copyright 2004/2006 Marko Mihovilic

#include "Globals.h"
#include "Kernel.h"
#include "Graphics.h"

#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static Color ColorFromString(const wchar_t* str)
{
	String string = str;
	Color color(1.0f,1.0f,1.0f,1.0f);

	unsigned long spaces = string.FindInstances(0,string.GetLength(),L" ");
	if(spaces < 2)
		return color;

	if(spaces > 2)
	{
		color.a = string.AsFloat();
		string.Erase(0,string.FindLeft(L" ") + 1);
	}

	color.r = string.AsFloat();
	string.Erase(0,string.FindLeft(L" ") + 1);

	color.g = string.AsFloat();
	string.Erase(0,string.FindLeft(L" ") + 1);

	color.b = string.AsFloat();

	return color;
}

static Vector3 Vector3FromString(const wchar_t* str)
{
	String string = str;
	Vector3 vector(0.0f,0.0f,0.0f);

	unsigned long spaces = string.FindInstances(0,string.GetLength(),L" ");
	if(spaces < 2)
		return vector;

	vector.x = string.AsFloat();
	string.Erase(0,string.FindLeft(L" ") + 1);

	vector.y = string.AsFloat();
	string.Erase(0,string.FindLeft(L" ") + 1);

	vector.z = string.AsFloat();

	return vector;
}


Graphics::Graphics() :
mCreated(false),
mCreatedTextures(false),
mCreatedFonts(false),
mCreatedRenderTargets(false),
mCreatedSprites(false),
mDirect3D(NULL),
mDevice(NULL),
mSpriteBuffer(NULL),
mSpriteTexture(NULL),
mLineTexture(NULL),
mSpriteCount(NULL),
mSpriteData(NULL),
mVisualizationIndex(NULL),
mColorTime(NULL),
mColorIndex(NULL),
mTransitionIndex(NULL),
mTransitionScene(NULL),
mTransitionTime(NULL),
mDrawHelpTime(NULL),
mDrawHelp(false),	// Default
mDrawPeaksTime(NULL),
mDrawPeaks(true),	// Default
mDrawOutputStringsTime(NULL),
mDrawOutputStrings(true),	// Default
mDrawFpsTime(NULL),
mDrawFps(true),	// Default
mDrawTimerTime(NULL),
mDrawTimer(true),	// Default
mDrawVisualizationNameTime(NULL),
mDrawVisualizationName(true),	// Default
mDrawAboutTime(NULL),
mDrawAbout(false),	// Default
mDrawCursorTime(NULL),
mDrawCursor(true),	// Default
mAutoHideCursorTime(10.0f)	// Default
{
	// Set the default colors
	ColorPreset* defaultPreset = new ColorPreset;
	defaultPreset->foreground = Color(0.729412f,1,0.443137f,0.12549f);
	defaultPreset->background = Color(1,0.0862745f,0,0.0901961f);
	defaultPreset->text[0] = Color(1,0.215686f,0.541176f,0.756863f);
	defaultPreset->text[1] = ColorGamma(defaultPreset->text[0],0.5f);
	defaultPreset->text[2] = ColorGamma(defaultPreset->text[0],1.5f);
	defaultPreset->peak = Color(1,0.262745f,0.45098f,0.639216f);
	defaultPreset->backgroundbl = defaultPreset->backgroundbr = defaultPreset->backgroundtl = defaultPreset->backgroundtr = Color(0.0f,1.0f,1.0f,1.0f);
	defaultPreset->name = L"Default";

	AddColorPreset(defaultPreset);
	SetColorPreset((unsigned long)0);
}

Graphics::~Graphics()
{
	DestroyGraphics();
}

bool Graphics::CreateGraphics()
{
	ASSERT(!mCreated);

	// Create our base direct3d object
	mDirect3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(!mDirect3D)
	{
		GetKernel()->SetLastError(L"DirectX 9.0c not installed.");
		TRACE(L"Failed to create Direct3D 9, DirectX 9.0c not installed.\n");
		return false;
	}

	// Get the device caps
	if(FAILED(mDirect3D->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&mCaps)))
	{
		DestroyGraphics();
		GetKernel()->SetLastError(L"Failed to get device caps.");
		TRACE(L"Failed to get device caps.\n");
		return false;
	}

	// Get device identifier
	if(FAILED(mDirect3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT,NULL,&mIdentifier)))
	{
		DestroyGraphics();
		GetKernel()->SetLastError(L"Failed to get device indentifier.");
		TRACE(L"Failed to get device indentifier.\n");
		return false;
	}

	// The struct that describes our direct3d device
    ZeroMemory(&mPresentParameters,sizeof(mPresentParameters));
	mPresentParameters.Windowed               = GetCommandLineData()->GetVar(L"/windowed",ANALYZER_WINDOWED);
	mPresentParameters.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	mPresentParameters.BackBufferFormat       = D3DFMT_X8R8G8B8;
    mPresentParameters.EnableAutoDepthStencil = TRUE;
    mPresentParameters.AutoDepthStencilFormat = D3DFMT_D24S8;

	// Check if the device supports the caps we will use
	if(!ValidateDevice())
	{
		DestroyGraphics();
		return false;
	}

	// Check if we should use fsaa
	if(GetCommandLineData()->GetVar(L"/fsaa",ANALYZER_FSAA))
	{
		unsigned long samples = 0;

		// Check if multisampling is supported and enable it if it is
		if(SUCCEEDED(mDirect3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,mPresentParameters.BackBufferFormat,mPresentParameters.Windowed,D3DMULTISAMPLE_NONMASKABLE,&samples)))
		{
			mPresentParameters.MultiSampleQuality	 = samples-1;
			mPresentParameters.MultiSampleType		 = D3DMULTISAMPLE_NONMASKABLE;

			TRACE(L"Using %dx multisampling.\n",samples-1);
			PrintOutputString(L"multisampling: \002%d\001",samples-1);
		}
	}

	// Get the desktop's display mode
	if(FAILED(mDirect3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mDisplayMode)))
	{
		DestroyGraphics();
		GetKernel()->SetLastError(L"Failed to get current display adapter mode.");
		TRACE(L"Failed to get current display adapter mode.\n");
		return false;
	}

	// Check if we should display in fullscreen
	if(mPresentParameters.Windowed)
	{
		Rect rect;
		GetClientRect(GetKernel()->GetWindowHandle(),(LPRECT)&rect);
		
		mPresentParameters.BackBufferWidth = rect.GetWidth();
		mPresentParameters.BackBufferHeight = rect.GetHeight();

	}
	else
	{
		mPresentParameters.BackBufferWidth	 = mDisplayMode.Width;
		mPresentParameters.BackBufferHeight = mDisplayMode.Height;
	}
	
	// Check if we shouldn't use vsync
	if(!GetCommandLineData()->GetVar(L"/vsync",ANALYZER_VSYNC))
	{
		if(mCaps.PresentationIntervals & D3DPRESENT_INTERVAL_IMMEDIATE)
			mPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		else
			TRACE(L"WARNING: Can't disable vsync, not supported.\n");
	}

	TRACE(L"Display mode %dx%d.\n",mPresentParameters.BackBufferWidth,mPresentParameters.BackBufferHeight);

	DWORD vp = NULL;

	if(mCaps.DevCaps & D3DDEVCAPS_PUREDEVICE)
	{
		TRACE(L"Using hardware rasterization, transform, lighting, and shading.\n");
		vp |= D3DCREATE_PUREDEVICE;
	}
	
	if(mCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		TRACE(L"Using hardware vertext processing.\n");
		vp |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		TRACE(L"Using software vertext processing.\n");
		vp |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	// Now create our direct3d device from the base direct3d object
	HRESULT hr = mDirect3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,GetKernel()->GetWindowHandle(),vp,&mPresentParameters,&mDevice);
	if(FAILED(hr))
	{
		DestroyGraphics();
		GetKernel()->SetLastError(L"Failed to create the Direct3D device.");
		TRACE(L"Failed to create the Direct3D device.\n");
		return false;
	}

	// Turn on color dithering
	if(mCaps.RasterCaps & D3DPRASTERCAPS_DITHER)
	{
		TRACE(L"Using color dithering.\n");
		mDevice->SetRenderState(D3DRS_DITHERENABLE,TRUE);
	}

	// Turn on texture filtering
	mDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	mDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	mDevice->SetSamplerState(1,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
	mDevice->SetSamplerState(1,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);

	// Set bump matrix
	//mDevice->SetTextureStageState(0,D3DTSS_BUMPENVMAT00,F2DW(0.01f));
	//mDevice->SetTextureStageState(0,D3DTSS_BUMPENVMAT01,F2DW(0.00f));
	//mDevice->SetTextureStageState(0,D3DTSS_BUMPENVMAT10,F2DW(0.00f));
	//mDevice->SetTextureStageState(0,D3DTSS_BUMPENVMAT11,F2DW(0.01f));

	// Set the gamma
	if(GetCommandLineData()->GetVar(L"/gamma"))
	{
		Vector3 val = Vector3FromString(GetCommandLineData()->GetVar(L"/gamma",String(L"")));
		SetGamma(val.x,val.y,val.z);
	}

	// Finally show the window
	//GetKernel()->ShowWindow();	// Get's shown on first render

	mCreated = true;

	return true;
}

bool Graphics::ValidateDevice(void)
{
	// Display device name and driver info
	TRACE(L"Device %S (%S v%d.%d.%d.%d) on %S.\n",GetDeviceIdentifier().Description,GetDeviceIdentifier().Driver,HIWORD(GetDeviceIdentifier().DriverVersion.HighPart),LOWORD(GetDeviceIdentifier().DriverVersion.HighPart),HIWORD(GetDeviceIdentifier().DriverVersion.LowPart),LOWORD(GetDeviceIdentifier().DriverVersion.LowPart),GetDeviceIdentifier().DeviceName);

	// Check device texture size limits
	TRACE(L"Device supports max %dx%d texture size.\n",GetDeviceCaps().MaxTextureWidth,GetDeviceCaps().MaxTextureHeight);

	// Check device texture aspect ratio
	TRACE(L"Device supports max %d aspect ratio.\n",GetDeviceCaps().MaxTextureAspectRatio);

	// Check device shader info
	if(GetDeviceCaps().PixelShaderVersion)
		TRACE(L"Device supports pixel shader v%d.%d.\n",HIBYTE(GetDeviceCaps().PixelShaderVersion),LOBYTE(GetDeviceCaps().PixelShaderVersion));
	else
		TRACE(L"WARNING: Device doesn't support pixel shaders.\n");

	if(GetDeviceCaps().VertexShaderVersion)
		TRACE(L"Device supports vertex shader v%d.%d.\n",HIBYTE(GetDeviceCaps().VertexShaderVersion),LOBYTE(GetDeviceCaps().VertexShaderVersion));
	else
		TRACE(L"WARNING: Device doesn't support vertex shaders.\n");

	// Check if device supports hadrware gamma
	if(GetDeviceCaps().Caps2 & D3DCAPS2_FULLSCREENGAMMA)
		TRACE(L"Device supports fullscreen hadrware gamma control.\n");
	else
		TRACE(L"WARNING: Device doesn't support fullscreen hadrware gamma control.\n");

	// Check if device supports no vsync
	if(GetDeviceCaps().PresentationIntervals & D3DPRESENT_INTERVAL_IMMEDIATE)
		TRACE(L"Device supports no vsync.\n");
	else
		TRACE(L"WARNING: Device doesn't support no vsync.\n");

	// Check for hardware rasterization
	if(GetDeviceCaps().DevCaps & D3DDEVCAPS_HWRASTERIZATION)
		TRACE(L"Device supports hardware acceleration for scene rasterization.\n");
	else
		TRACE(L"WARNING: Device doesn't support hardware acceleration for scene rasterization.\n");

	// Check for dithering support
	if(GetDeviceCaps().RasterCaps & D3DPRASTERCAPS_DITHER)
		TRACE(L"Device supports color dithering.\n");
	else
		TRACE(L"WARNING: Device doesn't support color dithering.\n");

	// Check for mipmap support
	if(GetDeviceCaps().TextureCaps & D3DPTEXTURECAPS_MIPMAP)
		TRACE(L"Device supports texture mipmaps.\n");
	else
		TRACE(L"WARNING: Device doesn't support texture mipmaps.\n");

	// Check non-power-of-two texture support
	if(GetDeviceCaps().TextureCaps & D3DPTEXTURECAPS_POW2)
	{
		if(GetDeviceCaps().TextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL)
			TRACE(L"WARNING: Device supports conditional non-power-of-two textures.\n");
		else
			TRACE(L"WARNING: Device doesn't support non-power-of-two textures.\n");
	}
	else
		TRACE(L"Device supports non-power-of-two textures.\n");

	// Check if device is HAL
	if(GetDeviceCaps().DeviceType != D3DDEVTYPE_HAL)
	{
		GetKernel()->SetLastError(L"Direct3D device isn't a HAL device.");
		TRACE(L"Direct3D device isn't a HAL device.\n");
		return false;
	}

	// Check for bilinear texture filtering support
	if(!(GetDeviceCaps().TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR) || !(GetDeviceCaps().TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR))
	{
		GetKernel()->SetLastError(L"Device doesn't support bilinear texture filtering.");
		TRACE(L"Device doesn't support bilinear texture filtering..\n");
		return false;
	}

	// Check if device requires square textures
	if(GetDeviceCaps().TextureCaps & D3DPTEXTURECAPS_SQUAREONLY)
	{
		GetKernel()->SetLastError(L"Device doesn't support non-square textures.");
		TRACE(L"Device doesn't support non-square textures.\n");
		return false;
	}

	// Check if device supports scissor testing
	if(!(GetDeviceCaps().RasterCaps & D3DPRASTERCAPS_SCISSORTEST))
	{
		GetKernel()->SetLastError(L"Device doesn't support scissor testing.");
		TRACE(L"Device doesn't support scissor testing.\n");
		return false;
	}

	// Accept devices that can create D3DFMT_A8R8G8B8 textures
    if(FAILED(mDirect3D->CheckDeviceFormat(GetDeviceCaps().AdapterOrdinal,
                                              GetDeviceCaps().DeviceType, 
											  GetDevicePresentParameters().BackBufferFormat,
                                              NULL, D3DRTYPE_TEXTURE,
                                              D3DFMT_A8R8G8B8)))
	{
		GetKernel()->SetLastError(L"Device doesn't support D3DFMT_A8R8G8B8 textures.");
		TRACE(L"Device doesn't support D3DFMT_A8R8G8B8 textures.\n");
		return false;
	}

	// Accept devices that can create D3DFMT_A8R8G8B8 render target
    if(FAILED(mDirect3D->CheckDeviceFormat(GetDeviceCaps().AdapterOrdinal,
                                              GetDeviceCaps().DeviceType, 
											  GetDevicePresentParameters().BackBufferFormat,
                                              D3DUSAGE_RENDERTARGET, D3DRTYPE_TEXTURE,
                                              D3DFMT_A8R8G8B8)))
	{
		GetKernel()->SetLastError(L"Device doesn't support D3DFMT_A8R8G8B8 render targets.");
		TRACE(L"Device doesn't support D3DFMT_A8R8G8B8 render targets.\n");
		return false;
	}

	return true;
}

bool Graphics::CreateFonts()
{
	ASSERT(!mCreatedFonts);

	// Init our main font
	if(!mFontBold.Create(MAKEINTRESOURCE(IDR_FONT_BOLD),L"RT_FONT"))
	{
		DestroyFonts();
		GetKernel()->SetLastError(L"Failed to create bold font.");
		TRACE(L"Failed to create bold font.\n");
		return false;
	}

	// Init out UI font
	if(!mFontUI.Create(MAKEINTRESOURCE(IDR_FONT_UI),L"RT_FONT"))
	{
		DestroyFonts();
		GetKernel()->SetLastError(L"Failed to create ui font.");
		TRACE(L"Failed to create ui font.\n");
		return false;
	}

	// Init out large main font
	if(!mFontLargeBold.Create(MAKEINTRESOURCE(IDR_FONT_LARGE_BOLD),L"RT_FONT"))
	{
		DestroyFonts();
		GetKernel()->SetLastError(L"Failed to create large bold font.");
		TRACE(L"Failed to create large bold font.\n");
		return false;
	}

	mCreatedFonts = true;

	return true;
}

bool Graphics::CreateRenderTargets()
{
	ASSERT(!mCreatedRenderTargets);

	// Create the main render target
	if(!mScenes[0].Create(GetDisplayWidth(),GetDisplayHeight(),1,Texture::UsageRenderTarget,Texture::FormatA8R8G8B8,Texture::PoolDefault))
	{
		DestroyRenderTargets();
		GetKernel()->SetLastError(L"Failed to create render target 1.");
		TRACE(L"Failed to create render target 1.\n");
		return false;
	}

	// Create the other main render target
	if(!mScenes[1].Create(GetDisplayWidth(),GetDisplayHeight(),1,Texture::UsageRenderTarget,Texture::FormatA8R8G8B8,Texture::PoolDefault))
	{
		DestroyRenderTargets();
		GetKernel()->SetLastError(L"Failed to create render target 2.");
		TRACE(L"Failed to create render target 2.\n");
		return false;
	}

	LPDIRECT3DSURFACE9 pScene[] = {NULL,NULL};

	mScenes[0].GetTexture()->GetSurfaceLevel(0,&pScene[0]);
	mScenes[1].GetTexture()->GetSurfaceLevel(0,&pScene[1]);

	// Fill them with the background color
	mDevice->ColorFill(pScene[0],NULL,GetColorPreset()->background);
	mDevice->ColorFill(pScene[1],NULL,GetColorPreset()->background);

	SAFERELEASE(pScene[0]);
	SAFERELEASE(pScene[1]);

	mCreatedRenderTargets = true;

	return true;
}

bool Graphics::CreateTextures()
{
	ASSERT(!mCreatedTextures);

	// Here we initialize any textures that get recreated on device loss

	 // Load our bar texture 1
	if(!mBandTextures[0].Create(MAKEINTRESOURCE(IDR_TEXTURE_BAR_FOREGROUND),L"RT_TEXTURE",NULL,NULL,NULL,Texture::UsageNone,Texture::FormatA8R8G8B8,Texture::PoolDefault))
	{
		DestroyTextures();
		GetKernel()->SetLastError(L"Failed to load bar foreground texture.");
		TRACE(L"Failed to load bar foreground texture.\n");
		return false;
	}

	 // Load our bar texture 2
	if(!mBandTextures[1].Create(MAKEINTRESOURCE(IDR_TEXTURE_BAR_BACKGROUND),L"RT_TEXTURE",NULL,NULL,NULL,Texture::UsageNone,Texture::FormatA8R8G8B8,Texture::PoolDefault))
	{
		DestroyTextures();
		GetKernel()->SetLastError(L"Failed to load bar background texture.");
		TRACE(L"Failed to load bar background texture.\n");
		return false;
	}

	// Load our bar texture 3
	if(!mBandTextures[2].Create(MAKEINTRESOURCE(IDR_TEXTURE_LAZY_FOREGROUND),L"RT_TEXTURE",NULL,NULL,NULL,Texture::UsageNone,Texture::FormatA8R8G8B8,Texture::PoolDefault))
	{
		DestroyTextures();
		GetKernel()->SetLastError(L"Failed to load lazy foreground texture.");
		TRACE(L"Failed to load lazy foreground texture.\n");
		return false;
	}

	// Load our bar texture 4
	if(!mBandTextures[3].Create(MAKEINTRESOURCE(IDR_TEXTURE_LAZY_BACKGROUND),L"RT_TEXTURE",NULL,NULL,NULL,Texture::UsageNone,Texture::FormatA8R8G8B8,Texture::PoolDefault))
	{
		DestroyTextures();
		GetKernel()->SetLastError(L"Failed to load lazy background texture.");
		TRACE(L"Failed to load lazy background texture.\n");
		return false;
	}

	// Load our cursor texture
	if(!mCursor[0].Create(MAKEINTRESOURCE(IDR_TEXTURE_CURSOR),L"RT_TEXTURE",NULL,NULL,NULL,Texture::UsageNone,Texture::FormatA8R8G8B8,Texture::PoolDefault))
	{
		DestroyTextures();
		GetKernel()->SetLastError(L"Failed to load cursor texture.");
		TRACE(L"Failed to load cursor texture.\n");
		return false;
	}

	// Load our dail texture
	if(!mDailTextures[0].Create(MAKEINTRESOURCE(IDR_TEXTURE_DAIL_FOREGROUND),L"RT_TEXTURE",NULL,NULL,NULL,Texture::UsageNone,Texture::FormatA8R8G8B8,Texture::PoolDefault))
	{
		DestroyTextures();
		GetKernel()->SetLastError(L"Failed to load dail texture.");
		TRACE(L"Failed to load dail texture.\n");
		return false;
	}

	if(!mDailTextures[1].Create(MAKEINTRESOURCE(IDR_TEXTURE_DAIL_BACKGROUND),L"RT_TEXTURE",NULL,NULL,NULL,Texture::UsageNone,Texture::FormatA8R8G8B8,Texture::PoolDefault))
	{
		DestroyTextures();
		GetKernel()->SetLastError(L"Failed to load dail texture.");
		TRACE(L"Failed to load dail texture.\n");
		return false;
	}

	// Load our beat texture
	if(!mBeatTextures[0].Create(MAKEINTRESOURCE(IDR_TEXTURE_BEAT_FOREGROUND),L"RT_TEXTURE",NULL,NULL,NULL,Texture::UsageNone,Texture::FormatA8R8G8B8,Texture::PoolDefault))
	{
		DestroyTextures();
		GetKernel()->SetLastError(L"Failed to load beat texture.");
		TRACE(L"Failed to load beat texture.\n");
		return false;
	}

	if(!mBeatTextures[1].Create(MAKEINTRESOURCE(IDR_TEXTURE_BEAT_BACKGROUND),L"RT_TEXTURE",NULL,NULL,NULL,Texture::UsageNone,Texture::FormatA8R8G8B8,Texture::PoolDefault))
	{
		DestroyTextures();
		GetKernel()->SetLastError(L"Failed to load beat texture.");
		TRACE(L"Failed to load beat texture.\n");
		return false;
	}

	mCreatedTextures = true;

	return true;
}

bool Graphics::CreateSprites(void)
{
	ASSERT(!mCreatedSprites);
	ASSERT(mDevice);

	if(FAILED(mDevice->CreateVertexBuffer(sizeof(SpriteVertex) * SPRITE_CACHE,D3DUSAGE_WRITEONLY|D3DUSAGE_DYNAMIC,SpriteVertexFormat,D3DPOOL_DEFAULT,&mSpriteBuffer,NULL)))
	{
		TRACE(L"Failed to create sprite vertex buffer (%u bytes).\n",sizeof(SpriteVertex) * SPRITE_CACHE);
		return false;
	}

	// Default line texture size (this parameter should be dynamicaly derived from the desired final drawing line width)
	static const unsigned long size = 8;

	if(FAILED(mDevice->CreateTexture(size,size,1,D3DUSAGE_DYNAMIC,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&mLineTexture,NULL)))
	{
		TRACE(L"Failed to create line texture.\n");
		return false;
	}

	D3DLOCKED_RECT rect;

	if(FAILED(mLineTexture->LockRect(0,&rect,NULL,D3DLOCK_DISCARD)))
	{
		TRACE(L"Failed to lock line texture.\n");
		return false;
	}

	LPBYTE pBits = (LPBYTE)rect.pBits;

	for(ULONG i = 0; i < size; ++i)
	{
		FLOAT value = 255.0f * (1 - powf(cosf(i / (FLOAT)(size) * M_PI), 8.0f));

		for(ULONG j = 0; j < size; ++j)
		{
			pBits[i * rect.Pitch + j*4 + 0] = 255;
			pBits[i * rect.Pitch + j*4 + 1] = 255;
			pBits[i * rect.Pitch + j*4 + 2] = 255;
			pBits[i * rect.Pitch + j*4 + 3] = value;
		}
	}
	
	mLineTexture->UnlockRect(0);

	/*
	if(FAILED(m_Device->CreateTexture(64,64,1,D3DUSAGE_DYNAMIC,D3DFMT_A8,D3DPOOL_DEFAULT,&m_LineTexture,NULL)))
	{
		Debug::TraceError("Failed to create line texture.\n");
		return false;
	}

	D3DLOCKED_RECT rect;

	if(FAILED(m_LineTexture->LockRect(0,&rect,NULL,D3DLOCK_DISCARD)))
	{
		Debug::TraceError("Failed to lock line texture.\n");
		return false;
	}

	LPBYTE pBits = (LPBYTE)rect.pBits;

	for(ULONG i = 0; i < 64; ++i)
	{
		pBits[i + rect.Pitch*0] = pBits[i + rect.Pitch*63] = 43;
		pBits[i + rect.Pitch*1] = pBits[i + rect.Pitch*62] = 86;
		pBits[i + rect.Pitch*2] = pBits[i + rect.Pitch*61] = 130;
		pBits[i + rect.Pitch*3] = pBits[i + rect.Pitch*60] = 173;
		pBits[i + rect.Pitch*4] = pBits[i + rect.Pitch*59] = 216;

		for(ULONG j = 5; j < 64 - 5; ++j)
			pBits[i + rect.Pitch*j] = 255;
	}
	
	m_LineTexture->UnlockRect(0);
	//*/

	/*
	if(FAILED(m_Device->CreateTexture(8,8,1,D3DUSAGE_DYNAMIC,D3DFMT_A8,D3DPOOL_DEFAULT,&m_LineTexture,NULL)))
	{
		Debug::TraceError("Failed to create line texture.\n");
		return false;
	}

	D3DLOCKED_RECT rect;

	if(FAILED(m_LineTexture->LockRect(0,&rect,NULL,D3DLOCK_DISCARD)))
	{
		Debug::TraceError("Failed to lock line texture.\n");
		return false;
	}

	LPBYTE pBits = (LPBYTE)rect.pBits;

	for(ULONG i = 0; i < 8; ++i)
	{
		pBits[i + rect.Pitch*0] = pBits[i + rect.Pitch*7] = 20;
		pBits[i + rect.Pitch*1] = pBits[i + rect.Pitch*6] = 60;
		pBits[i + rect.Pitch*2] = pBits[i + rect.Pitch*5] = 120;
		pBits[i + rect.Pitch*3] = pBits[i + rect.Pitch*4] = 255;
	}

	m_LineTexture->UnlockRect(0);
	//*/

	mCreatedSprites = true;

	return true;
}

void Graphics::DestroyFonts(void)
{
	mCreatedFonts = false;

	// Delete the fonts
	mFontBold.Destroy();
	mFontUI.Destroy();
	mFontLargeBold.Destroy();
}

void Graphics::DestroyRenderTargets(void)
{
	mCreatedRenderTargets = false;

	// Delete the render targets
	mScenes[0].Destroy();
	mScenes[1].Destroy();
}

void Graphics::DestroyTextures(void)
{
	mCreatedTextures = false;

	// Delete the textures
	mBeatTextures[0].Destroy();
	mBeatTextures[1].Destroy();

	mDailTextures[0].Destroy();
	mDailTextures[1].Destroy();

	mCursor[0].Destroy();

	mBandTextures[0].Destroy();
	mBandTextures[1].Destroy();

	mBandTextures[2].Destroy();
	mBandTextures[3].Destroy();
}

void Graphics::DestroySprites(void)
{
	mCreatedSprites = false;

	SAFERELEASE(mSpriteBuffer);
	SAFERELEASE(mLineTexture);
}

void Graphics::DestroyGraphics(void)
{
	// Restore the gamma
	RestoreGamma();

	mCreated = false;

	// Destroy objects
	DestroyRenderTargets();
	DestroyTextures();
	DestroyFonts();
	DestroySprites();

	// Delete transitions
	if(mTransitions.GetSize())
		mTransitions.Delete();

	// Delete color presets
	if(mColorPresets.GetSize())
		mColorPresets.Delete();

	// Delete visualizations
	if(mVisualizations.GetSize())
		mVisualizations.Delete();

	ClearOutputStrings();

	SAFERELEASE(mDevice);
	SAFERELEASE(mDirect3D);
}

bool Graphics::EnableAnisotropic()
{
	ASSERT(mCreated);

	// Get the device's caps
	D3DCAPS9 caps;
	if(FAILED(mDevice->GetDeviceCaps(&caps)))
	{
		GetKernel()->SetLastError(L"Failed to get device caps.");
		TRACE(L"Failed to get device caps.\n");
		return false;
	}

	// Check if anisotropic min filtering is supported and enabel it if it is
	if(caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC)
	{
		mDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_ANISOTROPIC);
		mDevice->SetSamplerState(0,D3DSAMP_MAXANISOTROPY,caps.MaxAnisotropy);

		TRACE(L"%dx Min Anisotropic filtering enabled.\n",caps.MaxAnisotropy);
	}
	else
		TRACE(L"Min Anisotropic filtering not supported by device.\n");

	// Check if anisotropic mag filtering is supported and enabel it if it is
	if(caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC)
	{
		mDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_ANISOTROPIC);
		mDevice->SetSamplerState(0,D3DSAMP_MAXANISOTROPY,caps.MaxAnisotropy);

		TRACE(L"%dx Mag Anisotropic filtering enabled.\n",caps.MaxAnisotropy);
	}
	else
		TRACE(L"Mag Anisotropic filtering not supported by device.\n");

	return true;
}

void Graphics::DrawLoading()
{
/*
	// Set up a world matrix
	Matrix4 matWorld;
	matWorld.Rotate(Vector3(0.0f,1.0f,0.0f),GetKernel()->GetAbsoluteTime() * 5.0f);

    Vector3 vEyePt(0.0f, 1.0f,-8.0f);
    Vector3 vLookatPt(0.0f, 1.0f, 0.0f);
    Vector3 vUpVec(0.0f, 1.0f, 0.0f);

	// Set up a view matrix
	Matrix4 matView;
	nGetDynamicD3DX()->D3DXMatrixLookAtLH((LPD3DXMATRIX)&matView, (LPD3DXVECTOR3)&vEyePt, (LPD3DXVECTOR3)&vLookatPt, (LPD3DXVECTOR3)&vUpVec);

	// Set up a projection matrix
	Matrix4 matProj;
	nGetDynamicD3DX()->D3DXMatrixPerspectiveFovLH((LPD3DXMATRIX)&matProj,nPI/4, (float)m_PresentParameters.BackBufferWidth/(float)m_PresentParameters.BackBufferHeight, 1.0f, 1000.0f);

	D3DVIEWPORT9 viewPort;
	m_pDevice->GetViewport(&viewPort);

	static const float width = 1.0f;
	static const float height = 1.0f;
	static const float depth = 1.0f;

	// Cube vertices
	Vector3 frontFace[] = {Vector3(-width/2,height/2,-depth/2),Vector3(width/2,height/2,-depth/2),Vector3(width/2,-height/2,-depth/2),Vector3(-width/2,-height/2,-depth/2),Vector3(-width/2,height/2,-depth/2)};
	Vector3 backFace[] = {Vector3(-width/2,height/2,depth/2),Vector3(width/2,height/2,depth/2),Vector3(width/2,-height/2,depth/2),Vector3(-width/2,-height/2,depth/2),Vector3(-width/2,height/2,depth/2)};
	
	Vector3 bottomLeftEdge[] = {Vector3(-width/2,-height/2,depth/2),Vector3(-width/2,-height/2,-depth/2)};
	Vector3 bottomRightEdge[] = {Vector3(width/2,-height/2,depth/2),Vector3(width/2,-height/2,-depth/2)};
	Vector3 topLeftEdge[] = {Vector3(-width/2,height/2,depth/2),Vector3(-width/2,height/2,-depth/2)};
	Vector3 topRightEdge[] = {Vector3(width/2,height/2,depth/2),Vector3(width/2,height/2,-depth/2)};

	nGetDynamicD3DX()->D3DXVec3ProjectArray((LPD3DXVECTOR3)frontFace,sizeof(Vector3),(LPD3DXVECTOR3)frontFace,sizeof(Vector3),&viewPort,&matProj,&matView,&matWorld,5);
	nGetDynamicD3DX()->D3DXVec3ProjectArray((LPD3DXVECTOR3)backFace,sizeof(Vector3),(LPD3DXVECTOR3)backFace,sizeof(Vector3),&viewPort,&matProj,&matView,&matWorld,5);

	nGetDynamicD3DX()->D3DXVec3ProjectArray((LPD3DXVECTOR3)bottomLeftEdge,sizeof(Vector3),(LPD3DXVECTOR3)bottomLeftEdge,sizeof(Vector3),&viewPort,&matProj,&matView,&matWorld,2);
	nGetDynamicD3DX()->D3DXVec3ProjectArray((LPD3DXVECTOR3)bottomRightEdge,sizeof(Vector3),(LPD3DXVECTOR3)bottomRightEdge,sizeof(Vector3),&viewPort,&matProj,&matView,&matWorld,2);
	nGetDynamicD3DX()->D3DXVec3ProjectArray((LPD3DXVECTOR3)topLeftEdge,sizeof(Vector3),(LPD3DXVECTOR3)topLeftEdge,sizeof(Vector3),&viewPort,&matProj,&matView,&matWorld,2);
	nGetDynamicD3DX()->D3DXVec3ProjectArray((LPD3DXVECTOR3)topRightEdge,sizeof(Vector3),(LPD3DXVECTOR3)topRightEdge,sizeof(Vector3),&viewPort,&matProj,&matView,&matWorld,2);

	Vector2 frontFaceScreen[] = {Vector2(frontFace[0].x,frontFace[0].y),Vector2(frontFace[1].x,frontFace[1].y),Vector2(frontFace[2].x,frontFace[2].y),Vector2(frontFace[3].x,frontFace[3].y),Vector2(frontFace[4].x,frontFace[4].y)};
	Vector2 backFaceScreen[] = {Vector2(backFace[0].x,backFace[0].y),Vector2(backFace[1].x,backFace[1].y),Vector2(backFace[2].x,backFace[2].y),Vector2(backFace[3].x,backFace[3].y),Vector2(backFace[4].x,backFace[4].y)};

	Vector2 bottomLeftEdgeScreen[] = {Vector2(bottomLeftEdge[0].x,bottomLeftEdge[0].y),Vector2(bottomLeftEdge[1].x,bottomLeftEdge[1].y)};
	Vector2 bottomRightEdgeScreen[] = {Vector2(bottomRightEdge[0].x,bottomRightEdge[0].y),Vector2(bottomRightEdge[1].x,bottomRightEdge[1].y)};
	Vector2 topLeftEdgeScreen[] = {Vector2(topLeftEdge[0].x,topLeftEdge[0].y),Vector2(topLeftEdge[1].x,topLeftEdge[1].y)};
	Vector2 topRightEdgeScreen[] = {Vector2(topRightEdge[0].x,topRightEdge[0].y),Vector2(topRightEdge[1].x,topRightEdge[1].y)};

	SetLineWidth(3.0f);

	DrawLines(frontFaceScreen,5,GetColorPreset().text[0]);
	DrawLines(backFaceScreen,5,GetColorPreset().text[0]);

	DrawLines(bottomLeftEdgeScreen,2,GetColorPreset().text[0]);
	DrawLines(bottomRightEdgeScreen,2,GetColorPreset().text[0]);
	DrawLines(topLeftEdgeScreen,2,GetColorPreset().text[0]);
	DrawLines(topRightEdgeScreen,2,GetColorPreset().text[0]);
*/

	Size size;
	Size sizel;
	Rect rect;
	Point point;
	String text = L"Initializing \002data\001";

	float delta = 0.5f + 0.5f * sinf(GetKernel()->GetAbsoluteTime() * 10.0f);
	float progressWidth = 4.0f + 80.0f * (delta < 0.5f ? delta : (1.0f - delta));
	float progressHeight = 4.0f;

	GetTextSize(GetBoldFont(),text,&size);

	point = Point(GetDisplayWidth() / 2 - size.cx / 2,GetDisplayHeight() / 2 - size.cy / 2);

	DrawShadowColorText(GetBoldFont(),point,text,GetColorPreset()->text);

	rect = Rect(GetDisplayWidth() / 2 - size.cx / 2,GetDisplayHeight() / 2 + size.cy / 2 + 1,GetDisplayWidth() / 2 + size.cx / 2,GetDisplayHeight() / 2 + size.cy / 2 + 2);

	DrawShadowFillRect(rect,GetColorPreset()->text[0]);

	rect.Offset(0,1 + progressHeight + 1 + 1);

	DrawShadowFillRect(rect,GetColorPreset()->text[0]);

	rect = Rect(GetDisplayWidth() / 2 - size.cx / 2 + size.cx * delta - progressWidth / 2,GetDisplayHeight() / 2 + size.cy / 2 + 3,GetDisplayWidth() / 2 - size.cx / 2 + size.cx * delta + progressWidth / 2,GetDisplayHeight() / 2 + size.cy / 2 + 3 + progressHeight);

	DrawShadowFillRect(rect,GetColorPreset()->text[1]);
}

void Graphics::DrawCursor()
{
	Color color = GetKernel()->GetUserInterface()->GetCursorColor();
	Point hotSpot(0,0);	// TODO Hard-coded

	// Make the cursor fade-in/fade-out when toggled
	if(mDrawCursor)
	{
		float alpha = max(min(GetKernel()->GetAbsoluteTime() - mDrawCursorTime,1.0f),0.0f);
		color.a *= alpha;

		// Auto-hide cursor
		if(alpha == 1.0f && mAutoHideCursorTime > 0.0)
			color.a *= max(min(mAutoHideCursorTime - (GetKernel()->GetAbsoluteTime() - GetKernel()->GetCursorPositionTime()) * 5.0,1.0f),0.0f);
	}
	else
		color.a *= 1.0f - max(min(GetKernel()->GetAbsoluteTime() - mDrawCursorTime,1.0f),0.0f);

	// Draw the cursor shadow
	DrawTexture(mCursor[0],GetKernel()->GetCursorPosition() - hotSpot + Point(1,1),Color(color.a * 0.2f,0,0,0));
	DrawTexture(mCursor[0],GetKernel()->GetCursorPosition() - hotSpot + Point(1,0),Color(color.a * 0.2f,0,0,0));
	DrawTexture(mCursor[0],GetKernel()->GetCursorPosition() - hotSpot + Point(0,1),Color(color.a * 0.2f,0,0,0));

	// Draw the cursor
	DrawTexture(mCursor[0],GetKernel()->GetCursorPosition() - hotSpot,color);
}

void Graphics::DrawStatusText()
{
	// Out text buffers
	wchar_t buffer[256];

	// Display the elapsed time the app has been running for
	long timeElapsed = clock();

	UINT nMs = (timeElapsed/10) % 100;
	DWORD dwSeconds = timeElapsed / 1000;
	UINT nHours = dwSeconds / 60 / 60;
	dwSeconds %= 3600;
	UINT nMins = dwSeconds / 60;
	dwSeconds %= 60;

	_swprintf(buffer,L"|\002%02d\001:\002%02d\001:\002%02d\001:\002%02d\001|",nHours,nMins,dwSeconds,nMs);
	
	// Draw the text
	Size size;
	GetTextSize(GetBoldFont(),buffer,&size);
	
	if(mDrawTimer)
		DrawPrintShadowColorText(GetBoldFont(),Point(mPresentParameters.BackBufferWidth-size.cx-5,5),buffer,(GetKernel()->GetAbsoluteTime() - mDrawTimerTime) * 100,GetColorPreset()->text);
	else
		DrawPrintShadowColorText(GetBoldFont(),Point(mPresentParameters.BackBufferWidth-size.cx-5,5),buffer,wcslen(buffer) - (GetKernel()->GetAbsoluteTime() - mDrawTimerTime) * 100,GetColorPreset()->text);

    // Keep track of the frame count
    static double lasttime = 0.0;
	static bool blink = false;

	// Update the fps stats every second
    if(GetKernel()->GetAbsoluteTime() - lasttime > 1.0f)
    {
		mFps = ANALYZER_FRAMERATE/GetKernel()->GetElapsedTime();
        lasttime = GetKernel()->GetAbsoluteTime();

		blink = !blink;
	}
	
	// Draw the fps stats
	_swprintf(buffer,L"|fps: \002%06.2f\001|",mFps);

	if(mDrawFps)
		DrawPrintShadowColorText(GetBoldFont(),Point(5,5),buffer,(GetKernel()->GetAbsoluteTime() - mDrawFpsTime) * 100,GetColorPreset()->text);
	else
		DrawPrintShadowColorText(GetBoldFont(),Point(5,5),buffer,wcslen(buffer) - (GetKernel()->GetAbsoluteTime() - mDrawFpsTime) * 100,GetColorPreset()->text);

	// Draw the name of the current vis
	_swprintf(buffer,L"|%s|",mVisualizations[mVisualizationIndex]->GetName().GetBuffer());
	
	static double nosignaltime = 0.0;

	// Check if the bands are all NULL and if so, display a "no signal" message
	if(GetKernel()->GetAudio()->GetVu() < 0.01)
	{
		nosignaltime += GetKernel()->GetElapsedTime();

		if(nosignaltime > 50.0f && blink)
			_swprintf(buffer,L"|no signal|");
	}
	else
		nosignaltime = 0.0;

	Size textSize;
	GetTextSize(GetBoldFont(),buffer,&textSize);

	if(mDrawVisualizationName)
		DrawPrintShadowColorText(GetBoldFont(),Point((mPresentParameters.BackBufferWidth - textSize.cx) / 2,5),buffer,(GetKernel()->GetAbsoluteTime() - mDrawVisualizationNameTime) * 100,GetColorPreset()->text);
	else
		DrawPrintShadowColorText(GetBoldFont(),Point((mPresentParameters.BackBufferWidth - textSize.cx) / 2,5),buffer,wcslen(buffer) - (GetKernel()->GetAbsoluteTime() - mDrawVisualizationNameTime) * 100,GetColorPreset()->text);
}

void Graphics::DrawOutputStrings()
{
	// Draw output strings
#define OUTPUSTSTRUNGS_BOTTOM

#ifdef OUTPUSTSTRUNGS_BOTTOM
	float y = mPresentParameters.BackBufferHeight - 5.0f - GetFontHeight(GetBoldFont());
	float dir = -1.0f;
#else
	float y = 5.0f + GetFontHeight(GetBoldFont());
	float dir = 1.0f;
#endif

#ifdef _DEBUG
	static const long MaxPrints = 32;	// Default 32
#else
	static const long MaxPrints = 8;	// Default 8
#endif
	static const float MaxPrintTime = 4.0f;	// Default 4.0
	static const float InAnimTime = 0.2f;	// Default 0.2
	static const float OutAnimTime = 0.2f;	// Default 0.2

	wchar_t buffer[256];

	for(long i = (long)mOutputStrings.GetSize()-MaxPrints; i < (long)mOutputStrings.GetSize(); ++i)
	{
		if(i < 0)
			continue;

		if(GetKernel()->GetAbsoluteTime() - mOutputStrings[i]->time > MaxPrintTime)
			continue;

		// If drawing of outputed strings is disabled only draw the strings that were
		// printed befor the drawing was disables, skip the others
		if(!mDrawOutputStrings && mOutputStrings[i]->time > mDrawOutputStringsTime)
			continue;

		float x = 5.0f;

		float inAnim = (GetKernel()->GetAbsoluteTime() - mOutputStrings[i]->time)/InAnimTime;
		float outAnim = (GetKernel()->GetAbsoluteTime() - mOutputStrings[i]->time - (MaxPrintTime-OutAnimTime))/OutAnimTime;

		inAnim = cubicf(0.0f,1.0f,inAnim);
		outAnim = cubicf(0.0f,1.0f,outAnim);

		Color color[2] = {GetColorPreset()->text[0],GetColorPreset()->text[1]};

		if(GetKernel()->GetAbsoluteTime() - mOutputStrings[i]->time - (MaxPrintTime-OutAnimTime) > 0.0f)
		{
			color[0].a *= 1.0f - outAnim;
			color[1].a *= 1.0f - outAnim;

			y += -dir*(outAnim * GetFontHeight(GetBoldFont()));
			x -= -dir*(outAnim * GetFontHeight(GetBoldFont()));
		}

		_swprintf(buffer,L"|%s|",mOutputStrings[i]->text.GetBuffer());

		DrawPrintShadowColorText(GetBoldFont(),Point(x,y),buffer,(GetKernel()->GetAbsoluteTime() - mOutputStrings[i]->time) * 100,color);
		y += dir*GetFontHeight(GetBoldFont());
	}
}

void Graphics::DrawAbout()
{
	static const float TextSpeed = 100.0f;

	Color color = GetColorPreset()->peak;

	static const wchar_t* titleText = L"Audio Analyzer \002" TOSTRINGW(ANALYZER_VERSION_MAJOR) L"." TOSTRINGW(ANALYZER_VERSION_MINOR) L"\001 by \002Marko Mihovilic\001";
	static const wchar_t* bodyText =
		L"\002Audio Analyzer\001 was written to provide the user with a nice application\n"
		L"to run while \002playing music or other audio\001 to provide \002pretty animations\001\n"
		L"which \002flow with the beat\001 of the sound.\n\n"
		L"It \002renders visual audio data\001. The user can chose from \002various display\n"
		L"visualization types\001. It has a \002totaly dynamic menu system\001 which can\n"
		L"be used to access all of \002Audio Analyzer's\001 features. It supports custom\n"
		L"\002color presets\001 which can be added/removed by modifying the \002presets\n"
		L"file\001. It can manipulate the \002keyboard leds\001 at the beat of the sound.";
	
	static const unsigned long margin = 4;
	static const unsigned long thicks = 2;
	static const unsigned long indent = 10;
	static const unsigned long space = 2;

	Size titleSize;
	Size bodySize;

	GetTextSize(GetLargeBoldFont(),titleText,&titleSize);
	GetTextSize(GetBoldFont(),bodyText,&bodySize);

	titleSize.cy += space;
	bodySize.cx += indent + indent;

	Rect backRect;

	backRect.left = GetDisplayWidth() / 2 - max(titleSize.cx,bodySize.cx) / 2;
	backRect.top = GetDisplayHeight() / 2 - (titleSize.cy + bodySize.cy) / 2;
	
	backRect.SetWidth(max(titleSize.cx,bodySize.cx));
	backRect.SetHeight(titleSize.cy + bodySize.cy);

	backRect.Inflate(margin,margin + thicks);

	//DrawShadowFillRect(backRect,Color(color.a * alpha,color.r,color.g,color.b));

	//DrawFillRect(Rect(backRect.left,backRect.top,backRect.right,backRect.top + backRect.GetHeight() / 3),Color(0.3f * alpha,1.0f,1.0f,1.0f),Color(0.3f * alpha,1.0f,1.0f,1.0f),Color(0.0f * alpha,1.0f,1.0f,1.0f),Color(0.0f * alpha,1.0f,1.0f,1.0f));
	//DrawFillRect(Rect(backRect.left,backRect.top + backRect.GetHeight() / 3*2,backRect.right,backRect.bottom),Color(0.0f * alpha,1.0f,1.0f,1.0f),Color(0.0f * alpha,1.0f,1.0f,1.0f),Color(0.3f * alpha,1.0f,1.0f,1.0f),Color(0.3f * alpha,1.0f,1.0f,1.0f));

	//DrawShadowFillRect(Rect(backRect.left,backRect.top,backRect.right,backRect.top + thicks),Color(color.a * alpha,color.r,color.g,color.b));
	//DrawShadowFillRect(Rect(backRect.left,backRect.bottom - thicks,backRect.right,backRect.bottom),Color(color.a * alpha,color.r,color.g,color.b));

	float time;

	if(mDrawAbout)
		time = (GetKernel()->GetAbsoluteTime() - mDrawAboutTime) * TextSpeed;
	else
		time = wcslen(bodyText) - (GetKernel()->GetAbsoluteTime() - mDrawAboutTime) * TextSpeed;

	DrawPrintShadowColorText(GetLargeBoldFont(),Point(backRect.left + margin,backRect.top + margin + thicks),titleText,time,GetColorPreset()->text);
	DrawPrintShadowColorText(GetBoldFont(),Point(backRect.left + margin + indent,backRect.top + margin + thicks + titleSize.cy),bodyText,time,GetColorPreset()->text);
}

void Graphics::DrawHelp()
{
	static const float TextSpeed = 100.0f;

	Color color = GetColorPreset()->peak;

	static const wchar_t* titleText = L"Audio Analyzer \002" TOSTRINGW(ANALYZER_VERSION_MAJOR) L"." TOSTRINGW(ANALYZER_VERSION_MINOR) L"\001 Keyboard Shortcuts";
	static const wchar_t* bodyText =
		L"\002Page Down\001 - go to next visualization\n"
		L"\002Page Up\001 - go to previous visualization\n"
		L"\002End\001 - set next color preset\n"
		L"\002Home\001 - set previous color preset\n"
		L"\002Insert\001 - toggle drawing of peaks\n"
		L"\002Delete\001 - clear messages\n"
		L"\002Enter\001 or \002Mouse Left\001 - open menu/menu command\n"
		L"\002Backspace\001 or \002Mouse Right\001 - close menu/go back\n"
		L"\002Mouse Middle\001 - open menu with mouse\n"
		L"\002Up/Down\001 or \002Mouse Wheel\001 - change menu selection";
	
	static const unsigned long margin = 4;
	static const unsigned long thicks = 2;
	static const unsigned long indent = 10;
	static const unsigned long space = 2;

	Size titleSize;
	Size bodySize;

	GetTextSize(GetLargeBoldFont(),titleText,&titleSize);
	GetTextSize(GetBoldFont(),bodyText,&bodySize);

	titleSize.cy += space;
	bodySize.cx += indent + indent;

	Rect backRect;

	backRect.left = GetDisplayWidth() / 2 - max(titleSize.cx,bodySize.cx) / 2;
	backRect.top = GetDisplayHeight() / 2 - (titleSize.cy + bodySize.cy) / 2;
	
	backRect.SetWidth(max(titleSize.cx,bodySize.cx));
	backRect.SetHeight(titleSize.cy + bodySize.cy);

	backRect.Inflate(margin,margin + thicks);

	//DrawShadowFillRect(backRect,Color(color.a * alpha,color.r,color.g,color.b));

	//DrawFillRect(Rect(backRect.left,backRect.top,backRect.right,backRect.top + backRect.GetHeight() / 3),Color(0.3f * alpha,1.0f,1.0f,1.0f),Color(0.3f * alpha,1.0f,1.0f,1.0f),Color(0.0f * alpha,1.0f,1.0f,1.0f),Color(0.0f * alpha,1.0f,1.0f,1.0f));
	//DrawFillRect(Rect(backRect.left,backRect.top + backRect.GetHeight() / 3*2,backRect.right,backRect.bottom),Color(0.0f * alpha,1.0f,1.0f,1.0f),Color(0.0f * alpha,1.0f,1.0f,1.0f),Color(0.3f * alpha,1.0f,1.0f,1.0f),Color(0.3f * alpha,1.0f,1.0f,1.0f));

	//DrawShadowFillRect(Rect(backRect.left,backRect.top,backRect.right,backRect.top + thicks),Color(color.a * alpha,color.r,color.g,color.b));
	//DrawShadowFillRect(Rect(backRect.left,backRect.bottom - thicks,backRect.right,backRect.bottom),Color(color.a * alpha,color.r,color.g,color.b));

	float time;

	if(mDrawHelp)
		time = (GetKernel()->GetAbsoluteTime() - mDrawHelpTime) * TextSpeed;
	else
		time = wcslen(bodyText) - (GetKernel()->GetAbsoluteTime() - mDrawHelpTime) * TextSpeed;

	DrawPrintShadowColorText(GetLargeBoldFont(),Point(backRect.left + margin,backRect.top + margin + thicks),titleText,time,GetColorPreset()->text);
	DrawPrintShadowColorText(GetBoldFont(),Point(backRect.left + margin + indent,backRect.top + margin + thicks + titleSize.cy),bodyText,time,GetColorPreset()->text);
}

void Graphics::DrawMenus()
{
	if(GetKernel()->GetMenus())
		GetKernel()->GetMenus()->GetMenuRoot()->RenderMenus();
}

void Graphics::SuspendOutputStrings()
{ 
	static const float MaxPrintTime = 4.0f;
	static const float InAnimTime = 0.2f;
	static const float OutAnimTime = 0.2f;

	for(unsigned long i = 0; i < mOutputStrings.GetSize(); ++i)
		if(GetKernel()->GetAbsoluteTime() - mOutputStrings[i]->time - (MaxPrintTime-OutAnimTime) < 0.0f)
			mOutputStrings[i]->time = GetKernel()->GetAbsoluteTime() - (MaxPrintTime-OutAnimTime);
}

void Graphics::Render()
{
	ASSERT(mCreated);

	HRESULT hr = NULL;

	// Check if device ok
	if(FAILED(hr = mDevice->TestCooperativeLevel()))
	{
		// Yield some CPU time to other processes
		TRACE(L"Sleeping on test coop level (50 ms).\n");
		Sleep(50);

		// The device has been lost but cannot be reset at this time. 
		// Therefore, rendering is not possible and we'll have to return 
		// and try again at a later time.
		if(hr == D3DERR_DEVICELOST)
		{
			GetKernel()->SetLastError(L"Display device lost.");
			TRACE(L"Display device lost.\n");
			Sleep(50);
			return;
		}

		// Read the desktop format since the user could have changed the desktop bitdepth or resolution
		mDirect3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT,&mDisplayMode);

		// The device has been lost but it can be reset at this time. 
		if(hr == D3DERR_DEVICENOTRESET)
		{
			// Try to reset the device
			if(!ResetDevice(mPresentParameters))
				return;
		}
	}

	// Don't render if all objects not created
	if(!mCreatedFonts || !mCreatedRenderTargets || !mCreatedTextures || !mCreatedSprites)
		return;

	// Show the window if not shown already
	if(!GetKernel()->GetWindowVisible())
		GetKernel()->ShowWindow();

	static const float ColorTime = 0.9f;

	double s = (GetKernel()->GetAbsoluteTime() - mColorTime)/ColorTime;

	s = min(1.0f,max(0.0f,s));
	
	// Lerp the current color
	mColor.background.Cosine(mColorPresets[mColorIndex]->background,s);
	mColor.backgroundbl.Cosine(mColorPresets[mColorIndex]->backgroundbl,s);
	mColor.backgroundbr.Cosine(mColorPresets[mColorIndex]->backgroundbr,s);
	mColor.backgroundtl.Cosine(mColorPresets[mColorIndex]->backgroundtl,s);
	mColor.backgroundtr.Cosine(mColorPresets[mColorIndex]->backgroundtr,s);
	mColor.foreground.Cosine(mColorPresets[mColorIndex]->foreground,s);
	mColor.peak.Cosine(mColorPresets[mColorIndex]->peak,s);
	mColor.text[0].Cosine(mColorPresets[mColorIndex]->text[0],s);
	mColor.text[1].Cosine(mColorPresets[mColorIndex]->text[1],s);
	mColor.text[2].Cosine(mColorPresets[mColorIndex]->text[2],s);

	IDirect3DSurface9*	pBackBuffer = NULL;
	IDirect3DSurface9*	pScene[] = {NULL,NULL};

	// Get the backbuffer
	mDevice->GetRenderTarget(0,&pBackBuffer);

	// Get the scene surface
	mScenes[mTransitionScene].GetTexture()->GetSurfaceLevel(0,&pScene[mTransitionScene]);
	mScenes[!mTransitionScene].GetTexture()->GetSurfaceLevel(0,&pScene[!mTransitionScene]);

	// Set the scene rt
	mDevice->SetRenderTarget(0,pScene[mTransitionScene]);

	// Clear the screen with the backgroun color
	mDevice->Clear(0,0,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,GetColorPreset()->background,1.0f,0);

	// Begin the scene
	if(SUCCEEDED(mDevice->BeginScene()))
	{
		SpriteBegin();

		// Draw the vis (gradient) background (which should clear it)
		if(!mVisualizations[mVisualizationIndex]->RenderBackground())
		{
			GetKernel()->SetLastError(L"Failed to render visualization background.\n");
			TRACE(L"Failed to render visualization background \"%s\".\n",mVisualizations[mVisualizationIndex]->GetName().GetBuffer());
		}

		// Draw the vis
		if(!mVisualizations[mVisualizationIndex]->Render())
		{
			GetKernel()->SetLastError(L"Failed to render visualization.\n");
			TRACE(L"Failed to render visualization \"%s\".\n",mVisualizations[mVisualizationIndex]->GetName().GetBuffer());
		}

		// Flush
		SpriteFlush();

		// Restore the backbuffer
		mDevice->SetRenderTarget(0,pBackBuffer);
		
		// Clear the screen with the backgroun color
		//mDevice->Clear(0,0,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,Color(0.0f,1.0f,1.0f,1.0f),1.0f,0);

		// Draw the old scene
		DrawTexture(mScenes[!mTransitionScene],Point(0,0),Color(1.0f,1.0f,1.0f,1.0f));
		mDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
		SpriteFlush();
		mDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);

		// Set the startup transition
		if(!mTransitionTime)
			SetTransition(rand() % GetTransitionCount());

		// Draw the scene
		DrawTextureTransition(mScenes[mTransitionScene],Point(0,0),Color(1.0f,1.0f,1.0f,1.0f),mTransitions[mTransitionIndex],(GetKernel()->GetAbsoluteTime() - mTransitionTime) * 4.0f);	// Increased the animation speed by 4
		mDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
		SpriteFlush();
		mDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);

		// Draw the vis overlay
		if(!mVisualizations[mVisualizationIndex]->RenderOverlay())
		{
			GetKernel()->SetLastError(L"Failed to render visualization.\n");
			TRACE(L"Failed to render visualization \"%s\".\n",mVisualizations[mVisualizationIndex]->GetName().GetBuffer());
		}

		// Draw frame
		//DrawRect(Rect(1,1,mPresentParameters.BackBufferWidth-1,mPresentParameters.BackBufferHeight-1),GetColorPreset()->foreground);

		// Call the ui's OnRender function
		if(GetKernel()->GetUserInterface())
			GetKernel()->GetUserInterface()->OnRender();

		// Draw ui stuff
		DrawStatusText();
		DrawOutputStrings();
		DrawMenus();
		DrawHelp();
		DrawAbout();
		DrawCursor();

		// End 2D drawing
		SpriteEnd();

		// Set the keyboard lights
		if(GetKernel()->GetLed())
			GetKernel()->SetLed(float(GetKernel()->GetAudio()->GetVu(0) + GetKernel()->GetAudio()->GetVu(0)) / 1.2f);

		// End the scene
		mDevice->EndScene();
	}

	// Release the surfaces
	SAFERELEASE(pBackBuffer);
	SAFERELEASE(pScene[0]);
	SAFERELEASE(pScene[1]);

	// Present the scene to the user
	mDevice->Present(NULL,NULL,NULL,NULL);
}

bool Graphics::ResetDevice(const PresentParameters& params)
{
	if(!mDevice)
		return false;

	// If the device can be restored, the application prepares the 
	// device by destroying all video-memory resources and any 
	// swap chains
	if(!InvalidateDeviceObjects())
	{
		GetKernel()->SetLastError(L"Failed to invalidate device objects.");
		TRACE(L"Failed to invalidate device objects.\n");
		return false;
	}

	// Local copy cause Reset modifys the struct
	PresentParameters pp = params;

	// Try to reset the device
	HRESULT hr = mDevice->Reset(&pp);
	if(FAILED(hr))
	{
		// Check if error other than device lost
		if(hr != D3DERR_DEVICELOST)
		{
			DestroyGraphics();
			GetKernel()->SetLastError(L"Failed to reset the display device, destroyed graphics.");
			TRACE(L"Failed to reset the display device, destroyed graphics.\n");
			Sleep(50);
			return false;
		}

		// The device was lost again, so continue waiting until it can be reset
		GetKernel()->SetLastError(L"Failed to reset the display device.");
		TRACE(L"Failed to reset the display device.\n");
		Sleep(50);
		return false;
	}

	// Store the presentation parameters
	mPresentParameters = params;

	TRACE(L"Display Device reset.\n");

	// Finally, a lost device must re-create resources (including  
	// video memory resources) after it has been reset
	if(!RestoreDeviceObjects())
	{
		DestroyGraphics();
		GetKernel()->SetLastError(L"Failed to restore device objects.");
		TRACE(L"Failed to restore device objects.\n");
		Sleep(50);
		return false;
	}

	return true;
}

bool Graphics::InvalidateDeviceObjects()
{
	//
	// TODO Add any device object invalidation here
	//
	DestroySprites();

	mFontBold.OnLostDevice();
	mFontUI.OnLostDevice();
	mFontLargeBold.OnLostDevice();

	DestroyRenderTargets();
	DestroyTextures();

	return true;
}

bool Graphics::RestoreDeviceObjects()
{
	//
	// TODO Add any device object restoration here
	//
	if(!CreateSprites())
	{
		TRACE(L"Failed to restore sprite.\n");
		return false;
	}

	if(!mFontBold.OnResetDevice())
	{
		TRACE(L"Failed to restore bold font.\n");
		return false;
	}

	if(!mFontUI.OnResetDevice())
	{
		TRACE(L"Failed to restore UI font.\n");
		return false;
	}

	if(!mFontLargeBold.OnResetDevice())
	{
		TRACE(L"Failed to restore UI font.\n");
		return false;
	}

	if(!CreateRenderTargets())
	{
		TRACE(L"Failed to restore render targets.\n");
		return false;
	}

	if(!CreateTextures())
	{
		TRACE(L"Failed to restore textures.\n");
		return false;
	}

	return true;
}

void Graphics::DrawShadowFillRect(const Rect& rect,const Color& color)
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	// Draw the rect
	static Rect rect2;
	static Color color2;

	rect2 = rect;
	rect2.Offset(1,1);

	color2 = GetColorPreset()->background;
	//color2.r = color2.g = color2.b = 0.0f;
	color2.Negative();
	color2.a = color.a * 0.1f;

	DrawFillRect(rect2,color2);
	DrawFillRect(rect,color);
}

void Graphics::DrawShadowRect(const Rect& rect,float width,const Color& color,unsigned char flags)
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	// Draw the rect
	static Rect rect2;
	static Color color2;

	rect2 = rect;
	rect2.Offset(1,1);

	color2 = GetColorPreset()->background;
	//color2.r = color2.g = color2.b = 0.0f;
	color2.Negative();
	color2.a = color.a * 0.1f;

	DrawRect(rect2,width,color2,flags);
	DrawRect(rect,width,color,flags);
}

void Graphics::DrawRect(const Rect& rect,float width,const Color& color,unsigned char flags)
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	// Horizontal lines
	if(flags & RectFlagTop)
		DrawFillRect(Rect(rect.left-width,rect.top-width,rect.right+width,rect.top),color);
	if(flags & RectFlagBottom)
		DrawFillRect(Rect(rect.left-width,rect.bottom,rect.right+width,rect.bottom+width),color);

	// Vertical lines
	if(flags & RectFlagLeft)
		DrawFillRect(Rect(rect.left-width,rect.top,rect.left,rect.bottom),color);
	if(flags & RectFlagRight)
		DrawFillRect(Rect(rect.right,rect.top,rect.right+width,rect.bottom),color);
}

void Graphics::DrawShadowCornerRect(const Rect& rect,float size,float width,const Color& color,unsigned char flags)
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	// Draw the rect
	static Rect rect2;
	static Color color2;

	rect2 = rect;
	rect2.Offset(1,1);

	color2 = GetColorPreset()->background;
	//color2.r = color2.g = color2.b = 0.0f;
	color2.Negative();
	color2.a = color.a * 0.1f;

	DrawCornerRect(rect2,size,width,color2,flags);
	DrawCornerRect(rect,size,width,color,flags);
}

void Graphics::DrawCornerRect(const Rect& rect,float size,float width,const Color& color,unsigned char flags)
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	// Horizontal lines
	if(flags & RectFlagTop)
	{
		DrawFillRect(Rect(rect.left-width,rect.top-width,rect.left+size+width,rect.top),color);
		DrawFillRect(Rect(rect.right-width-size,rect.top-width,rect.right+width,rect.top),color);
	}
	if(flags & RectFlagBottom)
	{
		DrawFillRect(Rect(rect.left-width,rect.bottom,rect.left+size+width,rect.bottom+width),color);
		DrawFillRect(Rect(rect.right-width-size,rect.bottom,rect.right+width,rect.bottom+width),color);
	}

	// Vertical lines
	if(flags & RectFlagLeft)
	{
		DrawFillRect(Rect(rect.left-width,rect.top,rect.left,rect.top+size),color);
		DrawFillRect(Rect(rect.left-width,rect.bottom-size,rect.left,rect.bottom),color);
	}
	if(flags & RectFlagRight)
	{
		DrawFillRect(Rect(rect.right,rect.top,rect.right+width,rect.top+size),color);
		DrawFillRect(Rect(rect.right,rect.bottom-size,rect.right+width,rect.bottom),color);
	}
}

void Graphics::DrawShadowCenterRect(const Rect& rect,float size,float width,const Color& color,unsigned char flags)
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	// Draw the rect
	static Rect rect2;
	static Color color2;

	rect2 = rect;
	rect2.Offset(1,1);

	color2 = GetColorPreset()->background;
	//color2.r = color2.g = color2.b = 0.0f;
	color2.Negative();
	color2.a = color.a * 0.1f;

	DrawCenterRect(rect2,size,width,color2,flags);
	DrawCenterRect(rect,size,width,color,flags);
}

void Graphics::DrawCenterRect(const Rect& rect,float size,float width,const Color& color,unsigned char flags)
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	// Horizontal lines
	if(flags & RectFlagTop)
		DrawFillRect(Rect(rect.left-width+(rect.GetWidth()-size)/2,rect.top-width,rect.right+width-(rect.GetWidth()-size)/2,rect.top),color);
	if(flags & RectFlagBottom)
		DrawFillRect(Rect(rect.left-width+(rect.GetWidth()-size)/2,rect.bottom,rect.right+width-(rect.GetWidth()-size)/2,rect.bottom+width),color);

	// Vertical lines
	if(flags & RectFlagLeft)
		DrawFillRect(Rect(rect.left-width,rect.top+(rect.GetHeight()-size)/2,rect.left,rect.bottom-(rect.GetHeight()-size)/2),color);
	if(flags & RectFlagRight)
		DrawFillRect(Rect(rect.right,rect.top+(rect.GetHeight()-size)/2,rect.right+width,rect.bottom-(rect.GetHeight()-size)/2),color);
}

void Graphics::DrawShadowText(const Font& font,const Point& point,const wchar_t* text,const Color& color)
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	// Draw the text
	static Point point2;
	static Color color2;

	point2 = point;
	point2.Offset(1,1);

	color2 = GetColorPreset()->background;
	//color2.r = color2.g = color2.b = 0.0f;
	color2.Negative();
	color2.a = color.a * 0.1f;

	font.DrawText(point2,text,0,color2);
	font.DrawText(point,text,0,color);
}

void Graphics::DrawText(const Font& font,const Point& point,const wchar_t* text,const Color& color)
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	// Draw the text
	font.DrawText(point,text,0,color);
}

void Graphics::DrawShadowColorText(const Font& font,const Point& point,const wchar_t* text,const Color* colors)
{
	// Don't draw if fully transparent
	if(colors->a <= 0.0f)
		return;

	// Draw the text
	static Point point2;
	static Color color2;

	point2 = point;
	point2.Offset(1,1);

	color2 = GetColorPreset()->background;
	//color2.r = color2.g = color2.b = 0.0f;
	color2.Negative();
	color2.a = colors->a * 0.1f;

	font.DrawText(point2,text,0,color2);
	font.DrawColorText(point,text,0,colors);
}

void Graphics::DrawColorText(const Font& font,const Point& point,const wchar_t* text,const Color* colors)
{
	// Don't draw if fully transparent
	if(colors->a <= 0.0f)
		return;

	// Draw the text
	font.DrawColorText(point,text,0,colors);
}

void Graphics::DrawPrintText(const Font& font,const Point& point,const wchar_t* text,float delta,const Color& color)
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	static const float BlinkTime = 50.0f;

	String textString(text);
	Color color2(color);
	
	if(delta < -BlinkTime)
		color2.a = 0.0f;
	else if(delta < 0.0f)
		color2.a *= sinf(-delta / 2.0f);
	else if(delta > textString.GetLength() + BlinkTime)
		color2.a = 0.0f;
	else if(delta > textString.GetLength())
		color2.a *= sinf(delta / 2.0f);

	unsigned long numChars = min(max(0.0f,delta),textString.GetLength());
	numChars = textString.GetLength() * cubicf(0.0f,1.0f,(float)numChars/(float)textString.GetLength());	// Comment-out for no cubic interpolation
	textString.Erase(numChars,textString.GetLength() - numChars);

	Point block;
	GetTextBlock(font,textString,&block);

	Size space;
	GetTextSize(font,L" ",&space);
	space.cx *= 2.0f;

	// Draw the text
	DrawText(font,point,textString,color);

	// Draw block
	DrawFillRect(Rect(point.x + 2 + block.x,point.y + block.y + space.cy - space.cy / 5,point.x + 2 + block.x + space.cx,point.y + block.y + space.cy),color2);
}

void Graphics::DrawPrintShadowText(const Font& font,const Point& point,const wchar_t* text,float delta,const Color& color)
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	static const float BlinkTime = 50.0f;

	String textString(text);
	Color color2(color);

	if(delta < -BlinkTime)
		color2.a = 0.0f;
	else if(delta < 0.0f)
		color2.a *= sinf(-delta / 2.0f);
	else if(delta > textString.GetLength() + BlinkTime)
		color2.a = 0.0f;
	else if(delta > textString.GetLength())
		color2.a *= sinf(delta / 2.0f);

	unsigned long numChars = min(max(0.0f,delta),textString.GetLength());
	numChars = textString.GetLength() * cubicf(0.0f,1.0f,(float)numChars/(float)textString.GetLength());	// Comment-out for no cubic interpolation
	textString.Erase(numChars,textString.GetLength() - numChars);

	Point block;
	GetTextBlock(font,textString,&block);

	Size space;
	GetTextSize(font,L" ",&space);
	space.cx *= 2.0f;

	// Draw the text
	DrawShadowText(font,point,textString,color);
	
	// Draw block
	DrawShadowFillRect(Rect(point.x + 2 + block.x,point.y + block.y + space.cy - space.cy / 5,point.x + 2 + block.x + space.cx,point.y + block.y + space.cy),color2);
}

void Graphics::DrawPrintShadowColorText(const Font& font,const Point& point,const wchar_t* text,float delta,const Color* colors)
{
	// Don't draw if fully transparent
	if(colors->a <= 0.0f)
		return;

	static const float BlinkTime = 50.0f;

	String textString(text);
	Color color2(colors[0]);

	if(delta < -BlinkTime)
		color2.a = 0.0f;
	else if(delta < 0.0f)
		color2.a *= sinf(-delta / 2.0f);
	else if(delta > textString.GetLength() + BlinkTime)
		color2.a = 0.0f;
	else if(delta > textString.GetLength())
		color2.a *= sinf(delta / 2.0f);

	unsigned long numChars = min(max(0.0f,delta),textString.GetLength());
	numChars = textString.GetLength() * cubicf(0.0f,1.0f,(float)numChars/(float)textString.GetLength());	// Comment-out for no cubic interpolation
	textString.Erase(numChars,textString.GetLength() - numChars);

	Point block;
	GetTextBlock(font,textString,&block);

	Size space;
	GetTextSize(font,L" ",&space);
	space.cx *= 2.0f;

	// Draw the text
	DrawShadowColorText(font,point,textString,colors);
	
	// Draw block
	Rect caret(point.x + 2 + block.x,point.y + block.y + space.cy - space.cy / 5,point.x + block.x + 2 + space.cx,point.y + block.y + space.cy);
	DrawShadowFillRect(caret,color2);
}

void Graphics::GetTextRect(const Font& font,const wchar_t* text,Rect* rect)
{
	font.GetTextRect(text,0,rect);
}

void Graphics::GetTextSize(const Font& font,const wchar_t* text,Size* size)
{
	font.GetTextSize(text,0,size);
}

void Graphics::GetTextBlock(const Font& font,const wchar_t* text,Point* block)
{
	font.GetTextBlock(text,0,block);
}

String GetPrivateProfileString(LPCWSTR lpAppName,LPCWSTR lpKeyName,LPCWSTR lpDefault,LPCWSTR lpFileName)
{
	TCHAR buffer[512];
	if(!GetPrivateProfileString(lpAppName,lpKeyName,lpDefault,buffer,_countof(buffer),lpFileName))
		return lpDefault;

	return buffer;
}

Color GetPrivateProfileColor(LPCWSTR lpAppName,LPCWSTR lpKeyName,const Color& lpDefault,LPCWSTR lpFileName)
{
	TCHAR buffer[512];
	if(!GetPrivateProfileString(lpAppName,lpKeyName,NULL,buffer,_countof(buffer),lpFileName))
		return lpDefault;

	if(!lstrlen(buffer))
		return lpDefault;

	return ColorFromString(buffer);
}

bool Graphics::ReadColorPresets(const wchar_t* filePath)
{
	String path;

	if(!filePath)
		path = GetKernel()->GetCombinedModulePath(L"Presets.ini");
	else
		path = filePath;

	TCHAR sections[4096];
	TCHAR section[512];
	LPCTSTR psections = sections;
	LPTSTR psection = section;

	GetPrivateProfileSectionNames(sections,_countof(sections),path);

	for(;;)
	{
		if(!psections[0])
		{
			psection[0] = 0;

			if(!lstrlen(section))
				break;

			//if(!lstrcmpi(section,TEXT("Presets")))
			//{
			//	authorName = GetPrivateProfileString();
			//}
			//else
			//{
				ColorPreset* preset = new ColorPreset;

				preset->name = GetPrivateProfileString(section,TEXT("id"),section,path);
				preset->foreground = GetPrivateProfileColor(section,TEXT("foreground"),mColorPresets[0]->foreground,path);
				preset->background = GetPrivateProfileColor(section,TEXT("background"),mColorPresets[0]->background,path);
				preset->text[0] = GetPrivateProfileColor(section,TEXT("text1"),mColorPresets[0]->text[0],path);
				preset->text[1] = GetPrivateProfileColor(section,TEXT("text2"),ColorGamma(preset->text[0],0.5f),path);
				preset->text[2] = GetPrivateProfileColor(section,TEXT("text3"),ColorGamma(preset->text[0],1.5f),path);
				preset->peak = GetPrivateProfileColor(section,TEXT("peak"),mColorPresets[0]->peak,path);
				preset->backgroundbl = GetPrivateProfileColor(section,TEXT("backgroundbl"),mColorPresets[0]->backgroundbl,path);
				preset->backgroundbr = GetPrivateProfileColor(section,TEXT("backgroundbr"),mColorPresets[0]->backgroundbr,path);
				preset->backgroundtl = GetPrivateProfileColor(section,TEXT("backgroundtl"),mColorPresets[0]->backgroundtl,path);
				preset->backgroundtr = GetPrivateProfileColor(section,TEXT("backgroundtr"),mColorPresets[0]->backgroundtr,path);

				AddColorPreset(preset);
			//}

			++psections;

			if(!psections[0])
				break;
			
			psection = section;
		}

		psection[0] = psections[0];

		++psections;
		++psection;
	}

	PrintOutputString(L"color presets loaded: \002%s\001",GetKernel()->GetStripedPath(path).GetBuffer());

	TRACE(L"Color preset file \"%s\" successfully loaded.\n",path.GetBuffer());
	return true;
}

bool Graphics::SpriteBegin(void)
{
	// Set-up for 2D drawing
    mDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
    mDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
    mDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
    mDevice->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);

    mDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG2);
    mDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);

    mDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
    mDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);

    mDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
    mDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
    mDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
    
    mDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
    mDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
    mDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);

    mDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	mDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);

	mDevice->SetRenderState(D3DRS_ZENABLE,FALSE);

	if(FAILED(mSpriteBuffer->Lock(NULL,sizeof(SpriteVertex) * SPRITE_CACHE,(LPVOID*)&mSpriteData,D3DLOCK_DISCARD)))
	{
		TRACE(L"Failed to lock vertex buffer size %d bytes.\n",sizeof(SpriteVertex) * SPRITE_CACHE);
		return false;
	}

	mSpriteCount = NULL;
	mSpriteTexture = NULL;

	return true;
}

void Graphics::SpriteEnd(void)
{
	SpriteFlush();

	mSpriteBuffer->Unlock();
}

bool Graphics::SpriteFlush(void)
{
	ASSERT(mSpriteBuffer);

	if(!mSpriteCount)
		return true;

	mSpriteBuffer->Unlock();
	mSpriteData = NULL;

	mDevice->SetFVF(SpriteVertexFormat);
	mDevice->SetTexture(0,mSpriteTexture);
	mDevice->SetStreamSource(0,mSpriteBuffer,0,sizeof(SpriteVertex));
	mDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,mSpriteCount / 3);

	mSpriteCount = NULL;
	mSpriteTexture = NULL;

	if(FAILED(mSpriteBuffer->Lock(NULL,sizeof(SpriteVertex) * SPRITE_CACHE,(LPVOID*)&mSpriteData,D3DLOCK_DISCARD)))
	{
		TRACE(L"Failed to lock vertex buffer size %d bytes.\n",sizeof(SpriteVertex) * SPRITE_CACHE);
		return false;
	}

	return true;
}

void Graphics::SetClipper(const Rect* rect)
{
	ASSERT(mDevice);

	if(rect)
	{
		mDevice->SetRenderState(D3DRS_SCISSORTESTENABLE,TRUE);
		mDevice->SetScissorRect((LPRECT)rect);
	}
	else
		mDevice->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE);
}

bool Graphics::GetClipper(Rect* rect)
{
	ASSERT(mDevice);

	unsigned long val = 0;
	mDevice->GetRenderState(D3DRS_SCISSORTESTENABLE,&val);
	
	if(rect)
		mDevice->GetScissorRect((LPRECT)rect);

	return val ? true : false;
}

bool Graphics::SpriteWrite(const Vector2& topleft,const Vector2& topright,const Vector2& bottomright,const Vector2& bottomleft,const Vector2& ttopleft,const Vector2& tbottomright,const Color& ctopleft,const Color& ctopright,const Color& cbottomright,const Color& cbottomleft,IDirect3DTexture9* texture)
{
	if(mSpriteCount + 6 >= SPRITE_CACHE || (mSpriteCount && mSpriteTexture != texture))
		if(!SpriteFlush())
			return false;

	mSpriteData[mSpriteCount].color = ctopleft;
	mSpriteData[mSpriteCount].texture.x = ttopleft.x;
	mSpriteData[mSpriteCount].texture.y = ttopleft.y;
	mSpriteData[mSpriteCount].position.z = 0.5f;
	mSpriteData[mSpriteCount].position.w = 1.0f;
	mSpriteData[mSpriteCount].position.x = topleft.x - 0.5f;
	mSpriteData[mSpriteCount].position.y = topleft.y - 0.5f;
	++mSpriteCount;

	mSpriteData[mSpriteCount].color = ctopright;
	mSpriteData[mSpriteCount].texture.x = tbottomright.x;
	mSpriteData[mSpriteCount].texture.y = ttopleft.y;
	mSpriteData[mSpriteCount].position.z = 0.5f;
	mSpriteData[mSpriteCount].position.w = 1.0f;
	mSpriteData[mSpriteCount].position.x = topright.x - 0.5f;
	mSpriteData[mSpriteCount].position.y = topright.y - 0.5f;
	++mSpriteCount;

	mSpriteData[mSpriteCount].color = cbottomright;
	mSpriteData[mSpriteCount].texture.x = tbottomright.x;
	mSpriteData[mSpriteCount].texture.y = tbottomright.y;
	mSpriteData[mSpriteCount].position.z = 0.5f;
	mSpriteData[mSpriteCount].position.w = 1.0f;
	mSpriteData[mSpriteCount].position.x = bottomright.x - 0.5f;
	mSpriteData[mSpriteCount].position.y = bottomright.y - 0.5f;
	++mSpriteCount;

	mSpriteData[mSpriteCount].color = cbottomright;
	mSpriteData[mSpriteCount].texture.x = tbottomright.x;
	mSpriteData[mSpriteCount].texture.y = tbottomright.y;
	mSpriteData[mSpriteCount].position.z = 0.5f;
	mSpriteData[mSpriteCount].position.w = 1.0f;
	mSpriteData[mSpriteCount].position.x = bottomright.x - 0.5f;
	mSpriteData[mSpriteCount].position.y = bottomright.y - 0.5f;
	++mSpriteCount;

	mSpriteData[mSpriteCount].color = cbottomleft;
	mSpriteData[mSpriteCount].texture.x = ttopleft.x;
	mSpriteData[mSpriteCount].texture.y = tbottomright.y;
	mSpriteData[mSpriteCount].position.z = 0.5f;
	mSpriteData[mSpriteCount].position.w = 1.0f;
	mSpriteData[mSpriteCount].position.x = bottomleft.x - 0.5f;
	mSpriteData[mSpriteCount].position.y = bottomleft.y - 0.5f;
	++mSpriteCount;

	mSpriteData[mSpriteCount].color = ctopleft;
	mSpriteData[mSpriteCount].texture.x = ttopleft.x;
	mSpriteData[mSpriteCount].texture.y = ttopleft.y;
	mSpriteData[mSpriteCount].position.z = 0.5f;
	mSpriteData[mSpriteCount].position.w = 1.0f;
	mSpriteData[mSpriteCount].position.x = topleft.x - 0.5f;
	mSpriteData[mSpriteCount].position.y = topleft.y - 0.5f;
	++mSpriteCount;

	mSpriteTexture = texture;

	return true;
}

bool Graphics::SpriteWrite(const Vector2& topleft,const Vector2& topright,const Vector2& bottomright,const Vector2& bottomleft,const Vector2& ttopleft,const Vector2& tbottomright,const Color& ctopleft,const Color& ctopright,const Color& cbottomright,const Color& cbottomleft,const Vector2& center,float angle,IDirect3DTexture9* texture)
{
	if(mSpriteCount + 6 >= SPRITE_CACHE || (mSpriteCount && mSpriteTexture != texture))
		if(!SpriteFlush())
			return false;

	float cangle = cosf(angle);
	float sangle = sinf(angle);

	mSpriteData[mSpriteCount].color = ctopleft;
	mSpriteData[mSpriteCount].texture.x = ttopleft.x;
	mSpriteData[mSpriteCount].texture.y = ttopleft.y;
	mSpriteData[mSpriteCount].position.z = 0.5f;
	mSpriteData[mSpriteCount].position.w = 1.0f;
	mSpriteData[mSpriteCount].position.x = center.x + ((topleft.x - center.x - 0.5f) * cangle - (topleft.y - center.y - 0.5f) * sangle) - 0.5f;
	mSpriteData[mSpriteCount].position.y = center.y + ((topleft.y - center.y - 0.5f) * cangle + (topleft.x - center.x - 0.5f) * sangle) - 0.5f;
	++mSpriteCount;

	mSpriteData[mSpriteCount].color = ctopright;
	mSpriteData[mSpriteCount].texture.x = tbottomright.x;
	mSpriteData[mSpriteCount].texture.y = ttopleft.y;
	mSpriteData[mSpriteCount].position.z = 0.5f;
	mSpriteData[mSpriteCount].position.w = 1.0f;
	mSpriteData[mSpriteCount].position.x = center.x + ((topright.x - center.x - 0.5f) * cangle - (topright.y - center.y - 0.5f) * sangle) - 0.5f;
	mSpriteData[mSpriteCount].position.y = center.y + ((topright.y - center.y - 0.5f) * cangle + (topright.x - center.x - 0.5f) * sangle) - 0.5f;
	++mSpriteCount;

	mSpriteData[mSpriteCount].color = cbottomright;
	mSpriteData[mSpriteCount].texture.x = tbottomright.x;
	mSpriteData[mSpriteCount].texture.y = tbottomright.y;
	mSpriteData[mSpriteCount].position.z = 0.5f;
	mSpriteData[mSpriteCount].position.w = 1.0f;
	mSpriteData[mSpriteCount].position.x = center.x + ((bottomright.x - center.x - 0.5f) * cangle - (bottomright.y - center.y - 0.5f) * sangle) - 0.5f;
	mSpriteData[mSpriteCount].position.y = center.y + ((bottomright.y - center.y - 0.5f) * cangle + (bottomright.x - center.x - 0.5f) * sangle) - 0.5f;
	++mSpriteCount;

	mSpriteData[mSpriteCount].color = cbottomright;
	mSpriteData[mSpriteCount].texture.x = tbottomright.x;
	mSpriteData[mSpriteCount].texture.y = tbottomright.y;
	mSpriteData[mSpriteCount].position.z = 0.5f;
	mSpriteData[mSpriteCount].position.w = 1.0f;
	mSpriteData[mSpriteCount].position.x = center.x + ((bottomright.x - center.x - 0.5f) * cangle - (bottomright.y - center.y - 0.5f) * sangle) - 0.5f;
	mSpriteData[mSpriteCount].position.y = center.y + ((bottomright.y - center.y - 0.5f) * cangle + (bottomright.x - center.x - 0.5f) * sangle) - 0.5f;
	++mSpriteCount;

	mSpriteData[mSpriteCount].color = cbottomleft;
	mSpriteData[mSpriteCount].texture.x = ttopleft.x;
	mSpriteData[mSpriteCount].texture.y = tbottomright.y;
	mSpriteData[mSpriteCount].position.z = 0.5f;
	mSpriteData[mSpriteCount].position.w = 1.0f;
	mSpriteData[mSpriteCount].position.x = center.x + ((bottomleft.x - center.x - 0.5f) * cangle - (bottomleft.y - center.y - 0.5f) * sangle) - 0.5f;
	mSpriteData[mSpriteCount].position.y = center.y + ((bottomleft.y - center.y - 0.5f) * cangle + (bottomleft.x - center.x - 0.5f) * sangle) - 0.5f;
	++mSpriteCount;

	mSpriteData[mSpriteCount].color = ctopleft;
	mSpriteData[mSpriteCount].texture.x = ttopleft.x;
	mSpriteData[mSpriteCount].texture.y = ttopleft.y;
	mSpriteData[mSpriteCount].position.z = 0.5f;
	mSpriteData[mSpriteCount].position.w = 1.0f;
	mSpriteData[mSpriteCount].position.x = center.x + ((topleft.x - center.x - 0.5f) * cangle - (topleft.y - center.y - 0.5f) * sangle) - 0.5f;
	mSpriteData[mSpriteCount].position.y = center.y + ((topleft.y - center.y - 0.5f) * cangle + (topleft.x - center.x - 0.5f) * sangle) - 0.5f;
	++mSpriteCount;

	mSpriteTexture = texture;

	return true;
}

void Graphics::DrawFillRect(const Rect& rect,const Color& color)
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	SpriteWrite(Vector2(rect.left,rect.top),Vector2(rect.right,rect.top),Vector2(rect.right,rect.bottom),Vector2(rect.left,rect.bottom),Vector2(0.0f,0.0f),Vector2(1.0f,1.0f),color,color,color,color,NULL);
}

void Graphics::DrawFillRect(const Rect& rect,const Color& colorTl,const Color& colorTr,const Color& colorBr,const Color& colorBl)
{
	// Don't draw if fully transparent
	if(colorTl.a <= 0.0f && colorTr.a <= 0.0f && colorBr.a <= 0.0f && colorBl.a <= 0.0f)
		return;

	SpriteWrite(Vector2(rect.left,rect.top),Vector2(rect.right,rect.top),Vector2(rect.right,rect.bottom),Vector2(rect.left,rect.bottom),Vector2(0.0f,0.0f),Vector2(1.0f,1.0f),colorTl,colorTr,colorBr,colorBl,NULL);
}

void Graphics::DrawLine(const Vector2& start,const Vector2& end,const Color& color,float width)
{
	float length = (end - start).Length();

	float x = width * (end.y - start.y)/length;
	float y = width * -(end.x - start.x)/length;

	//SpriteWrite(Vector2(220,210),Vector2(220+100,300),Vector2(210+100,300),Vector2(210,210),Vector2(0,0),Vector2(1,1),color,mLineTexture);
	//SpriteWrite(Vector2(207,207),Vector2(307,307),Vector2(293,293),Vector2(193,193),Vector2(0,0),Vector2(1,1),color,NULL);

	Vector2 topleft(start.x + x,start.y + y);
	Vector2 topright(end.x + x,end.y + y);
	Vector2 bottomright(end.x - x,end.y - y);
	Vector2 bottomleft(start.x - x,start.y - y);

//#error Fix this

	SpriteWrite(topleft,topright,bottomright,bottomleft,Vector2(0,0),Vector2(1,1),color,color,color,color,mLineTexture);
}

void Graphics::DrawLines(const Vector2* lines,unsigned long count,const Color& color,float width)
{
	for(unsigned long i = 1; i < count; ++i)
		DrawLine(lines[i-1],lines[i],color,width);
}

void Graphics::DrawTexture(const Texture& texture,const Point& point,const Color& color)
{
	D3DSURFACE_DESC desc;
	texture.GetTexture()->GetLevelDesc(0,&desc);
	DrawTexture(texture,Rect(point.x,point.y,point.x+desc.Width,point.y+desc.Height),color);
}

void Graphics::DrawTexture(const Texture& texture,const Rect& rect,const Color& color)
{
	if(color.a <= 0.0f)
		return;

	SpriteWrite(Vector2(rect.left,rect.top),Vector2(rect.right,rect.top),Vector2(rect.right,rect.bottom),Vector2(rect.left,rect.bottom),Vector2(0.0f,0.0f),Vector2(1.0f,1.0f),color,color,color,color,texture.GetTexture());
}

void Graphics::DrawTexture(const Texture& texture,const Point& point,const Point& center,float angle,const Color& color)
{
	D3DSURFACE_DESC desc;
	texture.GetTexture()->GetLevelDesc(0,&desc);
	DrawTexture(texture,Rect(point.x,point.y,point.x+desc.Width,point.y+desc.Height),center,angle,color);
}

void Graphics::DrawTexture(const Texture& texture,const Point& point,float angle,const Color& color)
{
	D3DSURFACE_DESC desc;
	texture.GetTexture()->GetLevelDesc(0,&desc);
	DrawTexture(texture,Rect(point.x,point.y,point.x+desc.Width,point.y+desc.Height),angle,color);
}

void Graphics::DrawTexture(const Texture& texture,const Rect& rect,float angle,const Color& color)
{
	SpriteWrite(Vector2(rect.left,rect.top),Vector2(rect.right,rect.top),Vector2(rect.right,rect.bottom),Vector2(rect.left,rect.bottom),Vector2(0.0f,0.0f),Vector2(1.0f,1.0f),color,color,color,color,Vector2(rect.left/2+rect.right/2,rect.top/2+rect.bottom/2),angle,texture.GetTexture());
}

void Graphics::DrawTexture(const Texture& texture,const Rect& rect,const Point& center,float angle,const Color& color)
{
	SpriteWrite(Vector2(rect.left,rect.top),Vector2(rect.right,rect.top),Vector2(rect.right,rect.bottom),Vector2(rect.left,rect.bottom),Vector2(0.0f,0.0f),Vector2(1.0f,1.0f),color,color,color,color,Vector2(center.x,center.y),angle,texture.GetTexture());
}

void Graphics::PrintOutputString(const wchar_t* fmt,...)
{
#ifdef _DEBUG
	static const long MaxPrints = 26;	// Default 26
#else
	static const long MaxPrints = 8;	// Default 8
#endif
	static const float MaxPrintTime = 4.0f;	// Default 4.0
	static const float InAnimTime = 0.2f;	// Default 0.2
	static const float OutAnimTime = 0.2f;	// Default 0.2

	static wchar_t buffer[1024];
    
    va_list args;
    va_start(args,fmt);
	_vsnwprintf(buffer,_countof(buffer),fmt,args);
    va_end(args);

	static const unsigned long StringLimit = 256;
	// Remove the first string if we are over our limit
	if(mOutputStrings.GetSize() > StringLimit)
		mOutputStrings.DeleteFront();

	OutputString* output = new OutputString;
	ASSERT(output);

	output->text = buffer;
	output->time = GetKernel()->GetAbsoluteTime();

	mOutputStrings.InsertBack(output);

	// Reset the time of the last message so it gets pushed down to make room for the new one
	if(mOutputStrings.GetSize() > MaxPrints)
		if(GetKernel()->GetAbsoluteTime() - mOutputStrings[mOutputStrings.GetSize()-MaxPrints]->time - (MaxPrintTime-OutAnimTime) < 0.0f)
			mOutputStrings[mOutputStrings.GetSize()-MaxPrints]->time = GetKernel()->GetAbsoluteTime() - (MaxPrintTime-OutAnimTime);
}

void Graphics::SetDrawHelp(bool draw)
{
	if(mDrawHelp == draw)
		return;

	TRACE(L"Help turned %s.\n",draw ? L"on" : L"off"); 
	PrintOutputString(L"help: \002%s\001",draw ? L"on" : L"off");

	mDrawHelp = draw; 
	mDrawHelpTime = GetKernel()->GetAbsoluteTime();
}

void Graphics::SetDrawOutputStrings(bool draw)
{
	if(mDrawOutputStrings == draw)
		return;

	mDrawOutputStrings = draw;

	if(!draw)
		SuspendOutputStrings();

	TRACE(L"Output strings drawing turned %s.\n",draw ? L"on" : L"off"); 
	PrintOutputString(L"output strings: \002%s\001",draw ? L"on" : L"off");

	mDrawOutputStringsTime = GetKernel()->GetAbsoluteTime();
}

bool Graphics::SetColorPreset(unsigned long index)
{
	if(index >= mColorPresets.GetSize())
		return false;

	if(index != mColorIndex)
	{
		TRACE(L"Color preset switched to \"%s\".\n",mColorPresets[index]->name.GetBuffer()); 
		PrintOutputString(L"color preset: \002%s\001",mColorPresets[index]->name.GetBuffer());
	}

	mColorIndex = index;
	mColorTime = GetKernel()->GetAbsoluteTime();

	// If this is the first preset (doesn't have a name), switch to the new one now
	if(!mColor.name.GetLength())
		mColor = *mColorPresets[mColorIndex];
	else	// Else just set the name
		mColor.name = mColorPresets[mColorIndex]->name;

	return true; 
}

void Graphics::DrawTextureTransition(const Texture& texture,const Point& point,const Color& color,Transition* effect,double time)
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	D3DSURFACE_DESC desc;
	texture.GetTexture()->GetLevelDesc(0,&desc);
	DrawTextureTransition(texture,Rect(point.x,point.y,point.x+desc.Width,point.y+desc.Height),color,effect,time);
}

void Graphics::DrawTextureTransition(const Texture& texture,const Rect& rect,const Color& color,Transition* effect,double time)
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	Rect rectOut = rect;
	Color colorOut = color;

	effect->CalcTransition(texture,&rectOut,&colorOut,time);

	DrawTexture(texture,rectOut,colorOut);
}

void Graphics::SetDrawFps(bool draw)
{
	if(mDrawFps == draw)
		return;

	TRACE(L"Fps drawing turned %s.\n",draw ? L"on" : L"off");
	PrintOutputString(L"fps: \002%s\001",draw ? L"on" : L"off");
	
	mDrawFps = draw;
	mDrawFpsTime = GetKernel()->GetAbsoluteTime();
}

void Graphics::SetDrawTimer(bool draw)
{
	if(mDrawTimer == draw)
		return;

	TRACE(L"Timer drawing turned %s.\n",draw ? L"on" : L"off");
	PrintOutputString(L"timer: \002%s\001",draw ? L"on" : L"off");
	
	mDrawTimer = draw;
	mDrawTimerTime = GetKernel()->GetAbsoluteTime();
}

void Graphics::SetDrawPeaks(bool draw)
{ 
	if(mDrawPeaks == draw)
		return;

	TRACE(L"Peak drawing turned %s.\n",draw ? L"on" : L"off");
	PrintOutputString(L"peaks: \002%s\001",draw ? L"on" : L"off");
	 
	mDrawPeaks = draw;
	mDrawPeaksTime = GetKernel()->GetAbsoluteTime();
}

void Graphics::SetDrawVisualizationName(bool draw)
{
	if(mDrawVisualizationName == draw)
		return;

	TRACE(L"Visualization name drawing turned %s.\n",draw ? L"on" : L"off");
	PrintOutputString(L"visualization name: \002%s\001",draw ? L"on" : L"off");
	
	mDrawVisualizationName = draw;
	mDrawVisualizationNameTime = GetKernel()->GetAbsoluteTime();
}

void Graphics::SetDrawAbout(bool draw)
{
	if(mDrawAbout == draw)
		return;

	TRACE(L"About screen drawing turned %s.\n",draw ? L"on" : L"off");
	PrintOutputString(L"about screen: \002%s\001",draw ? L"on" : L"off");
	
	mDrawAbout = draw;
	mDrawAboutTime = GetKernel()->GetAbsoluteTime();
}

void Graphics::SetDrawCursor(bool draw)
{
	if(mDrawCursor == draw)
		return;

	TRACE(L"Cursor drawing turned %s.\n",draw ? L"on" : L"off");
	PrintOutputString(L"cursor: \002%s\001",draw ? L"on" : L"off");
	
	mDrawCursor = draw;
	mDrawCursorTime = GetKernel()->GetAbsoluteTime();
}

bool Graphics::SetTransition(unsigned long index)
{
	//if(mTransition == index)
	//	return;

	if(index >= mTransitions.GetSize())
		return false;

	TRACE(L"Transition: \"%s\".\n",mTransitions[index]->GetName().GetBuffer());
	//PrintOutputString("transition: \002%d\001",index);
	
	mTransitionIndex = index;
	mTransitionScene = !mTransitionScene;
	mTransitionTime = GetKernel()->GetAbsoluteTime();

	return true;
}

bool Graphics::AddColorPreset(ColorPreset* preset)
{
	for(unsigned long i = 0; i < mColorPresets.GetSize(); ++i)
	{
		if(mColorPresets[i]->name == preset->name)
		{
			TRACE(L"Can't add preset \"%s\", already exists.\n",preset->name.GetBuffer());
			PrintOutputString(L"preset already exists: \002%s\001",preset->name.GetBuffer());
			
			return false;
		}
	}
		
	mColorPresets.InsertBack(preset);
	
	TRACE(L"Color preset \"%s\" added.\n",preset->name.GetBuffer());
	PrintOutputString(L"color preset added: \002%s\001",preset->name.GetBuffer());
	
	return true;
}

bool Graphics::AddVisualization(Visualization* vis)
{
	for(unsigned long i = 0; i < mVisualizations.GetSize(); ++i)
	{
		if(mVisualizations[i]->GetName() == vis->GetName())
		{
			TRACE(L"Can't add visualization \"%s\", already exists.\n",vis->GetName().GetBuffer());
			//PrintOutputString(L"visualization already exists: \002%s\001",vis->GetName().GetBuffer());
			
			return false;
		}
	}
		
	mVisualizations.InsertBack(vis);
	
	TRACE(L"Visualization \"%s\" added.\n",vis->GetName().GetBuffer());
	//PrintOutputString(L"visualization added: \002%s\001",vis->GetName().GetBuffer());
	
	return true;
}

bool Graphics::AddTransition(Transition* trans)
{
	for(unsigned long i = 0; i < mTransitions.GetSize(); ++i)
	{
		if(mTransitions[i]->GetName() == trans->GetName())
		{
			TRACE(L"Can't add transition \"%s\", already exists.\n",trans->GetName().GetBuffer());
			//PrintOutputString(L"transition already exists: \002%s\001",trans->GetName().GetBuffer());
			
			return false;
		}
	}
		
	mTransitions.InsertBack(trans);
	
	TRACE(L"Transition \"%s\" added.\n",trans->GetName().GetBuffer());
	//PrintOutputString(L"transition added: \002%s\001",trans->GetName().GetBuffer());
	
	return true;
}

bool Graphics::SetVisualization(unsigned long index)
{
	if(index >= mVisualizations.GetSize())
		return false;
	
	if(index != mVisualizationIndex)
	{
		TRACE(L"Visualization switched to \"%s\".\n",mVisualizations[index]->GetName().GetBuffer());
		PrintOutputString(L"visualization: \002%s\001",mVisualizations[index]->GetName().GetBuffer());
	}

	mVisualizationIndex = index;
	SetTransition(rand() % GetTransitionCount());

	return true;
}

bool Graphics::SetGamma(float contrast,float brightness,float gamma)
{
	if(!mDevice)
		return false;

	D3DGAMMARAMP ramp;
 
	int val;
 
	for(unsigned long i = 0; i < 256; ++i)
	{
		val = (int)((contrast+0.5f)*pow(i/255.f,1.0f/gamma)*65535.f + (brightness-0.5f)*32768.f - contrast*32768.f + 16384.f);
		ramp.red[i] = ramp.green[i] = ramp.blue[i] = min(max(val,0),65535);
	}
 
	if(mPresentParameters.Windowed || !(mCaps.Caps2 & D3DCAPS2_FULLSCREENGAMMA))
	{
		HDC hdc = GetDC(GetKernel()->GetWindowHandle());
		SetDeviceGammaRamp(hdc, &ramp);
		ReleaseDC(GetKernel()->GetWindowHandle(),hdc);
	}
	else
	{
		mDevice->SetGammaRamp(0, D3DSGR_CALIBRATE, &ramp);
	}

	return true;
}

bool Graphics::RestoreGamma()
{
	if(!mDevice)
		return false;

	D3DGAMMARAMP ramp;
 
	for(unsigned long i = 0; i < 256; ++i)
		ramp.red[i] = ramp.green[i] = ramp.blue[i] = i << 8;
 
	if(mPresentParameters.Windowed || !(mCaps.Caps2 & D3DCAPS2_FULLSCREENGAMMA))
	{
		HDC hdc = GetDC(GetKernel()->GetWindowHandle());
		SetDeviceGammaRamp(hdc, &ramp);
		ReleaseDC(GetKernel()->GetWindowHandle(),hdc);
	}
	else
	{
		mDevice->SetGammaRamp(0, D3DSGR_CALIBRATE, &ramp);
	}

	return true;
 }
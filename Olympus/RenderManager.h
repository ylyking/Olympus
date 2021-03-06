#pragma once

#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <vector>
#include "apex.h"
#include "Renderable.h"
#include "Camera.h"
#include "SkyBox.h"
#include "Scene.h"
#include "ScreenQuad.h"
#include "Object.h"
#include "OnScreen.h"
#include "FontSheet.h"
#include "GroundPlane.h"
#include "Sphere.h"
#include "Box.h"
#include "Projectile.h"
#include "GameTimer.h"
#include "ConstBuffers.h"
#include "LightHelper.h"
#include "ShadowManager.h"
#include "AssetManager.h"
#include <string>
using namespace std;

enum renderTargets
{
    backbuffer,
    postprocess,
    environment,
    depth
};

#define MAX_NUM_POINT_LIGHTS 21

namespace Colors
{
    XMGLOBALCONST XMFLOAT4 White          = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    XMGLOBALCONST XMFLOAT4 Black          = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    XMGLOBALCONST XMFLOAT4 Red            = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
    XMGLOBALCONST XMFLOAT4 Green          = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
    XMGLOBALCONST XMFLOAT4 Blue           = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
    XMGLOBALCONST XMFLOAT4 Yellow         = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
    XMGLOBALCONST XMFLOAT4 Cyan           = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
    XMGLOBALCONST XMFLOAT4 Magenta        = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
    XMGLOBALCONST XMFLOAT4 TorchOrange    = XMFLOAT4(0.7f, 0.45f, 0.2f, 1.0f);
                  
    XMGLOBALCONST XMFLOAT4 Silver		  = XMFLOAT4(0.75f, 0.75f, 0.75f, 1.0f);
    XMGLOBALCONST XMFLOAT4 LightSteelBlue = XMFLOAT4(0.69f, 0.77f, 0.87f, 1.0f);

    XMGLOBALCONST XMFLOAT4 NormalDiffuse  =	XMFLOAT4(.6f, .6f, .6f, 1);
    XMGLOBALCONST XMFLOAT4 NormalAmbient  =	XMFLOAT4(.4f, .4f, .4f, 1);
}

class RenderManager
{
public:
    
    int fps;
    float mspf;
    int SCREEN_WIDTH;
    int SCREEN_HEIGHT;


    RenderManager(ID3D11DeviceContext *devcon, 
                  ID3D11Device *dev, 
                  IDXGISwapChain *swapchain,
                  Apex *apex,
                  Camera *cam,
                  D3D11_VIEWPORT *viewport);

    void fpsCalc(GameTimer mTimer);
    void GetScreenParams(int mClientWidth, int mClientHeight);
    void Update(float dt);

    void Render(int renderType);
    void Render();
    void RenderToTarget(enum renderTargets);

    // Shadow Related Functions
    void RenderToShadow();
    void SetShadowSRV(int loc);

    //DEBUG
    void RenderManager::SetPosition(float x, float y, float z);
    void RenderManager::SetEmit(bool on);

    void RecompShaders();
    //void InitObjects();

    float Randomf(float low, float high);

    IDXGISwapChain *mSwapchain;             // the pointer to the swap chain interface
    ID3D11Device *mDev;                     // the pointer to our Direct3D device interface
    ID3D11DeviceContext *mDevcon; 
    D3D11_VIEWPORT *mViewport;

    ID3D11DepthStencilView *mZbuffer;       // the pointer to our depth buffer
    ID3D11DepthStencilView *mZbuffer2;       // the pointer to our depth buffer
    ID3D11ShaderResourceView* mDepthShaderResourceView;
    ID3D11Texture2D* mDepthTargetTexture;

    ID3D11RenderTargetView* mPostProcessRTV;
    ID3D11RenderTargetView* mEnvironmentRTV;
    ID3D11RenderTargetView *mBackbuffer;    // the pointer to our back buffer

    ID3D11BlendState* mBlendState;   // Our blend state
    ID3D11SamplerState *mSampState;

    Camera *mCam;
    Camera *mScreenCam;
    GroundPlane *mGrid;
    
    vector<Scene*> scene;
    
    int		mCurrentScene;


    ID3D11Buffer *sceneCBuffer;
    ID3D11Buffer *dirLightCBuffer;
    ID3D11Buffer *pointLightCBuffer;

    SkyBox *mSkyBox;
    ScreenQuad *mScreen;
    Projectile *projectile;

    ApexParticles* particles;
    ApexParticles* emitter;
    ApexParticles* torch1;
    ApexParticles* torch2;
    ApexCloth* mCloth;
    
    AssetManager* mAssetManager;

    SceneBuff sceneBuff;

    Apex                           *mApex;
    physx::apex::NxUserRenderer*    gRenderer;

    DirectionalLight mDirLight[2];
    PointLight		 mPointLight[2];

    bool emitterOn;

    FontSheet mFont;
    OnScreen mText;
    POINT textPos;
    POINT hairPos;
    POINT posPos;
    string sText;

    //Rasterizer Variables
    ID3D11RasterizerState*		pState;
    D3D11_RASTERIZER_DESC		raster;

    //Shadow Varibles
    D3D11_VIEWPORT mShadowPort;
    ID3D11Texture2D* pShadowMap;
    ID3D11DepthStencilView* pShadowMapDepthView;
    ID3D11ShaderResourceView* pShadowMapSRView;
    Camera *mShadowCam;
    ID3D11Buffer *shadowCBuffer[5];
    vector<ShadowBuff> shadowBuff[5];
    ShadowBuff shadBuffer;
    vector<ShadowManager*> shadows[5];
    ShadowManager* shad;


    int PartyMode;
    
    
    GameTimer			newTimer;
    vector<float>       sceneLoadTimes;

};


#endif

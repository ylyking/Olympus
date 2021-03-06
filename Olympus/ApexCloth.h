#pragma once

// Apex Cloth

#ifndef APEX_CLOTHING_H
#define APEX_CLOTHING_H

//#include "apex.h"
#include "NxApex.h"
#include "NxApexSDK.h"
#include "NxModuleClothing.h"
#include "NxClothingAsset.h"
#include "NxClothingActor.h"
#include "NxClothingCollision.h"

#include "NxParamUtils.h"
#include <NxApexRenderVolume.h>

#include <string>

#include "Camera.h"
#include <d3d11.h>
#include <d3dx11.h>
#include "Renderable.h"

//#include "ZeusRenderResources.h"

#include <vector>
using namespace std;
using namespace physx;

class ApexCloth : public Renderable
{
public:
    ApexCloth(float maxWind);
    ~ApexCloth();

    void InitPipeline();

    void CreateCloth(NxApexSDK* gApexSDK, NxApexScene* gApexScene,
                        ID3D11DeviceContext *devcon, 	ID3D11Device *dev,
                      physx::apex::NxUserRenderer* renderer, const char* filename, const char* texfile);
    virtual void Update();
    virtual void RecompileShader();
	virtual void Depth();

    void SetPosition(float x, float y, float z, float rx, float ry, float rz);
    virtual void Render(ID3D11Buffer *sceneBuff, Camera *mCam, int renderType);

    physx::apex::NxClothingActor* getClothingActor() {return clothingActor;}

private:
    ID3D11Device *mDev;
    ID3D11DeviceContext *mDevcon;

    ID3D11InputLayout   *mLayout;           // the pointer to the input layout
    ID3D11VertexShader  *mVS;               // the pointer to the vertex shader
    ID3D11GeometryShader  *mGS;             // the pointer to the vertex shader
    ID3D11PixelShader   *mPS;               // the pointer to the pixel shader
    ID3D11Buffer*		mConstBuffer;
    ID3D11ShaderResourceView *clothTexture;

    float mMaxWind;
    physx::apex::NxClothingActor* clothingActor;
            
    apex::NxUserRenderer*      gRenderer;
};

#endif
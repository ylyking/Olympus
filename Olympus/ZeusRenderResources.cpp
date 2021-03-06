#include "ZeusRenderResources.h"
// SampleApexRenderResources.cpp

/*******************************
* ZeusVertexBuffer
*********************************/

ZeusVertexBuffer::ZeusVertexBuffer(const physx::apex::NxUserRenderVertexBufferDesc& desc, ID3D11Device* dev, ID3D11DeviceContext* devcon) :
    mDevice(dev), mStride(0), mDevcon(devcon)
{
    
    for (physx::PxU32 i = 0; i < physx::apex::NxRenderVertexSemantic::NUM_SEMANTICS; i++)
    {
        physx::apex::NxRenderVertexSemantic::Enum apexSemantic = physx::apex::NxRenderVertexSemantic::Enum(i);
        physx::apex::NxRenderDataFormat::Enum apexFormat = desc.buffersRequest[i];
        
        if (apexFormat != physx::apex::NxRenderDataFormat::UNSPECIFIED)
        {
            if (apexSemantic == physx::apex::NxRenderVertexSemantic::POSITION || apexSemantic == physx::apex::NxRenderVertexSemantic::TEXCOORD0/* || apexSemantic == physx::apex::NxRenderVertexSemantic::NORMAL*/)
                mStride += physx::apex::NxRenderDataFormat::getFormatDataSize(apexFormat);
        }
    }


    D3D11_BUFFER_DESC d3ddesc;
    d3ddesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    d3ddesc.ByteWidth = desc.maxVerts * mStride;
    d3ddesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    d3ddesc.MiscFlags = 0;

    d3ddesc.Usage = D3D11_USAGE_DYNAMIC; //Until I know what better to use
    
    HRESULT hr = mDevice->CreateBuffer(&d3ddesc, NULL, &mVertexBuffer);
}

ZeusVertexBuffer::~ZeusVertexBuffer(void)
{
    if (mVertexBuffer)
    {
        mVertexBuffer->Release();
    }
}

bool ZeusVertexBuffer::getInteropResourceHandle(CUgraphicsResource& handle)
{
    return false; //For now we won't support CUDA
}

void ZeusVertexBuffer::writeBuffer(const physx::NxApexRenderVertexBufferData& data, physx::PxU32 firstVertex, physx::PxU32 numVertices)
{
    if (!mVertexBuffer || !numVertices)
    {
        return;
    }
    
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result;

    // Lock the vertex buffer so it can be written to.
    result = mDevcon->Map(mVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if(FAILED(result))
    {
        return;
    }
    physx::PxU32 currentStride = 0;
    physx::PxU32 numSem = 0;

    for (physx::PxU32 i = 0; i < physx::apex::NxRenderVertexSemantic::NUM_SEMANTICS; i++)
    {
        physx::apex::NxRenderVertexSemantic::Enum semantic = (physx::apex::NxRenderVertexSemantic::Enum)i;
        const physx::apex::NxApexRenderSemanticData& semanticData = data.getSemanticData(semantic);
        if (semantic == physx::apex::NxRenderVertexSemantic::POSITION || semantic == physx::apex::NxRenderVertexSemantic::TEXCOORD0 /*|| semantic == physx::apex::NxRenderVertexSemantic::NORMAL*/)
        {
            if (semanticData.data)
            {
                const void* srcData = semanticData.data;
                const physx::PxU32 srcStride = semanticData.stride;

                void* dstData = mappedResource.pData;
                PX_ASSERT(dstData && mStride);
                if (dstData && mStride)
                {
                    dstData = ((physx::PxU8*)dstData) + (firstVertex * mStride) + currentStride;

                    for (physx::PxU32 j = 0; j < numVertices; j++)
                    {
                        memcpy(dstData, srcData, srcStride); // This doesn't work for writing multiple semantics per buffer, think of a fix
                        srcData = ((physx::PxU8*)srcData) + srcStride;
                        dstData = ((physx::PxU8*)dstData) + srcStride;
                        //currentStride += srcStride;
                    }
                    //currentStride += srcStride;

                    //dstData = ((physx::PxU8*)dstData) + (firstVertex * mStride);

                    //for (physx::PxU32 j = 0; j < numVertices; j++)
                    //{
                    //    memcpy(dstData, srcData, srcStride); // This doesn't work for writing multiple semantics per buffer, think of a fix
                    //    srcData = ((physx::PxU8*)srcData) + srcStride;
                    //    dstData = ((physx::PxU8*)dstData) + srcStride;
                    //}
                }
            } 
        }

    }
        
    // Unlock the vertex buffer.
    mDevcon->Unmap(mVertexBuffer, 0);
}



/*******************************
* ZeusIndexBuffer
*********************************/

ZeusIndexBuffer::ZeusIndexBuffer(const physx::apex::NxUserRenderIndexBufferDesc& desc, ID3D11Device* dev, ID3D11DeviceContext* devcon) :
    mDevice(dev), mDevcon(devcon), mPrimitiveType(desc.primitives), mStride(0)
{
    mStride = physx::apex::NxRenderDataFormat::getFormatDataSize(desc.format);
    D3D11_BUFFER_DESC d3ddesc;
    d3ddesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    d3ddesc.ByteWidth = desc.maxIndices * mStride;
    d3ddesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    d3ddesc.MiscFlags = 0;
    
    d3ddesc.Usage = D3D11_USAGE_DYNAMIC; //Until I know what better to use


    HRESULT hr = mDevice->CreateBuffer(&d3ddesc, NULL, &mIndexBuffer);

}

ZeusIndexBuffer::~ZeusIndexBuffer(void)
{
    if(mIndexBuffer)
    {
        mIndexBuffer->Release();
    }
}

bool ZeusIndexBuffer::getInteropResourceHandle(CUgraphicsResource& handle)
{
    return false;
}

void ZeusIndexBuffer::writeBuffer(const void* srcData, physx::PxU32 srcStride, physx::PxU32 firstDestElement, physx::PxU32 numElements)
{
   if (!mIndexBuffer || !numElements)
    {
        return;
    }
    
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result;

    // Lock the vertex buffer so it can be written to.
    result = mDevcon->Map(mIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if(FAILED(result))
    {
        return;
    }
        
    if (srcData)
    {
        void* dstData = mappedResource.pData;
        void* dstDataCopy = dstData;

        PX_ASSERT(dstData);
        if (dstData && srcStride)
        {
            dstData = ((physx::PxU8*)dstData) + firstDestElement * srcStride;
            physx::PxU32 formatSize = mStride;
            for (physx::PxU32 j = 0; j < numElements; j++)
            {
                memcpy(dstData, srcData, formatSize);
                srcData = ((physx::PxU8*)srcData) + srcStride;
                dstData = ((physx::PxU8*)dstData) + srcStride;
            }
        }
    }
    

    // Unlock the index buffer.
    mDevcon->Unmap(mIndexBuffer, 0);
}

/*******************************
* ZeusSurfaceBuffer
*********************************/

ZeusSurfaceBuffer::ZeusSurfaceBuffer(const physx::apex::NxUserRenderSurfaceBufferDesc& desc)
{

}

ZeusSurfaceBuffer::~ZeusSurfaceBuffer(void)
{

}

void ZeusSurfaceBuffer::writeBuffer(const void* srcData, physx::PxU32 srcStride, physx::PxU32 firstDestElement, physx::PxU32 width, physx::PxU32 height, physx::PxU32 depth)
{

}


/*******************************
* ZeusBoneBuffer
*********************************/

ZeusBoneBuffer::ZeusBoneBuffer(const physx::apex::NxUserRenderBoneBufferDesc& desc)
{

}

ZeusBoneBuffer::~ZeusBoneBuffer(void)
{

}

void ZeusBoneBuffer::writeBuffer(const physx::apex::NxApexRenderBoneBufferData& data, physx::PxU32 firstBone, physx::PxU32 numBones)
{

}


/*******************************
* ZeusInstanceBuffer
*********************************/

ZeusInstanceBuffer::ZeusInstanceBuffer(const physx::apex::NxUserRenderInstanceBufferDesc& desc)
{
    mMaxInstances = desc.maxInstances;
    mInstanceBuffer = new struct InstanceBuffer[mMaxInstances];
}

ZeusInstanceBuffer::~ZeusInstanceBuffer(void)
{
    if (mInstanceBuffer)
    {
        delete [] mInstanceBuffer;
    }
}

void ZeusInstanceBuffer::writeBuffer(const physx::apex::NxApexRenderInstanceBufferData& data, physx::PxU32 firstInstance, physx::PxU32 numInstances)
{
    for(physx::PxU32 j = 0; j < numInstances; j++)
    {
        for (physx::PxU32 i = 0; i < physx::apex::NxRenderInstanceSemantic::NUM_SEMANTICS; i++)
        {
            physx::apex::NxRenderInstanceSemantic::Enum semantic = (physx::apex::NxRenderInstanceSemantic::Enum)i;
            const physx::apex::NxApexRenderSemanticData& semanticData = data.getSemanticData(semantic);
            if (semanticData.data)
            {
                
                switch (i)
                {
                case physx::apex::NxRenderInstanceSemantic::POSITION:
                    //mInstanceBuffer[j + firstInstance].Position = semanticData.data;
                    break;
                case physx::apex::NxRenderInstanceSemantic::ROTATION_SCALE:
                    
                    break;
                case physx::apex::NxRenderInstanceSemantic::VELOCITY_LIFE:
                    
                    break;
                case physx::apex::NxRenderInstanceSemantic::DENSITY:

                    break;
                case physx::apex::NxRenderInstanceSemantic::UV_OFFSET:

                    break;
                case physx::apex::NxRenderInstanceSemantic::LOCAL_OFFSET:

                    break;
                }
            }
        }
    }
}

bool ZeusInstanceBuffer::getInteropResourceHandle(CUgraphicsResource& handle)
{
    return false;
}


/*******************************
* ZeusSpriteBuffer
*********************************/

ZeusSpriteBuffer::ZeusSpriteBuffer(const physx::apex::NxUserRenderSpriteBufferDesc& desc, ID3D11Device* dev, ID3D11DeviceContext* devcon) :
    mDevice(dev), mStride(0), mDevcon(devcon)
{
    
    for (physx::PxU32 i = 0; i < physx::apex::NxRenderSpriteSemantic::NUM_SEMANTICS; i++)
    {
        physx::apex::NxRenderSpriteSemantic::Enum apexSemantic = physx::apex::NxRenderSpriteSemantic::Enum(i);
        physx::apex::NxRenderDataFormat::Enum apexFormat = desc.semanticFormats[i];
     
        if (apexFormat != physx::apex::NxRenderDataFormat::UNSPECIFIED)
        {
            if(apexSemantic == physx::apex::NxRenderSpriteSemantic::POSITION) // For right now only doing position
                mStride += physx::apex::NxRenderDataFormat::getFormatDataSize(apexFormat);
        }
    }

    D3D11_BUFFER_DESC d3ddesc;
    d3ddesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    d3ddesc.ByteWidth = desc.maxSprites * mStride;
    d3ddesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    d3ddesc.MiscFlags = 0;

    if(desc.hint == NxRenderBufferHint::STATIC)
    {
        d3ddesc.Usage = D3D11_USAGE_DEFAULT;
    }
    else if(desc.hint == NxRenderBufferHint::DYNAMIC)
    {
        d3ddesc.Usage = D3D11_USAGE_DYNAMIC;
    }
    else if(desc.hint == NxRenderBufferHint::STREAMING)
    {
        d3ddesc.Usage = D3D11_USAGE_DYNAMIC; //Until I know what better to use
    }
    else
        return;

    HRESULT hResult = mDevice->CreateBuffer(&d3ddesc, NULL, &mSpriteBuffer);
}

ZeusSpriteBuffer::~ZeusSpriteBuffer(void)
{
    if(mSpriteBuffer)
    {
        mSpriteBuffer->Release();
    }
}

bool ZeusSpriteBuffer::getInteropResourceHandle(CUgraphicsResource& handle)
{
    return false;
}

void ZeusSpriteBuffer::Render(int start, int count)
{
    UINT stride = (UINT)mStride;
    UINT offset = 0;
    mDevcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    mDevcon->IASetVertexBuffers(0, 1, &mSpriteBuffer, &stride, &offset);
    
    mDevcon->Draw(count, start);
}

void ZeusSpriteBuffer::writeBuffer(const physx::apex::NxApexRenderSpriteBufferData& data, physx::PxU32 firstSprite, physx::PxU32 numSprites)
{
    if (!mSpriteBuffer || !numSprites)
    {
        return;
    }
    
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result;

    // Lock the vertex buffer so it can be written to.
    result = mDevcon->Map(mSpriteBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if(FAILED(result))
    {
        return;
    }

    for (physx::PxU32 i = 0; i < physx::apex::NxRenderSpriteSemantic::NUM_SEMANTICS; i++)
    {
        physx::apex::NxRenderSpriteSemantic::Enum semantic = (physx::apex::NxRenderSpriteSemantic::Enum)i;
        const physx::apex::NxApexRenderSemanticData& semanticData = data.getSemanticData(semantic);
        if (semanticData.data)
        {
            const void* srcData = semanticData.data;
            const physx::PxU32 srcStride = semanticData.stride;

            physx::apex::NxRenderVertexSemantic::Enum apexSemantic = physx::apex::NxRenderVertexSemantic::NUM_SEMANTICS;
            if(semantic == physx::apex::NxRenderSpriteSemantic::POSITION)
            {
                void* dstData = mappedResource.pData;
                void* dstDataCopy = dstData;
                PX_ASSERT(dstData && mStride);
                if (dstData && mStride)
                {
                    dstData = ((physx::PxU8*)dstData) + firstSprite * mStride;
 
                    if (mStride == srcStride)
                    {
                        memcpy(dstData, srcData, mStride*numSprites);
                    }
                    else
                    {
                        for (physx::PxU32 j = 0; j < numSprites; j++)
                        {
                            memcpy(dstData, srcData, mStride);
                            srcData = ((physx::PxU8*)srcData) + srcStride;
                            dstData = ((physx::PxU8*)dstData) + mStride;
                        }
                    }
                }
            }
        }
    }

 //   // Get a pointer to the data in the vertex buffer.

 //   //const int num = &numSprites;
 //   // Copy the data into the vertex buffer.
    //VertexType* vrticesPtr;
 //   vrticesPtr = (VertexType*)mappedResource.pData + (firstSprite * mStride);
 //   VertexType* sauce = (VertexType*) malloc(mStride * 16);
 //   VertexType* v = new VertexType();
 //   for(physx::PxU32 i = 0; i < 16; i++)
 //   {
    //	v->x = 0.0f;
    //	v->y = 2.0f - (i * .125);
    //	v->z = 0.0f;
    //	
    //	//memcpy( sorcData + (sizeof(v) * i), (void*)v, sizeof(v) );
    //	//memcpy( sauce + (mStride * i), (void*)v, (mStride) );
 //       for (physx::PxU32 j = 0; j < physx::apex::NxRenderSpriteSemantic::NUM_SEMANTICS; j++)
 //       {
 //           physx::apex::NxRenderSpriteSemantic::Enum apexSemantic = (physx::apex::NxRenderSpriteSemantic::Enum)j;
 //           const physx::apex::NxApexRenderSemanticData& semanticData = data.getSemanticData(apexSemantic);
 //           if (semanticData.data)
 //           {
    //			if(apexSemantic == physx::apex::NxRenderSpriteSemantic::POSITION)
    //			{
    //				//memcpy(srcData + (mStride * i), semanticData.data, semanticData.stride);
    //				VertexType* srcPtr;
    //				srcPtr = (VertexType*)semanticData.data + (mStride * i);
    //				memcpy(vrticesPtr + (mStride * i), (void*)srcPtr, (mStride));
    //			}
 //           }
 //       }
 //   }

 //   //memcpy(vrticesPtr, (void*)sauce, (mStride));
    
    // Unlock the vertex buffer.
    mDevcon->Unmap(mSpriteBuffer, 0);

    //free(vrticesPtr);
    /*vrticesPtr = 0;
    delete(v);
    v = 0;*/
}


/*******************************
* ZeusRenderResource
*********************************/

ZeusRenderResource::ZeusRenderResource(const physx::apex::NxUserRenderResourceDesc& desc)
{
    mBoneBuffer = static_cast<ZeusBoneBuffer*>(desc.boneBuffer);
    mIndexBuffer = static_cast<ZeusIndexBuffer*>(desc.indexBuffer);
    mInstanceBuffer = static_cast<ZeusInstanceBuffer*>(desc.instanceBuffer);
    mSpriteBuffer = static_cast<ZeusSpriteBuffer*>(desc.spriteBuffer);

    mNumVertexBuffers = desc.numVertexBuffers;
    for(PxU32 i = 0; i < mNumVertexBuffers; i++)
    {
        mVertexBuffers.push_back( static_cast<ZeusVertexBuffer*>(desc.vertexBuffers[i]) );
    }

    setVertexBufferRange(desc.firstVertex, desc.numVerts);
    setIndexBufferRange(desc.firstIndex, desc.numIndices);
    setBoneBufferRange(desc.firstBone, desc.numBones);
    setInstanceBufferRange(desc.firstInstance, desc.numInstances);
    setSpriteBufferRange(desc.firstSprite, desc.numSprites);
    setMaterial(desc.material);
}

ZeusRenderResource::~ZeusRenderResource()
{
    if (mVertexBuffers.size()>0)
    {
       mVertexBuffers.clear();
    }
}

void ZeusRenderResource::setVertexBufferRange(physx::PxU32 firstVertex, physx::PxU32 numVerts)
{
    mVertexStart = firstVertex;
    mVertexCount = numVerts;
}

void ZeusRenderResource::setIndexBufferRange(physx::PxU32 firstIndex, physx::PxU32 numIndices)
{
    mIndexStart = firstIndex;
    mIndexCount = numIndices;
}

void ZeusRenderResource::setBoneBufferRange(physx::PxU32 firstBone, physx::PxU32 numBones)
{

}

void ZeusRenderResource::setInstanceBufferRange(physx::PxU32 firstInstance, physx::PxU32 numInstances)
{

}

void ZeusRenderResource::setSpriteBufferRange(physx::PxU32 firstSprite, physx::PxU32 numSprites)
{
    mSpriteStart = firstSprite;
    mSpriteCount = numSprites;
}

void ZeusRenderResource::setMaterial(void* material)
{

}

void ZeusRenderResource::Render()
{
    if(mSpriteBuffer)
    {
        mSpriteBuffer->Render(mSpriteStart, mSpriteCount);
    }
    else if(mIndexBuffer && mVertexBuffers[0])
    {
        //Render somehow?
        UINT* strides = (UINT*) malloc(sizeof(UINT) * mVertexBuffers.size());
        UINT* offsets = (UINT*) malloc(sizeof(UINT) * mVertexBuffers.size());
        ID3D11Buffer* buffers = (ID3D11Buffer*) malloc(sizeof(ID3D11Buffer) * mVertexBuffers.size());
        ID3D11Buffer* buffers2[2];
        UINT strides2[2];
        strides2[0] = 8;
        strides2[1] = 12;
        UINT offsets2[2];
        offsets2[0] = offsets2[1] = 0;

        buffers2[0] = mVertexBuffers[0]->mVertexBuffer;
        buffers2[1] = mVertexBuffers[1]->mVertexBuffer;

        for(int i = 0; i < (int)mVertexBuffers.size(); i++)
        {
            strides[i] = mVertexBuffers[i]->mStride;
            offsets[i] = 0;
            buffers[i] = *mVertexBuffers[i]->mVertexBuffer;
        }
        mIndexBuffer->mDevcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        mIndexBuffer->mDevcon->IASetIndexBuffer(mIndexBuffer->mIndexBuffer, DXGI_FORMAT_R32_UINT, offsets[0]);
        
        UINT stride = (UINT)12;
        UINT offset = 0;
        mIndexBuffer->mDevcon->IASetVertexBuffers(0, 2, buffers2, strides, offsets);
        
        mIndexBuffer->mDevcon->DrawIndexed(mIndexCount, mIndexStart, mVertexStart);
		free(strides);
		free(offsets);
		 free(buffers);
    }
}


/*****************************************/
/* ZeusRenderer                          */
/*****************************************/

ZeusRenderer::ZeusRenderer()
{

}

void ZeusRenderer::renderResource(const physx::apex::NxApexRenderContext& context)
{
    if (context.renderResource)
    {
        
        //static_cast<SampleApexRendererMesh*>(context.renderResource)->render(context, mForceWireframe, mOverrideMaterial);
        //Render it here
        static_cast<ZeusRenderResource*>(context.renderResource)->Render();
    }
}
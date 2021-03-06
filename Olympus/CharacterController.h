#pragma once
#include "Apex.h"
#include <math.h>
#include <time.h>

#ifdef _DEBUG
    #pragma comment(lib, "PhysX3CharacterKinematicCHECKED_x86.lib")
#else
    #pragma comment(lib, "PhysX3CharacterKinematic_x86.lib")
#endif

class CharacterController
{
public:
    PxController*		pCharacter;

    string				currentPos;
    string				movePos;

    bool gJump; 
    float gV0; 
    float gJumpTime; 

    bool jumpHappened;
    double times;
    time_t timer;
    bool zoomFlag;
    bool runFlag;
	float setSpeed;

    static const PxF32	minDist;

public:
    PxExtendedVec3 fpos;

    CharacterController(Apex *mApex);
    ~CharacterController(void);
    void MoveTo( float x, float y, float z );
    void Move( float x, float y, float z, float elapsedTime );
    void rotateX( float angle ) const;
	void SetSpeed(float speed);
    void zoom(bool zflag);
    void run(bool rflag);
    void scale( float x, float y, float z ) const;
    void boolJump(bool jump);
    void StartJump();
    void control(bool move, bool startJump, float x, float y, float z, float elapsedTime);

    int					mCurrentScene;
    void				SetScene(int sceneNum){mCurrentScene = sceneNum;}
};
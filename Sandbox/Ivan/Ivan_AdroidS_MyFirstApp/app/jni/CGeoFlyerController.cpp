/****************************************************************************
 * Copyright (c) 2013 GeoMind Srl. www.geomind.it
 * All rights reserved.
 *
 *---------------------------------------------------------------------------
 * This software is part of the GeoFlyerPro PROJECT by GeoMind Srl;
 * it is not allowed to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of software in parts or as a whole,
 * in source and binary forms, without a written permission of GeoMind Srl.
 *
 ****************************************************************************/

#include <assert.h>

#include "CGeoFlyerController.h"
#include "CFuffaDelegate.h"
#include "it_geomind_myfirstapp_GeoFlyerViewController.h"

CFuffaDelegate kFuffaDelegate;

struct EnvViewControllerPair
{
	JNIEnv* mJniEnv;
	jobject mViewController;
};


// ---------------------------------------------------
CGeoFlyerController::CGeoFlyerController( void* inViewController )
{
	EnvViewControllerPair* evc = reinterpret_cast<EnvViewControllerPair*>( inViewController );

	mJniEnv = evc->mJniEnv;

	jint rs = mJniEnv->GetJavaVM( &mJvm );
   assert( rs == JNI_OK );
	
	mViewController = reinterpret_cast<jclass>( mJniEnv->NewGlobalRef(evc->mViewController) );
	
	mDelegateCallback = 0;
	mDelegate = 0;
}

// ---------------------------------------------------
CGeoFlyerController::~CGeoFlyerController()
{
	if( mViewController )
		GetJniEnv()->DeleteGlobalRef( mViewController );
}

// ---------------------------------------------------
void 
CGeoFlyerController::MessageBox( const char* inTitle, const char* inMessage, 
											const std::vector< std::string >& inButtons, 
										   CGeoFlyerDelegate* inDelegate, GfDelegateCallback inCallback )
{
	if( !inButtons.size() )
		return;

	mDelegateCallback = inCallback;
	mDelegate = inDelegate;

	JNIEnv* env = GetJniEnv();

	jobjectArray buttonStrings = env->NewObjectArray( inButtons.size(),
																	  env->FindClass( "java/lang/String" ),
																	  env->NewStringUTF( "" ) );
	for( size_t i = 0; i < inButtons.size(); ++i )
		env->SetObjectArrayElement( buttonStrings, i, env->NewStringUTF( inButtons[i].c_str() ) );

	jstring title = env->NewStringUTF( inTitle );
	jstring msg = env->NewStringUTF( inMessage );

	jobject viewController = GetViewController();
	jclass cls = env->GetObjectClass( viewController );
	jmethodID mid = env->GetMethodID( cls, "messageBoxAndButtons", "(Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;)V" );
	env->CallVoidMethod( viewController, mid, title, msg, buttonStrings );
}

// ---------------------------------------------------	
void		
CGeoFlyerController::CGeoFlyerController::OnMessageBoxResult( int inChoice )
{
   if( mDelegate && mDelegateCallback )
   {
      mDelegateCallback( mDelegate, (void *) inChoice );
      mDelegate = 0;
      mDelegateCallback = 0;
   }
}

// ---------------------------------------------------	
inline JNIEnv* 
CGeoFlyerController::GetJniEnvThreadSafe() 
{ 
	JNIEnv* retval = 0;

	// questa funzione non e' testa contro memory leak.
	// prima di decommentare questo codice, documentatevi a riguardo
	/*
	int getEnvStat = mJvm->GetEnv( &retval, JNI_VERSION_1_6 );
	if( getEnvStat == JNI_OK )
		return retval;
	if( getEnvStat == JNI_EVERSION )
	{
		assert( 0 );
		return 0;
	}
	if( getEnvStat == JNI_EDETACHED ) 
	{
		jint rs = mJvm->AttachCurrentThread( &retval, 0 ); // \warning: qualcuno prima o poi deve fare la mJvm->DetachCurrentThread();
		assert( rs == JNI_OK );
		return retval;
	}
	*/

	return retval;
}





extern "C" {
/*
 * Class:     it_geomind_myfirstapp_GeoFlyerViewController
 * Method:    CreateGeoFlyerController
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_it_geomind_myfirstapp_GeoFlyerViewController_CreateGeoFlyerController
  (JNIEnv* inEnv, jobject inGeoFlyerViewControllerOwner )
{
	EnvViewControllerPair evc = { inEnv, inGeoFlyerViewControllerOwner };

	return reinterpret_cast<jlong>( 
			new CGeoFlyerController( &evc )
		);
}

/*
 * Class:     it_geomind_myfirstapp_GeoFlyerViewController
 * Method:    DestroyGeoFlyerController
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_it_geomind_myfirstapp_GeoFlyerViewController_DestroyGeoFlyerController
  (JNIEnv* inEnv, jobject inGeoFlyerViewControllerOwner, jlong inCGeoFlyerControllerPtr )
{
	CGeoFlyerController* ptr = reinterpret_cast<CGeoFlyerController*>(inCGeoFlyerControllerPtr);
	delete( ptr );
}

/*
 * Class:     it_geomind_myfirstapp_GeoFlyerViewController
 * Method:    DebugGeoFlyerControllerShowDialog
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_it_geomind_myfirstapp_GeoFlyerViewController_DebugGeoFlyerControllerShowDialog
  (JNIEnv* inEnv, jobject inGeoFlyerViewControllerOwner, jlong inCGeoFlyerControllerPtr )
{
	CGeoFlyerController* ptr = reinterpret_cast<CGeoFlyerController*>(inCGeoFlyerControllerPtr);

	std::vector<std::string> buttonsStr;
	buttonsStr.push_back( "Negative/No/Cancel" );
	buttonsStr.push_back( "Neutral/Not Now" );
	buttonsStr.push_back( "Positive/Ok" );

	ptr->MessageBox( "titolo", "messaggio", buttonsStr, &kFuffaDelegate, CFuffaDelegate::callback );
}

/*
 * Class:     it_geomind_myfirstapp_GeoFlyerViewController
 * Method:    OnMessageBoxResult
 * Signature: (JI)V
 */
JNIEXPORT void JNICALL Java_it_geomind_myfirstapp_GeoFlyerViewController_OnMessageBoxResult
  ( JNIEnv* inEnv, jobject inGeoFlyerViewControllerOwner, 
    jlong inCGeoFlyerControllerPtr, jint inChoce )
{
	CGeoFlyerController* ptr = reinterpret_cast<CGeoFlyerController*>(inCGeoFlyerControllerPtr);
	ptr->OnMessageBoxResult( inChoce );
}


}	// extern "C" 

